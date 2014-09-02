/* StreamRipperBufferThread.h */

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



#ifndef STREAMRIPPERBUFFERTHREAD_H
#define STREAMRIPPERBUFFERTHREAD_H

#include <QThread>
#include <QString>
#include <QFile>

class StreamRipperBufferThread : public QThread
{
    Q_OBJECT
public:
    StreamRipperBufferThread(QObject *parent = 0);
    virtual ~StreamRipperBufferThread();
    
protected:
    void run();

public:
    void setUri(QString uri);
    QString getUri();
    int getSize();
    void setBufferSize(int bs);

public slots:
    void terminate();



private:

    QString _uri;
    int _size;
    int _buffersize;
    QFile* _f;


    
};

#endif // STREAMRIPPERBUFFERTHREAD_H
