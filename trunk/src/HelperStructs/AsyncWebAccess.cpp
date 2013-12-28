/* AsyncWebAccess.cpp */

/* Copyright (C) 2013  Lucio Carreras
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

#include "HelperStructs/AsyncWebAccess.h"
#include "HelperStructs/Helper.h"



AsyncWebAccess::AsyncWebAccess(QObject* parent, int id) : QThread(parent){
    _id = id;
    _data = new QString();
}

AsyncWebAccess::~AsyncWebAccess(){


    if(this->isRunning()) return;
    qDebug() << "Thread " << _id << " deleted";
    QString** data_ptr = &(this->_data);
    delete _data;
    *data_ptr = 0;

}

void AsyncWebAccess::terminate(){
    emit terminated(_id);
    QThread::terminate();
}

void AsyncWebAccess::stop(){

    emit finished(_id);
    this->quit();

}

void AsyncWebAccess::run(){

    _data->clear();
	if(_url.size() == 0) return;

    Helper::read_http_into_str(_url, _data);

    emit finished(_id);
}



QString* AsyncWebAccess::get_data(){
    return _data;
}

void AsyncWebAccess::set_url(QString url){
	_url = url;
}
