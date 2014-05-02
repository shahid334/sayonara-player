#include "Engine/GStreamer/GSTConvertEngine.h"

GSTConvertEngine::GSTConvertEngine(QObject *parent) :
	Engine(parent)
{
	_settings = CSettingsStorage::getInstance();
	_pipeline = new GSTConvertPipeline(this);
}

void GSTConvertEngine::init(){}


// methods
bool GSTConvertEngine::set_uri(const MetaData& md, bool* start_play){

	// Gstreamer needs an URI
	gchar* uri = NULL;
	gchar* target_uri = NULL;

	_playing_stream = Helper::is_www(md.filepath);

	if (_playing_stream) {
		uri = g_filename_from_utf8(md.filepath.toUtf8(),
								   md.filepath.toUtf8().size(), NULL, NULL, NULL);
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!md.filepath.contains("://")) {

		uri = g_filename_to_uri(md.filepath.toLocal8Bit(), NULL, NULL);
	}

	else {
		uri = g_filename_from_utf8(md.filepath.toUtf8(),
								   md.filepath.toUtf8().size(), NULL, NULL, NULL);
	}

	ENGINE_DEBUG << "Set Uri current pipeline: " << uri;
	success = _pipeline->set_uri(uri);

	return success;
}

void GSTConvertEngine::change_track(const MetaData&, int pos_sec=-1, bool start_play=true){


	gst_obj_ref = this;
	_md = md;
	emit sig_dur_changed_ms(_md.length_ms);
	emit sig_pos_changed_s(0);

	play();

}

void GSTConvertEngine::change_track(const QString&, int pos_sec=-1, bool start_play=true ){

}

void GSTConvertEngine::play(){

	_pipeline->play();
	_state = StatePlay;
}

void GSTConvertEngine::pause(){
	return;
}

void GSTConvertEngine::stop(){

	_pipeline->stop();
	_state = StateStop;
}

// public from Gstreamer Callbacks
void GSTConvertEngine::set_track_finished(){
	emit sig_track_finished();
}

void GSTConvertEngine::set_cur_position_ms(qint64 v){
	emit sig_pos_changed_ms((quint64) v);
}


void GSTConvertEngine::set_volume(int vol){
	Q_UNUSED(vol);
}

void GSTConvertEngine::jump_abs_s(quint32 v){
	Q_UNUSED(v);
}

void GSTConvertEngine::jump_abs_ms(quint64 v){
	Q_UNUSED(v);
}

void GSTConvertEngine::jump_rel(quint32 v){
	Q_UNUSED(v);
}



