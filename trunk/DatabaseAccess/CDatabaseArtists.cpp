/* CDatabaseArtists.cpp */

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

#define ARTIST_ALBUM_TRACK_SELECTOR QString("SELECT ") + \
				"artists.artistid AS artistID, " + \
				"artists.name AS artistName, " + \
				"COUNT(tracks.trackid) AS artistNTracks, " + \
				"GROUP_CONCAT(albums.albumid) AS artistAlbums " + \
				"FROM artists, albums, tracks "

bool _db_fetch_artists(QSqlQuery& q, vector<Artist>& result){

	try{
		if (!q.exec()) {
			qDebug() << "SQL-Error: Could not get all artists from database";
			qDebug() << q.executedQuery();
			return false;
		}

		Artist artist;
		while (q.next()) {
			artist.id = q.value(0).toInt();
			artist.name = q.value(1).toString().trimmed();

			QStringList list = q.value(2).toString().split(',');
			list.removeDuplicates();
			artist.num_albums = list.size();
			artist.num_songs = q.value(3).toInt();

			result.push_back(artist);
		}

		return true;
	}

	catch (QString& ex) {
		qDebug() << "SQL-Exception (Fetch artists): ";
		qDebug() << ex;
		return false;
	}

	return true;
}


int CDatabaseConnector::getMaxArtistID(){
	DB_TRY_OPEN(m_database);

	int max_id = -1;
	try {
		QSqlQuery q (this -> m_database);
		q.prepare("SELECT MAX(artistID) FROM artists;");

		if (!q.exec()) {
			throw QString ("SQL - Error: Could not get max artistID from DB");
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

Artist CDatabaseConnector::getArtistByID(const int &id){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);

	Artist artist;
	vector<Artist> artists;

	if (id!=-1) {
		QString query = ARTIST_ALBUM_TRACK_SELECTOR +
					"WHERE artists.artistID = ? " +
					"AND tracks.artistID = artists.artistID " +
					"AND tracks.albumID = albums.albumID " +
					"GROUP BY artistName;";

		q.prepare(query);
		q.addBindValue(QVariant (id));

		_db_fetch_artists(q, artists);
		if(artists.size() > 0) artist = artists[0];
	}

	return artist;
}

int CDatabaseConnector::getArtistID (const QString & artist)  {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
    int artistID = -1;
    q.prepare("select artistID from artists where name == ?;");
    q.addBindValue(QVariant(artist));
    if (!q.exec()) {
        throw QString ("SQL - Error: get Artist ID" + artist);
    }
    if (q.next()) {
        artistID = q.value(0).toInt();
    }
    return artistID;
}

void CDatabaseConnector::getAllArtists(vector<Artist>& result, QString sort_order){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString query = ARTIST_ALBUM_TRACK_SELECTOR +
			"WHERE Tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid " +
			"GROUP BY artists.artistID, artists.name ";

	if(sort_order == "name asc") query += QString(" ORDER BY artistName ASC;");
	else if(sort_order == "name desc") query += QString(" ORDER BY artistName DESC;");
	else if(sort_order == "trackcount asc")query += QString(" ORDER BY artistNTracks ASC;");
	else if(sort_order == "trackcount desc")query += QString(" ORDER BY artistNTracks DESC;");
	else query += QString(";");

	q.prepare(query);

	_db_fetch_artists(q, result);

}

void CDatabaseConnector::getAllArtistsByAlbum(int album, vector<Artist>& result, QString order){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	q.prepare(	ARTIST_ALBUM_TRACK_SELECTOR +
				"WHERE Tracks.albumID = albums.albumID " +
				"AND artists.artistid = tracks.artistid " +
				"AND albums.albumid=" + QString::number(album) + " " +
				"GROUP BY artists.artistID, artists.name " +
				"ORDER BY " + order + "; ");

	_db_fetch_artists(q, result);
}

void CDatabaseConnector::getAllArtistsBySearchString(QString search, vector<Artist>& result, QString sort_order){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString query;
	query = QString("SELECT * FROM ( ") +
			ARTIST_ALBUM_TRACK_SELECTOR +
	"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.name LIKE :search_in_artist " +
	"			GROUP BY artists.artistid, artists.name " +
	"		UNION " +
			ARTIST_ALBUM_TRACK_SELECTOR +
	"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.name LIKE :search_in_album " +
	"			GROUP BY artists.artistid, artists.name " +
	"		UNION " +
			ARTIST_ALBUM_TRACK_SELECTOR +
	"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.title LIKE :search_in_title " +
	"			GROUP BY artists.artistid, artists.name " +
	"	)  " +
	"	GROUP BY artistID, artistName ";

	if(sort_order == "name asc") query += QString(" ORDER BY artistName ASC;");
	else if(sort_order == "name desc") query += QString(" ORDER BY artistName DESC;");
	else if(sort_order == "trackcount asc") query += QString(" ORDER BY artistNTracks ASC;");
	else if(sort_order == "trackcount desc") query += QString(" ORDER BY artistNTracks DESC;");
	else query += QString(";");

	q.prepare(query);
	q.bindValue(":search_in_title",QVariant(search));
	q.bindValue(":search_in_album",QVariant(search));
	q.bindValue(":search_in_artist",QVariant(search));

	_db_fetch_artists(q, result);
}


int CDatabaseConnector::insertArtistIntoDatabase (const QString & artist) {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO artists (name) values (:artist);");
    q.bindValue(":artist", QVariant(artist));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << q.executedQuery();
        throw QString ("SQL - Error: insertArtistIntoDatabase " + artist);
    }
    return this -> getArtistID (artist);
}

int CDatabaseConnector::insertArtistIntoDatabase (const Artist & artist) {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	try{
		q.prepare("INSERT INTO artists (artistid, name) values (:id, :name);");
		q.bindValue(":id", QVariant(artist.id));
		q.bindValue(":name", QVariant(artist.name));
		if (!q.exec()) {
			qDebug()<< q.lastQuery() << q.executedQuery();
			throw QString ("SQL - Error: insertArtistIntoDatabase " + artist.name);
		}
	}



   catch (QString& ex) {
   		qDebug() << "SQL - Error: insert albums into database";
   		qDebug() << ex;
   	}

    return this -> getArtistID (artist.name);
}

