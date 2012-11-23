/* GSTEngine.cpp */

/* Copyright (C) 2012  Lucio Carreras
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


#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/Equalizer_presets.h"
#include "Engine/Engine.h"
#include "Engine/GStreamer/GSTEngine.h"


#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsrc.h>

#include <string>
#include <vector>
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif


#include <QObject>
#include <QDate>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <qplugin.h>



using namespace std;

static GST_Engine*	obj_ref;


gboolean player_change_file(GstBin* pipeline, void* app){

    qDebug() << "Engine: player change file";
    Q_UNUSED(pipeline);
    Q_UNUSED(app);

    obj_ref->set_about_to_finish();

    return true;
}

static gboolean show_position(GstElement* pipeline){

	gint64 pos;

	GstFormat fmt = GST_FORMAT_TIME;
	gst_element_query_position(pipeline, &fmt, &pos);

	if(obj_ref != NULL && obj_ref->getState() == STATE_PLAY){
        obj_ref->set_cur_position((quint32)(pos / 1000000000)); // ms
	}

	return true;
}

static gboolean bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data){

	(void) bus;
	(void) user_data;

	switch (GST_MESSAGE_TYPE(msg)) {

		case GST_MESSAGE_EOS:
			if(obj_ref){
				obj_ref->set_track_finished();
			}
		break;

		case GST_MESSAGE_ERROR:
			GError *err;

			gst_message_parse_error(msg, &err, NULL);

            qDebug() << "Engine: GST_MESSAGE_ERROR: " << err->message << ": " << GST_MESSAGE_SRC_NAME(msg);
            obj_ref->stop();
			g_error_free(err);

			break;

		default:
			obj_ref->state_changed();
			break;
    }

    return true;
}



/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

GST_Engine::GST_Engine(){

	_name = "GStreamer Backend";
	_state = STATE_STOP;

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;
	_track_finished = true;

	_playing_stream = false;
	_sr_active = false;
	_sr_wanna_record = false;
	_sr_recording_dst = "";

    _gapless_track_available = false;
    _stream_recorder = new StreamRecorder();

    connect(_stream_recorder, SIGNAL(sig_initialized(bool)), this, SLOT(sr_initialized(bool)));
    connect(_stream_recorder, SIGNAL(sig_stream_ended()), this, SLOT(sr_ended()));


}

GST_Engine::~GST_Engine() {
    qDebug() << "Engine: close engine... ";

   if(_bus)
		gst_object_unref(_bus);

	if(_pipeline){
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
		gst_object_unref (GST_OBJECT (_pipeline));
	}
	obj_ref = 0;
}





void GST_Engine::init_play_pipeline(){



	/*
	 * Play pipeline:
	 *
	 * _audio_bin:
	 * =(O _audio_pad )  _equalizer -> _audio_sink
	 *
	 * Is mounted at audio sink of playbin2
	 */

	bool success = false;
	int i;

	i=0;

	// eq -> autoaudiosink is packaged into a bin
	do{
		// create equalizer element
		_pipeline = gst_element_factory_make("playbin2", "player");
		 if(!_pipeline) {
             qDebug() << "Engine: Pipeline sucks";
			 break;
		 }

		 _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
		_equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");
		_audio_sink = gst_element_factory_make("autoaudiosink", "alsasink");
		_audio_bin = gst_bin_new("audio-bin");

		if(!_bus){
            qDebug() << "Engine: Something went wrong with the bus";
			break;
		}

		if(!_equalizer)	{
            qDebug() << "Engine: Equalizer cannot be created";
			break;
		}

		if(!_audio_sink) {
            qDebug() << "Engine: Sink cannot be created";
            break;
		}

		if(!_audio_bin)	{
            qDebug() << "Engine: Bin cannot be created";
			break;
		}


		// create, link and add ghost pad
		gst_bus_add_watch(_bus, bus_state_changed, this);
		gst_bin_add_many(GST_BIN(_audio_bin), _equalizer, _audio_sink, NULL);
		gst_element_link(_equalizer, _audio_sink);
		_audio_pad = gst_element_get_static_pad(_equalizer, "sink");
		if(_audio_pad) {
			success = gst_element_add_pad(GST_ELEMENT(_audio_bin),  gst_ghost_pad_new("sink", _audio_pad));
			if(!success) break;
			g_object_set(G_OBJECT(_pipeline), "audio-sink", _audio_bin, NULL);
		}

       // g_signal_connect (_pipeline, "about-to-finish", G_CALLBACK (player_change_file), NULL);

		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_READY);
	} while(i);
}

void GST_Engine::init(){

	gst_init(0, 0);

    _stream_recorder->init();

	init_play_pipeline();

}





void GST_Engine::psl_gapless_track(const MetaData& md){
    qDebug() << "Engine: Gapless track " << md.title;
    _md_gapless = md;
    _gapless_track_available = true;
}



void GST_Engine::changeTrack(const QString& filepath){
    MetaData md;
    md.filepath = filepath;
    if(!ID3::getMetaDataOfFile(md)){
        stop();
        return;
    }
    changeTrack(md);
}


void GST_Engine::changeTrack(const MetaData& md){


    obj_ref = NULL;

    // Gstreamer needs an URI
    const gchar* uri = NULL;

    // when stream ripper, do not start playing
    bool start_playing = true;

	// Warning!! this order is important!!!
	stop();
	_meta_data = md;

	_playing_stream = false;
	if( md.filepath.startsWith("http") ){
		_playing_stream = true;
	}

	// stream && streamripper active
    if( _playing_stream && _sr_active ){

        int max_tries = 10;

        QString filepath = _stream_recorder->changeTrack(md, max_tries);
        if(filepath.size() == 0) {
            qDebug() << "Engine: Stream Ripper Error: Could not get filepath";
            return;
        }
        else {

            uri = g_filename_to_uri(g_filename_from_utf8(filepath.toUtf8(), filepath.toUtf8().size(), NULL, NULL, NULL), NULL, NULL);
            qDebug() << "Engine: Stream Ripper file = " << filepath;
            qDebug() << "Engine: Stream Ripper file = " << uri;
        }

        start_playing = false;

	}

	// stream, but don't wanna record
	else if(_playing_stream && !_sr_active){
		_playing_stream = true;

       // uri = md.filepath.toLocal8Bit();
        uri = g_filename_from_utf8(md.filepath.toUtf8(), md.filepath.toUtf8().size(), NULL, NULL, NULL);
        qDebug() << "Engine: Stream Ripper file = " << md.filepath;
        qDebug() << "Engine: Stream Ripper file = " << uri;

	}

	// no stream (not quite right because of mms, rtsp or other streams
	else if( !md.filepath.contains("://") ){

         uri = g_filename_to_uri(md.filepath.toLocal8Bit(), NULL, NULL);
	}

	else {
        uri =g_filename_from_utf8(md.filepath.toUtf8(), md.filepath.toUtf8().size(), NULL, NULL, NULL);
	}

    if(uri != NULL){
        // playing src
        qDebug() << "Engine:  set uri: " << uri;
        g_object_set(G_OBJECT(_pipeline), "uri", uri, NULL);

        g_timeout_add (500, (GSourceFunc) show_position, _pipeline);
    }

    emit total_time_changed_signal(_meta_data.length_ms);
    emit timeChangedSignal(0);

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;
	_track_finished = false;
    _gapless_track_available = false;

    if(start_playing)
        play();
}



void GST_Engine::play(){
	_track_finished = false;
	_state = STATE_PLAY;

    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

	obj_ref = this;
}




void GST_Engine::stop(){
	_state = STATE_STOP;


	// streamripper, wanna record is set when record button is pressed
    if( _playing_stream && _sr_active ){
        _stream_recorder->stop(_track_finished, !_sr_wanna_record);
	}

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);

	_track_finished = true;
}


void GST_Engine::pause(){
	_state = STATE_PAUSE;
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}


void GST_Engine::setVolume(int vol){

	float vol_val = (float) (vol * 1.0f / 100.0f);

	g_object_set(G_OBJECT(_pipeline), "volume", vol_val, NULL);

}

void GST_Engine::load_equalizer(vector<EQ_Setting>& vec_eq_settings){

	emit eq_presets_loaded(vec_eq_settings);
}


void GST_Engine::jump(int where, bool percent){

	Q_UNUSED(percent);

	_seconds_started = where * _meta_data.length_ms / 100;

	qint64 new_time_ns = where * _meta_data.length_ms * 10000; // nanoseconds
	if(!gst_element_seek_simple(_pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, new_time_ns)){
        qDebug() << "seeking failed";
	}

	emit timeChangedSignal(where);
}



void GST_Engine::eq_changed(int band, int val){

	double new_val = 0;
	new_val = val * 1.0;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else
		new_val = val * 0.75;

	char band_name[6];
	sprintf(band_name, "band%d", band);
	band_name[5] = '\0';

	g_object_set(G_OBJECT(_equalizer), band_name, new_val, NULL);
}


void GST_Engine::eq_enable(bool){

}


void GST_Engine::state_changed(){

}


void GST_Engine::set_cur_position(quint32 pos){

    if((quint32) _seconds_now == pos) return;
    _seconds_now = pos;

	if (!_scrobbled
			&& (_seconds_now - _seconds_started == 15
					|| _seconds_now - _seconds_started
					== _meta_data.length_ms / 2000)) {

		emit scrobble_track(_meta_data);
		_scrobbled = true;
	}

    emit timeChangedSignal(_seconds_now);
}


void GST_Engine::set_track_finished(){


    qDebug() << "Engine: Track finished";
    if(_sr_active && !_stream_recorder->getFinished()){
        changeTrack(_meta_data);
    }

    if(_sr_active) {
        _stream_recorder->stop(true, !_sr_wanna_record);
    }

	_track_finished = true;
	emit track_finished();
}

void GST_Engine::set_about_to_finish(){

    if(_gapless_track_available){
        _track_finished = true;
        //emit track_finished();

        changeTrack(_md_gapless);
        //_track_finished = true;
        //emit track_finished();
    }
}

int GST_Engine::getState(){
	return _state;
}

QString GST_Engine::getName(){
	return _name;
}


void GST_Engine::record_button_toggled(bool b){
    _sr_wanna_record = b;
}

void GST_Engine::psl_sr_set_active(bool b){
	_sr_active = b;
}

void GST_Engine::sr_initialized(bool b){
    if(b) play();
    //else stop();
}

void GST_Engine::sr_ended(){
    //emit track_finished();
}

void GST_Engine::sr_not_valid(){
    qDebug() << "Engine: Stream not valid.. Next file";
    emit track_finished();
}


Q_EXPORT_PLUGIN2(sayonara_gstreamer, GST_Engine)
