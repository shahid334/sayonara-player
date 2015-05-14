/* GSTPlaybackPipeline.cpp */

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

#include "Engine/GStreamer/GSTPlaybackPipeline.h"
#include "Engine/GStreamer/GSTEngineHelper.h"
#include "HelperStructs/globals.h"
#include <gst/app/gstappsink.h>




GSTPlaybackPipeline::GSTPlaybackPipeline(Engine* engine, QObject *parent) :
	GSTAbstractPipeline("Playback Pipeline", engine, parent)
{

	_speed_val = 1.0f;
	_speed_active = false;

	_timer = new QTimer();
	_timer->setInterval(5);

	connect(_timer, SIGNAL(timeout()), this, SLOT(play()));

}


GSTPlaybackPipeline::~GSTPlaybackPipeline() {

	delete _timer;
}

bool GSTPlaybackPipeline::init(GstState state){

	if(!GSTAbstractPipeline::init(state)){
		return false;
	}

	_settings->set(SetNoDB::MP3enc_found, _lame != NULL);
	qDebug() << "****Pipeline: constructor finished: ";


	REGISTER_LISTENER(Set::Engine_Vol, _sl_vol_changed);
	REGISTER_LISTENER(Set::Engine_ShowLevel, _sl_show_level_changed);
	REGISTER_LISTENER(Set::Engine_ShowSpectrum, _sl_show_spectrum_changed);
	REGISTER_LISTENER(SetNoDB::Broadcast_Clients, _sl_broadcast_clients_changed);

	return true;
}

bool GSTPlaybackPipeline::create_elements(){

	if(!create_element(&_audio_src, "uridecodebin", "src")) return false;
	if(!create_element(&_audio_convert, "audioconvert")) return false;
	if(!create_element(&_equalizer, "equalizer-10bands")) return false;
	if(!create_element(&_tee, "tee")) return false;

	if(!create_element(&_eq_queue, "queue", "eq_queue")) return false;
	if(!create_element(&_volume, "volume")) return false;
	if(!create_element(&_audio_sink, "autoaudiosink")) return false;

	if(!create_element(&_level_queue, "queue", "level_queue")) return false;
	if(!create_element(&_level, "level")) return false;
	if(!create_element(&_level_sink, "fakesink", "level_sink")) return false;

	if(!create_element(&_spectrum_queue, "queue", "spectrum_queue")) return false;
	if(!create_element(&_spectrum, "spectrum")) return false;
	if(!create_element(&_spectrum_sink,"fakesink", "spectrum_sink")) return false;


	if( !create_element(&_lame_queue, "queue", "lame_queue") ||
		!create_element(&_resampler, "audioresample", "lame_resampler") ||
		!create_element(&_lame, "lamemp3enc") ||
		!create_element(&_app_sink, "appsink", "lame_appsink") ||
		!create_element(&_fake_sink, "fakesink", "fakesink"))
	{
		_lame = NULL;
	}


	if(	!create_element(&_file_queue, "queue", "sr_queue") ||
		!create_element(&_file_resampler, "audioresample", "sr_resample") ||
		!create_element(&_file_lame, "lamemp3enc", "sr_lame")  ||
		!create_element(&_file_sink, "filesink", "sr_filesink") ||
		!create_element(&_file_fake_sink, "fakesink","sr_fakesink"))
	{
		_file_sink = NULL;
	}


	return true;
}

bool GSTPlaybackPipeline::add_and_link_elements(){

	bool success = false;
	GstPadTemplate* tee_src_pad_template;

	gst_bin_add_many(GST_BIN(_pipeline),
					 _audio_src, _audio_convert, _equalizer, _tee,

					 _eq_queue, _volume, _audio_sink,
					 _level_queue, _level, _level_sink,
					 _spectrum_queue, _spectrum, _spectrum_sink,

					 NULL);

	success = gst_element_link_many(_eq_queue, _volume, _audio_sink, NULL);
	if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) {
		return false;
	}

	success = gst_element_link_many(_level_queue, _level_sink, NULL);
	if(!_test_and_error_bool(success, "Engine: Cannot link Level pipeline")){
		return false;
	}

	success = gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
	if(!_test_and_error_bool(success, "Engine: Cannot link Spectrum pipeline")){
		return false;
	}

	success = gst_element_link_many(_audio_convert, _equalizer, _tee, NULL);
	if(!_test_and_error_bool(success, "Engine: Cannot link audio convert with tee")){
		return false;
	}

	if(_lame){
		gst_bin_add_many(GST_BIN(_pipeline), _lame_queue, _resampler, _lame, _app_sink, NULL);
		success = gst_element_link_many( _lame_queue, _resampler, _lame, _app_sink, NULL);
		_test_and_error_bool(success, "Engine: Cannot link lame stuff");
	}

	if(_file_sink){
		gst_bin_add_many(GST_BIN(_pipeline), _file_queue, _file_fake_sink, NULL);
		success = gst_element_link_many( _file_queue, _file_fake_sink, NULL);
		_test_and_error_bool(success, "Engine: Cannot link streamripper stuff");
	}

	// create tee pads
	tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (_tee), "src_%u");
	if(!_test_and_error(tee_src_pad_template, "Engine: _tee_src_pad_template is NULL")) {
		return false;
	}

	success = tee_connect(tee_src_pad_template, _level_queue, "Level");
	if(!_test_and_error_bool(success, "Engine: Cannot link level queue with tee")){
		return false;
	}

	tee_connect(tee_src_pad_template, _spectrum_queue, "Spectrum");
	if(!_test_and_error_bool(success, "Engine: Cannot link spectrum queue with tee")){
		return false;
	}

	tee_connect(tee_src_pad_template, _eq_queue, "Equalizer");
	if(!_test_and_error_bool(success, "Engine: Cannot link eq queue with tee")){
		return false;
	}

	if(_lame){
		success = tee_connect(tee_src_pad_template, _lame_queue, "Lame");
		if(!_test_and_error_bool(success, "Engine: Cannot link lame queue with tee")){
			_settings->set(SetNoDB::MP3enc_found, false);
		}
	}

	if(_file_sink){
		success = tee_connect(tee_src_pad_template, _file_queue, "Streamripper");
		if(!_test_and_error_bool(success, "Engine: Cannot link streamripper stuff")){
			_settings->set(Set::Engine_SR_Active, false);
		}
	}

	return true;
}

bool GSTPlaybackPipeline::configure_elements(){

	guint64 interval = 30000000;
	gint threshold = -crop_spectrum_at;


	g_object_set (G_OBJECT (_audio_src),
				  "use-buffering", true,
				  NULL);


	g_object_set (G_OBJECT (_level),
				  "message", true,
				  "interval", interval,
				  NULL);

	g_object_set (G_OBJECT (_spectrum),
				  "interval", interval,
				  "bands", N_BINS,
				  "threshold", threshold,
				  "message-phase", false,
				  "message-magnitude", true,
				  "multi-channel", false,
				  NULL);


	if(_lame){
		g_object_set(G_OBJECT (_lame),

					 "perfect-timestamp", true,
					 "target", 1,
					 "cbr", true,
					 "bitrate", 128,
					 "encoding-engine-quality", 2,
					 NULL);
	}

	if(_file_sink){
		g_object_set(G_OBJECT (_file_lame),

					 "perfect-timestamp", true,
					 "target", 1,
					 "cbr", true,
					 "bitrate", 128,
					 "encoding-engine-quality", 2,
					 NULL);
	}

	/* run synced and not as fast as we can */
	g_object_set(G_OBJECT (_level_sink), "sync", true, NULL);
	g_object_set(G_OBJECT (_level_sink), "async", false, NULL);
	g_object_set(G_OBJECT (_spectrum_sink), "sync", true, NULL);
	g_object_set(G_OBJECT (_spectrum_sink), "async", false, NULL);
	g_object_set(G_OBJECT (_file_sink), "sync", true, NULL);
	g_object_set(G_OBJECT (_file_sink), "async", false, NULL);
	g_object_set(G_OBJECT (_file_fake_sink), "sync", true, NULL);
	g_object_set(G_OBJECT (_file_fake_sink), "async", false, NULL);

	g_object_set(G_OBJECT(_app_sink), "emit-signals", true, NULL );

	g_signal_connect (_audio_src, "pad-added", G_CALLBACK (PipelineCallbacks::pad_added_handler), _audio_convert);
	g_signal_connect (_app_sink, "new-sample", G_CALLBACK(PipelineCallbacks::new_buffer), this);

	return true;
}

bool GSTPlaybackPipeline::tee_connect(GstPadTemplate* tee_src_pad_template, GstElement* queue, QString queue_name){


	GstPadLinkReturn s;
	GstPad* tee_queue_pad;
	GstPad* queue_pad;

	QString error_1 = QString("Engine: Tee-") + queue_name + " pad is NULL";
	QString error_2 = QString("Engine: ") + queue_name + " pad is NULL";
	QString error_3 = QString("Engine: Cannot link tee with ") + queue_name;

	tee_queue_pad = gst_element_request_pad(_tee, tee_src_pad_template, NULL, NULL);
	if(!_test_and_error(tee_queue_pad, error_1)){
		return false;
	}

	queue_pad = gst_element_get_static_pad(queue, "sink");
	if(!_test_and_error(queue_pad, error_2)) {
		return false;
	}

	s = gst_pad_link (tee_queue_pad, queue_pad);
	if(!_test_and_error_bool((s == GST_PAD_LINK_OK), error_3)) {
		return false;
	}

	g_object_set (queue, "silent", TRUE, NULL);

	return true;
}

void GSTPlaybackPipeline::set_ready(){

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_READY);
}

void GSTPlaybackPipeline::play() {

	_timer->stop();

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
	g_timeout_add(200, (GSourceFunc) PipelineCallbacks::show_position, this);

	if(_speed_active){
		set_speed(_speed_val);
	}
}


void GSTPlaybackPipeline::pause() {

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}


void GSTPlaybackPipeline::stop() {

	_timer->stop();

	_duration_ms = 0;
	_uri = 0;

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
}

void GSTPlaybackPipeline::_sl_vol_changed() {

	_vol = _settings->get(Set::Engine_Vol);
	if(_vol < 0) _vol = 0;
	if(_vol > 100) _vol = 100;

	float vol_val = (float) (_vol * 1.0f / 100.0f);

	g_object_set(G_OBJECT(_volume), "volume", vol_val, NULL);
}


void GSTPlaybackPipeline::unmute() {

	g_object_set(G_OBJECT(_volume), "mute", FALSE, NULL);
}


bool GSTPlaybackPipeline::_seek(gint64 ns){

	bool success;
	if(_speed_active){
		float f = _speed_val;
		success = gst_element_seek(_audio_src,
									f,
									GST_FORMAT_TIME,
									(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
									GST_SEEK_TYPE_SET, ns,
									GST_SEEK_TYPE_SET, _duration_ms * MIO);

	}

	else {
		success = gst_element_seek_simple( _audio_src,
								 GST_FORMAT_TIME,
								 (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SNAP_NEAREST),
								ns);
	}

	return success;
}

gint64 GSTPlaybackPipeline::seek_rel(double percent, gint64 ref_ns) {

	gint64 new_time_ns;

	if (percent > 1.0){
		new_time_ns = ref_ns;
	}

	else if(percent < 0){
		new_time_ns = 0;
	}

	else{
		new_time_ns = (percent * ref_ns); // nsecs
	}


	if( _seek(new_time_ns) ) {
		return new_time_ns;
	}

	return 0;
}


gint64 GSTPlaybackPipeline::seek_abs(gint64 ns) {

	if(ns < 0) ns = 0;

	if( _seek(ns) ) {
		return ns;
	}

	return 0;
}

void GSTPlaybackPipeline::set_speed(float f) {

	if(f < 0 && _speed_active) {

		_speed_active = false;
		_speed_val = 1.0f;
	}

	else if(f > 0 && !_speed_active) {

		_speed_active = true;
		_speed_val = f;

		_seek(_position_ms * MIO);
	}

	else if(f > 0 && _speed_active ) {

		_speed_val = f;

		_seek(_position_ms * MIO);
	}
}


void GSTPlaybackPipeline::_sl_show_level_changed() {

	bool active = _settings->get(Set::Engine_ShowLevel);

	GstState state;
    gst_element_get_state(GST_ELEMENT(_pipeline), &state, NULL, MRD);

	if(state == GST_STATE_PLAYING){
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
	}

	if(!active) {
		gst_element_unlink_many(_level_queue, _level, _level_sink, NULL);
		gst_element_link_many(_level_queue, _level_sink, NULL);
	}

	else{
		gst_element_unlink_many(_level_queue, _level_sink, NULL);
		gst_element_link_many(_level_queue, _level, _level_sink, NULL);
	}

	gst_element_set_state(GST_ELEMENT(_pipeline), state);
}


void GSTPlaybackPipeline::_sl_show_spectrum_changed() {

	bool active = _settings->get(Set::Engine_ShowSpectrum);

	GstState state;
    gst_element_get_state(GST_ELEMENT(_pipeline), &state, NULL, MRD);

	if(state == GST_STATE_PLAYING){
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
	}

	if(!active) {
		gst_element_unlink_many(_spectrum_queue, _spectrum, _spectrum_sink, NULL);
		gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
	}

	else{
		gst_element_unlink_many(_spectrum_queue, _spectrum_sink, NULL);
		gst_element_link_many(_spectrum_queue, _spectrum, _spectrum_sink, NULL);
	}


	gst_element_set_state(GST_ELEMENT(_pipeline), state);
}


void GSTPlaybackPipeline::_sl_broadcast_clients_changed(){

    int n_clients = _settings->get(SetNoDB::Broadcast_Clients);
    bool active = (n_clients > 0);

    qDebug() << "Broadcast active? " << active;

    GstState state;
    gst_element_get_state(GST_ELEMENT(_pipeline), &state, NULL, MRD);

    if(state == GST_STATE_PLAYING){
        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
    }

    if(!active) {
		gst_app_sink_set_max_buffers( GST_APP_SINK(_app_sink), 0);
		gst_app_sink_set_drop( GST_APP_SINK(_app_sink), false);

		gst_app_sink_set_drop( GST_APP_SINK(_app_sink), true);
		gst_app_sink_set_max_buffers( GST_APP_SINK(_app_sink), 1);
		gst_element_unlink_many( _lame_queue, _resampler, _lame, _app_sink, NULL);
        gst_element_link_many(_lame_queue, _app_sink, NULL);
    }

    else{
		gst_app_sink_set_max_buffers( GST_APP_SINK(_app_sink), 1);
		gst_app_sink_set_drop( GST_APP_SINK(_app_sink), true);

        gst_element_unlink_many(_lame_queue, _app_sink, NULL);
		gst_element_link_many( _lame_queue, _resampler, _lame, _app_sink, NULL);
    }

    g_object_set(G_OBJECT(_app_sink), "emit-signals", active, NULL );

    gst_element_set_state(GST_ELEMENT(_pipeline), state);
}



bool GSTPlaybackPipeline::set_uri(gchar* uri) {

	if(!uri) return false;

	gst_element_set_state(_pipeline, GST_STATE_NULL);
	g_object_set(G_OBJECT(_audio_src), "uri", uri, NULL);
	gst_element_set_state(_pipeline, GST_STATE_PAUSED);

	return true;
}


void GSTPlaybackPipeline::start_timer(qint64 play_ms) {

	if(play_ms > 0) {
		_timer->start(play_ms);
	}

	else {
		play();
	}
}


void GSTPlaybackPipeline::set_eq_band(QString band_name, double val) {

	g_object_set(G_OBJECT(_equalizer), band_name.toLocal8Bit(), val, NULL);
}


void GSTPlaybackPipeline::set_sr_path(QString path){

	if(!_file_sink) return;
	if(path == _sr_path) return;

	_sr_path = path;

	bool success = true;

	GstState state;

	gst_element_get_state(_pipeline, &state, NULL, MRD);
	bool nulled=false;



	if(path.isEmpty()){

		if(!gst_bin_get_by_name(GST_BIN(_pipeline), "sr_fakesink")){
			gst_element_set_state(_pipeline, GST_STATE_NULL);
			gst_bin_add(GST_BIN(_pipeline), _file_fake_sink);
			nulled = true;
		}


		if(gst_bin_get_by_name(GST_BIN(_pipeline), "sr_filesink")){
			gst_element_set_state(_pipeline, GST_STATE_NULL);
			gst_object_ref(_file_sink);
			gst_object_ref(_file_resampler);
			gst_object_ref(_file_lame);
			gst_element_unlink_many(_file_queue, _file_resampler, _file_lame, _file_sink, NULL);
			gst_bin_remove_many(GST_BIN(_pipeline), _file_resampler, _file_lame, _file_sink, NULL);
			nulled = true;
		}

		GstPad* pad = gst_element_get_static_pad(_file_fake_sink, "sink");
		if(pad && !gst_pad_is_linked(pad)){
			success = gst_element_link(_file_queue, _file_fake_sink);
		}
	}

	else{


		gst_element_set_state(_pipeline, GST_STATE_NULL);
		nulled = true;

		if(gst_bin_get_by_name(GST_BIN(_pipeline), "sr_fakesink")){
			gst_object_ref(_file_fake_sink);
			gst_element_unlink(_file_queue, _file_fake_sink);
			gst_bin_remove(GST_BIN(_pipeline), _file_fake_sink);
		}

		if(!gst_bin_get_by_name(GST_BIN(_pipeline), "sr_filesink")){
			gst_bin_add_many(GST_BIN(_pipeline), _file_resampler, _file_lame, _file_sink, NULL);
			success = gst_element_link_many(_file_queue, _file_resampler, _file_lame, _file_sink, NULL);
		}

		g_object_set(G_OBJECT(_file_sink), "location", path.toLocal8Bit().data(), NULL);

	}

	if(nulled){
		gst_element_set_state(_pipeline, state);
	}


	(void) success;
}
