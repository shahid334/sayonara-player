/* GSTPipeline.cpp */

/* Copyright (C) 2013  Lucio Carreras
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



#include "GSTPipeline.h"
#include "GSTEngineHelper.h"
#include "HelperStructs/globals.h"
#include "gst/interfaces/streamvolume.h"
#include "unistd.h"
#include <QDebug>



GSTPipeline::GSTPipeline(QObject *parent) :
    QObject(parent)
{

        bool success = false;
        int i = 0;
        _pipeline = 0;

        // eq -> autoaudiosink is packaged into a bin
        do {
            // create equalizer element
            GstElement* tmp_pipeline = gst_element_factory_make("playbin2", "player");
            _test_and_error(tmp_pipeline, "Engine: Pipeline sucks");


            _bus = gst_pipeline_get_bus(GST_PIPELINE(tmp_pipeline));
            _audio_bin = gst_bin_new("audio-bin");

            _equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");
            _volume = gst_element_factory_make("volume", "volume");

            _level = gst_element_factory_make("level", "level");
            _spectrum = gst_element_factory_make("spectrum", "spectrum");

            _level_audio_convert = gst_element_factory_make("audioconvert", "level_convert");
            _spectrum_audio_convert = gst_element_factory_make("audioconvert", "spectrum_convert");

            _audio_sink = gst_element_factory_make("autoaudiosink", "autoaudiosink");

            _eq_queue = gst_element_factory_make("queue", "eq_queue");
            _tee = gst_element_factory_make("tee", "tee");
            _level_queue = gst_element_factory_make("queue", "level_queue");
            _spectrum_queue = gst_element_factory_make("queue", "spectrum_queue");

            _level_sink = gst_element_factory_make("appsink", "level_sink");
            _spectrum_sink = gst_element_factory_make("fakesink", "spectrum_sink");


            if(!_test_and_error(_bus, "Engine: Something went wrong with the bus")) break;
            //if(!_test_and_error(_gio_src, "Engine: Giosrc fail")) break;
            if(!_test_and_error(_level_audio_convert, "Engine: Level converter fail")) break;
            if(!_test_and_error(_spectrum_audio_convert, "Engine: Spectrum converter fail")) break;
            if(!_test_and_error(_level, "Engine: Level cannot be created")) break;
            if(!_test_and_error(_spectrum, "Engine: Spectrum cannot be created")) break;
            if(!_test_and_error(_audio_bin, "Engine: Bin cannot be created")) break;
            if(!_test_and_error(_tee, "Engine: Tee cannot be created")) break;
            if(!_test_and_error(_equalizer, "Engine: Equalizer cannot be created")) break;
            if(!_test_and_error(_eq_queue, "Engine: Equalizer cannot be created")) break;
            if(!_test_and_error(_audio_sink, "Engine: Audio Sink cannot be created")) break;
            if(!_test_and_error(_level_queue, "Engine: Queue cannot be created")) break;
            if(!_test_and_error(_spectrum_queue, "Engine: Queue cannot be created")) break;



            //gst_bus_add_watch(_bus, level_handler, this);

            // create a bin that includes an equalizer and replace the sink with this bin
            gst_bin_add_many(GST_BIN(_audio_bin), _tee,
                             _eq_queue, _equalizer, _volume, _audio_sink,
                             _level_queue, _level_audio_convert, _level, _level_sink,
                             _spectrum_queue, _spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);


            success = gst_element_link_many(_level_queue, _level_sink, NULL);
            _test_and_error_bool(success, "Engine: Cannot link Level pipeline");

            success = gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
            _test_and_error_bool(success, "Engine: Cannot link Spectrum pipeline");

            success = gst_element_link_many(_eq_queue, _volume, _equalizer, _audio_sink, NULL);
            if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) break;

            // Connect tee

            GstPad* tee_pad;
            GstPad* tee_eq_pad;
            GstPad* eq_pad;

            GstPadLinkReturn s;


            // create tee pads
            _tee_src_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(_tee), "src%d");

            // "outputs" of tee to eq and queue

            tee_eq_pad = gst_element_request_pad(_tee, _tee_src_pad_template, NULL, NULL);
                if(!_test_and_error(tee_eq_pad, "Engine: tee_eq_pad is NULL")) break;
            eq_pad = gst_element_get_static_pad(_eq_queue, "sink");
                if(!_test_and_error(eq_pad, "Engine: eq pad is NULL")) break;

            _tee_level_pad = gst_element_request_pad(_tee, _tee_src_pad_template, NULL, NULL);
                if(!_test_and_error(_tee_level_pad, "Engine: Tee-Level Pad NULL")) break;
            _level_pad = gst_element_get_static_pad(_level_queue, "sink");
                if(!_test_and_error(_level_pad, "Engine: Level Pad NULL")) break;

            _tee_spectrum_pad = gst_element_request_pad(_tee, _tee_src_pad_template, NULL, NULL);
                if(!_test_and_error(_tee_spectrum_pad, "Engine: Tee-Spectrum Pad NULL")) break;
            _spectrum_pad = gst_element_get_static_pad(_spectrum_queue, "sink");
                if(!_test_and_error(_spectrum_pad, "Engine: Spectrum Pad NULL")) break;

            s = gst_pad_link (tee_eq_pad, eq_pad);
                _test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee eq with eq");
            s = gst_pad_link (_tee_level_pad, _level_pad);
                _test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee with level");
            s = gst_pad_link (_tee_spectrum_pad, _spectrum_pad);
                _test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee with spectrum");


            // "input" of tee pad
            tee_pad = gst_element_get_static_pad(_tee, "sink");

            if(!_test_and_error(tee_pad, "Engine: Cannot create tee pad")) break;

            // tell the sink_bin, that the input of tee = input of sink_bin
            success = gst_element_add_pad(GST_ELEMENT(_audio_bin), gst_ghost_pad_new("sink", tee_pad));
            _test_and_error_bool(success, "Engine: cannot add pad to audio bin");

            // replace playbin sink with this bin
            g_object_set(G_OBJECT(tmp_pipeline), "audio-sink", _audio_bin, NULL);


            g_object_set (_eq_queue,
                          "silent", TRUE,
                          NULL);

            g_object_set (_level_queue,
                          "silent", TRUE,
                          NULL);

            g_object_set(_spectrum_queue,
                         "silent", TRUE,
                         NULL);



            guint64 interval = 30000000;
            gint threshold = - crop_spectrum_at;


    /*****/
            GstCaps* audio_caps = gst_caps_from_string (AUDIO_CAPS);
            g_object_set (_level_sink,
                          "drop", TRUE,
                          "max-buffers", 1,
                          "caps", audio_caps,
                          "emit-signals", FALSE,
                          NULL);

           g_signal_connect (_level_sink, "new-buffer", G_CALLBACK (new_buffer), NULL);
    /*****/


            g_object_set (G_OBJECT (_level),
                          "message", TRUE,
                          "interval", interval / 10,
                          NULL);

            g_object_set (G_OBJECT (_spectrum),
                          "interval", interval,
                          "bands", N_BINS,
                          "threshold", threshold,
                          "message", TRUE,
                          "message-phase", FALSE,
                          "message-magnitude", TRUE,
                          NULL);


             /* run synced and not as fast as we can */
            g_object_set (G_OBJECT (_level_sink), "sync", TRUE, NULL);
            g_object_set (G_OBJECT (_level_sink), "async", FALSE, NULL);

            g_object_set (G_OBJECT (_spectrum_sink), "sync", TRUE, NULL);
            g_object_set (G_OBJECT (_spectrum_sink), "async", FALSE, NULL);

            gst_pad_set_active(_level_pad, false);
            gst_pad_set_active(_level_pad, true);


            gst_element_set_state(GST_ELEMENT(tmp_pipeline), GST_STATE_READY);

            success = true;
            _pipeline = tmp_pipeline;

            break;
        } while (i);


        if(success)gst_bus_add_watch(_bus, bus_state_changed, this);

        qDebug() << "Engine: constructor finished: " << success;
    }


GSTPipeline::~GSTPipeline(){
    if (_bus)
        gst_object_unref (_bus);

    if (_pipeline) {
        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
        gst_object_unref (GST_OBJECT(_pipeline));}
}

GstBus* GSTPipeline::get_bus(){
    return _bus;
}

GstElement* GSTPipeline::get_pipeline(){
    return _pipeline;
}


void GSTPipeline::play(){
    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

}

void GSTPipeline::pause(){
    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}


void GSTPipeline::stop(){
    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
}

void GSTPipeline::set_volume(int vol){

    _vol = vol;

    float vol_val = (float) (vol * 1.0f / 100.0f);
   g_object_set(G_OBJECT(_volume), "volume", vol_val, NULL);

}

void GSTPipeline::unmute(){

    g_object_set(G_OBJECT(_volume), "mute", FALSE, NULL);
}

gint64 GSTPipeline::seek_rel(float percent, gint64 ref_ns){

    gint64 new_time_ns;

     g_object_set(G_OBJECT(_volume), "mute", TRUE, NULL);

    if (percent > 1.0f)
        new_time_ns = ref_ns;

    else if(percent < 0)
        new_time_ns = 0;

    else{
        new_time_ns = (percent * ref_ns); // nsecs
    }

    if(gst_element_seek_simple(_pipeline,
        GST_FORMAT_TIME,
        (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
        new_time_ns)){

        return new_time_ns;
    }


    return 0;
}


gint64 GSTPipeline::seek_abs(gint64 ns){

    g_object_set(G_OBJECT(_volume), "mute", TRUE, NULL);

    if(gst_element_seek_simple(_pipeline,
        GST_FORMAT_TIME,
        (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
        ns)){

        return ns;
    }



    return 0;
}

void GSTPipeline::enable_level(bool b){

    g_object_set(_level_sink,
        "emit-signals", b,
        NULL);

    return;


    //gst_pad_set_active(_tee_level_pad, b);
    this->pause();
    if(!b){
        gst_element_unlink_many(_level_queue, _level_audio_convert, _level, _level_sink, NULL);
        gst_element_link_many(_level_queue, _level_sink, NULL);
    }
    else{
        gst_element_unlink_many(_level_queue, _level_sink, NULL);
        gst_element_link_many(_level_queue, _level_audio_convert, _level, _level_sink, NULL);

    }

}

void GSTPipeline::enable_spectrum(bool b){
    this->pause();
    if(!b){
        gst_element_unlink_many(_spectrum_queue, _spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);
        gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
    }
    else{
        gst_element_unlink_many(_spectrum_queue, _spectrum_sink, NULL);
        gst_element_link_many(_spectrum_queue,_spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);

    }
}


gint64 GSTPipeline::get_duration_ns(){

    GstFormat format=GST_FORMAT_TIME;
    gint64 duration=0;
    bool success = false;
    success = gst_element_query_duration(_pipeline, &format, &duration);
    if(!success) return -1;
    else return duration;
}


guint GSTPipeline::get_bitrate(){


    GstTagList *tags=NULL;
    guint rate=0;

    bool success = false;
    g_signal_emit_by_name(_pipeline, "get-audio-tags", 0, &tags);
    if(tags){

        success = gst_tag_list_get_uint (tags, GST_TAG_BITRATE, &rate);
        qDebug() << "tags there, bitrate = " << rate;
        if(success){
            return rate;
        }
    }

    return 0;
}

bool GSTPipeline::set_uri(gchar* uri){
    if(!uri) return false;
    g_object_set(G_OBJECT(_pipeline), "uri", uri, NULL);
    qDebug() << "Engine:  set uri: " << uri;
    return true;
}

void GSTPipeline::set_eq_band(QString band_name, double val){

    g_object_set(G_OBJECT(_equalizer), band_name.toLocal8Bit(), val, NULL);
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

