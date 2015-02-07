/* CoverLocation.cpp */

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




#include "CoverLookup/CoverLocation.h"
#include "HelperStructs/Helper.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDir>


CoverLocation::CoverLocation() {
	valid = false;
}

QString CoverLocation::get_cover_directory(){
	return Helper::getSayonaraPath() + QDir::separator() + "covers";
}

CoverLocation CoverLocation::getInvalidLocation() {

	CoverLocation cl;
	cl.cover_path = Helper::getSharePath() + "logo.png";
	cl.google_url = "";
	cl.valid = false;
	return cl;
}

void CoverLocation::print() const{

	qDebug() << "CoverLocation: " << cover_path;
	qDebug() << "CoverLocation: " << google_url;
}

QString CoverLocation::toString() const{
    return QString("Location ") + cover_path + " Url: " + google_url;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QString& artist_name) {

	QString cover_dir = get_cover_directory();
	CoverLocation ret;
	QString cover_token = Helper::calc_cover_token(artist_name, album_name);

	QString cover_path =  cover_dir + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	ret.cover_path = cover_path;
	ret.google_url = Helper::calc_google_album_adress(artist_name, album_name);
	ret.valid = true;

	return ret;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QStringList& artists) {

	QString major_artist = Helper::get_major_artist(artists);
	return get_cover_location(album_name, major_artist);
}

CoverLocation CoverLocation::get_cover_location(int album_id) {

	if(album_id < 0) return CoverLocation::getInvalidLocation();

	Album album;
	bool success = CDatabaseConnector::getInstance()->getAlbumByID(album_id, album);

	if(!success) {

		return getInvalidLocation();
	}

	return get_cover_location(album);
}


CoverLocation CoverLocation::get_cover_location(const Album& album) {
	int n_artists;

	n_artists = album.artists.size();

	CoverLocation cl;

	if( n_artists > 1 ) {
		cl = CoverLocation::get_cover_location(album.name, album.artists);
	}

	else if( n_artists == 1 ) {
		cl = CoverLocation::get_cover_location(album.name, album.artists[0]);
	}

	else {
		cl = CoverLocation::get_cover_location(album.name, "");
	}

	if(!album.cover_download_url.isEmpty()){
		cl.google_url = album.cover_download_url;
	}

	return cl;
}

CoverLocation CoverLocation::get_cover_location(const Artist& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist.name);

	if(!artist.cover_download_url.isEmpty()){
		cl.google_url = artist.cover_download_url;
	}

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const QString& artist) {

	if(artist.isEmpty()) return getInvalidLocation();

	QString cover_dir = get_cover_directory();
	CoverLocation ret;

	QString token = QString("artist_") + Helper::calc_cover_token(artist, "");
	QString target_file = cover_dir + QDir::separator() + token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	ret.cover_path = target_file;
	ret.google_url = Helper::calc_google_artist_adress(artist);
	ret.valid = true;

	return ret;
}


CoverLocation CoverLocation::get_cover_location(const MetaData& md) {

    CoverLocation cl;
    if(md.album_id >= 0){
        cl = get_cover_location(md.album_id);
    }

	if(!cl.valid){
		cl = get_cover_location(md.album, md.artist);
	}

	if(!md.cover_download_url.isEmpty()){
		cl.google_url = md.cover_download_url;
	}

	return cl;
}

