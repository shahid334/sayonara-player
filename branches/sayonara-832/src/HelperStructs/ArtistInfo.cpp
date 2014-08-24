/* ArtistInfo.cpp */

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



#include "HelperStructs/MetaDataInfo.h"

ArtistInfo::ArtistInfo(QObject* parent, const MetaDataList& lst) :
	MetaDataInfo(parent, lst){

	insert_number(Info_nAlbums, _albums.size());

	if(_artists.size() > 1){
		insert_number(Info_nArtists, _artists.size());
	}

	set_header();
	set_subheader();
	set_cover_location();
}

ArtistInfo::~ArtistInfo(){}

void ArtistInfo::set_header(){
	_header = calc_artist_str();

}

void ArtistInfo::set_subheader(){
	_subheader = "";
}

void ArtistInfo::set_cover_location(){

	if( _artists.size() == 1){
		QString artist = _artists[0];
		_cover_location = CoverLocation::get_cover_location(artist, _big);
	}

	else{
		_cover_location = CoverLocation::getInvalidLocation();
	}
}


QString ArtistInfo::get_cover_album(){
	return "";
}
