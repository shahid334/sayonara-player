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
_test_and_error(void* element, QString errorstr){

	if(!element){
		qDebug() << errorstr;
		return false;
	}

	return true;
}

bool
_test_and_error_bool(bool b, QString errorstr){
	if(!b){
		qDebug() << errorstr;
		return false;
	}

	return true;
}








void GSTAbstractPipeline::refresh_cur_position(gint64 cur_pos_ms){
	_position = cur_pos_ms;

	gint64 difference = get_duration_ms() - _position;

	if(_duration >= 0)
		emit sig_cur_pos_changed((qint64) (cur_pos_ms));

	if(difference < 500 && difference > 0) {
		emit sig_about_to_finish(difference - 50);
		_duration = -1;
	}
}


void GSTAbstractPipeline::about_to_finish(){
	gint64 position;
	gint64 duration;
	qint64 time2go;

	gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &duration);
	gst_element_query_duration(_pipeline, GST_FORMAT_TIME, &position);

	time2go = (duration - position) / MIO;

	// ms
	emit sig_about_to_finish(time2go);
}


GstBus* GSTAbstractPipeline::get_bus(){
	return _bus;
}


GstState GSTAbstractPipeline::get_state(){
	GstState state;
	gst_element_get_state(_pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
	return state;
}


GstElement* GSTAbstractPipeline::get_pipeline(){
	return _pipeline;
}

bool GSTAbstractPipeline::set_uri(gchar* uri){
	_uri = uri;
	return (_uri != NULL);
}

gchar* GSTAbstractPipeline::get_uri(){
	return _uri;
}

