/* GUI_Stream.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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


#include "GUI/stream/GUI_Stream.h"

GUI_Stream::GUI_Stream(QString name, QWidget *parent) :
	AbstractStream(name, parent),
	Ui::GUI_Stream()
{
	_title_fallback_name = tr("Radio Station");
	setup_parent(this);

	QMap<QString, QString> data;

	if( get_all_streams(data) ){
		setup_stations(data);
	}
}


GUI_Stream::~GUI_Stream() {

}

void GUI_Stream::language_changed() {
	retranslateUi(this);
}

bool GUI_Stream::get_all_streams(QMap<QString, QString>& result){
	return _db->getAllStreams(result);
}

bool GUI_Stream::add_stream(QString station_name, QString url){
	return _db->addStream(station_name, url);
}
bool GUI_Stream::delete_stream(QString station_name){
	return _db->deleteStream(station_name);
}
