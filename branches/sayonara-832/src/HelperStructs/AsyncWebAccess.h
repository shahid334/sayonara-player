/* AsyncWebAccess.h */

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



#ifndef ASYNCWEBACCESS_H_
#define ASYNCWEBACCESS_H_

#include <QThread>
#include <QString>
#include <QImage>

enum AwaDataType{
    AwaDataTypeString=0,
    AwaDataTypeImage
};

class AsyncWebAccess : public QThread {

	Q_OBJECT

signals:
    void finished(int);
    void finshed(QString);
    void terminated(int);

public slots:
    void terminate();

public:
    AsyncWebAccess(QObject* parent=0, int id=0, AwaDataType t=AwaDataTypeString);
	virtual ~AsyncWebAccess();

    QString* get_data();
    QImage* get_image();
	void set_url(QString url);
    void stop();

protected:
	void run();



private:
	QString _url;
    QString* _data;
    QImage* _img;
    int _type;
    int _id;

};


#endif
