/*
 * GSTEngine.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: luke
 */

#include "HelperStructs/MetaData.h"
#include "HelperStructs/id3.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "MP3_Listen/Engine.h"
#include "MP3_Listen/GSTEngine.h"


#include <gst/gst.h>
#include <string>
#include <vector>

#include <QObject>
#include <QDebug>
#include <QString>

using namespace std;

static GST_Engine*	obj_ref;


static gboolean show_position(GstElement* pipeline){
	qint64 pos;
	GstFormat fmt = GST_FORMAT_TIME;
	gst_element_query_position(pipeline, &fmt, &pos);

	if(obj_ref != NULL && obj_ref->getState() == STATE_PLAY){
		obj_ref->set_cur_position((quint32)(pos / 1000000000));
	}

	return true;
}

static gboolean bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data){

	(void) bus;
	(void) user_data;
	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_EOS:
			if(obj_ref){
				obj_ref->set_track_finished();
			}
		break;

		case GST_MESSAGE_ERROR:
			GError *err;

			gst_message_parse_error(msg, &err, NULL);
			qDebug() << "GST_MESSAGE_ERROR: " << err->message << ": " << GST_MESSAGE_SRC_NAME(msg);
			g_error_free(err);

			break;

		default:
			obj_ref->state_changed();
			break;
    }

    return true;
}





/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

GST_Engine::GST_Engine(){

	gst_init(0, 0);
	_state = STATE_STOP;

	_seconds_started = 0;
	_seconds_now = 0;
	_scrobbled = false;

	_pipeline = gst_element_factory_make("playbin2", "player");

	if(!_pipeline){
		qDebug() << "Cannot init Pipeline";

	}

	g_timeout_add (200, (GSourceFunc) show_position, _pipeline);
	_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
	if(!_bus){
		qDebug() << "Something went wrong with the bus";
	}

	gst_bus_add_watch(_bus, bus_state_changed, this);
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_READY);


	bool success = false;
	int i=0;
	do{
		_equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");
		if(!_equalizer){
			qDebug() << "Equalizer cannot be created";
			break;
		}

		_audio_sink = gst_element_factory_make("autoaudiosink", "alsa-sink");
		if(!_audio_sink){
			qDebug() << "Sink cannot be created";
			break;
		}

		_audio_bin = gst_bin_new("audio-bin");
		if(!_audio_bin){
			qDebug() << "Bin cannot be created";
			break;
		}

		gst_bin_add_many(GST_BIN(_audio_bin), _equalizer, _audio_sink, NULL);

		_audio_pad = gst_element_get_static_pad(_equalizer, "sink");
		if(!_audio_pad){
			qDebug() << "Pad cannot be fetched";
		}

		success = gst_element_add_pad(GST_ELEMENT(_audio_bin),  gst_ghost_pad_new("sink", _audio_pad));

	} while(i);

	if(success){
		g_object_set(G_OBJECT(_pipeline), "audio-sink", _audio_bin, NULL);
		success = gst_element_link(_equalizer, _audio_sink);
	}

}

GST_Engine::~GST_Engine() {
	gst_object_unref(_bus);
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (_pipeline));
	obj_ref = 0;
}



void GST_Engine::play(){

	_state = STATE_PLAY;
	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

	obj_ref = this;

}

void GST_Engine::stop(){
	_state = STATE_STOP;

	gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
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

void GST_Engine::load_equalizer(){

	CSettingsStorage * settings = CSettingsStorage::getInstance();
	vector<EQ_Setting> vec;
	settings->getEqualizerSettings(vec);

	//emit eq_found(availableEqualizers);
	emit eq_presets_loaded(vec);

}

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

	if(filename.toLower().left(4).compare("http") != 0)
		filename.push_front(QString("file://"));

	g_object_set(G_OBJECT(_pipeline), "uri", filename.toLocal8Bit().data(), NULL);
	g_timeout_add (500, (GSourceFunc) show_position, _pipeline);


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

	if(filename.toLower().left(4).compare("http") != 0)
		filename.push_front(QString("file://"));

	_meta_data = md;

	g_object_set(G_OBJECT(_pipeline), "uri", filename.toLocal8Bit().data(), NULL);

	emit total_time_changed_signal(_meta_data.length_ms);

	play();

}

void GST_Engine::eq_changed(int band, int val){

	double new_val = 0;
	new_val = val * 1.0;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else
		new_val = val * 0.75;

	char band_name[6];
	sprintf(band_name, "band%d", band);
	band_name[5] = '\0';

	g_object_set(G_OBJECT(_equalizer), band_name, new_val, NULL);
}

void GST_Engine::eq_enable(bool){

}

void GST_Engine::state_changed(){

}

void GST_Engine::set_cur_position(quint32 pos){

	_seconds_now = pos;

	if (!_scrobbled
			&& (_seconds_now - _seconds_started == 15
					|| _seconds_now - _seconds_started
					== _meta_data.length_ms / 2000)) {

		emit
		scrobble_track(_meta_data);
		_scrobbled = true;
	}

	emit timeChangedSignal(pos);
}

void GST_Engine::set_track_finished(){
	emit track_finished();
}

