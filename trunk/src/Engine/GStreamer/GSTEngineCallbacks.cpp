#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <gst/gstelement.h>
#include "HelperStructs/globals.h"
#include "Engine/GStreamer/GSTEngine.h"
#include "Engine/GStreamer/GSTEngineHelper.h"

#include <cmath>
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

void calc_level(GstBuffer* buffer, MyCaps* caps, float* l, float* r);

void new_buffer(GstElement* sink, void* data){

    Q_UNUSED(data);

    ENGINE_DEBUG;

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
    gst_obj_ref->set_level(l, r);
}


gboolean
level_handler (GstBus * bus, GstMessage * message, gpointer data){

    ENGINE_DEBUG;
    if(!gst_obj_ref) return TRUE;

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
    array_val = gst_structure_get_value (s, "rms");

    if(!array_val){
        qDebug() << "Cannot get array-val";
        return TRUE;
    }

    guint n_elements = gst_value_list_get_size(array_val);

    if(n_elements == 0) return TRUE;

    double* arr = new double[n_elements];
    for(guint i=0; i<n_elements; i++){
        const GValue* val = gst_value_list_get_value(array_val, i);
        if(!G_VALUE_HOLDS_DOUBLE(val)) {
            qDebug() << "Could not find a double";
            break;
        }

        arr[i] = g_value_get_double(val);
    }

    guint64 dur;
    gst_structure_get_clock_time(s, "timestamp", &dur);

    if(n_elements >= 2 && gst_obj_ref){
        gst_obj_ref->set_level(arr[0], arr[1]);
    }

    else if(n_elements == 1 && gst_obj_ref){
        gst_obj_ref->set_level(arr[0], arr[0]);
    }

    delete arr;

    return TRUE;

}



gboolean
spectrum_handler (GstBus * bus, GstMessage * message, gpointer data){

    Q_UNUSED(data);

    ENGINE_DEBUG;

    if(!gst_obj_ref) return TRUE;

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


        gst_obj_ref->set_spectrum(lst);

    return TRUE;

}

gboolean show_position(GstElement* pipeline) {

    ENGINE_DEBUG;

    gint64 pos;

    GstFormat fmt = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &fmt, &pos);

    if (gst_obj_ref && gst_obj_ref->getState() == STATE_PLAY) {
        gst_obj_ref->set_cur_position((quint32)(pos / MRD));
    }

    return true;
}

gboolean bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data) {

    (void) bus;
    (void) user_data;


    GstState old_state, new_state, pending_state;
    switch (GST_MESSAGE_TYPE(msg)) {

    case GST_MESSAGE_EOS:
        if (gst_obj_ref) {
            qDebug() << "Engine: Track finished";
            gst_obj_ref->set_track_finished();
        }
        break;

    case GST_MESSAGE_ERROR:
        GError *err;

        gst_message_parse_error(msg, &err, NULL);

        qDebug() << "Engine: GST_MESSAGE_ERROR: " << err->message << ": "
                 << GST_MESSAGE_SRC_NAME(msg);
        if(gst_obj_ref)
            gst_obj_ref->set_track_finished();
        g_error_free(err);

        break;

    case GST_MESSAGE_ELEMENT:
        if(!gst_obj_ref) break;

        if(gst_obj_ref->get_show_spectrum())
            return spectrum_handler(bus, msg, user_data);
        if(gst_obj_ref->get_show_level())
            return level_handler(bus, msg, user_data);

        break;

    case GST_MESSAGE_STATE_CHANGED:
        if(!gst_obj_ref) break;

        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);

        if ( new_state == GST_STATE_PLAYING) {
            gst_obj_ref->do_jump_play();
        }

        break;

    default:

        if(gst_obj_ref)
            gst_obj_ref->state_changed();
        break;
    }

    return true;
}




#define SCALE_SHORT 0.000000004f
#define BUFFER_SIZE 4096

void calc_level(GstBuffer* buffer, MyCaps* caps, float* l, float* r){

    float f_channel[2];
        f_channel[0] = 0;
        f_channel[1] = 0;

     //guint64 dur = GST_BUFFER_DURATION(buffer);
     gsize sz = GST_BUFFER_SIZE(buffer);
     guint8* c_buf = GST_BUFFER_DATA(buffer);
     float scale = 1.0f;

     // size of one element in bytes
     int item_size = caps->get_width() / 8;
     int n_channels = caps->get_channels();

     // array has sz bytes, but only sz / el_size elements
     // and every channel has it
     gsize end = sz - item_size;
     gsize start = 0;
     gsize thousand_samples = item_size * n_channels * 512;

     if(sz > thousand_samples) start = sz - thousand_samples;
     float inv_arr_channel_elements = (item_size * n_channels * 1.0) / (sz -start);

     int channel=0;
     float *v_f=0;
     short* v_s;
     float v;

     switch(caps->get_type()){

         case CapsTypeFloat:

             for(gsize i=start; i<end; i+=item_size){
                  v_f = (float*) (c_buf+i);

                  f_channel[channel] += ( (*v_f) * (*v_f));
                  channel = (channel + 1) % n_channels;
              }
             break;

         case CapsTypeInt:

             scale = SCALE_SHORT;

             for(gsize i=start; i<end; i+=item_size){
                 v_s = (short*) (c_buf + i);
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
     }

     gst_buffer_unref(buffer);
}




