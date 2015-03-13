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

float log_10[20001];
float lo_128[128];


/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

void _calc_log10_lut() {
	for(int i=0; i<=20000; i++) {
		log_10[i] = log10(i / 20000.0f);
	}

	for(int i=0; i<128; i++) {
		lo_128[i] = i*128.0f;
	}
}


GSTPlaybackEngine::GSTPlaybackEngine(QObject* parent) :
	Engine(parent)
{

	ENGINE_DEBUG;
	_md.id = -1;
	_calc_log10_lut();

	_caps = new MyCaps();
	_caps->set_parsed(false);

	_name = PLAYBACK_ENGINE;

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;

	_scrobbled = false;
	_playing_stream = false;
	_sr_wanna_record = false;

	_stream_recorder = new StreamRecorder();

	_wait_for_gapless_track = false;
	_may_start_timer = false;

	_jump_play_s = 0;

	if( _settings->get(Set::PL_LastTrack) >= 0 &&
		_settings->get(Set::PL_Load) &&
		_settings->get(Set::PL_RememberTime))
	{
		_jump_play_s = _settings->get(Set::Engine_CurTrackPos_s);
	}

	connect(_stream_recorder, SIGNAL(sig_initialized(bool)), this, SLOT(sr_initialized(bool)));
	connect(_stream_recorder, SIGNAL(sig_stream_ended()), this,
			SLOT(sr_ended()));
	connect(_stream_recorder, SIGNAL(sig_stream_not_valid()), this,
			SLOT(sr_not_valid()));

	REGISTER_LISTENER(Set::Engine_SR_Active, _change_sr_active);
	REGISTER_LISTENER(Set::Engine_Gapless, _change_gapless);

	init();
}


GSTPlaybackEngine::~GSTPlaybackEngine() {

	delete _pipeline;
}


void GSTPlaybackEngine::init() {

	gst_init(0, 0);

	_stream_recorder->init();

	_pipeline = new GSTPlaybackPipeline(this);
	_other_pipeline = NULL;

	connect(_pipeline, SIGNAL(sig_about_to_finish(qint64)), this, SLOT(set_about_to_finish(qint64)));
	connect(_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));
	connect(_pipeline, SIGNAL(sig_data(uchar*, quint64)), this, SLOT(new_data(uchar*, quint64)));
}



void GSTPlaybackEngine::change_track_gapless(const MetaData& md) {

	bool success = set_uri(md);

    if (!success) return;

	_md = md;

	_caps->set_parsed(false);

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;

	_scrobbled = false;
}



void GSTPlaybackEngine::change_track(const QString& filepath, bool start_play) {

	MetaData md;
	md.set_filepath(filepath);
	if (!ID3::getMetaDataOfFile(md)) {
		stop();
		return;
	}

	change_track(md, start_play);
}


void GSTPlaybackEngine::change_track(const MetaData& md, bool start_play) {

	qDebug() << "Change track " << md.title;

	if( ! md.is_equal(_md ) && (_md.id >= 0) ){
		qDebug() << md.filepath() << " not equal " << _md.filepath();
		qDebug() << "Md id = " << _md.id;
		qDebug() << "-->Jump play = 0";
		_jump_play_s = 0;
	}

	bool success = false;
	if(md.radio_mode() != RadioModeOff){
		_settings->set(Set::Engine_CurTrackPos_s, 0);
	}

	if(_wait_for_gapless_track) {
		change_track_gapless(md);
		_wait_for_gapless_track = false;
		return;
	}

	stop();

	success = set_uri(md);
    if (!success){
		return;
    }

	_md = md;

	_caps->set_parsed(false);

	_scrobble_begin_ms = 0;
	_cur_pos_ms = 0;
	_scrobbled = false;

	emit sig_pos_changed_s(_jump_play_s);

	if(start_play){
		play();
	}

	else{
		pause();
	}
}



bool GSTPlaybackEngine::set_uri(const MetaData& md) {

	// Gstreamer needs an URI
	gchar* uri = NULL;
    bool success = false;

	_playing_stream = Helper::is_www(md.filepath());

	// stream && streamripper active
	if (_playing_stream && _sr_active) {

		int max_tries = 10;

		QString filepath = _stream_recorder->changeTrack(md, max_tries);
		if (filepath.size() == 0) {
			qDebug() << "Engine: Stream Ripper Error: Could not get filepath";
			return false;
		}

		// standard stream is already uri
		else {
			uri = g_filename_from_utf8(md.filepath().toUtf8(),
									   md.filepath().toUtf8().size(), NULL, NULL, NULL);
		}
	}

	// stream, but don't wanna record
	// stream is already uri
	else if (_playing_stream && !_sr_active) {
		uri = g_filename_from_utf8(md.filepath().toUtf8(),
								   md.filepath().toUtf8().size(), NULL, NULL, NULL);
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!md.filepath().contains("://")) {

		uri = g_filename_to_uri(md.filepath().toLocal8Bit(), NULL, NULL);
	}

	else {
		uri = g_filename_from_utf8(md.filepath().toUtf8(),
								   md.filepath().toUtf8().size(), NULL, NULL, NULL);
	}

	if(_wait_for_gapless_track) {

        if(_other_pipeline){
            success = _other_pipeline->set_uri(uri);
        }
	}

	if(!success) {
		success = _pipeline->set_uri(uri);
	}

	return success;
}


void GSTPlaybackEngine::play() {

	_pipeline->play();
	_may_start_timer = _gapless;
}


void GSTPlaybackEngine::stop() {

    // streamripper, wanna record is set when record button is pressed
	if (_playing_stream && _sr_active) {
		_stream_recorder->stop(!_sr_wanna_record);
	}

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


void GSTPlaybackEngine::jump_abs_s(quint32 pos_s) {

	gint64 new_time_ns;

	new_time_ns = _pipeline->seek_abs(pos_s * MRD);

	_scrobble_begin_ms = new_time_ns / MIO;
}


void GSTPlaybackEngine::jump_abs_ms(quint64 pos_ms) {

	gint64 new_time_ns;

	new_time_ns = _pipeline->seek_abs(pos_ms * MIO);

	_scrobble_begin_ms = new_time_ns / MIO;
}


void GSTPlaybackEngine::jump_rel(quint32 where) {

	gint64 new_time_ns;
	float p = where / 100.0f;


	new_time_ns = _pipeline->seek_rel(p, _md.length_ms * MIO);

	_scrobble_begin_ms = new_time_ns / MIO;
}

void GSTPlaybackEngine::jump_rel_ms(qint64 where){

    qint64 cur_pos_ms;

    cur_pos_ms = _pipeline->get_position_ms();
    cur_pos_ms += where;

    _pipeline->seek_abs( (gint64) (cur_pos_ms * MIO) );
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


void GSTPlaybackEngine::update_bitrate(quint32 bitrate) {

	if(_md.bitrate == bitrate) return;

	_md.bitrate = bitrate;

	emit sig_md_changed(_md);
}


void GSTPlaybackEngine::update_duration() {

	_pipeline->refresh_duration();

	qDebug() << "===========================";
	qDebug() << Q_FUNC_INFO;
	qDebug() << "Jump play = " << _jump_play_s;
	qDebug() << "get duration = " << _pipeline->get_duration_ms();
	qDebug() << "state = " << _pipeline->get_state();
	qDebug() << "===========================";

	if( _jump_play_s > 0 &&
		(_pipeline->get_duration_ms() > 0) &&
		( (_pipeline->get_state() == GST_STATE_PLAYING || _pipeline->get_state() == GST_STATE_PAUSED) ))
	{

		_pipeline->seek_abs(_jump_play_s * GST_SECOND);

		qDebug() << "Reset jump play";

		_jump_play_s = 0;
	}

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


void GSTPlaybackEngine::set_track_finished() {

	if ( _sr_active && _playing_stream ) {
		_stream_recorder->stop(!_sr_wanna_record);
	}

	if( ! _gapless ) {
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

    if(!_gapless) return;

	if(_may_start_timer) {
		_other_pipeline->start_timer(time2go);
	}

	_may_start_timer = false;
	_wait_for_gapless_track = true;

	emit sig_track_finished();
}


void GSTPlaybackEngine::unmute() {
	_pipeline->unmute();

    if(_other_pipeline){
        _other_pipeline->unmute();
    }
}


void GSTPlaybackEngine::_change_gapless() {

	_gapless = _settings->get(Set::Engine_Gapless);

	if(_gapless) {

		if(!_other_pipeline) {
			_other_pipeline = new GSTPlaybackPipeline(this);
			connect(_other_pipeline, SIGNAL(sig_about_to_finish(qint64)), this, SLOT(set_about_to_finish(qint64)));
			connect(_other_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));
			connect(_other_pipeline, SIGNAL(sig_data(uchar*, quint64)), this, SLOT(new_data(uchar*, quint64)));
		}

		_may_start_timer = true;
	}

	else {
		_may_start_timer = false;
	}
}


void  GSTPlaybackEngine::psl_set_speed(float f) {
	_pipeline->set_speed(f);

    if(_other_pipeline){
        _other_pipeline->set_speed(f);
    }
}


void GSTPlaybackEngine::_change_sr_active(){
	_sr_active = _settings->get(Set::Engine_SR_Active);
}

