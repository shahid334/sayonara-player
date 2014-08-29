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
	valid = true;
}


CoverLocation CoverLocation::getInvalidLocation() {

	CoverLocation cl;
    cl.cover_path = Helper::getIconPath("logo.png");
	cl.google_url = "";
	cl.valid = false;
	return cl;
}

void CoverLocation::print() const{

	qDebug() << "CoverLocation: " << cover_path;
	qDebug() << "CoverLocation: " << google_url;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QString& artist_name, bool big) {

	CoverLocation ret;
	QString cover_token = Helper::calc_cover_token(artist_name, album_name, big);
	QString cover_path =  QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")) {
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	ret.cover_path = cover_path;
	ret.google_url = Helper::calc_google_album_adress(artist_name, album_name, big);

	return ret;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QStringList& artists, bool big) {

	QString major_artist = Helper::get_major_artist(artists);
	return get_cover_location(album_name, major_artist, big);
}

CoverLocation CoverLocation::get_cover_location(int album_id, bool big) {

	if(album_id < 0) return CoverLocation::getInvalidLocation();

	Album album;
	bool success = CDatabaseConnector::getInstance()->getAlbumByID(album_id, album);

	if(!success) {

		return getInvalidLocation();
	}

	return get_cover_location(album, big);
}


CoverLocation CoverLocation::get_cover_location(const Album& album, bool big) {
	int n_artists;

	n_artists = album.artists.size();
	if( n_artists > 1 ) {
		return CoverLocation::get_cover_location(album.name, album.artists, big);
	}

	else if( n_artists == 1 ) {
		return CoverLocation::get_cover_location(album.name, album.artists[0], big);
	}

	else {
		return CoverLocation::get_cover_location(album.name, "", big);
	}
}

CoverLocation CoverLocation::get_cover_location(const Artist& artist, bool big) {
	return CoverLocation::get_cover_location(artist.name, big);
}


CoverLocation CoverLocation::get_cover_location(const QString& artist, bool big) {
	CoverLocation ret;

	QString token = QString("artist_") + Helper::calc_cover_token(artist, "", big);
	QString target_file = QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers" + QDir::separator() + token + ".jpg";

	if(!QFile::exists(QDir::homePath() + QDir::separator() +".Sayonara" + QDir::separator() + "covers")) {
		QDir().mkdir(QDir::homePath() + QDir::separator() + ".Sayonara" + QDir::separator() + "covers");
	}

	ret.cover_path = target_file;
	ret.google_url = Helper::calc_google_artist_adress(artist, big);

	return ret;
}


CoverLocation CoverLocation::get_cover_location(const MetaData& md, bool big) {

	return get_cover_location(md.album, md.artist, big);
}

