/* GSTEngineCallbacks.cpp */

/* Copyright (C) 2011-2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <gst/gstelement.h>
#include "HelperStructs/globals.h"
#include "Engine/GStreamer/GSTPlaybackEngine.h"
#include "Engine/GStreamer/GSTEngineHelper.h"

#include <cmath>

double arr[2];
gboolean
EngineCallbacks::level_handler (GstBus * bus, GstMessage * message, gpointer data) {

	GValueArray* rms_arr;
	Engine* engine = (Engine*) data;
   // ENGINE_DEBUG;
	if(!engine) return TRUE;

	Q_UNUSED(bus);

	const GstStructure *s = gst_message_get_structure (message);
	const gchar *name = gst_structure_get_name (s);

	if(!s) {
		qDebug() << "structure is null";
		return TRUE;
	}

    if ( strcmp(name, "level") != 0 ) {
        return TRUE;
    }

	GstClockTime endtime;
	const GValue *array_val;


    if ( gst_structure_get_clock_time(s, "endtime", &endtime) != true ){
		qDebug() << "Could not parse endtime";
    }

    array_val = gst_structure_get_value (s, "peak");

	if(!array_val) {
		return TRUE;
	}

    guint n_elements;

    rms_arr = (GValueArray*) g_value_get_boxed(array_val);
    n_elements = rms_arr->n_values;

    if(n_elements == 0) {
        return TRUE;
    }

	guint max = (n_elements < 2) ? n_elements : 2;
	for(guint i=0; i<max; i++) {

		const GValue* val = rms_arr->values + i;
		double d;

		if(!G_VALUE_HOLDS_DOUBLE(val)) {
			qDebug() << "Could not find a double";
			break;
		}

		d = g_value_get_double(val);
		if(d < 0)
			arr[i] = d;

	}

	guint64 dur;
	gst_structure_get_clock_time(s, "timestamp", &dur);

	if(n_elements >= 2) {
		engine->set_level(arr[0], arr[1]);
	}

	else if(n_elements == 1) {
		engine->set_level(arr[0], arr[0]);
	}

	return TRUE;
}


gboolean
EngineCallbacks::spectrum_handler (GstBus * bus, GstMessage * message, gpointer data) {

    Q_UNUSED(data);

	Engine* engine = (Engine*) data;
	if(!engine) return TRUE;

    const GstStructure *s = gst_message_get_structure (message);
    const gchar *name = gst_structure_get_name (s);
    const GValue *magnitudes;
    const GValue *mag;
    QList<float> lst;

    guint i;
    GstClockTime endtime;

    if(!s) {
        return TRUE;
    }

    if ( strcmp(name, "spectrum") != 0 ) {
        return TRUE;
    }


    if ( gst_structure_get_clock_time (s, "endtime", &endtime) != true ){
        qDebug() << "Could not parse endtime";
    }

    magnitudes = gst_structure_get_value (s, "magnitude");

    for (i = 0; i < N_BINS; ++i) {

        float f;

        mag = gst_value_list_get_value (magnitudes, i);
        if (!mag) continue;

        f = (g_value_get_float (mag) + crop_spectrum_at) / (crop_spectrum_at * 1.0f);
        lst << f;
    }

	engine->set_spectrum(lst);

    return TRUE;
}


gboolean EngineCallbacks::bus_state_changed(GstBus *bus, GstMessage *msg, gpointer data) {

    (void) bus;

	Engine* engine = (Engine*) data;

	int msg_type = GST_MESSAGE_TYPE(msg);

	switch (msg_type) {

        case GST_MESSAGE_EOS:
            if (!engine) break;

            engine->set_track_finished();

            break;

        case GST_MESSAGE_ELEMENT:
            if(!engine) break;

            if(engine->get_show_spectrum())
                return spectrum_handler(bus, msg, engine);

            if(engine->get_show_level())
                return level_handler(bus, msg, engine);

            break;

        case GST_MESSAGE_STATE_CHANGED:
			break;


        case GST_MESSAGE_ASYNC_DONE:
            break;

        case GST_MESSAGE_TAG:
            GstTagList* tags;
			quint32 br;

            bool success;

            tags = NULL;
            gst_message_parse_tag(msg, &tags);
			success = gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &br);

			if(br != 0 && success) {

				br = (quint32) (br / 1000) * 1000;
				engine->update_bitrate(br);
            }

			gst_tag_list_unref(tags);

			break;

		case GST_MESSAGE_DURATION_CHANGED:
			engine->update_duration();
			break;


        case GST_MESSAGE_ERROR:
            GError *err;

            gst_message_parse_error(msg, &err, NULL);

            qDebug() << "Engine: GST_MESSAGE_ERROR: " << err->message << ": "
                     << GST_MESSAGE_SRC_NAME(msg);

			if(engine){
				engine->set_track_finished();
			}

            g_error_free(err);

            break;

        default:

            if(engine) {
                engine->unmute();
            }
            break;
    }

    return true;
}
