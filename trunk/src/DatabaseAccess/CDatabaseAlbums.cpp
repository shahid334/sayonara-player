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
#include "HelperStructs/Filter.h"

#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QObject>
#include <QSqlError>

#include <cstdlib>

using namespace Sort;

#define ALBUM_ARTIST_TRACK_SELECTOR QString("SELECT ") + \
			"albums.albumID AS albumID, " + \
			"albums.name AS albumName, " + \
			"SUM(tracks.length) / 1000 AS albumLength, " + \
			"COUNT(tracks.trackid) AS albumNTracks, " + \
			"MAX(tracks.year) AS albumYear, " + \
			"group_concat(artists.name) AS albumArtists, " + \
            "group_concat(tracks.discnumber) AS discnumbers " + \
			"FROM albums, artists, tracks "

bool _db_fetch_albums(QSqlQuery& q, AlbumList& result) {

	result.clear();

		try{
			if (!q.exec()) {
                qDebug() << q.executedQuery() << ",   " << q.boundValues();
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

				QStringList discnumberList = q.value(6).toString().split(',');
				album.discnumbers.clear();
				foreach(QString disc, discnumberList){
					int d = disc.toInt();
					if(album.discnumbers.contains(d)) continue;
					
					album.discnumbers << d;
				}

				if(album.discnumbers.size() == 0)
					album.discnumbers << 1;
				album.n_discs = album.discnumbers.size();

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

static QString _create_order_string(SortOrder sortorder){

	switch(sortorder){

		case AlbumNameAsc:
			return QString (" ORDER BY albumName ASC ");

		case AlbumNameDesc:
			return QString (" ORDER BY albumName DESC ");

		case AlbumYearAsc:
			return QString (" ORDER BY albumYear ASC, albumName ASC ");

		case AlbumYearDesc:
			return QString (" ORDER BY albumYear DESC, albumName ASC ");

        case AlbumTracksAsc:
            return QString (" ORDER BY albumNTracks ASC, albumName ASC ");

        case AlbumTracksDesc:
            return QString (" ORDER BY albumNTracks DESC, albumName ASC ");

        case AlbumDurationAsc:
            return QString (" ORDER BY albumLength ASC, albumName ASC ");

        case AlbumDurationDesc:
            return QString (" ORDER BY albumLength DESC, albumName ASC ");

		default:
			return "";
	}
}

int CDatabaseConnector::getAlbumID (const QString & album)  {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
    int albumID = -1;

    q.prepare("SELECT albumID FROM albums WHERE name = ?;");
	q.addBindValue(QVariant(album));

    if (q.exec()) {
        if (q.next()) {
            albumID = q.value(0).toInt();
        }
    }

	return albumID;
}

int CDatabaseConnector::getMaxAlbumID(){
	DB_TRY_OPEN(_database);

	int max_id = -1;

	try {
		QSqlQuery q (*_database);
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
		QSqlError er = this -> _database->lastError();
		return max_id;
	}

	return max_id;
}

bool CDatabaseConnector::getAlbumByID(const int& id, Album& album){

	DB_TRY_OPEN(_database);

	AlbumList albums;

	QSqlQuery q (*_database);
	QString querytext =
			ALBUM_ARTIST_TRACK_SELECTOR +
			"WHERE albums.albumID = :id AND tracks.albumID = albums.albumID AND artists.artistID = tracks.artistID;";


	q.prepare(querytext);
	q.bindValue(":id", QVariant(id));

	_db_fetch_albums(q, albums);
    if(albums.size() > 0){
        album = albums[0];
        return true;
    }

    return false;
}

void CDatabaseConnector::getAllAlbums(AlbumList& result, SortOrder sortorder){

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
	QString querytext =
			ALBUM_ARTIST_TRACK_SELECTOR +
			"WHERE tracks.albumID = albums.albumID and artists.artistid = tracks.artistid " +
            "GROUP BY albums.albumID, albumName";


	querytext += _create_order_string(sortorder) + ";";

	q.prepare(querytext);

	_db_fetch_albums(q, result);
}


void CDatabaseConnector::getAllAlbumsByArtist(QList<int> artists, AlbumList& result, Filter filter,SortOrder sortorder){
	DB_TRY_OPEN(_database);

    QStringList lst_artist_names;


	QSqlQuery q (*_database);

	// fetch all albums
	QString querytext =
			ALBUM_ARTIST_TRACK_SELECTOR +
			"WHERE tracks.albumID = albums.albumID AND "
			"artists.artistid = tracks.artistid AND ";

	if(artists.size() == 0) return;

	else if(artists.size() > 1){
		querytext += "(artists.artistid = :artist_id ";
		for(int i=1; i<artists.size(); i++){
			querytext += QString("OR artists.artistid = :artist_id_" + QString::number(i) + " ");
		}

		querytext += ") ";
	}

	else{
		querytext += "artists.artistid = :artist_id ";
	}


	if(filter.filtertext.length() > 0){
		switch(filter.by_searchstring){
            case BY_GENRE:
                querytext += QString("AND tracks.genre LIKE :filter1 ");			// track title is like filter
                break;

            case BY_FILENAME:
                querytext += QString("AND tracks.filename LIKE :filter1 ");			// track title is like filter

				break;

			case BY_FULLTEXT:
			default:
                querytext += QString("AND tracks.trackID IN ( "
                                     "SELECT t2.trackID "
                                     "FROM tracks t2 "
                                     "WHERE t2.cissearch LIKE :filter1 ") +			// track title is like filter

								"UNION SELECT t3.trackid "+			// album title is like filter
								"FROM tracks t3, albums "+
                                "WHERE albums.albumid = t3.albumid AND albums.cissearch LIKE :filter2 "+

								"UNION SELECT t4.trackid " +		// artist title is like filter
								"FROM tracks t4, albums, artists " +
                                "WHERE t4.albumid = albums.albumid AND t4.artistid = artists.artistid AND artists.cissearch LIKE :filter3 "
							") ";
				break;
		}
	}

    querytext += QString("GROUP BY albums.albumID, albumName ");
	querytext += _create_order_string(sortorder) + ";";

	q.prepare(querytext);

	q.bindValue(":artist_id", QVariant(artists[0]));
	for(int i=1; i<artists.size(); i++){
		q.bindValue(QString(":artist_id_") + QString::number(i), artists[i]);
	}

	if(filter.filtertext.length() > 0){
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring){
            case BY_GENRE:
                break;
            case BY_FILENAME:
				break;
			default:
				q.bindValue(":filter2", QVariant(filter.filtertext));
				q.bindValue(":filter3", QVariant(filter.filtertext));
				break;
		}
	}


    _db_fetch_albums(q, result);

}

void CDatabaseConnector::getAllAlbumsByArtist(int artist, AlbumList& result, Filter filter, SortOrder sortorder){

	QList<int> list;
	list << artist;
	getAllAlbumsByArtist(list, result, filter, sortorder);
}

void CDatabaseConnector::getAllAlbumsBySearchString(Filter filter, AlbumList& result, SortOrder sortorder){

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
	QString query;
	if(filter.by_searchstring == BY_FULLTEXT){
			query = QString("SELECT * FROM ( ") +
					ALBUM_ARTIST_TRACK_SELECTOR +
                        "WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.cissearch LIKE :search_in_album " +
						"GROUP BY albums.albumid, albums.name " +
					"UNION " +
					ALBUM_ARTIST_TRACK_SELECTOR +
                        "WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.cissearch LIKE :search_in_title " +
						"GROUP BY albums.albumid, albums.name " +
					"UNION " +
					ALBUM_ARTIST_TRACK_SELECTOR +
                        "WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.cissearch LIKE :search_in_artist " +
						"GROUP BY albums.albumid, albums.name " +
				") " +
                "GROUP BY albumID, albumName";
	}
	else if(filter.by_searchstring == BY_FILENAME){
		query = ALBUM_ARTIST_TRACK_SELECTOR +
					"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.filename LIKE :search_in_filename " +
                    "GROUP BY albums.albumID, albumName";
	}

    else if(filter.by_searchstring == BY_GENRE){
       query = ALBUM_ARTIST_TRACK_SELECTOR +
                    "WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.genre LIKE :search_in_genre " +
                    "GROUP BY albums.albumID, albumName";
    }

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);


	if(filter.by_searchstring == BY_FULLTEXT){
		q.bindValue(":search_in_title",QVariant(filter.filtertext));
		q.bindValue(":search_in_album",QVariant(filter.filtertext));
		q.bindValue(":search_in_artist",QVariant(filter.filtertext));
	}

	else if(filter.by_searchstring == BY_FILENAME){
		q.bindValue(":search_in_filename", QVariant(filter.filtertext));
	}

    else if(filter.by_searchstring == BY_GENRE){
        q.bindValue(":search_in_genre", QVariant(filter.filtertext));
    }

	_db_fetch_albums(q, result);
}


int CDatabaseConnector::insertAlbumIntoDatabase (const QString & album) {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
    q.prepare("INSERT INTO albums (name, cissearch) values (:album, :cissearch);");
    q.bindValue(":album", QVariant(album));
    q.bindValue(":cissearch", QVariant(album.toLower()));
    if (!q.exec()) {
        throw QString ("SQL - Error: insertAlbumIntoDatabase " + album);
    }
    return this -> getAlbumID (album);
}

int CDatabaseConnector::insertAlbumIntoDatabase (const Album & album) {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
    try{
        q.prepare("INSERT INTO albums (albumid, name, cissearch) values (:id, :name, :cissearch);");
    	q.bindValue(":id", QVariant(album.id));
        q.bindValue(":name", QVariant(album.name));
        q.bindValue(":cissearch", QVariant(album.name.toLower()));

        if (!q.exec()) {
            throw QString ("SQL - Error: insertAlbumIntoDatabase " + album.name);
        }
        return this -> getAlbumID (album.name);
    }

    catch (QString& ex) {
    		qDebug() << ex;
    		QSqlError er = this -> _database->lastError();
    		qDebug() << er.driverText();
    		qDebug() << er.databaseText();
    		qDebug() << er.databaseText();
    		return -1;
    	}

    return -1;
}

