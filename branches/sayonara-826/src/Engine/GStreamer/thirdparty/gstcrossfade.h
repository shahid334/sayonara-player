/* GStreamer
 * Copyright (C) 2008 Andre Moreira Magalhaes <andrunko@gmail.com>
 *
 * gstcrossfade.h: Header for GstCrossfade element
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

#ifndef __GST_CROSSFADE_H__
#define __GST_CROSSFADE_H__

#include <gst/gst.h>
#include <gst/controller/gstcontroller.h>

G_BEGIN_DECLS

#define GST_TYPE_CROSSFADE            (gst_crossfade_get_type())
#define GST_CROSSFADE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CROSSFADE,GstCrossfade))
#define GST_IS_CROSSFADE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CROSSFADE))
#define GST_CROSSFADE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_CROSSFADE,GstCrossfadeClass))
#define GST_IS_CROSSFADE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_CROSSFADE))
#define GST_CROSSFADE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_CROSSFADE,GstCrossfadeClass))

typedef struct _GstCrossfade      GstCrossfade;
typedef struct _GstCrossfadeClass GstCrossfadeClass;

/**
 * GstCrossfade:
 *
 * The crossfade object structure.
 */
struct _GstCrossfade {
  GstBin          parent;

  GstElement     *adder;
  /* pad counter, used for creating unique request pads */
  gint            padcount;
  guint64         duration;

  GstElement     *cur_volume;
  GstElement     *old_volume;

  GstController  *cntrl_xfadeout;
  GstController  *cntrl_xfadein;
};

struct _GstCrossfadeClass {
  GstBinClass parent_class;
};

GType gst_crossfade_get_type (void);

G_END_DECLS

#endif /* __GST_CROSSFADE_H__ */
