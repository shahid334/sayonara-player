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
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif


void EngineCallbacks::new_buffer(GstElement* sink, void* data){

	/*Q_UNUSED(data);

	//ENGINE_DEBUG;

    GstBuffer* buffer;
    g_signal_emit_by_name(sink, "pull-buffer", &buffer);

    if(!buffer) return;
    if(!gst_obj_ref) return;

    MyCaps* my_caps = gst_obj_ref->get_caps();

    if(!my_caps->is_parsed() ) {
		my_caps->parse(gst_buffer_get_caps(buffer));

    }

    float l, r;
    calc_level(buffer, my_caps, &l, &r);
	gst_obj_ref->set_level(l, r);*/
}

double arr[2];
gboolean
EngineCallbacks::level_handler (GstBus * bus, GstMessage * message, gpointer data){

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

	if (strcmp (name, "level")) return TRUE;

	GstClockTime endtime;
	const GValue *array_val;


	if (!gst_structure_get_clock_time (s, "endtime", &endtime))
		qDebug() << "Could not parse endtime";
	/* we can get the number of channels as the length of any of the value
		  * lists */
	array_val = gst_structure_get_value (s, "peak");

	if(!array_val){
		qDebug() << "Cannot get array-val";
		return TRUE;
	}

	rms_arr = (GValueArray*) g_value_get_boxed(array_val);

	guint n_elements = rms_arr->n_values;


	if(n_elements == 0) return TRUE;

	guint max = (n_elements < 2) ? n_elements : 2;
	for(guint i=0; i<max; i++){

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

	if(n_elements >= 2){
		engine->set_level(arr[0], arr[1]);
	}

	else if(n_elements == 1){
		engine->set_level(arr[0], arr[0]);
	}

	return TRUE;

}



gboolean
EngineCallbacks::spectrum_handler (GstBus * bus, GstMessage * message, gpointer data){

    Q_UNUSED(data);

	//ENGINE_DEBUG;
	Engine* engine = (Engine*) data;
	if(!engine) return TRUE;

    const GstStructure *s = gst_message_get_structure (message);
    const gchar *name = gst_structure_get_name (s);

    if(!s) {
        qDebug() << "structure is null";
        return TRUE;
    }

    if (strcmp (name, "spectrum")) return TRUE;


    const GValue *magnitudes;
    const GValue *mag;

    guint i;
    GstClockTime endtime;


    if (!gst_structure_get_clock_time (s, "endtime", &endtime))
        qDebug() << "Could not parse endtime";

    magnitudes = gst_structure_get_value (s, "magnitude");

    QList<float> lst;

    for (i = 0; i < N_BINS; ++i) {

        mag = gst_value_list_get_value (magnitudes, i);
        if (!mag) continue;

        float f;
        f = (g_value_get_float (mag) + crop_spectrum_at) / (crop_spectrum_at * 1.0f);
         lst << f;
    }


	engine->set_spectrum(lst);

    return TRUE;

}


gboolean EngineCallbacks::bus_state_changed(GstBus *bus, GstMessage *msg, gpointer data) {

    (void) bus;

	Engine* engine = (Engine*) data;


    GstState old_state, new_state, pending_state;
	int msg_type = GST_MESSAGE_TYPE(msg);


	switch (msg_type) {

    case GST_MESSAGE_EOS:
		if (!engine) break;

		qDebug() << "Engine: Track finished";
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
		if(!engine) break;

        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);

		if ( new_state == GST_STATE_PLAYING) {
			engine->do_jump_play();
		}

        break;

    case GST_MESSAGE_ASYNC_DONE:

        break;

	case GST_MESSAGE_TAG:
		GstTagList* tags;
		uint val;
		bool success;

		tags = NULL;
		gst_message_parse_tag(msg, &tags);
		success = gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &val);
		if(val != 0 && success){

			val = (uint) (round( val / 1000.0) * 1000.0);

			engine->update_bitrate(val);
		}
		break;


    case GST_MESSAGE_ERROR:
        GError *err;

        gst_message_parse_error(msg, &err, NULL);

        qDebug() << "Engine: GST_MESSAGE_ERROR: " << err->message << ": "
                 << GST_MESSAGE_SRC_NAME(msg);
		if(engine)
			engine->set_track_finished();
        g_error_free(err);

        break;

    default:

		if(engine){
			engine->unmute();
        }
        break;
    }

    return true;
}




#define SCALE_SHORT 0.000000004f
#define BUFFER_SIZE 4096

void EngineCallbacks::calc_level(GstBuffer* buffer, MyCaps* caps, float* l, float* r){

	/*if(!buffer) return;
	if(!caps) return;

	GstMapInfo map_info;

    float f_channel[2];
        f_channel[0] = 0;
        f_channel[1] = 0;


     float scale = 1.0f;

	 gst_buffer_map(buffer, &map_info, GST_MAP_READ);

     // size of one element in bytes
     int item_size = caps->get_width() / 8;
     int n_channels = caps->get_channels();

     // array has sz bytes, but only sz / el_size elements
     // and every channel has it
	 gsize end = map_info.size - item_size;
     gsize start = 0;
     gsize thousand_samples = item_size * n_channels * 512;

	 if(map_info.size > thousand_samples) start = map_info.size - thousand_samples;
	 float inv_arr_channel_elements = (item_size * n_channels * 1.0) / (map_info.size -start);

     int channel=0;
     float *v_f=0;
     short* v_s;
     float v;

     switch(caps->get_type()){

         case CapsTypeFloat:

             for(gsize i=start; i<end; i+=item_size){
				  v_f = (float*) (map_info.data + i);

                  f_channel[channel] += ( (*v_f) * (*v_f));
                  channel = (channel + 1) % n_channels;
              }
             break;

         case CapsTypeInt:

             scale = SCALE_SHORT;

             for(gsize i=start; i<end; i+=item_size){
				 v_s = (short*) (map_info.data + i + i);
                 v = (float) (*v_s);
                 f_channel[channel] += (v * v);
                 channel = (channel + 1) % n_channels;
             }

             break;

         default: break;
     }


     for(int i=0; i<n_channels && i < 2; i++){

         float val = f_channel[i] * inv_arr_channel_elements * scale;
         if(val > 1.0f) val = 1.0f;
         f_channel[i] = 10.0f * LOOKUP_LOG(val);
     }

     if(n_channels >= 2){
         *l = f_channel[0];
         *r = f_channel[1];
     }

     else if(n_channels == 1){
         *l = f_channel[0];
         *r = f_channel[0];
	 }*/

	 gst_buffer_unref(buffer);
}




