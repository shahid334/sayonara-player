#include "Engine/GStreamer/StreamRecorder.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsrc.h>

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDateTime>


StreamRecorder* obj_ref = 0;



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

            qDebug() << "SR: GST_MESSAGE_ERROR: " << err->message << ": " << GST_MESSAGE_SRC_NAME(msg);

            g_error_free(err);

            break;

        default:
            break;
    }

    return true;
}




StreamRecorder::StreamRecorder(QObject *parent) :
    QObject(parent)
{
    _buffer_size = 32769;
    _stream_ended = true;

    _settings = CSettingsStorage::getInstance();
    obj_ref = this;
    _try = 2;
}


StreamRecorder::~StreamRecorder(){
    if(_sr_thread)
        delete _sr_thread;
}


void StreamRecorder::init(){

        /*
     * _rec_pipeline:
     *
     * _rec_src -> _rec_cvt -> _rec_enc -> _rec_dst
     *
     */


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


    _sr_thread = new StreamRipperBufferThread();
    if(!_sr_thread) _initialized = false;
    else connect(_sr_thread, SIGNAL(finished()), this, SLOT(thread_finished()));

}


QString StreamRecorder::changeTrack(const MetaData& md, int trys){


    _max_tries = trys;
    _md = md;
    if(!_sr_thread) return "";
    if(_sr_thread->isRunning()) _sr_thread->terminate();

     gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_NULL);

    // stream file to _sr_recording_dst
    QString title = _md.title;
    QString org_src_filename = _md.filepath;	// some url


    title.replace(" ", "_");
    title.replace("/", "_");
    title.replace("\\", "_");
    if(Helper::is_soundfile(md.filepath))
        _sr_recording_dst = Helper::getSayonaraPath() + title + "." + _md.filepath.right(3);
    else
        _sr_recording_dst = Helper::getSayonaraPath() + title + "_" + QDateTime::currentDateTime().toString("yyMMdd_hhmm") + ".mp3";


    // record from org_src_filename to new_src_filename
    g_object_set(G_OBJECT(_rec_src), "location", org_src_filename.toLocal8Bit().data(), NULL);
    g_object_set(G_OBJECT(_rec_dst), "location", _sr_recording_dst.toLocal8Bit().data(), NULL);
    g_object_set(G_OBJECT(_rec_src), "blocksize", _buffer_size, NULL);

    _stream_ended = false;
    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_PLAYING);

    _sr_thread->setUri(_sr_recording_dst);
    _try = 0;
    _sr_thread->start();

    return _sr_recording_dst;
}


QString StreamRecorder::stop(bool track_finished, bool delete_track){

    _stream_ended = true;
    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_NULL);

    if(_sr_thread->isRunning()) _sr_thread->terminate();

    QFile f(_sr_recording_dst);

    bool complete_tracks = _settings->getStreamRipperCompleteTracks();
    QString sr_path = _settings->getStreamRipperPath();

    if( (!track_finished && complete_tracks) || delete_track){
        qDebug() << "Remove File (1)" << f.fileName();
        f.remove();
        return "";
    }

    // Final path where track is saved
    QDir dir(sr_path);
        dir.mkdir(_md.artist);
        dir.cd(_md.artist);

    // remove directories in front of filename
    QString src_name = f.fileName();
    QString fname_wo_path = src_name.right( src_name.size() - src_name.lastIndexOf(QDir::separator()) );
    QString dst_name = 	dir.path() + QDir::separator() + fname_wo_path;

    bool success = 	f.copy(dst_name);
    qDebug() << "Remove File (2)" << f.fileName();
    f.remove();

    if(!success){
        qDebug() << "SR: unable to copy " <<  _sr_recording_dst << " to " << dir.path() + QDir::separator() + fname_wo_path;
        return "";
    }

    else
        return dst_name;
}


void StreamRecorder::thread_finished(){

    qint64 size = _sr_thread->getSize();

    bool success = true;

    if(size < 50 || !QFile::exists(_sr_recording_dst)){
        _stream_ended = true;
        qDebug() << "Stream not valid (1)";
        emit sig_stream_not_valid();
        return;
    }

    if( (size < _buffer_size && !_stream_ended) || !QFile::exists(_sr_recording_dst)){

        if(_try < 25 || _max_tries == -1) _sr_thread->start();
        qDebug() << "SR: Could not init because file size = " << size << ". Try again to buffer";
        success = false;
        qDebug() << "SR: Try " << _try;
        _try++;
    }

    else{
        qDebug() << "SR:Init. file size = " << size;
    }

    if(_try < 25 || _max_tries == -1)
        emit sig_initialized(success);

    else emit {
        _stream_ended = true;
        qDebug() << "Stream not valid (2)";
        emit sig_stream_not_valid();
    }
}


void StreamRecorder::endOfStream(){
    gst_element_set_state(GST_ELEMENT(_rec_pipeline), GST_STATE_NULL);
    _sr_thread->terminate();
    _stream_ended = true;

    if(QFile::exists(_sr_recording_dst)){
        emit sig_stream_not_valid();
        return;
    }


    QFile f(_sr_recording_dst);

    qDebug() << "SR: Stream ended" << f.size();
    f.close();

    emit sig_stream_ended();
}

bool StreamRecorder::getFinished(){
    return _stream_ended;
}
