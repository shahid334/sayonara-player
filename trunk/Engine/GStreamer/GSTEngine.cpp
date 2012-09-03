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
#include <string>
#include <vector>

#include <QObject>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QDir>
#include <qplugin.h>

using namespace std;

static GST_Engine*	obj_ref;


static gboolean show_position(GstElement* pipeline){

	qint64 pos;

	GstFormat fmt = GST_FORMAT_TIME;
	gst_element_query_position(pipeline, &fmt, &pos);

	if(obj_ref != NULL && obj_ref->getState() == STATE_PLAY){
		obj_ref->set_cur_position((quint32)(pos / 1000000000));
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
			qDebug() << "GST_MESSAGE_ERROR: " << err->message << ": " << GST_MESSAGE_SRC_NAME(msg);

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
	_sr_path = "";
	_sr_complete_tracks = true;
	_sr_create_playlist = true;

}

GST_Engine::~GST_Engine() {

	if(_bus)
		gst_object_unref(_bus);

	if(_pipeline){
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
		gst_object_unref (GST_OBJECT (_pipeline));
	}
	obj_ref = 0;
}



void GST_Engine::init_record_pipeline(){

	int i=0;

	/*
	 * _rec_pipeline:
	 *
	 * _rec_src -> _rec_cvt -> _rec_enc -> _rec_dst
	 *
	 */

	do{
		_rec_pipeline = gst_pipeline_new("rec_pipeline");
		_rec_src = gst_element_factory_make("souphttpsrc", "rec_uri");
		_rec_cvt = gst_element_factory_make("audioconvert", "rec_cvt");
		_rec_enc = gst_element_factory_make("lamemp3enc", "rec_enc");
		_rec_dst = gst_element_factory_make("filesink", "rec_sink");

		if(!_rec_pipeline) {
			qDebug() << "Record: pipeline error";
			break;
		}

		if(!_rec_src) {
			qDebug() << "Record: src error";
			break;
		}

		if(!_rec_cvt) {
			qDebug() << "Record: cvt error";
			break;
		}

		if(!_rec_enc) {
			qDebug() << "Record: enc error";
			break;
		}

		if(!_rec_dst) {
			qDebug() << "Record: sink error";
			break;
		}

		gst_bin_add_many(GST_BIN(_rec_pipeline), _rec_src, /*_rec_cvt, _rec_enc,*/ _rec_dst, NULL);
		gst_element_link( _rec_src, /*_rec_cvt, _rec_enc,*/ _rec_dst);


	} while(i);
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
			 qDebug() << "Pipeline sucks";
			 break;
		 }

		 _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
		_equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");
		_audio_sink = gst_element_factory_make("autoaudiosink", "alsasink");
		_audio_bin = gst_bin_new("audio-bin");

		if(!_bus){
			qDebug() << "Something went wrong with the bus";
			break;
		}

		if(!_equalizer)	{
			qDebug() << "Equalizer cannot be created";
			break;
		}

		if(!_audio_sink) {
			qDebug() << "Sink cannot be created";
			break;
		}

		if(!_audio_bin)	{
			qDebug() << "Bin cannot be created";
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

		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_READY);
	} while(i);
}

void GST_Engine::init(){


	gst_init(0, 0);

	init_record_pipeline();
	init_play_pipeline();

}


QString GST_Engine::init_streamripper(const MetaData& md){

	QString artist = md.artist;
	QString title = md.title;
	QString org_src_filename = md.filepath;	// some url
	QString new_src_filename; 				// file in .Sayonara directory
	QString new_src_filename_uri;			// GStreamer needs an URI

	title.replace(" ", "_");

	_sr_recording_dst = Helper::getSayonaraPath() + title + "." + md.filepath.right(3);
	new_src_filename = _sr_recording_dst;

	// record from org_src_filename to new_src_filename
	g_object_set(G_OBJECT(_rec_src), "location", org_src_filename.toLocal8Bit().data(), NULL);
	g_object_set(G_OBJECT(_rec_dst), "location", new_src_filename.toLocal8Bit().data(), NULL);
	g_object_set(G_OBJECT(_rec_src), "blocksize", 16384, NULL);

	qDebug() << "Streaming from " << org_src_filename << " to " << new_src_filename;

	new_src_filename_uri = QString("file://") + new_src_filename;
	return new_src_filename_uri;
}


void GST_Engine::changeTrack(const MetaData& md){

	// Gstreamer needs an URI
	QString new_src_filename_uri;

	obj_ref = NULL;

	// Warning!! this order is important!!!
	stop();
	_meta_data = md;

	_playing_stream = false;

	if( md.filepath.startsWith("http") ){
		_playing_stream = true;
	}

	// stream && streamripper active
	if(_playing_stream && _sr_active){
		new_src_filename_uri = init_streamripper(md);
	}

	// stream, but don't wanna record
	else if(_playing_stream && !_sr_active){
		_playing_stream = true;
		new_src_filename_uri = md.filepath;
	}

	// no stream (not quite right because of mms, rtsp or other streams
	else if( !md.filepath.contains("://") ){
		new_src_filename_uri = QString("file://") + md.filepath;
	}

	else {
		new_src_filename_uri = md.filepath;
	}

	// playing src
	g_object_set(G_OBJECT(_pipeline), "uri", new_src_filename_uri.toLocal8Bit().data(), NULL);
	g_timeout_add (500, (GSourceFunc) show_position, _pipeline);

	emit total_time_changed_signal(_meta_data.length_ms);

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;
	_track_finished = false;

	play();

}

bool GST_Engine::start_streamripper(){

	bool success = true;
	gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_PLAYING);

	// initially fill the buffer
	qint64 max = 10000000;
	qint64 interval = 100000;
	qint64 sz = 0;



	/*
	 * _sr_recording_dst = .Sayonara/filename
	 */
	QFile* f = new QFile(_sr_recording_dst);

	do{
		sz = f->size();

		usleep(interval);
		max -= interval;
		if(max <= 0) success = false;
	} while(sz < 32000 && max > 0);

	f->close();

	return success;

}

void GST_Engine::play(){
	_track_finished = false;
	_state = STATE_PLAY;

	bool success = true;

	if(_playing_stream && _sr_active){
		success = start_streamripper();
	}

	if(success)
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

	// unable to buffer
	else{
		set_track_finished();
	}
	obj_ref = this;
}



void GST_Engine::stop_streamripper(){

	if(!_track_finished && _sr_complete_tracks) return;

	//  _sr_recording_dst = .Sayonara/filename
	QFile f(_sr_recording_dst);

	// Final path where track is saved
	QDir dir(_sr_path);
		dir.mkdir(_meta_data.artist);
		dir.cd(_meta_data.artist);

	// remove directories in front of filename
	QString src_name = f.fileName();
	QString fname_wo_path = src_name.right( src_name.size() - src_name.lastIndexOf(QDir::separator()) );
	QString dst_name = 	dir.path() + QDir::separator() + fname_wo_path;

	bool success = 	f.copy(dst_name);
	if(!success){
		qDebug() << "unable to copy " <<  _sr_recording_dst << " to " << dir.path() + QDir::separator() + fname_wo_path;
	}

	else{
		_meta_data.filepath = dst_name;
		ID3::setMetaDataOfFile(_meta_data);
		if(_sr_create_playlist){
			emit sig_valid_strrec_track(_meta_data);
		}
	}

	f.remove();

}

void GST_Engine::stop(){
	_state = STATE_STOP;

	// streamripper, wanna record is set when record button is pressed
	if( _playing_stream && _sr_wanna_record ){
		stop_streamripper();
	}

	else if( _playing_stream && ! _sr_wanna_record){
		QFile f(_sr_recording_dst);
		f.remove();
	}

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
	gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_NULL);

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
}


void GST_Engine::changeTrack(const QString& filepath){

	MetaData md = ID3::getMetaDataOfFile(filepath);
	changeTrack(md);
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

	if((quint32)_seconds_now == pos) return;
	_seconds_now = pos;

	if (!_scrobbled
			&& (_seconds_now - _seconds_started == 15
					|| _seconds_now - _seconds_started
					== _meta_data.length_ms / 2000)) {

		emit scrobble_track(_meta_data);
		_scrobbled = true;
	}

	emit timeChangedSignal(pos);
}
void GST_Engine::set_track_finished(){

	_track_finished = true;
	emit track_finished();
}

int GST_Engine::getState(){
	return _state;
}

QString GST_Engine::getName(){
	return _name;
}


void GST_Engine::record_button_toggled(bool b){
	_sr_wanna_record = (_sr_active && b);
}

void GST_Engine::psl_strrip_set_active(bool b){
	_sr_active = b;
}

void GST_Engine::psl_strrip_set_path(const QString& path){
	_sr_path = path;
}

void GST_Engine::psl_strrip_complete_tracks(bool b){
	_sr_complete_tracks = b;
}
void GST_Engine::psl_strrip_set_create_playlist(bool b){
	_sr_create_playlist = b;
}



Q_EXPORT_PLUGIN2(sayonara_gstreamer, GST_Engine)
