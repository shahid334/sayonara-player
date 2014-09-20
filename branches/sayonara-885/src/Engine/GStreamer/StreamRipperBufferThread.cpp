/* StreamRipperBufferThread.cpp */

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

StreamRipperBufferThread::~StreamRipperBufferThread() {

}


void StreamRipperBufferThread::setUri(QString uri) {
    _uri = uri;
}

void StreamRipperBufferThread::run() {

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

    if(_f != NULL) {
        if(_f->isOpen()) _f->close();
        delete _f;
    }

    _f = new QFile(_uri);

    int n_loops = 0;
    do{
        usleep(interval);
        max -= interval;

        n_loops++;
        if(max <= 0) break;

        _size = _f->size();

    } while(_size < _buffersize && max > 0);

    _f->close();
}

QString StreamRipperBufferThread::getUri() {
    return _uri;
}

int StreamRipperBufferThread::getSize() {
    return _size;
}

void StreamRipperBufferThread::setBufferSize(int bs) {
    _buffersize = bs;
}


void StreamRipperBufferThread::terminate() {

    if(_f->isOpen()) _f->close();
    _size = 0;
    QThread::terminate();
}
