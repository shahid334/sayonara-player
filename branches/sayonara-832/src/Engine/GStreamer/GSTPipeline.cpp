/* GSTPipeline.cpp */

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



#include "Engine/GStreamer/GSTPipeline.h"



bool
_test_and_error(void* element, QString errorstr) {

	if(!element) {
		qDebug() << errorstr;
		return false;
	}

	return true;
}

bool
_test_and_error_bool(bool b, QString errorstr) {
	if(!b) {
		qDebug() << errorstr;
		return false;
	}

	return true;
}


void PipelineCallbacks::pad_added_handler(GstElement *src, GstPad *new_pad, gpointer data) {
	GstElement* ac = (GstElement*) data;
	GstPad* ac_pad = gst_element_get_static_pad(ac, "sink");

	if(ac_pad && gst_pad_is_linked(ac_pad)) {
		ENGINE_DEBUG << " Decoder and tee are already linked";
		return;
	}

	GstPadLinkReturn s = gst_pad_link(new_pad, ac_pad);
	if(s != GST_PAD_LINK_OK) ENGINE_DEBUG << " Could not link decoder with tee";
}

gboolean PipelineCallbacks::show_position(gpointer data) {

	gint64 pos;
	GstState state;
	GSTAbstractPipeline* pipeline = (GSTAbstractPipeline*) data;
	GstElement* p = pipeline->get_pipeline();


	if(!p) return false;
	state = pipeline->get_state();

	if(state != GST_STATE_PLAYING && state != GST_STATE_PAUSED) return false;

	ENGINE_DEBUG;

#if GST_CHECK_VERSION(1, 0, 0)
    gst_element_query_position(p, GST_FORMAT_TIME, &pos);
#else
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_position(p, &format, &pos);
#endif

	pipeline->refresh_cur_position(pos / MIO);

	return true;
}


void GSTAbstractPipeline::refresh_cur_position(gint64 cur_pos_ms) {
	_position = cur_pos_ms;

	gint64 difference = get_duration_ms() - _position;

	if(_duration >= 0)
		emit sig_pos_changed_ms((qint64) (cur_pos_ms));

	if(difference < 500 && difference > 0) {
		emit sig_about_to_finish(difference - 50);
		_duration = -1;
	}
}


void GSTAbstractPipeline::about_to_finish() {
	gint64 position;
	gint64 duration;
	qint64 time2go;




#if GST_CHECK_VERSION(1, 0, 0)
    gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &duration);
	gst_element_query_position(_pipeline, GST_FORMAT_TIME, &position);

#else
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_duration(_pipeline, &format, &duration);
	gst_element_query_position(_pipeline, &format, &position);

#endif



	time2go = (duration - position) / MIO;

	// ms
	emit sig_about_to_finish(time2go);
}

void GSTAbstractPipeline::set_speed(float f) {
	Q_UNUSED(f);
}

void GSTAbstractPipeline::finished() {
	emit sig_finished();
}


GstBus* GSTAbstractPipeline::get_bus() {
	return _bus;
}


GstState GSTAbstractPipeline::get_state() {
	GstState state;
	gst_element_get_state(_pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
	return state;
}


GstElement* GSTAbstractPipeline::get_pipeline() {
	return _pipeline;
}

bool GSTAbstractPipeline::set_uri(gchar* uri) {
	_uri = uri;
	return (_uri != NULL);
}

gchar* GSTAbstractPipeline::get_uri() {
	return _uri;
}

