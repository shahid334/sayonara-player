#include "StreamRipperBufferThread.h"
#include <QFile>
#include <QString>
#include <QDebug>
#include <unistd.h>


StreamRipperBufferThread::StreamRipperBufferThread(QObject *parent) :
    QThread(parent)
{
    _f = NULL;

}

StreamRipperBufferThread::~StreamRipperBufferThread(){

}


void StreamRipperBufferThread::setUri(QString uri){
    _uri = uri;
}

void StreamRipperBufferThread::run(){

    qDebug() << "Buffer file " << _uri;
    // initially fill the buffer
    qint64 max = 5000000;
    qint64 interval = 10000;

    _size = 0;

    /*
     * _sr_recording_dst = .Sayonara/filename
     */

    if(!QFile::exists(_uri)) {
        qDebug() << "Buffer thread: file does not exist";
        return;
    }

    if(_f != NULL){
        if(_f->isOpen()) _f->close();
        delete _f;
    }

    _f = new QFile(_uri);

    int n_loops = 0;
    do{

        _size = _f->size();

        usleep(interval);
        max -= interval;

        n_loops++;
        if(max <= 0) break;

    } while(_size < 32000 && max > 0);

    _f->close();
}

QString StreamRipperBufferThread::getUri(){
    return _uri;
}

qint64 StreamRipperBufferThread::getSize(){
    return _size;
}


void StreamRipperBufferThread::terminate(){

    if(_f->isOpen()) _f->close();
    _size = 0;
    QThread::terminate();
}
