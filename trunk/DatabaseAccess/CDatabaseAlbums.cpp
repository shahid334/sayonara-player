/* CDatabaseAlbums.cpp */

/* Copyright (C) 2011  Lucio Carreras
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


#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>

#define ALBUM_ARTIST_TRACK_SELECTOR QString("SELECT ") + \
			"albums.albumID AS albumID, " + \
			"albums.name AS albumName, " + \
			"SUM(tracks.length) / 1000 AS albumLength, " + \
			"COUNT(tracks.trackid) AS albumNTracks, " + \
			"MAX(tracks.year) AS albumYear, " + \
			"group_concat(artists.name) AS albumArtists " + \
			"FROM albums, artists, tracks "

bool _db_fetch_albums(QSqlQuery& q, vector<Album>& result) {

		try{
			if (!q.exec()) {

					qDebug() << "SQL-Error: Could not get all albums from database";
					//qDebug() << q.executedQuery();
					return false;
			}

			Album album;
			while (q.next()) {
				album.id = q.value(0).toInt();
				album.name = q.value(1).toString().trimmed();
				album.length_sec = q.value(2).toInt();
				album.num_songs = q.value(3).toInt();
				album.year = q.value(4).toInt();
				QStringList artistList = q.value(5).toString().split(',');
				artistList.removeDuplicates();
				album.artists = artistList;
				album.is_sampler = (artistList.size() > 1);

				result.push_back(album);
			}

			return true;
		}

		catch (QString& ex) {
			qDebug() << "SQL-Exception (Fetch albums): ";
			qDebug() << ex;
			return false;
		}

		return true;
}


int CDatabaseConnector::getAlbumID (const QString & album)  {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	int albumID = -1;

	q.prepare("SELECT albumID FROM albums WHERE name == ?;");
	q.addBindValue(QVariant(album));

	if (!q.exec()) {
		throw QString ("SQL - Error: getAlbumID" + album);
	}

	if (q.next()) {
		albumID = q.value(0).toInt();
	}

	return albumID;
}

int CDatabaseConnector::getMaxAlbumID(){
	DB_TRY_OPEN(m_database);

	int max_id = -1;

	try {
		QSqlQuery q (this -> m_database);
		q.prepare("SELECT MAX(albumID) FROM albums;");

		if (!q.exec()) {
			throw QString ("SQL - Error: Could not get max albumID from DB");
		}

		while (q.next()) {
			max_id = q.value(0).toInt();
		}

		return max_id;
	}

	catch (QString& ex) {
		qDebug() << ex;
		QSqlError er = this -> m_database.lastError();
		return max_id;
	}

	return max_id;
}

Album CDatabaseConnector::getAlbumByID(const int& id){

	DB_TRY_OPEN(m_database);

	vector<Album> albums;
	Album album;

	QSqlQuery q (this -> m_database);
	QString querytext =
			ALBUM_ARTIST_TRACK_SELECTOR +
			"WHERE albums.albumID = :id AND tracks.albumID = albums.albumID AND artists.artistID = tracks.artistID;";


	q.prepare(querytext);
	q.bindValue(":id", QVariant(id));

	_db_fetch_albums(q, albums);
	if(albums.size() > 0)
		album = albums[0];

	return album;
}

void CDatabaseConnector::getAllAlbums(vector<Album>& result, QString sort_order){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString querytext =
			ALBUM_ARTIST_TRACK_SELECTOR +
			"WHERE tracks.albumID = albums.albumID and artists.artistid = tracks.artistid " +
			"GROUP BY albumID, albumName";


	if(sort_order == "name asc") querytext += QString(" ORDER BY albumName ASC;");
	else if(sort_order == "name desc") querytext += QString(" ORDER BY albumName DESC;");
	else if(sort_order == "year asc") querytext += QString(" ORDER BY albumYear ASC;");
	else if(sort_order == "year desc") querytext += QString(" ORDER BY albumYear DESC;");
	else querytext += ";";

	q.prepare(querytext);

	_db_fetch_albums(q, result);
}

void CDatabaseConnector::getAllAlbumsByArtist(int artist, vector<Album>& result, QString filter, QString sort_order){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString querytext =
			ALBUM_ARTIST_TRACK_SELECTOR +
			"WHERE tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid AND artists.artistid = :artist_id ";

	if(filter.length() > 0){
			querytext += QString("AND tracks.trackid IN ( ") +
									"SELECT t2.trackid " +
									"FROM tracks t2 "+
									"WHERE t2.title LIKE :filter1 "+

									"UNION SELECT t3.trackid "+
									"FROM tracks t3, albums "+
									"WHERE albums.albumid = t3.albumid AND albums.name LIKE :filter2 "+

									"UNION SELECT t4.trackid " +
									"FROM tracks t4, albums, artists " +
									"WHERE t4.albumid = albums.albumid AND t4.artistid = artists.artistid AND artists.name LIKE :filter3 "
								") ";
	}


	querytext += QString("GROUP BY albumID, albumName ");
	if(sort_order == "name asc") querytext += QString(" ORDER BY albumName ASC;");
	else if(sort_order == "name desc") querytext += QString(" ORDER BY albumName DESC;");
	else if(sort_order == "year asc") querytext += QString(" ORDER BY albumYear ASC;");
	else if(sort_order == "year desc") querytext += QString(" ORDER BY albumYear DESC;");
	else querytext += ";";

	q.prepare(querytext);
	q.bindValue(":artist_id", QVariant(artist));
	if(filter.length() > 0){
		q.bindValue(":filter1", QVariant(filter));
		q.bindValue(":filter2", QVariant(filter));
		q.bindValue(":filter3", QVariant(filter));
	}


	_db_fetch_albums(q, result);
}

void CDatabaseConnector::getAllAlbumsBySearchString(QString search, vector<Album>& result, QString sort_order){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString query;
	query = QString("SELECT * FROM ( ") +
			ALBUM_ARTIST_TRACK_SELECTOR +
				"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.name LIKE :search_in_album " +
				"GROUP BY albums.albumid, albums.name " +
			"UNION " +
			ALBUM_ARTIST_TRACK_SELECTOR +
				"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.title LIKE :search_in_title " +
				"GROUP BY albums.albumid, albums.name " +
			"UNION " +
			ALBUM_ARTIST_TRACK_SELECTOR +
				"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.name LIKE :search_in_artist " +
				"GROUP BY albums.albumid, albums.name " +
		") " +
		"GROUP BY albumID, albumName";

	if(sort_order == "name asc") query += QString(" ORDER BY albumName ASC;");
	else if(sort_order == "name desc") query += QString(" ORDER BY albumName DESC;");
	else if(sort_order == "year asc")query += QString(" ORDER BY albumYear ASC;");
	else if(sort_order == "year desc")query += QString(" ORDER BY albumYear DESC;");


	q.prepare(query);
	q.bindValue(":search_in_title",QVariant(search));
	q.bindValue(":search_in_album",QVariant(search));
	q.bindValue(":search_in_artist",QVariant(search));

	_db_fetch_albums(q, result);
}


int CDatabaseConnector::insertAlbumIntoDatabase (const QString & album) {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO albums (name) values (:album);");
    q.bindValue(":album", QVariant(album));
    if (!q.exec()) {
        throw QString ("SQL - Error: insertAlbumIntoDatabase " + album);
    }
    return this -> getAlbumID (album);
}

int CDatabaseConnector::insertAlbumIntoDatabase (const Album & album) {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
    try{
    	q.prepare("INSERT INTO albums (albumid, name) values (:id, :name);");
    	q.bindValue(":id", QVariant(album.id));
    	    q.bindValue(":name", QVariant(album.name));
    	    if (!q.exec()) {
    	        throw QString ("SQL - Error: insertAlbumIntoDatabase " + album.name);
    	    }
    	    return this -> getAlbumID (album.name);
    }

    catch (QString& ex) {
    		qDebug() << ex;
    		QSqlError er = this -> m_database.lastError();
    		qDebug() << er.driverText();
    		qDebug() << er.databaseText();
    		qDebug() << er.databaseText();
    		return -1;
    	}

    return -1;
}

