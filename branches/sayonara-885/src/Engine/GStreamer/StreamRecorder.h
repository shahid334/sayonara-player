/* StreamRecorder.h */

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



#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include "HelperStructs/MetaData.h"
#include "Settings/Settings.h"
#include "Engine/GStreamer/StreamRipperBufferThread.h"


#include <QObject>

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsrc.h>

class StreamRecorder : public QObject
{
    Q_OBJECT
public:
    StreamRecorder(QObject *parent = 0);
    virtual ~StreamRecorder();
    
signals:
    void sig_initialized(bool);
    void sig_stream_ended();
    void sig_stream_not_valid();

    
public slots:

private slots:
    void thread_finished();

public:
    void init();
    bool start();

    bool stop(bool delete_track = false);

    QString changeTrack(const MetaData& md, int max_tries);
    void set_new_stream_session();



    void endOfStream();
    bool getFinished();

private:
    static gboolean bus_state_changed(GstBus *bus, GstMessage *msg, void *user_data);

    MetaData    _md;
    qint64      _buffer_size;
    bool        _initialized;
    bool        _stream_ended;

    QString    _sr_recording_dst;

    StreamRipperBufferThread* _sr_thread;
    Settings*         _settings;

    GstElement* _rec_src;
    GstElement* _rec_dst;
    GstElement* _rec_pipeline;

    GstBus*     _bus;

    int        _try;
    int        _max_tries;
    bool       _thread_is_running;


    QString     _pl_file_path;
    QString     _session_path;
    QString		_session_playlist_name;
    MetaDataList _session_collector;


    QString check_session_path(QString sr_path);
    bool save_file();
    bool init_thread(QString filename);
    bool terminate_thread_if_running();
    
};

#endif // STREAMRECORDER_H
