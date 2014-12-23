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


struct ArtistCache {

	CDatabaseConnector* _db;
	QMap<int, Artist> _artists_by_id;
	QMap<QString, Artist> _artists_by_name;

	ArtistCache() {

		_db = CDatabaseConnector::getInstance();
		update();
	}

	~ArtistCache() {

	}

	void update() {

		_artists_by_id.clear();
		_artists_by_name.clear();

		ArtistList artists;
		_db->getAllArtists(artists, ArtistNameAsc, true);

		foreach(Artist artist, artists){
			_artists_by_id.insert(artist.id, artist);
			_artists_by_name.insert(artist.name, artist);
		}
	}

	bool getArtistByID(int id, Artist& artist) {

		if(!_artists_by_id.keys().contains(id)) {
			artist.id = -1;
			return false;
		}

		artist = _artists_by_id[id];
		return true;
	}

	bool getArtistByName(QString name, Artist& artist) {

		if(!_artists_by_name.keys().contains(name)) {
			artist.id = -1;
			return false;
		}

		artist =  _artists_by_name[name];
		return true;
	}
};

ArtistCache artist_cache;

#define ARTIST_ALBUM_TRACK_SELECTOR QString("SELECT ") + \
				"artists.artistid AS artistID, " + \
				"artists.name AS artistName, " + \
				"COUNT(tracks.trackid) AS artistNTracks, " + \
                "GROUP_CONCAT(albums.albumid) AS artistAlbums " + \
				"FROM artists, albums, tracks "

bool CDatabaseConnector::_db_fetch_artists(QSqlQuery& q, ArtistList& result) {

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif

	result.clear();

	DB_RETURN_NOT_OPEN_BOOL(_database);

	if (!q.exec()) {
		show_error("Could not get all artists from database");
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
		show_error("Cannot get max artist id");
		return -1;
	}

	while (q.next()) {
		max_id = q.value(0).toInt();
	}

	return max_id;
}

bool CDatabaseConnector::getArtistByID(const int &id, Artist& artist) {

	if(id == -1) return false;
	if(artist_cache.getArtistByID(id, artist)) return true;

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif


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

    bool success = _db_fetch_artists(q, artists);

    if(artists.size() > 0) {
        success = true;
        artist = artists[0];
    }

    else success = false;


    return success;
}

int CDatabaseConnector::getArtistID (const QString & artist)  {

	Artist a;
	if( artist_cache.getArtistByName(artist, a) ){
		return a.id;
	}

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif


	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);
    int artistID = -1;
    q.prepare("select artistID from artists where name == ?;");
    q.addBindValue(QVariant(artist));

    if (!q.exec()) {
		return -1;
    }

    if (q.next()) {
        artistID = q.value(0).toInt();
    }

    return artistID;
}

void CDatabaseConnector::getAllArtists(ArtistList& result, SortOrder sortorder, bool also_empty) {

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif


	DB_RETURN_NOT_OPEN_VOID(_database);

	QSqlQuery q (*_database);
	QString query = ARTIST_ALBUM_TRACK_SELECTOR;

	if(!also_empty){
			query += "WHERE Tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid ";
	}

	query += "GROUP BY artists.artistID, artists.name ";

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	_db_fetch_artists(q, result);

}

void CDatabaseConnector::getAllArtistsByAlbum(int album, ArtistList& result, SortOrder sortorder) {

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif


	DB_RETURN_NOT_OPEN_VOID(_database);

	QSqlQuery q (*_database);
	q.prepare(	ARTIST_ALBUM_TRACK_SELECTOR +
				"WHERE Tracks.albumID = albums.albumID " +
				"AND artists.artistid = tracks.artistid " +
				"AND albums.albumid=" + QString::number(album) + " " +
				"GROUP BY artists.artistID, artists.name " +
				_create_order_string(sortorder) + "; ");

	_db_fetch_artists(q, result);
}

void CDatabaseConnector::getAllArtistsBySearchString(Filter filter, ArtistList& result, SortOrder sortorder) {

#ifdef DEBUG_DB
    qDebug() << Q_FUNC_INFO;
#endif

	DB_RETURN_NOT_OPEN_VOID(_database);

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

	_db_fetch_artists(q, result);
}


int CDatabaseConnector::insertArtistIntoDatabase (const QString & artist) {

	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);
    q.prepare("INSERT INTO artists (name, cissearch) values (:artist, :cissearch);");
    q.bindValue(":artist", QVariant(artist));
    q.bindValue(":cissearch", QVariant(artist.toLower()));

	if (!q.exec()) {
		show_error(QString("Cannot insert artist ") + artist);
		return -1;
    }

	Artist a;
	artist_cache.update();
	artist_cache.getArtistByName(artist, a);
	return a.id;
}

int CDatabaseConnector::insertArtistIntoDatabase (const Artist & artist) {

	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);

	q.prepare("INSERT INTO artists (artistid, name, cissearch) values (:id, :name, :cissearch);");
	q.bindValue(":id", QVariant(artist.id));
	q.bindValue(":name", QVariant(artist.name));
	q.bindValue(":cissearch", QVariant(artist.name.toLower()));

	if (!q.exec()) {
		show_error(QString("Cannot insert (2) artist ") + artist.name);
		return -1;
	}

	Artist a;
	artist_cache.update();
	artist_cache.getArtistByName(artist.name, a);
	return a.id;
}

