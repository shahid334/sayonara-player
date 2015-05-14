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


#define WATCH_INTERVAL 200


bool _debug = false;


/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

GSTPlaybackEngine::GSTPlaybackEngine(QObject* parent) :
	Engine(parent)
{

	ENGINE_DEBUG;

	_pipeline = 0;
	_other_pipeline = 0;

	_md.id = -1;

	_caps = new MyCaps();
	_caps->set_parsed(false);

	_name = PLAYBACK_ENGINE;

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;

	_scrobbled = false;
	_playing_stream = false;
	_sr_wanna_record = false;

	_stream_recorder = new StreamRecorder(this);

	_gapless_state = Gapless_NoGapless;

	REGISTER_LISTENER(Set::Engine_SR_Active, _change_sr_active);
}


GSTPlaybackEngine::~GSTPlaybackEngine() {

	delete _pipeline;
	delete _stream_recorder;
	delete _caps;
}


bool GSTPlaybackEngine::init() {

	gst_init(0, 0);

	_pipeline = new GSTPlaybackPipeline(this);
	if(!_pipeline->init()){
		return false;
	}

	_other_pipeline = NULL;

	connect(_pipeline, SIGNAL(sig_about_to_finish(qint64)), this, SLOT(set_about_to_finish(qint64)));
	connect(_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));
	connect(_pipeline, SIGNAL(sig_data(uchar*, quint64)), this, SLOT(new_data(uchar*, quint64)));

	REGISTER_LISTENER(Set::Engine_Gapless, _change_gapless);
	return true;
}



void GSTPlaybackEngine::change_track_gapless(const MetaData& md) {

	set_uri(md);
	_md_gapless = md;
}



void GSTPlaybackEngine::change_track(const QString& filepath) {

	MetaData md;
	md.set_filepath(filepath);
	if (!ID3::getMetaDataOfFile(md)) {
		stop();
		return;
	}

	change_track(md);
}


void GSTPlaybackEngine::change_track(const MetaData& md) {

	bool success = false;
	emit sig_pos_changed_s(0);

	if( _gapless_state == Gapless_AboutToFinish ) {
		change_track_gapless(md);
		_gapless_state = Gapless_TrackFetched;
		return;
	}

	success = set_uri(md);
	if (!success){
		return;
	}

	_md = md;

	_caps->set_parsed(false);

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;
	_scrobbled = false;
}



bool GSTPlaybackEngine::set_uri(const MetaData& md) {

	// Gstreamer needs an URI
	gchar* uri = NULL;
	bool success = false;

	_playing_stream = Helper::is_www(md.filepath());

	// stream, but don't wanna record
	// stream is already uri
	if (_playing_stream) {


       _pipeline->set_sr_path("");

		uri = g_filename_from_utf8( 
				md.filepath().toUtf8(),
				md.filepath().toUtf8().size(), 
				NULL, NULL, NULL);
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!md.filepath().contains("://")) {

		uri = g_filename_to_uri(
			md.filepath().toLocal8Bit(), 
			NULL, NULL);
	}

	else {
		uri = g_filename_from_utf8(
			md.filepath().toUtf8(),
			md.filepath().toUtf8().size(), 
			NULL, NULL, NULL);
	}

	success = _pipeline->set_uri(uri);

	return success;
}


void GSTPlaybackEngine::play() {

	if(_gapless_state == Gapless_AboutToFinish) return;
	if(_gapless_state == Gapless_TrackFetched) return;

	_pipeline->play();

	_stream_recorder->changeTrack(_md);

    if(_sr_wanna_record){
        QString filename = _stream_recorder->get_dst_file();
        _pipeline->set_sr_path(filename);
	}
}


void GSTPlaybackEngine::stop() {

	_gapless_state = Gapless_NoGapless;

	_pipeline->stop();

	if(_other_pipeline){
		_other_pipeline->stop();
	}

	_settings->set(Set::Engine_CurTrackPos_s, 0);
	emit sig_pos_changed_s(0);
}


void GSTPlaybackEngine::pause() {
    _pipeline->pause();
}



void GSTPlaybackEngine::jump_abs_ms(quint64 pos_ms) {

	gint64 new_time_ns;

	new_time_ns = _pipeline->seek_abs(pos_ms * MIO);
	_scrobble_begin_ms = new_time_ns / MIO;
}

void GSTPlaybackEngine::jump_rel_ms(quint64 ms) {

	gint64 new_time_ns;

	quint64 new_time_ms = _pipeline->get_position_ms() + ms;
	new_time_ns = _pipeline->seek_abs(new_time_ms * MIO);

	_scrobble_begin_ms = new_time_ns / MIO;
}


void GSTPlaybackEngine::jump_rel(double percent) {

	gint64 new_time_ns;

	new_time_ns = _pipeline->seek_rel(percent, _md.length_ms * MIO);
	_scrobble_begin_ms = new_time_ns / MIO;
}



void GSTPlaybackEngine::eq_changed(int band, int val) {

	double new_val = 0;
	new_val = val * 1.0;
	if (val > 0) {
		new_val = val * 0.25;
	}

    else{
        new_val = val * 0.75;
    }

	QString band_name = QString("band") + QString::number(band);
	_pipeline->set_eq_band(band_name, new_val);

	if(_other_pipeline)
		_other_pipeline->set_eq_band(band_name, new_val);
}


MyCaps* GSTPlaybackEngine::get_caps() {
	return _caps;
}


void GSTPlaybackEngine::update_duration() {

	_pipeline->refresh_duration();

	if(_gapless_state == Gapless_AboutToFinish) return;
	if(_gapless_state == Gapless_TrackFetched) return;

	qint64 duration_ms = _pipeline->get_duration_ms();
	quint32 duration_s = duration_ms / 1000;
	quint32 md_duration_s = _md.length_ms / 1000;

	if(duration_s <= 0) return;
	if(duration_s == md_duration_s) return;
	if(duration_s > 15000) return;

	_md.length_ms = duration_ms;

	emit sig_md_changed(_md);
}


void GSTPlaybackEngine::set_cur_position_ms(qint64 pos_ms) {

	if((_gapless_state == Gapless_AboutToFinish) ||
	   (_gapless_state == Gapless_TrackFetched))
	{
		emit sig_pos_changed_s(0);
		return;
	}

	qint64 playtime_ms;
	qint32 pos_sec;
	qint32 cur_pos_sec;

	pos_sec = pos_ms / 1000;
	cur_pos_sec = _cur_pos_ms / 1000;

    if ( cur_pos_sec == pos_sec ){
        return;
    }

	_cur_pos_ms = pos_ms;

	playtime_ms = _cur_pos_ms - _scrobble_begin_ms;

	if (!_scrobbled && playtime_ms >= 5000) {

		emit sig_scrobble(_md);
		_scrobbled = true;
	}

	if(_md.radio_mode() == RadioModeOff){
		_settings->set(Set::Engine_CurTrackPos_s, pos_sec);
	}

	else{
		_settings->set(Set::Engine_CurTrackPos_s, 0);
	}

	emit sig_pos_changed_s( pos_sec );
}

void GSTPlaybackEngine::set_track_ready(){

	qint64 cur_pos_s = _pipeline->get_position_ms() / 1000;
	qint64 desired_pos_s = _settings->get(Set::Engine_CurTrackPos_s);

	if(cur_pos_s == desired_pos_s){
		return;
	}

	_pipeline->seek_abs(desired_pos_s * GST_SECOND);
	_is_first_track = false;
}


void GSTPlaybackEngine::set_track_finished() {

	if( _gapless_state == Gapless_NoGapless ) {
		emit sig_track_finished();
	}

	else{

		_gapless_state = Gapless_Playing;

		_other_pipeline->stop();

		_md = _md_gapless;
		_cur_pos_ms = 0;
		_caps->set_parsed(false);

		_scrobble_begin_ms = 0;
		_scrobbled = false;
	}
}


void GSTPlaybackEngine::set_about_to_finish(qint64 time2go) {

	if(_gapless_state == Gapless_NoGapless) {
		return;
	}

	if(_gapless_state == Gapless_AboutToFinish) {
		return;
	}

	if(_other_pipeline){
		GSTPlaybackPipeline* tmp;
		tmp = _pipeline;
		_pipeline = _other_pipeline;
		_other_pipeline = tmp;
	}

	_gapless_state = Gapless_AboutToFinish;
	_pipeline->start_timer(time2go);

	emit sig_track_finished();
}


void GSTPlaybackEngine::unmute() {
	_pipeline->unmute();

	if(_other_pipeline){
		_other_pipeline->unmute();
	}
}


void GSTPlaybackEngine::_change_gapless() {

	bool gapless = _settings->get(Set::Engine_Gapless);

	if(gapless) {

		if(!_other_pipeline) {

			_other_pipeline = new GSTPlaybackPipeline(this);
			if(!_other_pipeline->init()){
				_gapless_state = Gapless_NoGapless;
				return;
			}

			connect(_other_pipeline, SIGNAL(sig_about_to_finish(qint64)), this, SLOT(set_about_to_finish(qint64)));
			connect(_other_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));
			connect(_other_pipeline, SIGNAL(sig_data(uchar*, quint64)), this, SLOT(new_data(uchar*, quint64)));
		}

		_gapless_state = Gapless_Playing;
	}

	else {
		_gapless_state = Gapless_NoGapless;
	}
}


void GSTPlaybackEngine::psl_set_speed(float f) {
	_pipeline->set_speed(f);

    if(_other_pipeline){
        _other_pipeline->set_speed(f);
    }
}


void GSTPlaybackEngine::_change_sr_active(){
	_sr_active = _settings->get(Set::Engine_SR_Active);
    if(!_sr_active){
        record_button_toggled(false);
    }
}


void GSTPlaybackEngine::record_button_toggled(bool b){
    Engine::record_button_toggled(b);

    QString dst_file;

    _stream_recorder->activate(b);
    _stream_recorder->changeTrack(_md);


    dst_file = _stream_recorder->get_dst_file();
    if(!_pipeline) return;

    _pipeline->set_sr_path(dst_file);
}


void GSTPlaybackEngine::update_md(const MetaData& md){

	if(md.bitrate != 0){
		_md.bitrate = md.bitrate;
	}

	if( Helper::is_www(_md.filepath()) &&
		!md.title.isEmpty() &&
		md.title != _md.title)
	{
		_md.title = md.title;

		emit sig_md_changed(_md);

		_stream_recorder->changeTrack(_md);
		_pipeline->set_sr_path(_stream_recorder->get_dst_file());
	}
}
