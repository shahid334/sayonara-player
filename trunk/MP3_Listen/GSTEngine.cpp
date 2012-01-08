/*
 * GSTEngine.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: luke
 */

#include "HelperStructs/MetaData.h"
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

	if(obj_ref != NULL){
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



GST_Engine::GST_Engine(QObject* parent) : QObject(parent){
	// TODO Auto-generated constructor stub
	//gst_init(0, 0);

	gst_init(0, 0);
	_state = STATE_STOP;
	_loop = g_main_loop_new(NULL, FALSE);
		if(!_loop){
			qDebug() << "Cannot init loop";

		}

	_volume = gst_element_factory_make("playbin", "volume");
	if(!_volume){
		qDebug() << "cannot init volume";
	}




}

GST_Engine::~GST_Engine() {
	// TODO Auto-generated destructor stub
}


bool GST_Engine::init(){

}


void GST_Engine::play(){

	_state = STATE_PLAY;
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
	obj_ref = this;
	g_main_loop_run(_loop);
}

void GST_Engine::stop(){
	_state = STATE_STOP;
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);

	//gst_object_unref(GST_OBJECT(_pipeline));
}

void GST_Engine::pause(){
	_state = STATE_PAUSE;
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

	qint64 new_time = where * _meta_data.length_ms * 10000;
	if(!gst_element_seek_simple(_pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, new_time)){
		qDebug() << "seeking failed";
	}

}

void GST_Engine::changeTrack(const MetaData& md){
	_meta_data = md;

	changeTrack(md.filepath);
}

void GST_Engine::changeTrack(const QString& filepath){

	stop();
	obj_ref = NULL;
		QString filename = filepath;
		filename.push_front(QString("file://"));

		_pipeline = gst_element_factory_make("playbin", "player");
		//gst_bin_add(GST_BIN(_pipeline), _volume);

		if(!_pipeline){
			qDebug() << "Cannot init Pipeline";
			return;
		}

		g_object_set(G_OBJECT(_pipeline), "uri", filename.toStdString().c_str(), NULL);
		g_timeout_add (200, (GSourceFunc) show_position, _pipeline);

		_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));



		gst_bus_add_watch(_bus, bus_state_changed, this);
		gst_object_unref(_bus);

		GstFormat fmt = GST_FORMAT_TIME;
		gint64 duration = 0;

		bool success = gst_element_query_duration(_pipeline, &fmt, &duration);
		qDebug() << "success = " << success << "; duration = " << duration;
		duration = duration / 1000;

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

	emit timeChangedSignal(pos);
}

int GST_Engine::getState(){
	return _state;
}
