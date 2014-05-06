#include "HelperStructs/Helper.h"
#include "HelperStructs/Tagging/id3.h"
#include "Engine/GStreamer/GSTConvertEngine.h"

#include <qplugin.h>


GSTConvertEngine::GSTConvertEngine(QObject *parent)
{
	_settings = CSettingsStorage::getInstance();
	_pipeline = new GSTConvertPipeline(this);
	_name = CONVERT_ENGINE;

	connect(_pipeline, SIGNAL(sig_pos_changed_ms(qint64)), this, SLOT(set_cur_position_ms(qint64)));
}

void GSTConvertEngine::init(){}


// methods
bool GSTConvertEngine::set_uri(const MetaData& md, bool* start_play){

	// Gstreamer needs an URI
	gchar* uri = NULL;
	gchar* target_uri = NULL;
	QString target_uri_str;
	bool success = false;

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


	QString filename = Helper::get_filename_of_path(md.filepath);
	int idx = filename.lastIndexOf(".");
	if(idx > 0){
		filename = filename.left(idx);
	}

	filename = _settings->getConvertTgtPath() + "/" + filename + ".mp3";


	target_uri = g_filename_from_utf8(filename.toUtf8(),
							   filename.toUtf8().size(), NULL, NULL, NULL);

	ENGINE_DEBUG << "Set Uri current pipeline: " << uri;
	success = _pipeline->set_uri(uri);
	_pipeline->set_target_uri(target_uri);

	_md_target = md;
	_md_target.filepath = filename;

	return success;
}

void GSTConvertEngine::change_track(const MetaData& md, int pos_sec, bool start_play){

	stop();
	_md = md;
	emit sig_dur_changed_ms(_md.length_ms);
	emit sig_pos_changed_s(0);

	set_uri(md, &start_play);

	play();

}

void GSTConvertEngine::change_track(const QString& str, int pos_sec, bool start_play){

}

void GSTConvertEngine::play(){

	_pipeline->play();
	_state = StatePlay;
	g_timeout_add(200, (GSourceFunc) PipelineCallbacks::show_position, this);
}

void GSTConvertEngine::pause(){
	return;
}

void GSTConvertEngine::stop(){

	_pipeline->stop();

	_state = StateStop;
	ID3::setMetaDataOfFile(_md_target);
}

// public from Gstreamer Callbacks
void GSTConvertEngine::set_track_finished(){
	emit sig_track_finished();
}

void GSTConvertEngine::set_cur_position_ms(qint64 v){
	emit sig_pos_changed_s((quint32) v / 1000);
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


