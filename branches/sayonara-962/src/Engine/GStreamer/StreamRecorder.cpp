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
#include "Engine/GStreamer/GSTPipeline.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/PlaylistParser.h"
#include "HelperStructs/Tagging/id3.h"

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsrc.h>

#include <QFile>
#include <QDir>
#include <QDateTime>

static bool _sr_debug = false;
#define SR_DEBUG if(_sr_debug) qDebug() << "SR: " << Q_FUNC_INFO

static QString get_time_str() {


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

gboolean StreamRecorder::bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data) {

    (void) bus;

    StreamRecorder* obj_ref = (StreamRecorder*) user_data;

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

        default:
            break;
    }

    return true;
}


StreamRecorder::StreamRecorder(QObject *parent) :
	QObject(parent),
	SayonaraClass(),
	_session_collector(0)
{
    _buffer_size = 32767;
    _stream_ended = true;
    _rec_pipeline = NULL;

    _try = 2;
    _session_path = get_time_str();
    _sr_thread = 0;
    _thread_is_running = false;

    QDir d(Helper::getSayonaraPath());

    // delete old stream ripper files
    QStringList lst = d.entryList(Helper::get_soundfile_extensions());
	for( const QString& str : lst) {

		qDebug() << "Remove " << d.absolutePath() + QDir::separator() + str;
        QFile f(d.absolutePath() + QDir::separator() + str);
        f.remove();
    }
}


StreamRecorder::~StreamRecorder() {
    if(_sr_thread) {
        delete _sr_thread;
        _sr_thread = NULL;
    }
}


void StreamRecorder::init() {

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
        _rec_src = gst_element_factory_make("souphttpsrc", "rec_src");
        _rec_dst = gst_element_factory_make("filesink", "rec_sink");

        if(!_rec_pipeline) {
            qDebug() << "SR: pipeline error";
            break;
        }

        if(!_rec_src) {
            qDebug() << "SR: src error";
            break;
        }

        if(!_rec_dst) {
            qDebug() << "SR: sink error";
            break;
        }

        gst_bin_add_many(GST_BIN(_rec_pipeline), _rec_src, _rec_dst, NULL);
        gst_element_link_many(_rec_src, _rec_dst, NULL);
        //g_signal_connect (_rec_src, "pad-added", G_CALLBACK (PipelineCallbacks::pad_added_handler), _rec_dst);

    } while(i);

    if(!_rec_src ||
       !_rec_pipeline ||
       !_rec_dst){
       _initialized = false;
    }

    else {
        gst_bus_add_watch(_bus, StreamRecorder::bus_state_changed, this);
    }
}



bool StreamRecorder::init_thread(QString filename) {
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


bool StreamRecorder::terminate_thread_if_running() {
    SR_DEBUG;
    if(_sr_thread) {
        if(_sr_thread->isRunning()) {
            _sr_thread->terminate();
        }
    }

    _thread_is_running = false;
    return true;
}


void StreamRecorder::set_new_stream_session() {

    SR_DEBUG;
    _session_path = get_time_str();
    _session_collector.clear();

	QString sr_path =_settings->get(Set::Engine_SR_Path);
    QString session_path = check_session_path(sr_path);

    _session_playlist_name = session_path + QDir::separator() + get_time_str() + ".m3u";
}


QString StreamRecorder::changeTrack(const MetaData& md, int trys) {

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

    qDebug() << "SR: Src = " << org_src_filename;
    g_object_set(G_OBJECT(_rec_src), "location", org_src_filename.toLocal8Bit().data(), NULL);
    g_object_set(G_OBJECT(_rec_dst), "location", _sr_recording_dst.toLocal8Bit().data(), NULL);   

    _stream_ended = false;

    bool success = init_thread(_sr_recording_dst);

    if(success) {
        gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_PLAYING);

        _sr_thread->start();
        _thread_is_running = true;
        return _sr_recording_dst;
    }

    return "";
}


bool StreamRecorder::stop(bool delete_track) {

    SR_DEBUG;

    gint64 duration=0;

    bool save_success = true;

    terminate_thread_if_running();


#if GST_CHECK_VERSION(1, 0, 0)
    gst_element_query_duration(_rec_pipeline, GST_FORMAT_TIME, &duration);
#else
    GstFormat format = GST_FORMAT_TIME;
    gst_element_query_duration(_rec_pipeline, &format, &duration);
#endif

    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_READY);

	if( (!_stream_ended) || delete_track) {
        qDebug() << "Remove w/o saving";
        QFile::remove(_sr_recording_dst);
        return false;
    }

    save_success = save_file();
    QFile::remove(_sr_recording_dst);

    _stream_ended = true;

    return save_success;
}


bool StreamRecorder::save_file() {

    SR_DEBUG;

	QString sr_path = _settings->get(Set::Engine_SR_Path);

	QString session_path = check_session_path(sr_path);

    QDir dir(session_path);
        dir.mkdir(_md.artist);
        dir.cd(_md.artist);


    if(!QFile::exists(_sr_recording_dst)) {
        qDebug() << "SR: " << _sr_recording_dst << " does not exist";
        return false;
    }

    QFile f(_sr_recording_dst);

    // remove directories in front of filename
    QString src_dir, src_fname;
    QString src_name = f.fileName();

    Helper::split_filename(src_name, src_dir, src_fname);

    QString dst_name = 	dir.path() + QDir::separator() + src_fname;

    bool success = 	f.copy(dst_name);

    if(!success) {
        return false;
    }

    _md.filepath = dst_name;
    ID3::setMetaDataOfFile(_md);

    _session_collector.push_back(_md);

    PlaylistParser::save_playlist(_session_playlist_name, _session_collector, true);

    return success;
}



void StreamRecorder::thread_finished() {

    SR_DEBUG;

    _thread_is_running = false;
    qint64 size = _sr_thread->getSize();

    if(!QFile::exists(_sr_recording_dst)) {

        _stream_ended = true;
        emit sig_stream_not_valid();
    }


    else if( size < _buffer_size ) {

        _stream_ended = true;
        _try++;

        if(_try < _max_tries || _max_tries == -1) {
            _sr_thread->start();
            _thread_is_running = true;
        }

        else{
            qDebug() << "SR: Tried so hard to buffer, but not successful :'-(";
            emit sig_stream_not_valid();
        }
    }

    else {
        emit sig_initialized(true);
    }
}


void StreamRecorder::endOfStream() {

    SR_DEBUG;

    if(_thread_is_running) return;

	_stream_ended = true;
    emit sig_stream_ended();
}


QString StreamRecorder::check_session_path(QString sr_path) {

    SR_DEBUG;

	bool create_session_path =_settings->get(Set::Engine_SR_SessionPath);

    if(!create_session_path) return sr_path;

    if(!QFile::exists(sr_path + QDir::separator() + _session_path)) {

        QDir dir(sr_path);
        dir.mkdir(_session_path);
    }

    return sr_path + QDir::separator() + _session_path;
}


bool StreamRecorder::getFinished() {

    return _stream_ended;
}
