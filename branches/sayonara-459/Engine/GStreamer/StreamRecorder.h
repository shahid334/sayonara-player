#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include "HelperStructs/MetaData.h"
#include "HelperStructs/CSettingsStorage.h"
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
    MetaData    _md;
    qint64      _buffer_size;
    bool        _initialized;
    bool        _stream_ended;

    QString    _sr_recording_dst;

    StreamRipperBufferThread* _sr_thread;
    CSettingsStorage*         _settings;

    GstElement* _rec_src;
    GstElement* _rec_dst;
    GstElement* _rec_pipeline;
    GstElement* _rec_enc;
    GstElement* _rec_cvt;
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
    
};

#endif // STREAMRECORDER_H
