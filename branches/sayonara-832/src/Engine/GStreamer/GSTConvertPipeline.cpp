/* GSTConvertPipeline.cpp */

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



#include "Engine/GStreamer/GSTConvertPipeline.h"





static void pad_added_handler (GstElement *src, GstPad *new_pad, gpointer data){

	GstElement* ac = (GstElement*) data;
	GstPad* ac_pad = gst_element_get_static_pad(ac, "sink");

	if(ac_pad && gst_pad_is_linked(ac_pad)) {
		ENGINE_DEBUG << " Decoder and audioconvert are already linked";
		return;

	}

	GstPadLinkReturn s = gst_pad_link(new_pad, ac_pad);
	if(s != GST_PAD_LINK_OK) ENGINE_DEBUG << " Could not link decoder with audioconvert";
}




GSTConvertPipeline::GSTConvertPipeline(QObject *parent)
{

	GstElement* tmp_pipeline;

	bool status = false;
	_pipeline = 0;
	_position = 0;
	_duration = 0;

	do{

		tmp_pipeline = gst_pipeline_new("ConvertPipeline");
		_test_and_error(tmp_pipeline, "Engine: Covert Pipeline sucks");
		_bus = gst_pipeline_get_bus(GST_PIPELINE(tmp_pipeline));

		_audio_src = gst_element_factory_make("uridecodebin", "src");
		_audio_convert = gst_element_factory_make("audioconvert", "audio_convert");
		_audio_sink = gst_element_factory_make("filesink", "filesink");
		_lame = gst_element_factory_make("lame", "lame");

		if(!_test_and_error(_bus, "CvtEngine: Somethink went wrong with the bus")) break;
		if(!_test_and_error(_audio_src, "CvtEngine: Source creation fail")) break;
		if(!_test_and_error(_lame, "CvtEngine: Lame  failed")) break;
		if(!_test_and_error(_audio_sink, "CvtEngine: Cannot create audio sink")) break;
		if(!_test_and_error(_audio_convert, "CvtEngine: Cannot create audio convert")) break;

		gst_bin_add_many(GST_BIN(tmp_pipeline), _audio_src, _audio_convert, _audio_sink);

		gst_bin_link_many(_audio_convert, _audio_sink, NULL);
		g_signal_connect (_audio_src, "pad-added", G_CALLBACK (pad_added_handler), _audio_convert);
		status = true;

	} while(0);

	set_quality(LameBitrate_192);

	if(status)
		_pipeline = tmp_pipeline;
}


GSTConvertPipeline::~GSTConvertPipeline(){

}



bool GSTConvertPipeline::set_uri(gchar* uri){

	if(!uri) return false;

	ENGINE_DEBUG << "Pipeline: " << uri;

	g_object_set(G_OBJECT(_audio_src), "uri", uri, NULL);
	gst_element_set_state(_pipeline, GST_STATE_PAUSED);

	return true;
}

bool GSTConvertPipeline::set_target_uri(gchar* uri){
	g_object_set(G_OBJECT(_audio_sink), "location", uri, NULL);
}



void GSTConvertPipeline::play(){
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
	g_timeout_add(200, (GSourceFunc) show_position, this);
}



void GSTConvertPipeline::pause(){
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}


void GSTConvertPipeline::stop(){
	_duration = 0;
	_uri = 0;

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
}


qint64 GSTConvertPipeline::get_duration_ms(){

	if(_duration != 0) return _duration;

	gint64 duration=0;
	bool success = false;
	GstState state = get_state();

	if(state == GST_STATE_PAUSED || state == GST_STATE_PLAYING)
		success = gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &duration);

	if(!success ) {
		_duration = 0;
		return -1;
	}

	_duration = (qint64) (duration / MIO);

	return _duration;
}



qint64 GSTConvertPipeline::get_position_ms(){

	gint64 position=0;
	bool success = false;
	GstState state = get_state();

	success = gst_element_query_position(_pipeline, GST_FORMAT_TIME, &position);

	if(!success ) {
		_position = 0;
		return -1;
	}

	_position = (qint64) (position / MIO);

	return _position;
}



guint GSTConvertPipeline::get_bitrate(){

	GstTagList *tags=NULL;
	guint rate=0;

	bool success = false;

	if(tags){

		success = gst_tag_list_get_uint (tags, GST_TAG_BITRATE, &rate);

		if(success){
			qDebug() << "tags there, bitrate = " << rate;
			return rate;
		}
	}

	return 0;
}


void GSTConvertPipeline::set_quality(LameBitrate quality){

	int cbr=-1;
	double vbr=-1.0;

	switch(quality){

		case
		case LameBitrate_64:
		case LameBitrate_128:
		case LameBitrate_192:
		case LameBitrate_256:
		case LameBitrate_320:
			cbr = (int) quality;
			break;

		case LameBitrate_var_0:
		case LameBitrate_var_1:
		case LameBitrate_var_2:
		case LameBitrate_var_3:
		case LameBitrate_var_4:
		case LameBitrate_var_5:
		case LameBitrate_var_6:
		case LameBitrate_var_7:
		case LameBitrate_var_8:
		case LameBitrate_var_9:
			vbr = (double) ((int) quality);
			break;

		default:
			cbr = LameBitrate_192;
			break;
	}


	if(cbr > 0){

		g_object_set(_lame,
					 "cbr", true,
					 "bitrate", cbr,
					 NULL);
	}

	else {
		g_object_set(_lame,
					 "cbr", false,
					 "quality", cbr,
					 NULL);
	}
}
