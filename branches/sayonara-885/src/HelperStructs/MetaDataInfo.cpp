/* MetaDataInfo.cpp */

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


MetaDataInfo::MetaDataInfo(QObject* parent, const MetaDataList& lst) :
	QObject(parent),
	SayonaraClass()
{

	if(lst.size() == 0 ) return;

	quint64 length = 0;
	quint64 filesize = 0;
	quint16 year_min = 0xFFFF;
	quint16 year_max = 0;
	quint32 bitrate_min = 0xFFFFFFFF;
	quint32 bitrate_max = 0;
	quint16 tracknum = 0;
	bool calc_track_num = (lst.size() == 1);

	QStringList genres;

	year_min = 0xFFFF;
	year_max = 0;

	foreach(MetaData md, lst){

		if(!_artists.contains(md.artist)){
			_artists << md.artist;
		}

		if(!_albums.contains(md.album)){
			_albums << md.album;
		}

		length += md.length_ms;
		filesize += md.filesize;

		if(calc_track_num){
			tracknum = md.track_num;
		}

		// bitrate
		if(md.bitrate < bitrate_min && md.bitrate != 0) bitrate_min = md.bitrate;
		if(md.bitrate > bitrate_max) bitrate_max = md.bitrate;

		// year
		if(md.year < year_min && md.year != 0) year_min = md.year;
		if(md.year > year_max) year_max = md.year;

		// genre
		foreach(QString genre, md.genres){

			QString first_upper = Helper::cvtQString2FirstUpper(genre).trimmed();

			if(first_upper.size() == 0) continue;
			if(genres.contains( first_upper ) ) continue;

			genres.append(first_upper.trimmed());
		}

		// paths
		if(!Helper::is_www(md.filepath)){
			QString filename, dir;
			Helper::split_filename(md.filepath, dir, filename);
			if( !_paths.contains(dir)){
				_paths << dir;
			}
		}
		else{
			_paths << md.filepath;
		}
	}

	if(bitrate_max > 0){
		insert_interval(Info_Bitrate, bitrate_min / 1000, bitrate_max / 1000);
	}

	if(year_max > 0){
		insert_interval(Info_Year, year_min, year_max);
	}

	insert_number(Info_nTracks, lst.size());
	insert_filesize(filesize);
	insert_playing_time(length);

	insert_genre(genres);

	set_header(lst);
	set_subheader(tracknum);
	set_cover_location(lst);
}



MetaDataInfo::~MetaDataInfo(){

}


void MetaDataInfo::set_header(){}
void MetaDataInfo::set_header(const MetaDataList& lst){

	if(lst.size() == 1){
        const MetaData& md = lst[0];
		_header = md.title;
	}

	else{
		_header = tr("Various tracks");
	}
}


void MetaDataInfo::set_subheader(){}
void MetaDataInfo::set_subheader(quint16 tracknum){

	_subheader = tr("by ") + calc_artist_str();

	if(tracknum){
		_subheader += CAR_RET + calc_tracknum_str(tracknum) + " " + tr("track on") + " ";
	}

	else{
		_subheader += CAR_RET + tr("on") + " ";
	}

	_subheader += calc_album_str();
}

void MetaDataInfo::set_cover_location(){}
void MetaDataInfo::set_cover_location(const MetaDataList& lst){

	if(lst.size() == 1){
        const MetaData& md = lst[0];
		_cover_location = CoverLocation::get_cover_location(md);
	}

	else if(_albums.size() == 1 && _artists.size() == 1){
		QString album = _albums[0];
		QString artist = _artists[0];
		_cover_location = CoverLocation::get_cover_location(album, artist);
	}

	else if(_albums.size() == 1){
		QString album = _albums[0];
		_cover_location = CoverLocation::get_cover_location(album, _artists);
	}

	else {
		_cover_location = CoverLocation::getInvalidLocation();
	}
}



QString MetaDataInfo::calc_artist_str(){

	QString str;

	if( _artists.size() == 1 ){
		str = _artists[0];
	}

	else{
		str = QString::number(_artists.size()) + " " + tr("Various artists");
	}

	return str;
}


QString MetaDataInfo::calc_album_str(){

	QString str;

	if( _albums.size() == 1){
		str = _albums[0];
	}

	else{
		str = QString::number(_albums.size()) + " " + tr("Various albums");
	}

	return str;
}

QString MetaDataInfo::calc_tracknum_str( quint16 tracknum ){

	QString str;
	switch (tracknum) {
		case 1:
			str = tr("1st");

			break;
		case 2:
			str = tr("2nd");
			break;
		case 3:
			str = tr("3rd");
			break;
		default:
			str = QString::number(tracknum) + tr("th");
		break;
	}

	return str;
}

void MetaDataInfo::insert_playing_time(quint64 len){
	QString str = Helper::cvt_ms_to_string(len);
	_info.insert(Info_PlayingTime, str);
}

void MetaDataInfo::insert_genre(const QStringList& lst){
	QString str = lst.join(", ");
	_info.insert(Info_Genre, str);
}

void MetaDataInfo::insert_filesize(quint64 filesize){
	QString str = Helper::calc_filesize_str(filesize);
	_info.insert(Info_Filesize, str);
}


QString MetaDataInfo::get_header(){
	return _header;
}

QString MetaDataInfo::get_subheader(){
	return _subheader;
}


QString MetaDataInfo::get_info_string(InfoStrings idx){
	switch(idx){

		case Info_nTracks:
			return tr("#Tracks") + ": ";
		case Info_nAlbums:
			return tr("#Albums") + ": ";
		case Info_nArtists:
			return tr("#Artists") + ": ";
		case Info_Filesize:
			return tr("Filesize") + ": ";
		case Info_PlayingTime:
			return tr("Playing time") + ": ";
		case Info_Year:
			return tr("Year") + ": ";
		case Info_Sampler:
			return tr("Sampler? ");
		case Info_Bitrate:
			return tr("Bitrate") + ": ";
		case Info_Genre:
			return tr("Genre") + ": ";
		default: break;

	}

	return "";
}

QMap<InfoStrings, QString> MetaDataInfo::get_info(){
	return _info;
}

QString MetaDataInfo::get_info_as_string(){
	QString str;

	foreach( InfoStrings key, _info.keys() ){
		str += BOLD(get_info_string(key)) + _info.value(key) + CAR_RET;
	}

	return str;
}

QStringList MetaDataInfo::get_paths(){
	return _paths;
}

QString MetaDataInfo::get_paths_as_string(){

	QString str;
	QString lib_path = _settings->get(Set::Lib_Path);

	foreach(QString path, _paths){

		QString name = path;
		name.replace(lib_path, "...");

		QString link = Helper::createLink(name, path, false);
		str += link + CAR_RET;
	}

	return str;
}

CoverLocation MetaDataInfo::get_cover_location(){
	return _cover_location;
}

QString MetaDataInfo::get_cover_artist(){

	if(_artists.size() == 0) return "";

	if(_artists.size() > 1) return "Various artists";

	return _artists[0];
}

QString MetaDataInfo::get_cover_album(){
	if(_albums.size() == 0) return "";
	return _albums[0];
}






