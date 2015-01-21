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

bool CDatabaseConnector::db_fetch_artists(QSqlQuery& q, ArtistList& result) {

	result.clear();

	if (!q.exec()) {
		show_error("Could not get all artists from database", q);
		return false;
	}

	if(!q.last()){
		return true;
	}

	int i=0;
	int n_rows = q.at() + 1;

	qDebug() << "N rows = " << n_rows;

	result.resize(n_rows);

	for(bool is_element=q.first(); is_element; is_element = q.next(), i++){

		Artist& artist = result[i];

		artist.id = q.value(0).toInt();
		artist.name = q.value(1).toString().trimmed();
		artist.num_songs = q.value(2).toInt();

		QStringList list = q.value(3).toString().split(',');
		list.removeDuplicates();
		artist.num_albums = list.size();

		result[i] = artist;
	}

	qDebug() << "Got " << result.size() << ", " << i << " artists";

	return true;
}

static QString _create_order_string(SortOrder sort) {

	switch(sort) {
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

int CDatabaseConnector::getMaxArtistID() {

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif

	DB_RETURN_NOT_OPEN_INT(_database);

	int max_id = -1;

	QSqlQuery q (*_database);
	q.prepare("SELECT MAX(artistID) FROM artists;");

	if (!q.exec()) {
		show_error("Cannot get max artist id", q);
		return -1;
	}

	while (q.next()) {
		max_id = q.value(0).toInt();
	}

	return max_id;
}

bool CDatabaseConnector::getArtistByID(const int &id, Artist& artist) {

	if(id == -1) return false;

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);

	ArtistList artists;

    QString query = ARTIST_ALBUM_TRACK_SELECTOR +
                "WHERE artists.artistID = ? " +
                "AND tracks.artistID = artists.artistID " +
                "AND tracks.albumID = albums.albumID " +
                "GROUP BY artistName;";

    q.prepare(query);
    q.addBindValue(QVariant (id));

	bool success = db_fetch_artists(q, artists);

    if(artists.size() > 0) {
        success = true;
        artist = artists[0];
    }

    else success = false;

	return success;
}

int CDatabaseConnector::getArtistID (const QString & artist)  {

	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);
    int artistID = -1;
    q.prepare("select artistID from artists where name == ?;");
	q.addBindValue(artist);

    if (!q.exec()) {
		return -1;
    }

    if (q.next()) {
        artistID = q.value(0).toInt();
    }

    return artistID;
}

bool CDatabaseConnector::getAllArtists(ArtistList& result, SortOrder sortorder, bool also_empty) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	QString query = ARTIST_ALBUM_TRACK_SELECTOR;

	if(!also_empty){
			query += "WHERE Tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid ";
	}

	query += "GROUP BY artists.artistID, artists.name ";

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	return db_fetch_artists(q, result);

}

bool CDatabaseConnector::getAllArtistsByAlbum(int album, ArtistList& result, SortOrder sortorder) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	q.prepare(	ARTIST_ALBUM_TRACK_SELECTOR +
				"WHERE Tracks.albumID = albums.albumID " +
				"AND artists.artistid = tracks.artistid " +
				"AND albums.albumid=" + QString::number(album) + " " +
				"GROUP BY artists.artistID, artists.name " +
				_create_order_string(sortorder) + "; ");

	return db_fetch_artists(q, result);
}

bool CDatabaseConnector::getAllArtistsBySearchString(Filter filter, ArtistList& result, SortOrder sortorder) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	QString query;

	switch(filter.by_searchstring) {

        case BY_GENRE:
            query = ARTIST_ALBUM_TRACK_SELECTOR +
                            "	WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.genre LIKE :search_in_genre " +
							"	GROUP BY artists.artistid, artists.name ";
            break;

		case BY_FILENAME:
			query = ARTIST_ALBUM_TRACK_SELECTOR +
							"	WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.filename LIKE :search_in_title " +
							"	GROUP BY artists.artistid, artists.name ";
			break;

		case BY_FULLTEXT:
		default:
			query = QString("SELECT * FROM ( ") +
					ARTIST_ALBUM_TRACK_SELECTOR +
            "			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.cissearch LIKE :search_in_artist " +
			"			GROUP BY artists.artistid, artists.name " +
			"		UNION " +
					ARTIST_ALBUM_TRACK_SELECTOR +
            "			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.cissearch LIKE :search_in_album " +
			"			GROUP BY artists.artistid, artists.name " +
			"		UNION " +
					ARTIST_ALBUM_TRACK_SELECTOR +
            "			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.cissearch LIKE :search_in_title " +
			"			GROUP BY artists.artistid, artists.name " +
			"	)  " +
			"	GROUP BY artistID, artistName ";
			break;
	}


	query += _create_order_string(sortorder) + ";";

	q.prepare(query);
	switch(filter.by_searchstring) {

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

	return db_fetch_artists(q, result);
}


int CDatabaseConnector::insertArtistIntoDatabase (const QString& artist) {

	DB_RETURN_NOT_OPEN_INT(_database);

	int id = getArtistID(artist);
	if(id >= 0){
		return id;
	}

	QSqlQuery q (*_database);
	q.prepare("INSERT INTO artists (name, cissearch) values (:artist, :cissearch);");
	q.bindValue(":artist", artist);
	q.bindValue(":cissearch", artist.toLower());

	if (!q.exec()) {
		show_error(QString("Cannot insert artist ") + artist, q);
		return -1;
    }

	return getArtistID(artist);
}

int CDatabaseConnector::insertArtistIntoDatabase (const Artist & artist) {

	DB_RETURN_NOT_OPEN_INT(_database);

	if(artist.id >= 0){
		updateArtist(artist);
		return artist.id;
	}

	return insertArtistIntoDatabase(artist.name);

}


int CDatabaseConnector::updateArtist(const Artist &artist){
	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);

	if(artist.id < 0) return -1;

	q.prepare("UPDATE artists SET name = :name, cissearch = :cissearch WHERE artistid = :artist_id;");
	q.bindValue(":name", artist.name);
	q.bindValue(":cissearch", artist.name.toLower());
	q.bindValue(":artist_id", artist.id);

	if (!q.exec()) {
		show_error(QString("Cannot insert (2) artist ") + artist.name, q);
		return -1;
	}

	return artist.id;
}

