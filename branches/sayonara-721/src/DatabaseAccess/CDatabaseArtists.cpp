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


#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QObject>
#include <QSqlError>

#include <cstdlib>

using namespace Sort;

#define ARTIST_ALBUM_TRACK_SELECTOR QString("SELECT ") + \
				"artists.artistid AS artistID, " + \
				"artists.name AS artistName, " + \
				"COUNT(tracks.trackid) AS artistNTracks, " + \
				"GROUP_CONCAT(albums.albumid) AS artistAlbums " + \
				"FROM artists, albums, tracks "

bool _db_fetch_artists(QSqlQuery& q, ArtistList& result){

	result.clear();

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
			artist.num_songs = q.value(2).toInt();

			QStringList list = q.value(3).toString().split(',');
			list.removeDuplicates();
			artist.num_albums = list.size();


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

static QString _create_order_string(SortOrder sort){

	switch(sort){
		case ArtistNameAsc:
			return QString(" ORDER BY artistName ASC ");
		case ArtistNameDesc:
			return QString(" ORDER BY artistName DESC ");
		case ArtistTrackcountAsc:
			return QString(" ORDER BY artistNTracks ASC, artistName ASC ");
		case ArtistTrackcountDesc:
			return QString(" ORDER BY artistNTracks DESC, artistName DESC ");
		default:
			return  "";
	}
}

int CDatabaseConnector::getMaxArtistID(){
	DB_TRY_OPEN(_database);

	int max_id = -1;
	try {
		QSqlQuery q (*_database);
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
		QSqlError er = this -> _database->lastError();
		return max_id;
	}

	return max_id;
}

bool CDatabaseConnector::getArtistByID(const int &id, Artist& artist){

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);

	ArtistList artists;
    if(id == -1) return false;


    QString query = ARTIST_ALBUM_TRACK_SELECTOR +
                "WHERE artists.artistID = ? " +
                "AND tracks.artistID = artists.artistID " +
                "AND tracks.albumID = albums.albumID " +
                "GROUP BY artistName;";

    q.prepare(query);
    q.addBindValue(QVariant (id));

    bool success = _db_fetch_artists(q, artists);

    if(artists.size() > 0){
        success = true;
        artist = artists[0];
    }

    else success = false;


    return success;
}

int CDatabaseConnector::getArtistID (const QString & artist)  {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
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

void CDatabaseConnector::getAllArtists(ArtistList& result, SortOrder sortorder){

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
	QString query = ARTIST_ALBUM_TRACK_SELECTOR +
			"WHERE Tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid " +
			"GROUP BY artists.artistID, artists.name ";

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	_db_fetch_artists(q, result);

}

void CDatabaseConnector::getAllArtistsByAlbum(int album, ArtistList& result, SortOrder sortorder){

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
	q.prepare(	ARTIST_ALBUM_TRACK_SELECTOR +
				"WHERE Tracks.albumID = albums.albumID " +
				"AND artists.artistid = tracks.artistid " +
				"AND albums.albumid=" + QString::number(album) + " " +
				"GROUP BY artists.artistID, artists.name " +
				_create_order_string(sortorder) + "; ");

	_db_fetch_artists(q, result);
}

void CDatabaseConnector::getAllArtistsBySearchString(Filter filter, ArtistList& result, SortOrder sortorder){

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
	QString query;

	switch(filter.by_searchstring){

        case BY_GENRE:
            query = ARTIST_ALBUM_TRACK_SELECTOR +
                            "	WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.genre LIKE :search_in_genre " +
                            "	GROUP BY artistID, artistName ";
            break;

		case BY_FILENAME:
			query = ARTIST_ALBUM_TRACK_SELECTOR +
							"	WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.filename LIKE :search_in_title " +
							"	GROUP BY artistID, artistName ";
			break;

		case BY_FULLTEXT:
		default:
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
			break;
	}


	query += _create_order_string(sortorder) + ";";

	q.prepare(query);
	switch(filter.by_searchstring){

        case BY_GENRE:
            q.bindValue(":search_in_genre", QVariant(filter.filtertext));
            break;
		case BY_FILENAME:
			q.bindValue(":search_in_filename",QVariant(filter.filtertext));
			break;

		case BY_FULLTEXT:
		default:
			q.bindValue(":search_in_title",QVariant(filter.filtertext));
			q.bindValue(":search_in_album",QVariant(filter.filtertext));
			q.bindValue(":search_in_artist",QVariant(filter.filtertext));
			break;
	}

	_db_fetch_artists(q, result);
}


int CDatabaseConnector::insertArtistIntoDatabase (const QString & artist) {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
    q.prepare("INSERT INTO artists (name) values (:artist);");
    q.bindValue(":artist", QVariant(artist));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << q.executedQuery();
        throw QString ("SQL - Error: insertArtistIntoDatabase " + artist);
    }
    return this -> getArtistID (artist);
}

int CDatabaseConnector::insertArtistIntoDatabase (const Artist & artist) {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
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

