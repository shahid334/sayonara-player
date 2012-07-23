/* CDatabaseTracks.cpp */

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

#define TRACK_SELECTOR QString("SELECT ") + \
	"tracks.trackID AS trackID, " \
	"tracks.title AS trackTitle, "	\
	"tracks.length AS trackLength, " \
	"tracks.year AS trackYear, " \
	"tracks.bitrate AS trackBitrate, " \
	"tracks.filename AS trackFilename, " \
	"tracks.track AS trackNum, " \
	"albums.albumID AS albumID, " \
	"artists.artistID AS artistID, " \
	"albums.name AS albumName, " \
	"artists.name AS artistName " \
	"FROM tracks, albums, artists " \
	"WHERE artists.artistID = tracks.artistID " \
	"AND albums.albumID = tracks.albumID "

bool _db_fetch_tracks(QSqlQuery& q, vector<MetaData>& result){

	try{
		if (!q.exec()) {
			qDebug() << "SQL-Error: Cannot fetch tracks from database";
			qDebug() << q.executedQuery();
		}

		while (q.next()) {

			MetaData data;
			data.id = 		 q.value(0).toInt();
			data.title = 	 q.value(1).toString();
			data.length_ms = q.value(2).toInt();
			data.year = 	 q.value(3).toInt();
			data.bitrate = 	 q.value(4).toInt();
			data.filepath =  q.value(5).toString();
			data.track_num = q.value(6).toInt();
			data.album_id =  q.value(7).toInt();
			data.artist_id = q.value(8).toInt();
			data.album = 	 q.value(9).toString().trimmed();
			data.artist = 	 q.value(10).toString().trimmed();
			data.is_extern = false;

			result.push_back(data);
		}

		return true;
	}
	catch (QString& ex) {
		qDebug() << "SQL-Exception (Fetch tracks):";
		qDebug() << ex;

		return false;
	}

	return true;
}


QString CDatabaseConnector::append_track_sort_string(QString querytext, QString sort){

	if(sort == "artist asc") querytext += QString(" ORDER BY artistName ASC, albumName ASC, trackNum;");
	else if(sort == "artist desc") querytext += QString(" ORDER BY artistName DESC, albumName ASC, trackNum;");
	else if(sort == "album asc") querytext += QString(" ORDER BY albumName ASC, trackNum;");
	else if(sort == "album desc") querytext += QString(" ORDER BY albumName DESC, trackNum;");
	else if(sort == "title asc") querytext += QString(" ORDER BY trackTitle ASC;");
	else if(sort == "title desc") querytext += QString(" ORDER BY trackTitle DESC;");
	else if(sort == "track asc") querytext += QString(" ORDER BY trackNum ASC;");
	else if(sort == "track desc") querytext += QString(" ORDER BY trackNum DESC;");
	else if(sort == "year asc") querytext += QString(" ORDER BY trackYear ASC;");
	else if(sort == "year desc") querytext += QString(" ORDER BY trackYear DESC;");
	else if(sort == "length asc") querytext += QString(" ORDER BY trackLength ASC;");
	else if(sort == "length desc") querytext += QString(" ORDER BY trackLength DESC;");
	else if(sort == "bitrate asc") querytext += QString(" ORDER BY trackBitrate ASC;");
	else if(sort == "bitrate desc") querytext += QString(" ORDER BY trackBitrate DESC;");
	else querytext += ";";

	return querytext;
}


int CDatabaseConnector::getTrackByPath(QString path){
	DB_TRY_OPEN(m_database);

	vector<MetaData> vec_data;
	QSqlQuery q (this -> m_database);

	QString querytext = TRACK_SELECTOR + " AND tracks.filename = :filename;";
	q.prepare(querytext);
	q.bindValue(":filename", path);

	if(!_db_fetch_tracks(q, vec_data)) return -1;

	if(vec_data.size() == 0)
		return -2;

	else
		return vec_data[0].id;
}

MetaData CDatabaseConnector::getTrackById(int id){
	DB_TRY_OPEN(m_database);

	MetaData data;
	vector<MetaData> vec_data;

	QSqlQuery q (this -> m_database);
	QString querytext = TRACK_SELECTOR + " AND tracks.trackID = :track_id;";

	q.prepare(querytext);
	q.bindValue(":track_id", QVariant(id));

	_db_fetch_tracks(q, vec_data);

	if(vec_data.size() > 0)
		data = vec_data[0];

	return data;
}


int CDatabaseConnector::getTracksFromDatabase (std::vector<MetaData> & returndata, QString sort) {
	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);

	QString querytext = append_track_sort_string(TRACK_SELECTOR, sort);
	q.prepare(querytext);

	_db_fetch_tracks(q, returndata);

    return 0;
}

void CDatabaseConnector::getAllTracksByAlbum(int album, vector<MetaData>& returndata, Filter filter, QString sort){
	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString querytext = TRACK_SELECTOR +
			"AND tracks.albumid=:albumid ";


	if(filter.filtertext.length() > 0 ){


		switch(filter.by_searchstring){

			case BY_FILENAME:
				querytext += "AND tracks.filename LIKE :filter1 ";
				break;

			case BY_FULLTEXT:
			default:
				// consider the case, that the search string may fit to the title
				// union the case that the search string may fit to the album
				querytext += QString("AND tracks.trackid IN ( ") +
									"	SELECT t2.trackid " +
									"	FROM tracks t2 "+
									"	WHERE t2.title LIKE :filter1 "+

									"	UNION SELECT t3.trackid "+
									"	FROM tracks t3, albums a2 "+
									"	WHERE a2.albumid = t3.albumid AND a2.name LIKE :filter2 "+

									"	UNION SELECT t4.trackid " +
									"	FROM tracks t4, albums a3, artists ar2" +
									"	WHERE t4.albumid = a3.albumid " +
									"	AND t4.artistid = ar2.artistid " +
									"	AND ar2.name LIKE :filter3 "

								") ";
				break;
		}

	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":albumid", QVariant(album));

	if(filter.filtertext.length() > 0){
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring){

			case BY_FILENAME:
				break;

			case BY_FULLTEXT:
			default:
				q.bindValue(":filter2", QVariant(filter.filtertext));
				q.bindValue(":filter3", QVariant(filter.filtertext));
				break;
		}

	}

	_db_fetch_tracks(q, returndata);

}

void CDatabaseConnector::getAllTracksByArtist(int artist, vector<MetaData>& returndata, Filter filter, QString sort){
	DB_TRY_OPEN(m_database);

	MetaData data;

	QSqlQuery q (this -> m_database);
	QString querytext = TRACK_SELECTOR +
					"AND tracks.artistID=:artistID ";

	if(filter.filtertext.length() > 0 ){
		switch(filter.by_searchstring){

			case BY_FILENAME:
				querytext += "AND tracks.filename LIKE :filter1 ";
				break;

			case BY_FULLTEXT:
			default:
				querytext += QString("AND tracks.trackid IN ( ") +
							"	SELECT t2.trackid "
							"	FROM tracks t2 "
							"	WHERE t2.title LIKE :filter1 "
							"	UNION SELECT t3.trackID "
							"	FROM tracks t3, albums a2"
							"	WHERE a2.albumID = t3.albumID AND a2.name LIKE :filter2 "
							"	UNION SELECT t4.trackID "
							"	FROM tracks t4, albums a3, artists ar2"
							"	WHERE t4.albumid = a3.albumid AND t4.artistID = ar2.artistID AND ar2.name LIKE :filter3 "
						") ";
				break;
		}
		// consider the case, that the search string may fit to the title
		// union the case that the search string may fit to the album

	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":artist_id", QVariant(artist));

	if(filter.filtertext.length() > 0 ){
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring){

			case BY_FILENAME:
				break;
			case BY_FULLTEXT:

			default:
				q.bindValue(":filter2", QVariant(filter.filtertext));
				q.bindValue(":filter3", QVariant(filter.filtertext));
				break;
		}
	}

	_db_fetch_tracks(q, returndata);
}

void CDatabaseConnector::getAllTracksBySearchString(Filter filter, vector<MetaData>& result, QString sort){

	DB_TRY_OPEN(m_database);

	MetaData data;

	QSqlQuery q (this -> m_database);
	QString querytext;
	QString subquery = TRACK_SELECTOR;

	switch(filter.by_searchstring){

		case BY_FILENAME:
			querytext = TRACK_SELECTOR  +
						"AND tracks.filename LIKE :search_in_filename ";
			break;

		case BY_FULLTEXT:
		default:
			querytext = QString("SELECT * FROM ( ") +
					subquery + "AND tracks.title LIKE :search_in_title " +
					"UNION " +
					subquery + "AND albums.name LIKE :search_in_album " +
					"UNION " +
					subquery + "AND artists.name LIKE :search_in_artist " +
					" )";
			break;
	}



	querytext = append_track_sort_string(querytext, sort);
	q.prepare(querytext);

	switch(filter.by_searchstring){

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



	_db_fetch_tracks(q, result);
}


int CDatabaseConnector::deleteTrack(MetaData& md){
	DB_TRY_OPEN(m_database);

	try {
			QSqlQuery q (this -> m_database);
			QString querytext = QString("DELETE FROM tracks WHERE trackID = :track_id;");

			q.prepare(querytext);
			q.bindValue(":track_id", QVariant(md.id));

			if (!q.exec()) {
				throw QString ("SQL - Error: delete track from Database:  cannot execute query");
			}

			else{
				deleteFromAllPlaylists(md.id);
			}

			return 0;
		}

		catch (QString& ex) {
			qDebug() << "SQL - Error: getTracksFromDatabase";
			qDebug() << ex;
			return -1;
		}
}

int CDatabaseConnector::deleteTracks(vector<MetaData>& vec_tracks){

	DB_TRY_OPEN(m_database);

	int success = 0;

	m_database.transaction();

	for(uint i=0; i<vec_tracks.size(); i++){

		if( deleteTrack(vec_tracks[i]) == 0){
			success ++;
		}
	}

	m_database.commit();

	return success;
}

int CDatabaseConnector::updateTrack(MetaData& data){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	try{
		q.prepare("UPDATE Tracks SET albumID = :albumID, artistID = :artistID, title = :title, year = :year, track = :track WHERE TrackID = :trackID");

		q.bindValue(":albumID",QVariant(data.album_id));
		q.bindValue(":artistID",QVariant(data.artist_id));
		q.bindValue(":title",QVariant(data.title));
		q.bindValue(":track",QVariant(data.track_num));
		q.bindValue(":year",QVariant(data.year));
		q.bindValue(":trackID", QVariant(data.id));

		if (!q.exec()) {

			throw QString ("SQL - Error: update track " + data.filepath);
		}
	}

	catch (QString& ex) {
		qDebug() << "SQL - Error: getTracksFromDatabase";
		qDebug() << ex;
		return -1;
	}

	return 0;
}


int CDatabaseConnector::insertTrackIntoDatabase (MetaData & data, int artistID, int albumID) {

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);

	data.filepath.replace("//", "/");
	data.filepath.replace("\\\\", "\\");

	int track_id = getTrackByPath(data.filepath);

	if(track_id > 0){
		data.id = track_id;
		data.artist_id = artistID;
		data.album_id = albumID;

		updateTrack(data);
		return 0;
	}

	QString querytext = QString("INSERT INTO tracks ") +
				"(filename,albumID,artistID,title,year,length,track,bitrate) " +
				"VALUES "+
				"(:filename,:albumID,:artistID,:title,:year,:length,:track,:bitrate); ";

	q.prepare(querytext);

	q.bindValue(":filename",QVariant(data.filepath));
    q.bindValue(":albumID",QVariant(albumID));
    q.bindValue(":artistID",QVariant(artistID));
    q.bindValue(":length",QVariant(data.length_ms));
    q.bindValue(":year",QVariant(data.year));
    q.bindValue(":title",QVariant(data.title));
    q.bindValue(":track",QVariant(data.track_num));
    q.bindValue(":bitrate",QVariant(data.bitrate));


    if (!q.exec()) {
    	if(track_id < 0)
    		throw QString ("SQL - Error: insert track into database " + data.filepath);
    	else
    		throw QString ("SQL - Error: update track in database " + data.filepath);
    }
    return 0;
}
