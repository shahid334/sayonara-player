/*
 * GSTEngine.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: luke
 */

#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include "MP3_Listen/GSTEngine.h"

#include <gst/gst.h>
#include <string>

#include <QObject>
#include <QDebug>
#include <QString>

using namespace std;


static GMainLoop*	_loop;
static GST_Engine*	obj_ref;


static gboolean show_position(GstElement* pipeline){
	qint64 pos;
	GstFormat fmt = GST_FORMAT_TIME;
	gst_element_query_position(pipeline, &fmt, &pos);

	if(obj_ref != NULL && obj_ref->getState() == STATE_PLAY){
		obj_ref->set_cur_position((quint32)(pos / 1000000000));
	}
}



static gboolean bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data){

	(void) bus;
	(void) user_data;
	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS:
			qDebug("End-of-stream");
			g_main_loop_quit(_loop);
		break;

		case GST_MESSAGE_ERROR:
			GError *err;
			gst_message_parse_error(msg, &err, NULL);
			qDebug() << err->message;
			g_error_free(err);

			g_main_loop_quit(_loop);
			break;

		default:
			GST_Engine* eng = (GST_Engine*) user_data;
			eng->state_changed();
			break;
    }

    return true;
}




GST_PipelineThread::GST_PipelineThread(){

}

GST_PipelineThread::~GST_PipelineThread(){
	if(this->isRunning()) this->quit();
}

void GST_PipelineThread::run(){
	g_main_loop_run (_loop);
}





/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

GST_Engine::GST_Engine(QObject* parent) : QObject(parent){
	// TODO Auto-generated constructor stub
	//gst_init(0, 0);

	gst_init(0, 0);
	_state = STATE_STOP;

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	_loop = g_main_loop_new(NULL, FALSE);
		if(!_loop){
			qDebug() << "Cannot init loop";

		}

	_volume = gst_element_factory_make("playbin", "volume");
	if(!_volume){
		qDebug() << "cannot init volume";
	}


	_pipeline = gst_element_factory_make("playbin", "player");
	if(!_pipeline){
		qDebug() << "Cannot init Pipeline";
		return;
	}

	_thread = new GST_PipelineThread();



}

GST_Engine::~GST_Engine() {
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (_pipeline));

	if(_thread->isRunning())
		_thread->quit();

	delete _thread;
}


bool GST_Engine::init(){
	return true;
}


void GST_Engine::play(){

	_state = STATE_PLAY;
	_thread->start();
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
	obj_ref = this;

}

void GST_Engine::stop(){
	_state = STATE_STOP;
	_thread->quit();
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);

	//gst_object_unref(GST_OBJECT(_pipeline));
}

void GST_Engine::pause(){
	_state = STATE_PAUSE;
	qDebug() << "pause";
	//g_main_loop_quit(_loop);
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);


}

void GST_Engine::setVolume(qreal vol){
	if(!_volume) return;
	float vol_val = (float) vol / 100.0f;
	g_object_set(G_OBJECT(_pipeline), "volume", vol_val, NULL);

}

/**
* TODO: Where in what? Percent, seconds or egss?
*/
void GST_Engine::jump(int where, bool percent){

	Q_UNUSED(percent);


	_seconds_started = where * _meta_data.length_ms / 100;


	qint64 new_time_ns = where * _meta_data.length_ms * 10000; // nanoseconds
	if(!gst_element_seek_simple(_pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, new_time_ns)){
		qDebug() << "seeking failed";
	}

}

void GST_Engine::changeTrack(const MetaData& md){

	stop();
	_meta_data = md;
	QString filename = _meta_data.filepath;
	obj_ref = NULL;

	filename.push_front(QString("file://"));

	g_object_set(G_OBJECT(_pipeline), "uri", filename.toLocal8Bit().data(), NULL);
	g_timeout_add (200, (GSourceFunc) show_position, _pipeline);

	_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));

	gst_bus_add_watch(_bus, bus_state_changed, this);
	gst_object_unref(_bus);

	emit total_time_changed_signal(_meta_data.length_ms);

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	play();

}


void GST_Engine::changeTrack(const QString& filepath){

	//stop();
	MetaData md = ID3::getMetaDataOfFile(filepath);
	changeTrack(md);
	return;
	obj_ref = NULL;
	QString filename = filepath;
	filename.push_front(QString("file://"));

	_meta_data = md;

	g_object_set(G_OBJECT(_pipeline), "uri", filename.toStdString().c_str(), NULL);
	g_timeout_add (200, (GSourceFunc) show_position, _pipeline);

	_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));

	gst_bus_add_watch(_bus, bus_state_changed, this);
	gst_object_unref(_bus);

	emit total_time_changed_signal(_meta_data.length_ms);

	play();

}

void GST_Engine::eq_changed(int, int){

}

void GST_Engine::eq_enable(bool){

}

void GST_Engine::state_changed(){

}

void GST_Engine::set_cur_position(quint32 pos){

	_seconds_now = pos;

	if (!_scrobbled
			&& (_seconds_now - _seconds_started == 20
					|| _seconds_now - _seconds_started
					== _meta_data.length_ms / 2000)) {
		qDebug() << "scrobble track";
		emit
		scrobble_track(_meta_data);
		_scrobbled = true;

		qDebug() << "scrobble track end";
	}

	emit timeChangedSignal(pos);
}

int GST_Engine::getState(){
	return _state;
}
