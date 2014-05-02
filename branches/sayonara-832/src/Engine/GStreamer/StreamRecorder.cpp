/* StreamRecorder.cpp */

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



#include "Engine/GStreamer/StreamRecorder.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/PlaylistParser.h"
#include "HelperStructs/Tagging/id3.h"

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsrc.h>

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDateTime>

static bool _sr_debug = true;
#define SR_DEBUG if(_sr_debug) qDebug() << "SR: " << Q_FUNC_INFO


StreamRecorder* obj_ref = 0;

static QString get_time_str(){


    QString time_str;
    QDateTime cur = QDateTime::currentDateTime();

    QString weekday = cur.date().shortDayName(cur.date().dayOfWeek() );
    QString year = QString::number(cur.date().year());
    QString month = cur.date().shortMonthName(cur.date().month());
    QString day = QString::number(cur.date().day());
    QString hr = QString::number(cur.time().hour());
    QString min = QString::number(cur.time().minute());

    time_str = weekday + "_" + year + "-" + month + "-" + day + "_" + hr + ":" + min;
    return time_str;
}

static gboolean bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data){

    (void) bus;
    (void) user_data;

    switch (GST_MESSAGE_TYPE(msg)) {

        case GST_MESSAGE_EOS:
            if(obj_ref) obj_ref->endOfStream();
			break;

        case GST_MESSAGE_ERROR:
            GError *err;

            gst_message_parse_error(msg, &err, NULL);

			SR_DEBUG << "SR: GST_MESSAGE_ERROR: " << err->message << ": " << GST_MESSAGE_SRC_NAME(msg);

            g_error_free(err);

            break;

		case GST_MESSAGE_BUFFERING:
			gint percent;
			gst_message_parse_buffering(msg, &percent);
			SR_DEBUG << "SR: Buffering... " << percent;
			break;

		case GST_MESSAGE_STREAM_STATUS:
			GstStreamStatusType type;
			gst_message_parse_stream_status(msg, &type, NULL);
			SR_DEBUG << "Stream status = " << type;
			break;

		case GST_MESSAGE_DURATION:
			GstFormat format;
			gint64 duration;

			format=GST_FORMAT_TIME;
			duration = 0;

			gst_message_parse_duration(msg, &format, &duration);
			SR_DEBUG << "New duration: " << duration;
			break;

        default:
            break;
    }

    return true;
}


StreamRecorder::StreamRecorder(QObject *parent) :
	QObject(parent), _session_collector(0)
{
    _buffer_size = 32767;
    _stream_ended = true;
    _settings = CSettingsStorage::getInstance();
    _rec_pipeline = NULL;

    _try = 2;
    _session_path = get_time_str();
    _sr_thread = 0;
    _thread_is_running = false;

    QDir d(Helper::getSayonaraPath());

    // delete old stream ripper files
    QStringList lst = d.entryList(Helper::get_soundfile_extensions());
    foreach(QString str, lst){
        qDebug() << "Remove " << d.absolutePath() + QDir::separator() + str;
        QFile f(d.absolutePath() + QDir::separator() + str);
        f.remove();
    }

    obj_ref = this;
}


StreamRecorder::~StreamRecorder(){
    if(_sr_thread){
        delete _sr_thread;
        _sr_thread = NULL;
    }
}


void StreamRecorder::init(){

        /*
     * _rec_pipeline:
     *
     * _rec_src -> _rec_cvt -> _rec_enc -> _rec_dst
     *
     */

    SR_DEBUG;

    int i = 0;
    do{
        _rec_pipeline = gst_pipeline_new("rec_pipeline");
        _bus = gst_pipeline_get_bus(GST_PIPELINE(_rec_pipeline));
        _rec_src = gst_element_factory_make("souphttpsrc", "rec_uri");
        _rec_cvt = gst_element_factory_make("audioconvert", "rec_cvt");
        _rec_enc = gst_element_factory_make("lamemp3enc", "rec_enc");
        _rec_dst = gst_element_factory_make("filesink", "rec_sink");

        if(!_rec_pipeline) {
            qDebug() << "SR: pipeline error";
            break;
        }

        if(!_rec_src) {
            qDebug() << "SR: src error";
            break;
        }

        if(!_rec_cvt) {
            qDebug() << "SR: cvt error";
            break;
        }

        if(!_rec_enc) {
            qDebug() << "SR: enc error";
            break;
        }

        if(!_rec_dst) {
            qDebug() << "SR: sink error";
            break;
        }

        gst_bin_add_many(GST_BIN(_rec_pipeline), _rec_src, /*_rec_cvt, _rec_enc,*/ _rec_dst, NULL);
        gst_element_link( _rec_src, /*_rec_cvt, _rec_enc,*/ _rec_dst);


    } while(i);

    if(!_rec_src ||
       !_rec_pipeline ||
       !_rec_cvt ||
       !_rec_enc ||
       !_rec_dst)
       _initialized = false;

    else gst_bus_add_watch(_bus, bus_state_changed, this);

    obj_ref = this;
}



bool StreamRecorder::init_thread(QString filename){
SR_DEBUG;

    if(_sr_thread) {

        disconnect(_sr_thread, SIGNAL(finished()), this, SLOT(thread_finished()));

        if(_sr_thread->isRunning())
            _sr_thread->terminate();

        delete _sr_thread;
        _sr_thread = 0;
    }

    _sr_thread = new StreamRipperBufferThread();

    if(!_sr_thread) return false;

    _sr_thread->setUri(filename);
    _sr_thread->setBufferSize(_buffer_size);

    connect(_sr_thread, SIGNAL(finished()), this, SLOT(thread_finished()));
    return true;

}

bool StreamRecorder::terminate_thread_if_running(){
    SR_DEBUG;
    if(_sr_thread){
        if(_sr_thread->isRunning()){
            _sr_thread->terminate();
        }
    }

    _thread_is_running = false;
    return true;
}




void StreamRecorder::set_new_stream_session(){

    SR_DEBUG;
    _session_path = get_time_str();
    _session_collector.clear();

    QString sr_path = _settings->getStreamRipperPath();
    QString session_path = check_session_path(sr_path);
    qDebug() << "session_path = " << session_path;

    _session_playlist_name = session_path + QDir::separator() + get_time_str() + ".m3u";
    qDebug() << "session playlist name = " << _session_playlist_name;
}


QString StreamRecorder::changeTrack(const MetaData& md, int trys){

    SR_DEBUG;

	_md = md;
    _max_tries = trys;
    _stream_ended = true;
    _try = 0;

    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_NULL);


    // stream file to _sr_recording_dst
    QString title = _md.title;
    QString org_src_filename = _md.filepath;

    title.replace(" ", "_");
    title.replace("/", "_");
    title.replace("\\", "_");

    if(Helper::is_soundfile(md.filepath))
        _sr_recording_dst = Helper::getSayonaraPath() + title + ".mp3";
    else
        _sr_recording_dst = Helper::getSayonaraPath() + title + "_" + QDateTime::currentDateTime().toString("yyMMdd_hhmm") + ".mp3";

    // record from org_src_filename to new_src_filename
    g_object_set(G_OBJECT(_rec_src), "location", org_src_filename.toLocal8Bit().data(), NULL);
    g_object_set(G_OBJECT(_rec_dst), "location", _sr_recording_dst.toLocal8Bit().data(), NULL);
    g_object_set(G_OBJECT(_rec_src), "blocksize", _buffer_size, NULL);
    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_READY);



    _stream_ended = false;
    bool success = init_thread(_sr_recording_dst);

    if(success){
        gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_PLAYING);


        _sr_thread->start();
        _thread_is_running = true;
        return _sr_recording_dst;
    }

    return "";
}


bool StreamRecorder::stop(bool delete_track){

    SR_DEBUG;

    bool complete_tracks = _settings->getStreamRipperCompleteTracks();
    bool save_success = true;

    _stream_ended = true;
    terminate_thread_if_running();

	gint64 duration=0;
	bool success = false;
	success = gst_element_query_duration(_rec_pipeline, GST_FORMAT_TIME, &duration);
	qDebug() << "SR: Stream length: " << duration << ": " << success;


    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_READY);


    if( (!_stream_ended && complete_tracks) || delete_track){
        qDebug() << "Remove w/o saving";
        QFile::remove(_sr_recording_dst);
        return false;
    }

    save_success = save_file();
    QFile::remove(_sr_recording_dst);

    if(!save_success)
        return false;

    return true;
}


bool StreamRecorder::save_file(){

    SR_DEBUG;

    QString sr_path = _settings->getStreamRipperPath();
    QString session_path = check_session_path(sr_path);

    QDir dir(session_path);
        dir.mkdir(_md.artist);
        dir.cd(_md.artist);


    if(!QFile::exists(_sr_recording_dst)){
        qDebug() << "SR: " << _sr_recording_dst << " does not exist";
        return false;
    }

    QFile f(_sr_recording_dst);

    // remove directories in front of filename
    QString src_name = f.fileName();
    QString fname_wo_path = src_name.right( src_name.size() - src_name.lastIndexOf(QDir::separator()) );
    QString dst_name = 	dir.path() + QDir::separator() + fname_wo_path;

    bool success = 	f.copy(dst_name);

    if(!success){
        qDebug() << "SR: unable to copy " <<  _sr_recording_dst << " to " << dir.path() + QDir::separator() + fname_wo_path;
        return false;
    }


    qDebug() << "SR: Remove File w save" << f.fileName() << " -> " << dst_name;
    _md.filepath = dst_name;
    ID3::setMetaDataOfFile(_md);

    _session_collector.push_back(_md);

    qDebug() << "save playlist as " << _session_playlist_name;
    PlaylistParser::save_playlist(_session_playlist_name, _session_collector, true);

    return success;
}



void StreamRecorder::thread_finished(){

    SR_DEBUG;

    _thread_is_running = false;
    qint64 size = _sr_thread->getSize();
    qDebug() << "Thread finished " << size;

    if(!QFile::exists(_sr_recording_dst)){
        qDebug() << "SR: Stream not valid (File not existent)";

        _stream_ended = true;

        emit sig_stream_not_valid();
    }


    else if( size < _buffer_size ){

        _stream_ended = true;
        _try++;

        if(_try < _max_tries || _max_tries == -1) {
            qDebug() << "SR: Try to buffer once more";
            _sr_thread->start();
            _thread_is_running = true;
        }

        else{
            qDebug() << "SR: Tried so hard to buffer, but not successful :'-(";
            emit sig_stream_not_valid();
        }
    }

    else
        emit sig_initialized(true);
}


void StreamRecorder::endOfStream(){

    SR_DEBUG;

    if(_thread_is_running) return;

	_stream_ended = true;
    emit sig_stream_ended();
}


QString StreamRecorder::check_session_path(QString sr_path){

    SR_DEBUG;

    bool create_session_path = _settings->getStreamRipperSessionPath();
    if(!create_session_path) return sr_path;

    if(!QFile::exists(sr_path + QDir::separator() + _session_path)){

        QDir dir(sr_path);
        dir.mkdir(_session_path);
    }

    return sr_path + QDir::separator() + _session_path;
}


bool StreamRecorder::getFinished(){

    return _stream_ended;
}
