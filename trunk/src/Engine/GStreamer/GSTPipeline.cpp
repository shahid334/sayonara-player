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
#include <gst/app/gstappsink.h>



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
        ENGINE_DEBUG << " Pads are already linked";
		return;
	}

	GstPadLinkReturn s = gst_pad_link(new_pad, ac_pad);
	if(s != GST_PAD_LINK_OK) {
        ENGINE_DEBUG << " Could not link pads";
		qDebug() << "Cannot link Error ";
		switch(s){
			case GST_PAD_LINK_WRONG_HIERARCHY:
				qDebug() << "Cause: Wrong hierarchy";
				break;

			case GST_PAD_LINK_WAS_LINKED:
				qDebug() << "Cause: Pad was already linked";
				break;
			case GST_PAD_LINK_WRONG_DIRECTION:
				qDebug() << "Cause: Pads have wrong direction";
				break;
			case GST_PAD_LINK_NOFORMAT:
				qDebug() << "Cause: Pads have incompatible format";
				break;
			case GST_PAD_LINK_NOSCHED:
				qDebug() << "Cause: Pads cannot cooperate scheduling";
				break;
			case GST_PAD_LINK_REFUSED:
			default:
				qDebug() << "Cause: Refused because of different reason";
				break;
		}
	}
}

gboolean PipelineCallbacks::show_position(gpointer data) {

	GstState state;
	GSTAbstractPipeline* pipeline = (GSTAbstractPipeline*) data;
	GstElement* p = pipeline->get_pipeline();

	if(!p) return false;
	state = pipeline->get_state();

	if( state != GST_STATE_PLAYING &&
		state != GST_STATE_PAUSED &&
		state != GST_STATE_READY) return false;

	ENGINE_DEBUG;

	//pipeline->refresh_duration();
	pipeline->refresh_position();
	pipeline->check_about_to_finish();

	return true;
}
//#define TCP_BUFFER_SIZE 32768


#define TCP_BUFFER_SIZE 16384
static uchar data[TCP_BUFFER_SIZE];

GstFlowReturn PipelineCallbacks::new_buffer(GstElement *sink, gpointer p){

	GstSample* sample;
	GstBuffer* buffer;
	gsize size = 0;
    gsize size_new = 0;

	GSTAbstractPipeline* pipeline = (GSTAbstractPipeline*) p;

	sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
	if(!sample) {
		qDebug() << "No sample";
		return GST_FLOW_OK;
	}

	buffer = gst_sample_get_buffer(sample);
	if(!buffer) {
		qDebug() << "No buffer";
		return GST_FLOW_OK;
	}

	size = gst_buffer_get_size(buffer);
	size_new = gst_buffer_extract(buffer, 0, data, size);
	pipeline->set_data(data, size_new);

	gst_buffer_unref(buffer);

	return GST_FLOW_OK;
}


GSTAbstractPipeline::GSTAbstractPipeline(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
    _position_ms = 0;
    _duration_ms = 0;
    _bus = 0;
    _pipeline = 0;
    _uri = NULL;
}

void GSTAbstractPipeline::refresh_position() {

	gint64 pos;
#if GST_CHECK_VERSION(1, 0, 0)
	gst_element_query_position(GST_ELEMENT(_pipeline), GST_FORMAT_TIME, &pos);
#else
	GstFormat format = GST_FORMAT_TIME;
	gst_element_query_position(GST_ELEMENT(_pipeline), &format, &pos);
#endif

	_position_ms = pos / GST_MSECOND;
}

void GSTAbstractPipeline::refresh_duration(){

	gint64 dur;

#if GST_CHECK_VERSION(1, 0, 0)
	gst_element_query_duration(GST_ELEMENT(_pipeline), GST_FORMAT_TIME, &dur);
#else
	GstFormat format = GST_FORMAT_TIME;
	gst_element_query_duration(GST_ELEMENT(_pipeline), &format, &dur);
#endif

	_duration_ms = dur / MIO;
}

void GSTAbstractPipeline::set_data(uchar* data, quint64 size){

	emit sig_data(data, size);
}

void GSTAbstractPipeline::check_about_to_finish(){

	gint64 difference = _duration_ms - _position_ms;

	if(_duration_ms >= 0){
		emit sig_pos_changed_ms((qint64) (_position_ms));
	}

	if(difference < 500 && difference > 0 && !_about_to_finish) {

		_about_to_finish = true;
		emit sig_about_to_finish(difference - 50);
	}

	else {
		_about_to_finish = false;
	}
}

qint64 GSTAbstractPipeline::get_duration_ms(){
    return _duration_ms;
}

qint64 GSTAbstractPipeline::get_position_ms(){
    return _position_ms;
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


