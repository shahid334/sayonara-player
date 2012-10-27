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

    
public slots:

private slots:
    void thread_finished();

public:
    void init();
    QString changeTrack(const MetaData& md);
    bool start();
    QString stop(bool track_finished, bool delete_track = false);


    void endOfStream();

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
    
};

#endif // STREAMRECORDER_H
