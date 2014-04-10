/* GStreamer
 * Copyright (C) 2008 Andre Moreira Magalhaes <andrunko@gmail.com>
 *
 * gstcrossfade.c: Crossfade element, N* in, one out
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/**
 * SECTION:element-crossfade
 *
 * <refsect2>
 * <para>
 * The Crossfade allows to crossfade between audio streams.
 * It will mix the data using an adder and proper crossfade them.
 * Mixed data is clamped to the min/max values of the data format.
 * </para>
 * <title>Example launch line</title>
 * <para>
 * <programlisting>
 * gst-launch filesrc location=testcrossfade1.ogg ! decodebin ! audioconvert ! crossfade duration=5000 name=crossfade ! alsasink filesrc location=testcrossfade2.ogg ! decodebin ! audioconvert ! crossfade.
 * </programlisting>
 * This pipeline crossfade from testcrossfade1.ogg to testcrossfade2.ogg with a 5000 miliseconds duration.
 * </para>
 * </refsect2>
 */

#define VERSION "0.1.0"
#define PACKAGE "gst-plugins"
#define GST_PACKAGE_NAME "GStreamer Plugins source release"
#define GST_PACKAGE_ORIGIN "Unknown package origin"

#include "gstcrossfade.h"

#include <gst/audio/audio.h>
#include <gst/base/gstbasetransform.h>

#define GST_CAT_DEFAULT gst_crossfade_debug
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);

enum
{
  PROP_0,
  PROP_DURATION
};

/* elementfactory information */
static const GstElementDetails gst_crossfade_details = GST_ELEMENT_DETAILS ("Crossfade",
    "Generic/Audio",
    "Crossfade audio",
    "Andre Moreira Magalhaes <andrunko@gmail.com>");

static GstStaticPadTemplate gst_crossfade_src_template =
    GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_AUDIO_INT_PAD_TEMPLATE_CAPS "; "
        GST_AUDIO_FLOAT_PAD_TEMPLATE_CAPS)
    );

static GstStaticPadTemplate gst_crossfade_sink_template =
    GST_STATIC_PAD_TEMPLATE ("sink%d",
    GST_PAD_SINK,
    GST_PAD_REQUEST,
    GST_STATIC_CAPS (GST_AUDIO_INT_PAD_TEMPLATE_CAPS "; "
        GST_AUDIO_FLOAT_PAD_TEMPLATE_CAPS)
    );

static void gst_crossfade_class_init (GstCrossfadeClass * klass);
static void gst_crossfade_init (GstCrossfade * crossfade);
static void gst_crossfade_dispose (GObject * object);
static void gst_crossfade_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_crossfade_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static GstPad *gst_crossfade_request_new_pad (GstElement * element,
    GstPadTemplate * temp, const gchar * unused);
static void gst_crossfade_release_pad (GstElement * element, GstPad * pad);
static GstPadLinkReturn gst_crossfade_volume_pad_link (GstPad * pad,
    GstPad * peer);

static GstController *gst_crossfade_start_xfade (GstElement *volume,
    gdouble start, gdouble end,
    guint64 position, guint64 duration);

static GstElementClass *parent_class = NULL;

GType
gst_crossfade_get_type (void)
{
  static GType crossfade_type = 0;

  if (G_UNLIKELY (crossfade_type == 0)) {
    static const GTypeInfo crossfade_info = {
      sizeof (GstCrossfadeClass),
      NULL,
      NULL,
      (GClassInitFunc) gst_crossfade_class_init,
      NULL,
      NULL,
      sizeof (GstCrossfade),
      0,
      (GInstanceInitFunc) gst_crossfade_init,
    };

    crossfade_type = g_type_register_static (GST_TYPE_BIN, "GstCrossfade",
        &crossfade_info, 0);
    GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, "crossfade", 0,
        "audio crossfade element");
  }
  return crossfade_type;
}

static void
gst_crossfade_class_init (GstCrossfadeClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;

  gobject_class->dispose = gst_crossfade_dispose;
  gobject_class->set_property = gst_crossfade_set_property;
  gobject_class->get_property = gst_crossfade_get_property;

  g_object_class_install_property (gobject_class, PROP_DURATION,
      g_param_spec_uint64 ("duration", "Duration", "Crossfade duration in miliseconds",
          0, G_MAXUINT64, 5000, G_PARAM_READWRITE));

  gstelement_class = (GstElementClass *) klass;

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_crossfade_src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_crossfade_sink_template));

  gst_element_class_set_details (gstelement_class, &gst_crossfade_details);

  parent_class = g_type_class_peek_parent (klass);

  gstelement_class->request_new_pad = gst_crossfade_request_new_pad;
  gstelement_class->release_pad = gst_crossfade_release_pad;
}

static void
gst_crossfade_init (GstCrossfade * crossfade)
{
  GstPad *pad, *gpad;

  /* we create the adder element only once */
  crossfade->adder = gst_element_factory_make ("adder", "adder");
  if (!crossfade->adder) {
    GST_WARNING_OBJECT (crossfade, "can't find adder element, crossfade will not work");
    return;
  }

  /* add the adder element */
  if (!gst_bin_add (GST_BIN (crossfade), crossfade->adder)) {
    GST_WARNING_OBJECT (crossfade, "Could not add adder element, crossfade will not work");
    gst_object_unref (crossfade->adder);
    crossfade->adder = NULL;
    return;
  }

  /* get the sinkpad */
  pad = gst_element_get_pad (crossfade->adder, "src");

  /* ghost the sink pad to ourself */
  gpad = gst_ghost_pad_new ("src", pad);
  gst_pad_set_active (gpad, TRUE);
  gst_element_add_pad (GST_ELEMENT (crossfade), gpad);

  gst_object_unref (pad);

  crossfade->padcount = 0;
  crossfade->duration = 5000;
  crossfade->cur_volume = crossfade->old_volume = NULL;
  crossfade->cntrl_xfadeout = crossfade->cntrl_xfadein = NULL;
}

static void
gst_crossfade_dispose (GObject * object)
{
  GstCrossfade *crossfade = GST_CROSSFADE (object);

  if (crossfade->cntrl_xfadeout)
      g_object_unref (crossfade->cntrl_xfadeout);
  if (crossfade->cntrl_xfadein)
      g_object_unref (crossfade->cntrl_xfadein);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gst_crossfade_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GstCrossfade *crossfade;

  crossfade = GST_CROSSFADE (object);

  switch (prop_id) {
    case PROP_DURATION:
      g_value_set_uint64 (value, crossfade->duration);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_crossfade_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GstCrossfade *crossfade;

  crossfade = GST_CROSSFADE (object);

  switch (prop_id) {
    case PROP_DURATION:
      crossfade->duration = g_value_get_uint64 (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstPad *
gst_crossfade_request_new_pad (GstElement * element, GstPadTemplate * templ,
    const gchar * unused)
{
  GstCrossfade *crossfade;
  GstElement *volume;
  gint padcount;
  gchar *name;
  GstPad *pad, *gpad;

  if (templ->direction != GST_PAD_SINK) {
    GST_WARNING_OBJECT (element, "request new pad that is not a SINK pad");
    return NULL;
  }

  crossfade = GST_CROSSFADE (element);

  volume = gst_element_factory_make ("volume", NULL);
  if (!volume) {
    GST_WARNING_OBJECT (crossfade, "Could not create volume element");
    return NULL;
  }

  if (!gst_bin_add (GST_BIN (crossfade), volume)) {
    GST_WARNING_OBJECT (crossfade, "Could not add volume element");
    gst_object_unref (volume);
    return NULL;
  }

  gst_element_sync_state_with_parent (volume);

  if (!gst_element_link (volume, crossfade->adder)) {
    GST_WARNING_OBJECT (crossfade, "Could not link volume element to adder element");
    gst_object_unref (volume);
    return NULL;
  }

  /* increment pad counter */
  padcount = g_atomic_int_exchange_and_add (&crossfade->padcount, 1);
  name = g_strdup_printf ("sink%d", padcount);
  GST_DEBUG_OBJECT (crossfade, "request new pad %s", name);
  /* get the sinkpad */
  pad = gst_element_get_pad (volume, "sink");
  gst_pad_set_link_function (pad, gst_crossfade_volume_pad_link);
  /* ghost the sink pad to ourself */
  gpad = gst_ghost_pad_new (name, pad);
  gst_pad_set_active (gpad, TRUE);
  gst_element_add_pad (GST_ELEMENT (crossfade), gpad);
  g_free (name);
  gst_object_unref (pad);

  return gpad;
}

static void
gst_crossfade_release_pad (GstElement * element, GstPad * pad)
{
  GstCrossfade *crossfade;

  crossfade = GST_CROSSFADE (element);

  GST_DEBUG_OBJECT (crossfade, "release pad %s:%s", GST_DEBUG_PAD_NAME (pad));

  gst_element_remove_pad (element, pad);
}

static GstPadLinkReturn
gst_crossfade_volume_pad_link (GstPad * pad, GstPad * peer)
{
  GstElement *volume;
  GstCrossfade *crossfade;

  volume = GST_ELEMENT (gst_pad_get_parent (pad));
  crossfade = GST_CROSSFADE (gst_element_get_parent (volume));

  GST_DEBUG_OBJECT (crossfade, "Volume pad linked");

  crossfade->old_volume = crossfade->cur_volume;
  crossfade->cur_volume = volume;

  if (crossfade->old_volume) {
    GstFormat format = GST_FORMAT_TIME;
    gint64 position = -1;

    if (crossfade->cntrl_xfadeout) {
      g_object_unref (crossfade->cntrl_xfadeout);
    }
    if (crossfade->cntrl_xfadein) {
      g_object_unref (crossfade->cntrl_xfadein);
    }

    gst_element_query_position (crossfade->old_volume, &format, &position);
    if (position == -1)
        position = 0;
    crossfade->cntrl_xfadeout = gst_crossfade_start_xfade (crossfade->old_volume,
        1.0, 0.0,
        position, crossfade->duration);

    gst_element_query_position (crossfade->cur_volume, &format, &position);
    /* FIXME should we use position here also ? */
    crossfade->cntrl_xfadein = gst_crossfade_start_xfade (crossfade->cur_volume,
        0.0, 1.0,
        0, crossfade->duration);
  }

  gst_object_unref (crossfade);
  gst_object_unref (volume);

  return GST_PAD_LINK_OK;
}

static GstController *
gst_crossfade_start_xfade (GstElement *volume, gdouble start, gdouble end,
    guint64 position, guint64 duration)
{
  GstController *c;
  GstInterpolationControlSource *csource;
  GValue value = {0,};

  c = gst_controller_new (G_OBJECT (volume), "volume", NULL);
  gst_base_transform_set_passthrough (GST_BASE_TRANSFORM (volume), FALSE);

  csource = gst_interpolation_control_source_new ();
  gst_interpolation_control_source_set_interpolation_mode (csource,
      GST_INTERPOLATE_LINEAR);
  gst_controller_set_control_source (c, "volume",
      GST_CONTROL_SOURCE (csource));
  g_object_unref (csource);

  g_value_init (&value, G_TYPE_DOUBLE);
  g_value_set_double (&value, start);
  gst_interpolation_control_source_set (csource,
      position, &value);
  g_value_set_double (&value, end);
  gst_interpolation_control_source_set (csource,
      position + duration * GST_MSECOND, &value);
  g_value_unset (&value);

  return c;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "crossfade",
           GST_RANK_NONE, GST_TYPE_CROSSFADE)) {
    return FALSE;
  }

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "crossfade",
    "Crossfade audio",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
