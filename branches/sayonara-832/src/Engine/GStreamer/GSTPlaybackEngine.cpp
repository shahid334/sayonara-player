/* GSTPlaybackEngine.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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

#include <qplugin.h>

#include "HelperStructs/Helper.h"
#include "HelperStructs/Tagging/id3.h"
#include "Engine/GStreamer/GSTPlaybackEngine.h"

#include <gst/gsturi.h>
#include <cmath>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif


#define WATCH_INTERVAL 200


bool _debug = false;

float log_10[20001];
float lo_128[128];


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


GSTPlaybackEngine::GSTPlaybackEngine() {

	ENGINE_DEBUG;
	_calc_log10_lut();

	_caps = new MyCaps();
	_caps->set_parsed(false);

	_settings = CSettingsStorage::getInstance();
	_name = PLAYBACK_ENGINE;
	_state = StateStop;

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;
	_scrobbled = false;
	_playing_stream = false;
	_sr_active = false;
	_sr_wanna_record = false;

	_stream_recorder = new StreamRecorder();
	_last_track = _settings->getLastTrack();

	_wait_for_gapless_track = false;
	_gapless = false;
	_may_start_timer = false;

	connect(_stream_recorder, SIGNAL(sig_initialized(bool)), this, SLOT(sr_initialized(bool)));
	connect(_stream_recorder, SIGNAL(sig_stream_ended()), this,
			SLOT(sr_ended()));
	connect(_stream_recorder, SIGNAL(sig_stream_not_valid()), this,
			SLOT(sr_not_valid()));

	init();
}


GSTPlaybackEngine::~GSTPlaybackEngine() {

	ENGINE_DEBUG;

	qDebug() << "Engine: close engine... ";

	_settings->updateLastTrack();
	delete _pipeline;
}


void GSTPlaybackEngine::init() {

	ENGINE_DEBUG;

	gst_init(0, 0);

	_stream_recorder->init();

	_pipeline = new GSTPlaybackPipeline(this);
	_other_pipeline = NULL;

	connect(_pipeline, SIGNAL(sig_about_to_finish(qint64)), this, SLOT(set_about_to_finish(qint64)));
	connect(_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));

	_show_level = false;
	_show_spectrum = false;
}


void GSTPlaybackEngine::change_track(const QString& filepath, int pos_sec, bool start_play) {

	ENGINE_DEBUG << filepath << ", " << pos_sec << ",  " << start_play;

	MetaData md;
	md.filepath = filepath;
	if (!ID3::getMetaDataOfFile(md)) {
		stop();
		return;
	}

	change_track(md, pos_sec, start_play);
}


void GSTPlaybackEngine::change_track_gapless(const MetaData& md, int pos_sec, bool start_play){


	start_play = true;
	pos_sec = 0;
	bool success = set_uri(md, &start_play);

	if (!success)
		return;

	_md = md;

	_caps->set_parsed(false);

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;
	_scrobbled = false;
	_jump_play = 0;
}


void GSTPlaybackEngine::change_track(const MetaData& md, int pos_sec, bool start_play) {

	ENGINE_DEBUG << md.filepath << ", " << pos_sec << ",  " << start_play;

	if(_wait_for_gapless_track) {

		ENGINE_DEBUG << "change track gapless";
		change_track_gapless(md, pos_sec, start_play);
		_wait_for_gapless_track = false;
		return;
	}

	stop();

	bool success = set_uri(md, &start_play);
	if (!success)
		return;

	_md = md;

	_caps->set_parsed(false);

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;
	_scrobbled = false;

	_jump_play = pos_sec;

	emit sig_dur_changed_ms(_md.length_ms);
	emit sig_pos_changed_s(pos_sec);

	if (start_play){
		play();
	}

	// pause if streamripper is not active
	else if (!start_play && !(_playing_stream && _sr_active))
		pause();
}



bool GSTPlaybackEngine::set_uri(const MetaData& md, bool* start_play) {

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
			gchar* filename = g_filename_from_utf8(filepath.toUtf8(),
												   filepath.toUtf8().size(), NULL, NULL, NULL);
			uri = g_filename_to_uri( filename, NULL, NULL);
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


void GSTPlaybackEngine::play() {

	ENGINE_DEBUG;

	_state = StatePlay;
	_pipeline->play();

	_may_start_timer = _gapless;
}


void GSTPlaybackEngine::do_jump_play(){

	if(_playing_stream) {
		_jump_play = -1;
		return;
	}
	if(_jump_play < 0) return;
	ENGINE_DEBUG;

	_pipeline->seek_abs(_jump_play * GST_SECOND);

	_jump_play = -1;
}


void GSTPlaybackEngine::stop() {

	ENGINE_DEBUG;

	_state = StateStop;

	// streamripper, wanna record is set when record button is pressed
	if (_playing_stream && _sr_active) {
		qDebug() << "Engine: stop... Playing stream";
		_stream_recorder->stop(!_sr_wanna_record);
	}

	_pipeline->stop();

	if(_other_pipeline)
		_other_pipeline->stop();

	emit sig_pos_changed_s(0);
}


void GSTPlaybackEngine::pause() {
	ENGINE_DEBUG;
	_state = StatePause;

	_pipeline->pause();
}


void GSTPlaybackEngine::set_volume(int vol) {
	ENGINE_DEBUG;
	_vol = vol;
	_pipeline->set_volume(vol);

	if(_other_pipeline)
		_other_pipeline->set_volume(vol);
}


void GSTPlaybackEngine::jump_abs_s(quint32 where) {

	ENGINE_DEBUG;

	gint64 new_time_ns;

	new_time_ns = _pipeline->seek_abs(where * MRD);

	_scrobble_begin_ms = new_time_ns / MIO;
}


void GSTPlaybackEngine::jump_abs_ms(quint64 where) {

	ENGINE_DEBUG;

	gint64 new_time_ns;

	new_time_ns = _pipeline->seek_abs(where * MIO);

	_scrobble_begin_ms = new_time_ns / MIO;
}


void GSTPlaybackEngine::jump_rel(quint32 where) {

	ENGINE_DEBUG;

	gint64 new_time_ns;

	float p = where / 100.0f;
	new_time_ns = _pipeline->seek_rel(p, _md.length_ms * MIO);

	_scrobble_begin_ms = new_time_ns / MIO;
}


void GSTPlaybackEngine::eq_changed(int band, int val) {

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

void GSTPlaybackEngine::eq_enable(bool) {
	ENGINE_DEBUG;
}


void GSTPlaybackEngine::psl_calc_level(bool b){
	ENGINE_DEBUG;

	_show_level = b;
	if(b) _show_spectrum = false;

	_pipeline->enable_level(b);

	if(_other_pipeline)
		_other_pipeline->enable_level(b);

	if(_state == StatePlay)
		_pipeline->play();
}


void GSTPlaybackEngine::psl_calc_spectrum(bool b){
	ENGINE_DEBUG;

	_show_spectrum = b;
	if(b) _show_level = false;

	_pipeline->enable_spectrum(b);

	if(_other_pipeline)
		_other_pipeline->enable_spectrum(b);

	if(_state == StatePlay)
		_pipeline->play();
}


bool GSTPlaybackEngine::get_show_level(){
	return _show_level;
}


bool GSTPlaybackEngine::get_show_spectrum(){
	return _show_spectrum;
}


MyCaps* GSTPlaybackEngine::get_caps(){
	return _caps;
}


void GSTPlaybackEngine::update_bitrate(qint32 bitrate){

	_md.bitrate = bitrate;
	emit sig_bitrate_changed(bitrate);
}


void GSTPlaybackEngine::set_cur_position_ms(qint64 pos_ms) {

	ENGINE_DEBUG << pos_ms;
	qint64 playtime_ms;
	qint32 pos_sec;
	qint32 cur_pos_sec;

	pos_sec = pos_ms / 1000;
	cur_pos_sec = _cur_pos_ms / 1000;

	if ( cur_pos_sec == pos_sec )
		return;

	_cur_pos_ms = pos_ms;

	playtime_ms = _cur_pos_ms - _scrobble_begin_ms;

	if (!_scrobbled && playtime_ms >= 5000) {

		emit sig_scrobble(_md);
		_scrobbled = true;
	}

	_last_track->id = _md.id;
	_last_track->filepath = _md.filepath;
	_last_track->pos_sec = pos_sec;

	emit sig_pos_changed_s( pos_sec );
}


void GSTPlaybackEngine::set_track_finished() {

	ENGINE_DEBUG;


	if( _state == StateStop ) return;

	if ( _sr_active && _playing_stream ) {
		_stream_recorder->stop(!_sr_wanna_record);
	}

	if( ! _gapless ){
		emit sig_track_finished();
	}

	else{

		GSTPlaybackPipeline* tmp;
		tmp = _pipeline;
		_pipeline = _other_pipeline;
		_other_pipeline = tmp;

		_other_pipeline->stop();
		play();
	}
}


void GSTPlaybackEngine::set_about_to_finish(qint64 time2go) {

	ENGINE_DEBUG << "About to finish in " << time2go << "ms";

	if(!_gapless) return;

	if(_may_start_timer){

		_other_pipeline->start_timer(time2go);
	}

	_may_start_timer = false;

	_wait_for_gapless_track = true;

	emit sig_track_finished();
}


void GSTPlaybackEngine::unmute(){
	_pipeline->unmute();

	if(_other_pipeline)
		_other_pipeline->unmute();
}


void GSTPlaybackEngine::psl_set_gapless(bool b){

	if(b){

		if(!_other_pipeline){
			_other_pipeline = new GSTPlaybackPipeline(this);
			connect(_other_pipeline, SIGNAL(sig_about_to_finish(qint64)), this, SLOT(set_about_to_finish(qint64)));
			connect(_other_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));
		}

		_other_pipeline->set_volume(_pipeline->get_volume());
		_may_start_timer = true;
		_gapless = true;

	}

	else {
		_may_start_timer = false;
		_gapless = false;

	}
}



