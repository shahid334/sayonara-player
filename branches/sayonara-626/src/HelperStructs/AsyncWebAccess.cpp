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



AsyncWebAccess::AsyncWebAccess(QObject* parent) : QThread(parent){
}

AsyncWebAccess::~AsyncWebAccess(){}

void AsyncWebAccess::run(){

	_data.clear();
	if(_url.size() == 0) return;
	Helper::read_http_into_str(_url, _data);
}

bool AsyncWebAccess::get_data(QString& data){
	data = _data;
	return (_data.size() > 0);
}

void AsyncWebAccess::set_url(QString url){
	_url = url;
}
