/* GUI_Podcasts.cpp */

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



#include "GUI/stream/GUI_Podcasts.h"

GUI_Podcasts::GUI_Podcasts(QString name, QWidget *parent) :
	AbstractStream(name, parent),
	Ui::GUI_Podcasts()
{
	setup_parent(this);
	_title_fallback_name = tr("Podcast");

	QMap<QString, QString> data;

	if( get_all_streams(data) ){
		setup_stations(data);
	}
}


GUI_Podcasts::~GUI_Podcasts() {

}

void GUI_Podcasts::language_changed() {
	retranslateUi(this);
}


bool GUI_Podcasts::get_all_streams(QMap<QString, QString>& result){
	return _db->getAllPodcasts(result);
}

bool GUI_Podcasts::add_stream(QString station_name, QString url){
	return _db->addPodcast(station_name, url);
}
bool GUI_Podcasts::delete_stream(QString station_name){
	return _db->deletePodcast(station_name);
}
