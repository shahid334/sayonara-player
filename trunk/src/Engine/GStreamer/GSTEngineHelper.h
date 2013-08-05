#ifndef GSTENGINEHELPER_H
#define GSTENGINEHELPER_H

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <gst/gstelement.h>
#include "HelperStructs/globals.h"

#include <glib.h>

#include <cmath>
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include <QObject>
#include <QDebug>
#include <QString>

#define ENGINE_DEBUG if(_debug) qDebug() << Q_FUNC_INFO

#define CHUNK_SIZE 1024
#define SAMPLE_RATE 44100 /* Samples per second we are sending */
#define AUDIO_CAPS "audio/x-raw-int,channels=2,rate=%d,signed=(boolean)true,width=16,depth=16,endianness=BYTE_ORDER"

#define LOOKUP_LOG(x) log_10[(int) (20000 * x)]

float log_10[20001];
float lo_128[128];
int crop_spectrum_at = 75;

static GST_Engine* gst_obj_ref=NULL;
bool __start_at_beginning = false;
int __start_pos_beginning = 0;
bool _debug = true;


gboolean player_change_file(GstBin* pipeline, void* app) {

    ENGINE_DEBUG;

    Q_UNUSED(pipeline);
    Q_UNUSED(app);

    if(gst_obj_ref)
        gst_obj_ref->set_about_to_finish();

    return true;
}

static void new_buffer(GstElement* sink, void* data){
ENGINE_DEBUG;
    Q_UNUSED(data);

    GstBuffer* buffer;
    g_signal_emit_by_name(sink, "pull-buffer", &buffer);
    if(!buffer) return;

    if(gst_obj_ref)
        gst_obj_ref->set_buffer(buffer);
}



static gboolean
level_handler (GstBus * bus, GstMessage * message, gpointer data){

ENGINE_DEBUG;
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



static gboolean
spectrum_handler (GstBus * bus, GstMessage * message, gpointer data){
ENGINE_DEBUG;
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

    if(gst_obj_ref)
        gst_obj_ref->set_spectrum(lst);

    return TRUE;

}



static gboolean show_position(GstElement* pipeline) {
ENGINE_DEBUG;
    if (!__start_at_beginning)
        return false;

    gint64 pos;

    GstFormat fmt = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &fmt, &pos);

    if (gst_obj_ref && gst_obj_ref->getState() == STATE_PLAY) {


        gst_obj_ref->set_cur_position((quint32)(pos / 1000000000)); // ms
    }

    return true;
}

static gboolean bus_state_changed(GstBus *bus, GstMessage *msg,
                                  void *user_data) {
ENGINE_DEBUG;
    (void) bus;
    (void) user_data;

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

    case GST_MESSAGE_ASYNC_DONE:

        if (__start_at_beginning == false) {
            __start_at_beginning = true;
            if(gst_obj_ref)
                gst_obj_ref->jump(__start_pos_beginning, false);
        }

        break;

    default:
        if(gst_obj_ref)
            gst_obj_ref->state_changed();
        break;
    }

    return true;
}


bool _test_and_error(void* element, QString errorstr){

    if(!element){
        qDebug() << errorstr;
        return false;
    }

    return true;
}

bool _test_and_error_bool(bool b, QString errorstr){
    if(!b){
        qDebug() << errorstr;
        return false;
    }

    return true;
}


#endif // GSTENGINECALLBACKS_H
