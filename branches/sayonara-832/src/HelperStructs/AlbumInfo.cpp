/* AlbumInfo.cpp */

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

AlbumInfo::AlbumInfo(QObject* parent, const MetaDataList& lst) :
	MetaDataInfo(parent, lst){

	QString str_sampler;

	if(_albums.size() > 1){
		insert_number(Info_nAlbums, _albums.size());
	}

	if(_artists.size() > 1){
		insert_number(Info_nArtists, _artists.size());
	}

	if(_albums.size() == 1 && _artists.size() > 1){
		str_sampler = tr("yes");
		_info.insert(Info_Sampler, str_sampler);
	}

	else if(_albums.size() == 1 && _artists.size() == 1){
		str_sampler = tr("no");
		_info.insert(Info_Sampler, str_sampler);
	}

	set_header();
	set_subheader();
	set_cover_location();
}

AlbumInfo::~AlbumInfo(){}

void AlbumInfo::set_header(){
	_header = calc_album_str();
}

void AlbumInfo::set_subheader(){
	_subheader = tr("by") + " " + calc_artist_str();
}

void AlbumInfo::set_cover_location(){

	if( _albums.size() == 1){
		_cover_location = CoverLocation::get_cover_location(_albums[0], _artists);
	}

	else{
		_cover_location = CoverLocation::getInvalidLocation();
	}
}

