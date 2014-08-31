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

#include "unistd.h"
#include <QDebug>



GSTPlaybackPipeline::GSTPlaybackPipeline(Engine* engine, QObject *parent)
{


	bool status = false;
	int i = 0;
	_pipeline = 0;
	_position = 0;
	_duration = 0;

	_timer = new QTimer();
	_timer->setInterval(5);

	connect(_timer, SIGNAL(timeout()), this, SLOT(play()));


	// eq -> autoaudiosink is packaged into a bin
	do {
		bool success = false;
		// create equalizer element
		GstElement* tmp_pipeline = gst_pipeline_new("Pipeline");
		_test_and_error(tmp_pipeline, "Engine: Pipeline sucks");

		_audio_src = gst_element_factory_make("uridecodebin", "src");

		_bus = gst_pipeline_get_bus(GST_PIPELINE(tmp_pipeline));

		_audio_convert = gst_element_factory_make("audioconvert", "audio_convert");
		//_speed = gst_element_factory_make("scaletempo", "speed");
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

		_level_sink = gst_element_factory_make("fakesink", "level_sink");
		_spectrum_sink = gst_element_factory_make("fakesink", "spectrum_sink");


		if(!_test_and_error(_bus, "Engine: Something went wrong with the bus")) break;
		if(!_test_and_error(_audio_src, "Engine: Source creation fail")) break;
		if(!_test_and_error(_level, "Engine: Level cannot be created")) break;
		if(!_test_and_error(_spectrum, "Engine: Spectrum cannot be created")) break;
		if(!_test_and_error(_tee, "Engine: Tee cannot be created")) break;
		if(!_test_and_error(_equalizer, "Engine: Equalizer cannot be created")) break;
		if(!_test_and_error(_eq_queue, "Engine: Equalizer cannot be created")) break;
		if(!_test_and_error(_audio_sink, "Engine: Audio Sink cannot be created")) break;
		if(!_test_and_error(_level_queue, "Engine: Queue cannot be created")) break;
		if(!_test_and_error(_spectrum_queue, "Engine: Queue cannot be created")) break;
		if(!_test_and_error(_level_audio_convert, "Engine: Level converter fail")) break;
		if(!_test_and_error(_spectrum_audio_convert, "Engine: Spectrum converter fail")) break;


		gst_object_ref(_audio_src);

		gst_bin_add_many(GST_BIN(tmp_pipeline),
			_audio_src,	_audio_convert, _equalizer, _tee,

			_eq_queue, _volume, _audio_sink,
			_level_queue, _level_audio_convert, _level, _level_sink,
			_spectrum_queue, _spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);


		_speed_active = false;
		/*if(_speed) {
			gst_bin_add(GST_BIN(tmp_pipeline), _speed);
		}*/


		success = gst_element_link_many(_level_queue, _level_sink, NULL);
		_test_and_error_bool(success, "Engine: Cannot link Level pipeline");


		success = gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
		_test_and_error_bool(success, "Engine: Cannot link Spectrum pipeline");

		success = gst_element_link_many(_eq_queue, _volume, _audio_sink, NULL);
		if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) break;

		success = gst_element_link_many(_audio_convert, _equalizer, _tee, NULL);
		if(!_test_and_error_bool(success, "Engine: Cannot link audio convert with tee")) break;


		g_signal_connect (_audio_src, "pad-added", G_CALLBACK (PipelineCallbacks::pad_added_handler), _audio_convert);

		// Connect tee

		GstPad* tee_eq_pad;
		GstPad* eq_pad;
		GstPadLinkReturn s;

		// create tee pads
		_tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (_tee), "src_%u");
		if(!_test_and_error(_tee_src_pad_template, "Engine: _tee_src_pad_template is NULL")) break;

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

		g_object_set (_eq_queue, "silent", TRUE, NULL);
		g_object_set (_level_queue, "silent", TRUE, NULL);
		g_object_set(_spectrum_queue, "silent", TRUE, NULL);

		guint64 interval = 30000000;
		gint threshold = - crop_spectrum_at;

		/*if(_speed) {
			g_object_set(G_OBJECT(_speed),
						 "search", "0",   // 14  [0, 500]
						 "stride", 30,  // 30  [1, 5000]
						 "overlap", 0.2,   // 0.2 [0, 1]
					NULL);


		}*/

		g_object_set (G_OBJECT (_level),
					  "message", TRUE,
					  "interval", interval,
					  NULL);

		g_object_set (G_OBJECT (_spectrum),
					  "interval", interval,
					  "bands", N_BINS,
					  "threshold", threshold,
					  "message-phase", FALSE,
					  "message-magnitude", TRUE,
					  "multi-channel", FALSE,
					  NULL);



		 /* run synced and not as fast as we can */
		g_object_set (G_OBJECT (_level_sink), "sync", TRUE, NULL);
		g_object_set (G_OBJECT (_level_sink), "async", FALSE, NULL);

		g_object_set (G_OBJECT (_spectrum_sink), "sync", TRUE, NULL);
		g_object_set (G_OBJECT (_spectrum_sink), "async", FALSE, NULL);

		gst_pad_set_active(_level_pad, false);
		gst_pad_set_active(_level_pad, true);


		gst_element_set_state(tmp_pipeline, GST_STATE_READY);

		success = true;
		status = true;
		_pipeline = tmp_pipeline;

		break;
	} while (i);

	if(status) gst_bus_add_watch(_bus, EngineCallbacks::bus_state_changed, engine);
	else {
		_pipeline=0;
		qDebug() << "****Pipeline: constructor finished: " << status;
		return;
	}

	qDebug() << "****Pipeline: constructor finished: " << status;
}


GSTPlaybackPipeline::~GSTPlaybackPipeline() {
	if (_bus)
		gst_object_unref (_bus);

	if (_pipeline) {
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
		gst_object_unref (GST_OBJECT(_pipeline));
	}

	delete _timer;
}



void GSTPlaybackPipeline::play() {
	_timer->stop();

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
	g_timeout_add(200, (GSourceFunc) PipelineCallbacks::show_position, this);

}

void GSTPlaybackPipeline::pause() {
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}


void GSTPlaybackPipeline::stop() {

	_timer->stop();

	_duration = 0;
	_uri = 0;

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);

}

void GSTPlaybackPipeline::set_volume(int vol) {

	_vol = vol;

	float vol_val = (float) (vol * 1.0f / 100.0f);
	g_object_set(G_OBJECT(_volume), "volume", vol_val, NULL);
}

int GSTPlaybackPipeline::get_volume() {
	return _vol;
}

void GSTPlaybackPipeline::unmute() {

	g_object_set(G_OBJECT(_volume), "mute", FALSE, NULL);
}

gint64 GSTPlaybackPipeline::seek_rel(float percent, gint64 ref_ns) {

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
		new_time_ns)) {

		return new_time_ns;
	}

	else qDebug() << "r Cannot seek to " << new_time_ns / 1000000;

	return 0;
}


gint64 GSTPlaybackPipeline::seek_abs(gint64 ns) {


	if(ns == 0) return 0;

	g_object_set(G_OBJECT(_volume), "mute", TRUE, NULL);

	if(gst_element_seek_simple(_audio_src,
		GST_FORMAT_TIME,(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
		ns)) {

		return ns;
	}

	else qDebug() << "Cannot seek to " << ns / 1000000;



	return 0;
}

void GSTPlaybackPipeline::set_speed(float f) {

	//if(!_speed) return;


	if(f < 0 && _speed_active) {

		_speed_active = false;

		/*gst_element_unlink_many(_audio_convert, _speed, _equalizer, NULL);
		gst_element_link( _audio_convert, _equalizer );*/
	}

	else if(f > 0 && !_speed_active) {

		gint64 pos, dur;

		_speed_active = true;
		/*gst_element_unlink_many(_audio_convert, _equalizer, NULL);
		gst_element_link_many(_audio_convert, _speed, _equalizer, NULL);*/



#if GST_CHECK_VERSION(1, 0, 0)
    gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos);
    gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &dur);
#else
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position(_pipeline, &format, &pos);
    gst_element_query_duration(_pipeline, &format, &dur);
#endif

		gst_element_seek(_audio_src,
						 f,
						 GST_FORMAT_TIME,
						 (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
						 GST_SEEK_TYPE_SET, pos,
						 GST_SEEK_TYPE_SET, dur);


	}

	else if(f > 0 && _speed_active ) {

		gint64 pos, dur;
		qDebug() << "Seek";


#if GST_CHECK_VERSION(1, 0, 0)
    gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos);
    gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &dur);
#else
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position(_pipeline, &format, &pos);
    gst_element_query_duration(_pipeline, &format, &dur);
#endif


		gst_element_seek(_audio_src,
						 f,
						 GST_FORMAT_TIME,
						 (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
						 GST_SEEK_TYPE_SET, pos,
						 GST_SEEK_TYPE_SET, dur);

	}
}

void GSTPlaybackPipeline::enable_level(bool b) {

	GstState state;
	gst_element_get_state(GST_ELEMENT(_pipeline), &state, NULL, GST_CLOCK_TIME_NONE);


	if(state == GST_STATE_PLAYING)
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);


	if(!b) {
		gst_element_unlink_many(_level_queue, _level_audio_convert, _level, _level_sink, NULL);
		gst_element_link_many(_level_queue, _level_sink, NULL);
	}

	else{
		gst_element_unlink_many(_level_queue, _level_sink, NULL);
		gst_element_link_many(_level_queue, _level_audio_convert, _level, _level_sink, NULL);
	}

	if(state == GST_STATE_PLAYING)
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
}


void GSTPlaybackPipeline::enable_spectrum(bool b) {

	GstState state;
	gst_element_get_state(GST_ELEMENT(_pipeline), &state, NULL, GST_CLOCK_TIME_NONE);


	if(state == GST_STATE_PLAYING)
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);

	if(!b) {
		gst_element_unlink_many(_spectrum_queue, _spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);
		gst_element_link_many(_spectrum_queue, _spectrum_sink, NULL);
	}
	else{
		gst_element_unlink_many(_spectrum_queue, _spectrum_sink, NULL);
		gst_element_link_many(_spectrum_queue, _spectrum_audio_convert, _spectrum, _spectrum_sink, NULL);
	}

	if(state == GST_STATE_PLAYING)
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

}


qint64 GSTPlaybackPipeline::get_position_ms() {

	gint64 position=0;
	bool success = false;

#if GST_CHECK_VERSION(1, 0, 0)
    success = gst_element_query_position(_pipeline, GST_FORMAT_TIME, &position);
#else
        GstFormat format = GST_FORMAT_TIME;
    success = gst_element_query_position(_pipeline, &format, &position);
#endif

	if(!success ) {
		_position = 0;
		return -1;
	}

	_position = (qint64) (position / MIO);

	return _position;
}


qint64 GSTPlaybackPipeline::get_duration_ms() {

	if(_duration != 0) return _duration;

	gint64 duration=0;
	bool success = false;
	GstState state = get_state();

    if(state == GST_STATE_PAUSED || state == GST_STATE_PLAYING){

#if GST_CHECK_VERSION(1, 0, 0)
	   success = gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &duration);
#else
        GstFormat format = GST_FORMAT_TIME;
	   success = gst_element_query_duration(_pipeline, &format, &duration);
#endif


    }


	if(!success ) {
		_duration = 0;
		return -1;
	}

	_duration = (qint64) (duration / MIO);

	return _duration;
}


guint GSTPlaybackPipeline::get_bitrate() {


	GstTagList *tags=NULL;
	guint rate=0;

	bool success = false;

	if(tags) {

		success = gst_tag_list_get_uint (tags, GST_TAG_BITRATE, &rate);

		if(success) {
			qDebug() << "tags there, bitrate = " << rate;
			return rate;
		}
	}

	return 0;
}


bool GSTPlaybackPipeline::set_uri(gchar* uri) {

	if(!uri) return false;

	ENGINE_DEBUG << "Pipeline: " << uri;

	g_object_set(G_OBJECT(_audio_src), "uri", uri, NULL);
	gst_element_set_state(_pipeline, GST_STATE_PAUSED);

	return true;
}




void GSTPlaybackPipeline::start_timer(qint64 play_ms) {

	ENGINE_DEBUG << "Start in " << play_ms << "ms";

	if(play_ms > 0) _timer->start(play_ms);
	else play();
}


void GSTPlaybackPipeline::set_eq_band(QString band_name, double val) {

	g_object_set(G_OBJECT(_equalizer), band_name.toLocal8Bit(), val, NULL);
}




