/* Album.cpp */

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



#include "HelperStructs/MetaData/Album.h"
#include <QStringList>

Album::Album() : LibraryItem() {
    name = "";
    id = -1;
    num_songs = 0;
    length_sec = 0;
    year = 0;
    n_discs = 1;
    is_sampler = false;
    rating = 0;
}

Album::Album(const Album& album) : LibraryItem(album){

	name = album.name;
	id = album.id;
	num_songs = album.num_songs;
	length_sec = album.length_sec;
	year = album.year;
	artists = album.artists;
	discnumbers = album.discnumbers;
	n_discs = album.n_discs;
	rating = album.rating;
	is_sampler = album.is_sampler;
}


Album::~Album() {
	
}


QVariant Album::toVariant(const Album& album) {

	QVariant var; 
	var.setValue(album);
	return var;

}


bool Album::fromVariant(const QVariant& v, Album& album) {

	if( !v.canConvert<Album>() ) return false;
	album =	v.value<Album>();
	return true;
}

void Album::print() const{

	qDebug() << id << ": "
			 << name << " by "
			 << artists.size() << " Artists ("
			 << length_sec << "), "
			 << year;
}



