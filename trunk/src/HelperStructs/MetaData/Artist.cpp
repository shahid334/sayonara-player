/* Artist.cpp */

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



#include "HelperStructs/MetaData/Artist.h"

Artist::Artist() : LibraryItem() {
    id = -1;
    name = "";
    num_songs = 0;
    num_albums = 0;
}


Artist::~Artist() {

}


QVariant Artist::toVariant(const Artist& artist) {

	QVariant var;
	var.setValue(artist);
	return var;
}


bool Artist::fromVariant(const QVariant& v, Artist& artist) {

	if( !v.canConvert<Artist>() ) return false;
	
	artist = v.value<Artist>();
	return true;
}

void Artist::print() const {

	qDebug() << id << ": " << name << ": " << num_songs << " Songs, " << num_albums << " Albums";

}


