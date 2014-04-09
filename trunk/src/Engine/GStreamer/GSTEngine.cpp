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
#include "HelperStructs/globals.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Equalizer_presets.h"
#include "Engine/Engine.h"
#include "Engine/GStreamer/GSTEngine.h"
#include "Engine/GStreamer/GSTEngineHelper.h"
#include "Engine/GStreamer/GSTPipelineExperimental.h"

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <glib.h>
#include <cmath>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include <QDebug>
#include <qplugin.h>


bool _debug = false;

float log_10[20001];
float lo_128[128];

GST_Engine* gst_obj_ref = 0;


/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

void _calc_log10_lut(){
	for(int i=0; i<=20000; i++){
		log_10[i] = log10(i / 20000.0f);
	}

	for(int i=0; i<128; i++){
		lo_128[i] = i*128.0f;
	}
}

GST_Engine::GST_Engine() {

	ENGINE_DEBUG;
	_calc_log10_lut();

	_caps = new MyCaps();
	_caps->set_parsed(false);

	_settings = CSettingsStorage::getInstance();
	_name = "GStreamer Backend";
	_state = STATE_STOP;

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;
	_playing_stream = false;
	_sr_active = false;
	_sr_wanna_record = false;

	_stream_recorder = new StreamRecorder();
	_last_track = _settings->getLastTrack();

	_wait_for_gapless_track = false;
	_may_start_timer = false;

	connect(_stream_recorder, SIGNAL(sig_initialized(bool)), this, SLOT(sr_initialized(bool)));
	connect(_stream_recorder, SIGNAL(sig_stream_ended()), this,
			SLOT(sr_ended()));
	connect(_stream_recorder, SIGNAL(sig_stream_not_valid()), this,
			SLOT(sr_not_valid()));
}


GST_Engine::~GST_Engine() {

	ENGINE_DEBUG;

	qDebug() << "Engine: close engine... ";

	_settings->updateLastTrack();
	delete _pipeline;

	gst_obj_ref = 0;
}


void GST_Engine::init() {

	ENGINE_DEBUG;

	gst_init(0, 0);

	_stream_recorder->init();

	_pipeline = new GSTPipelineExperimental();
	_pipeline->set_gapless(false);
	_other_pipeline = NULL;

	_show_level = false;
	_show_spectrum = false;

	psl_set_gapless(false);
}


void GST_Engine::changeTrack(const QString& filepath, int pos_sec, bool start_play) {

	ENGINE_DEBUG << filepath << ", " << pos_sec << ",  " << start_play;

	MetaData md;
	md.filepath = filepath;
	if (!ID3::getMetaDataOfFile(md)) {
		stop();
		return;
	}

	changeTrack(md, pos_sec, start_play);
}

void GST_Engine::changeTrackGapless(const MetaData& md, int pos_sec, bool start_play){


	start_play = true;
	pos_sec = 0;
	bool success = set_uri(md, &start_play);

	if (!success)
		return;

	gst_obj_ref = this;

	_meta_data = md;

	_caps->set_parsed(false);

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;
	_jump_play = 0;


}


void GST_Engine::changeTrack(const MetaData& md, int pos_sec, bool start_play) {

	ENGINE_DEBUG << md.filepath << ", " << pos_sec << ",  " << start_play;

	if(_wait_for_gapless_track) {

		ENGINE_DEBUG << "change track gapless";
		changeTrackGapless(md, pos_sec, start_play);
		_wait_for_gapless_track = false;
		return;
	}

	stop();

	bool success = set_uri(md, &start_play);
	if (!success)
		return;

	gst_obj_ref = this;

	_meta_data = md;

	_caps->set_parsed(false);

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;
	_jump_play = pos_sec;

	emit total_time_changed_signal(_meta_data.length_ms);
	emit timeChangedSignal(pos_sec);

	if (start_play){
		play();
	}

	// pause if streamripper is not active
	else if (!start_play && !(_playing_stream && _sr_active))
		pause();
}



bool GST_Engine::set_uri(const MetaData& md, bool* start_play) {

	// Gstreamer needs an URI
	gchar* uri = NULL;

	_playing_stream = Helper::is_www(md.filepath);

	// stream && streamripper active
	if (_playing_stream && _sr_active) {

		int max_tries = 10;

		QString filepath = _stream_recorder->changeTrack(md, max_tries);
		if (filepath.size() == 0) {
			qDebug() << "Engine: Stream Ripper Error: Could not get filepath";
			return false;
		}

		// lfm stuff is no uri?
		if (md.radio_mode == RADIO_LFM) {
			uri = g_filename_to_uri(
					g_filename_from_utf8(filepath.toUtf8(),
							filepath.toUtf8().size(), NULL, NULL, NULL), NULL,
							NULL);
		}

		// standard stream is already uri
		else {
			uri = g_filename_from_utf8(md.filepath.toUtf8(),
					md.filepath.toUtf8().size(), NULL, NULL, NULL);
		}

		*start_play = false;
	}

	// stream, but don't wanna record
	// stream is already uri
	else if (_playing_stream && !_sr_active) {
		uri = g_filename_from_utf8(md.filepath.toUtf8(),
				md.filepath.toUtf8().size(), NULL, NULL, NULL);
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!md.filepath.contains("://")) {

		uri = g_filename_to_uri(md.filepath.toLocal8Bit(), NULL, NULL);
	}

	else {
		uri = g_filename_from_utf8(md.filepath.toUtf8(),
				md.filepath.toUtf8().size(), NULL, NULL, NULL);
	}

	ENGINE_DEBUG << " uri = " << uri;

	bool success = false;

	if(_wait_for_gapless_track) {

		ENGINE_DEBUG << "Set Uri next pipeline: " << uri;
		if(_other_pipeline)
			success = _other_pipeline->set_uri(uri);
	}

	if(!success) {
		ENGINE_DEBUG << "Set Uri current pipeline: " << uri;
		success = _pipeline->set_uri(uri);
	}

	return success;
}

void GST_Engine::play() {

	ENGINE_DEBUG;

	gst_obj_ref = this;

	_state = STATE_PLAY;
	_pipeline->play();
	_may_start_timer = _pipeline->get_gapless();

	g_timeout_add( 100, (GSourceFunc) show_position, _pipeline->get_pipeline() );
}

void GST_Engine::do_jump_play(){

	if(_playing_stream) {
		_jump_play = -1;
		return;
	}
	if(_jump_play < 0) return;
	ENGINE_DEBUG;

	this->jump(_jump_play, false);
	_jump_play = -1;
}


void GST_Engine::stop() {

	ENGINE_DEBUG;

	_state = STATE_STOP;

	// streamripper, wanna record is set when record button is pressed
	if (_playing_stream && _sr_active) {
		qDebug() << "Engine: stop... Playing stream";
		_stream_recorder->stop(!_sr_wanna_record);
	}

	_pipeline->stop();

	if(_other_pipeline)
		_other_pipeline->stop();

	emit timeChangedSignal(0);
}


void GST_Engine::pause() {
	ENGINE_DEBUG;
	_state = STATE_PAUSE;

	_pipeline->pause();
}

void GST_Engine::setVolume(int vol) {
	ENGINE_DEBUG;
	_vol = vol;
	_pipeline->set_volume(vol);

	if(_other_pipeline)
		_other_pipeline->set_volume(vol);
}

void GST_Engine::load_equalizer(vector<EQ_Setting>& vec_eq_settings) {
ENGINE_DEBUG;
	emit eq_presets_loaded(vec_eq_settings);
}

void GST_Engine::jump(int where, bool percent) {
ENGINE_DEBUG;
	gint64 new_time_ns;
	if (where < 0)
		where = 0;

	if (percent) {
		float p = where / 100.0f;
		new_time_ns = _pipeline->seek_rel(p, _meta_data.length_ms * 1000000);
	}

	else {
		new_time_ns = _pipeline->seek_abs(where * MRD);
	}

	_seconds_started = new_time_ns / MRD;
}

void GST_Engine::eq_changed(int band, int val) {
ENGINE_DEBUG;
	double new_val = 0;
	new_val = val * 1.0;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else
		new_val = val * 0.75;

	QString band_name = QString("band") + QString::number(band);
	_pipeline->set_eq_band(band_name, new_val);

	if(_other_pipeline)
		_other_pipeline->set_eq_band(band_name, new_val);
}

void GST_Engine::eq_enable(bool) {
ENGINE_DEBUG;
}

void GST_Engine::psl_calc_level(bool b){
	ENGINE_DEBUG;

	_show_level = b;
	if(b) _show_spectrum = false;

	_pipeline->enable_level(b);

	if(_other_pipeline)
		_other_pipeline->enable_level(b);

	if(_state == STATE_PLAY)
		_pipeline->play();
}


void GST_Engine::psl_calc_spectrum(bool b){
	ENGINE_DEBUG;
	_show_spectrum = b;
	if(b) _show_level = false;

	_pipeline->enable_spectrum(b);

	if(_other_pipeline)
		_other_pipeline->enable_spectrum(b);

	if(_state == STATE_PLAY)
		_pipeline->play();
}


bool GST_Engine::get_show_level(){
	return _show_level;
}

bool GST_Engine::get_show_spectrum(){
	return _show_spectrum;
}

void GST_Engine::state_changed() {}

MyCaps* GST_Engine::get_caps(){
	return _caps;
}

void GST_Engine::update_bitrate(qint32 bitrate){

	_meta_data.bitrate = bitrate;
	emit sig_bitrate_changed(bitrate);
}


void GST_Engine::set_cur_position_ms(quint64 pos_ms) {

	ENGINE_DEBUG << pos_ms;
	quint32 pos_sec = pos_ms / 1000;
	gint64 duration_ns = _pipeline->get_duration_ns();

	if(_meta_data.length_ms == 0 || _meta_data.bitrate == 0){

		guint bitrate = _pipeline->get_bitrate();

		if(duration_ns > 0)
			_meta_data.length_ms = duration_ns / MIO;

		if(bitrate  > 0)
			_meta_data.bitrate = bitrate;

		if(duration_ns > 0 && bitrate > 0)
			emit track_time_changed(_meta_data);
	}


	if( (pos_ms >= (duration_ns / MIO ) - 500) && _may_start_timer){

			// _other_pipeline should never be zero because _may_start_timer is
			// only set to true if gapless mode is active
			_other_pipeline->start_timer(_meta_data.length_ms - pos_ms);
			set_about_to_finish();
	}

	if ((quint32) _seconds_now == pos_sec)
		return;

	_seconds_now = pos_sec;
	int playtime = _seconds_now - _seconds_started;

	if (!_scrobbled ) {

		if ( playtime >= 5 || playtime == _meta_data.length_ms / 2000 ) {

			emit scrobble_track(_meta_data);
			_scrobbled = true;

		}
	}

	_last_track->id = _meta_data.id;
	_last_track->filepath = _meta_data.filepath;
	_last_track->pos_sec = pos_sec;

	emit timeChangedSignal(_seconds_now);
}


void GST_Engine::set_track_finished() {

	ENGINE_DEBUG;


	if( _state == STATE_STOP ) return;

	if ( _sr_active && _playing_stream ) {
		_stream_recorder->stop(!_sr_wanna_record);
	}

	if( ! _pipeline->get_gapless() ){
		emit track_finished();
	}

	else{

		GSTPipelineExperimental* tmp;
		tmp = _pipeline;
		_pipeline = _other_pipeline;
		_other_pipeline = tmp;

		play();
		_other_pipeline->stop();

	}
}


void GST_Engine::set_about_to_finish() {

	_may_start_timer = false;
	bool gapless = _pipeline->get_gapless();

	if(!gapless) return;

	_wait_for_gapless_track = true;

	emit track_finished();
}


void GST_Engine::set_level(float right, float left){

	ENGINE_DEBUG << "l: " << left << ", r: " << right;

	emit sig_level(right, left);
}

void GST_Engine::set_spectrum(QList<float> & lst){

	ENGINE_DEBUG << lst.size();
	emit sig_spectrum(lst);
}


int GST_Engine::getState() {
	return _state;
}

QString GST_Engine::getName() {
	return _name;
}

void GST_Engine::record_button_toggled(bool b) {
	ENGINE_DEBUG << b;
	_sr_wanna_record = b;
}

void GST_Engine::psl_sr_set_active(bool b) {
	ENGINE_DEBUG << b;
	_sr_active = b;
}

void GST_Engine::psl_new_stream_session() {
	ENGINE_DEBUG;
	_stream_recorder->set_new_stream_session();
}

void GST_Engine::sr_initialized(bool b) {

	ENGINE_DEBUG << b;

	if (b)
		play();

}

void GST_Engine::sr_ended() {
	ENGINE_DEBUG;
}

void GST_Engine::sr_not_valid() {
	ENGINE_DEBUG;
	qDebug() << "Engine: Stream not valid.. Next file";
	qDebug() << "Send track finished (3)";
	emit track_finished();
}

void GST_Engine::unmute(){
	_pipeline->unmute();

	if(_other_pipeline)
		_other_pipeline->unmute();
}

void GST_Engine::psl_set_gapless(bool b){

	if(b){

		if(!_other_pipeline){
			_other_pipeline = new GSTPipelineExperimental();
			_other_pipeline->set_gapless(true);
			_pipeline->set_gapless(true);
		}
	}

	else{

		if(_other_pipeline){
			_pipeline->set_gapless(false);
			delete _other_pipeline;
		}
	}
}

Q_EXPORT_PLUGIN2(sayonara_gstreamer, GST_Engine)
