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
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QObject>
#include <QSqlError>
#include <QDir>

#include <cstdlib>

using namespace Sort;

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
    "artists.name AS artistName, " \
    "tracks.genre AS genrename, "\
    "tracks.filesize AS filesize, " \
    "tracks.discnumber AS discnumber " \
    "FROM tracks " \
    "INNER JOIN albums ON tracks.albumID = albums.albumID " \
    "INNER JOIN artists ON tracks.artistID = artists.artistID "



bool _db_fetch_tracks(QSqlQuery& q, MetaDataList& result){

	result.clear();

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
            data.genres =	 q.value(11).toString().split(",");
            data.filesize =  q.value(12).toInt();
            data.discnumber = q.value(13).toInt();

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


QString CDatabaseConnector::append_track_sort_string(QString querytext, SortOrder sort){


    if(sort == TrackArtistAsc) querytext += QString(" ORDER BY artistName ASC, discnumber ASC, albumName ASC, trackNum;");
    else if(sort == TrackArtistDesc) querytext += QString(" ORDER BY artistName DESC, discnumber ASC, albumName ASC, trackNum;");
    else if(sort == TrackAlbumAsc) querytext += QString(" ORDER BY discnumber ASC, albumName ASC, trackNum;");
    else if(sort == TrackAlbumDesc) querytext += QString(" ORDER BY discnumber ASC, albumName DESC, trackNum;");
	else if(sort == TrackTitleAsc) querytext += QString(" ORDER BY trackTitle ASC;");
	else if(sort == TrackTitleDesc) querytext += QString(" ORDER BY trackTitle DESC;");
	else if(sort == TrackNumAsc) querytext += QString(" ORDER BY trackNum ASC;");
	else if(sort == TrackNumDesc) querytext += QString(" ORDER BY trackNum DESC;");
	else if(sort == TrackYearAsc) querytext += QString(" ORDER BY trackYear ASC;");
	else if(sort == TrackYearDesc) querytext += QString(" ORDER BY trackYear DESC;");
	else if(sort == TrackLenghtAsc) querytext += QString(" ORDER BY trackLength ASC;");
	else if(sort == TrackLengthDesc) querytext += QString(" ORDER BY trackLength DESC;");
	else if(sort == TrackBitrateAsc) querytext += QString(" ORDER BY trackBitrate ASC;");
	else if(sort == TrackBitrateDesc) querytext += QString(" ORDER BY trackBitrate DESC;");
    else if(sort == TrackSizeAsc) querytext += QString(" ORDER BY filesize ASC;");
    else if(sort == TrackSizeDesc) querytext += QString(" ORDER BY filesize DESC;");

	else querytext += ";";

	return querytext;
}


void CDatabaseConnector::getMultipleTracksByPath(QStringList& paths, MetaDataList& v_md){

    DB_TRY_OPEN(_database);
     DB_RETURN_NOT_OPEN_VOID(_database);

    _database->transaction();

    foreach(QString path, paths){
        MetaData md = getTrackByPath(path);
        v_md.push_back(md);
    }

    _database->commit();

}


MetaData CDatabaseConnector::getTrackByPath(QString path){
	DB_TRY_OPEN(_database);

	MetaDataList vec_data;
	QDir d(path);
    path = d.absolutePath();

	QSqlQuery q (*_database);

	QString querytext = TRACK_SELECTOR + " AND tracks.filename = :filename;";
	q.prepare(querytext);
    q.bindValue(":filename", path);

	MetaData md;
	md.id = -1;
    md.filepath = path;

    if(!_db_fetch_tracks(q, vec_data)) return md;

    if(vec_data.size() == 0){
        md.is_extern = true;
        return md;
    }

    return vec_data[0];

}

MetaData CDatabaseConnector::getTrackById(int id){
	DB_TRY_OPEN(_database);

	MetaDataList vec_data;

	QSqlQuery q (*_database);
	QString querytext = TRACK_SELECTOR + " AND tracks.trackID = :track_id;";

	q.prepare(querytext);
	q.bindValue(":track_id", QVariant(id));

	MetaData md;
	md.id = -1;

    if(!_db_fetch_tracks(q, vec_data)) return md;

    if(vec_data.size() == 0) {
        md.is_extern = true;
        return md;
    }
	return vec_data[0];
}


int CDatabaseConnector::getTracksFromDatabase (MetaDataList & returndata, SortOrder sort) {
	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);

	QString querytext = append_track_sort_string(TRACK_SELECTOR, sort);
	q.prepare(querytext);

	_db_fetch_tracks(q, returndata);

    return 0;
}

void CDatabaseConnector::getAllTracksByAlbum(int album, MetaDataList& returndata, Filter filter, SortOrder sort, int discnumber){
	QList<int> list;
	MetaDataList mdlist;
	list << album;
	returndata.clear();

	getAllTracksByAlbum(list, mdlist, filter, sort);
	
	if(discnumber < 0) returndata = mdlist;
	
	foreach(MetaData md, mdlist){
		if(discnumber != md.discnumber) continue;
		returndata.push_back(md);
	}
}

void CDatabaseConnector::getAllTracksByAlbum(QList<int> albums, MetaDataList& returndata, Filter filter, SortOrder sort){
	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);
	QString querytext = TRACK_SELECTOR;

	if(albums.size() == 0) return;

	else if(albums.size() == 1){
		querytext += "AND tracks.albumid=:albumid ";
	}

	else {
		querytext += "AND (tracks.albumid=:albumid ";
		for(int i=1; i<albums.size(); i++){
			querytext += "OR tracks.albumid=:albumid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	if(filter.filtertext.length() > 0 ){


		switch(filter.by_searchstring){
			case BY_GENRE:
                querytext += "AND tracks.genre LIKE :filter1 ";
				break;

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
	q.bindValue(":albumid", QVariant(albums[0]));
	for(int i=1; i<albums.size(); i++){
		q.bindValue(QString(":albumid_") + QString::number(i), albums[i]);
	}

	if(filter.filtertext.length() > 0){
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring){
			case BY_GENRE:
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

void CDatabaseConnector::getAllTracksByArtist(int artist, MetaDataList& returndata, Filter filter, SortOrder sort){
	QList<int> list;
	list << artist;
	getAllTracksByArtist(list, returndata, filter, sort);
}

void CDatabaseConnector::getAllTracksByArtist(QList<int> artists, MetaDataList& returndata, Filter filter, SortOrder sort){
	DB_TRY_OPEN(_database);

	MetaData data;

	QSqlQuery q (*_database);
	QString querytext = TRACK_SELECTOR;



	if(artists.size() == 0) return;

	else if(artists.size() == 1){
		querytext += "AND tracks.artistid=:artistid ";
	}

	else {
		querytext += "AND (tracks.artistid=:artistid ";
		for(int i=1; i<artists.size(); i++){
			querytext += "OR tracks.artistid=:artistid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	if(filter.filtertext.length() > 0 ){
		switch(filter.by_searchstring){

			case BY_GENRE:
                    querytext += "AND tracks.genre LIKE :filter1";
				break;

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
	q.bindValue(":artist_id", QVariant(artists[0]));
	for(int i=1; i<artists.size(); i++){
		q.bindValue(QString(":artistid_") + QString::number(i), artists[i]);
	}

	if(filter.filtertext.length() > 0 ){
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring){
			case BY_GENRE:
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


void CDatabaseConnector::getAllTracksBySearchString(Filter filter, MetaDataList& result, SortOrder sort){

	DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_VOID(_database);

	MetaData data;

	QSqlQuery q (*_database);
	QString querytext;
    QString subquery = TRACK_SELECTOR;

	switch(filter.by_searchstring){

		case BY_GENRE:
			querytext = TRACK_SELECTOR +
                        "AND genrename LIKE :search_in_genre ";
		break;

		case BY_FILENAME:
			querytext = TRACK_SELECTOR  +

                        "AND tracks.filename LIKE :search_in_filename ";
			break;

		case BY_FULLTEXT:
		default:

                  querytext = TRACK_SELECTOR + " AND tracks.trackID IN ("+
                            "SELECT tracks.trackID FROM tracks WHERE tracks.title LIKE :search_in_title " +
                            "UNION "+
                            "SELECT tracks.trackID FROM tracks INNER JOIN albums ON tracks.albumID = albums.albumID AND albums.name LIKE :search_in_album "
                            "UNION "+
                            "SELECT tracks.trackID FROM tracks INNER JOIN artists ON tracks.artistID = artists.artistID AND artists.name LIKE :search_in_artist "
                            ") ";

			break;
	}




	querytext = append_track_sort_string(querytext, sort);
	q.prepare(querytext);


	switch(filter.by_searchstring){

		case BY_FILENAME:
			q.bindValue(":search_in_filename",QVariant(filter.filtertext));
			break;
		case BY_GENRE:
			q.bindValue(":search_in_genre", QVariant(filter.filtertext));
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
	DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_INT(_database);

    //deleteTrackIndexes(md.id);

    try {
			QSqlQuery q (*_database);
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




int CDatabaseConnector::deleteTracks(MetaDataList& vec_tracks){

	DB_TRY_OPEN(_database);
     DB_RETURN_NOT_OPEN_INT(_database);

	int success = 0;

	_database->transaction();

	for(uint i=0; i<vec_tracks.size(); i++){

		if( deleteTrack(vec_tracks[i]) == 0){
			success ++;
		}
	}

	_database->commit();

	return success;
}

int CDatabaseConnector::updateTrack(MetaData& data){

	DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);

        q.prepare("UPDATE Tracks SET albumID=:albumID, artistID=:artistID, title=:title, year=:year, track=:track, genre=:genre, filesize=:filesize, discnumber=:discnumber WHERE TrackID = :trackID;");

		q.bindValue(":albumID",QVariant(data.album_id));
		q.bindValue(":artistID",QVariant(data.artist_id));
        q.bindValue(":title",QVariant(data.title));
		q.bindValue(":track",QVariant(data.track_num));
		q.bindValue(":year",QVariant(data.year));
		q.bindValue(":trackID", QVariant(data.id));
        q.bindValue(":genre", QVariant(data.genres.join(",")));
        q.bindValue(":filesize", QVariant(data.filesize));
        q.bindValue(":discnumber", QVariant(data.discnumber));

        if (!q.exec()) {
            qDebug() << ("SQL - Error: update track " + data.filepath);
		}

	return 0;
}


int CDatabaseConnector::insertTrackIntoDatabase (MetaData & data, int artistID, int albumID) {

	DB_TRY_OPEN(_database);

	QSqlQuery q (*_database);

	data.filepath.replace("//", "/");
	data.filepath.replace("\\\\", "\\");


	MetaData md =  getTrackByPath(data.filepath);
	int track_id = md.id;


	if(track_id > 0){
		data.id = md.id;
		data.artist_id = artistID;
		data.album_id = albumID;

        updateTrack(data);
		return 0;
	}


	QString querytext = QString("INSERT INTO tracks ") +
                "(filename,albumID,artistID,title,year,length,track,bitrate,genre,filesize,discnumber) " +
				"VALUES "+
                "(:filename,:albumID,:artistID,:title,:year,:length,:track,:bitrate,:genre,:filesize,:discnumber); ";

	q.prepare(querytext);

	q.bindValue(":filename",QVariant(data.filepath));
    q.bindValue(":albumID",QVariant(albumID));
    q.bindValue(":artistID",QVariant(artistID));
    q.bindValue(":length",QVariant(data.length_ms));
    q.bindValue(":year",QVariant(data.year));
    q.bindValue(":title",QVariant(data.title));
    q.bindValue(":track",QVariant(data.track_num));
    q.bindValue(":bitrate",QVariant(data.bitrate));
    q.bindValue(":genre", QVariant(data.genres.join(",")));
    q.bindValue(":filesize", QVariant(data.filesize));
    q.bindValue(":discnumber", QVariant(data.discnumber));


    if (!q.exec()) {
    	if(track_id < 0)
    		throw QString ("SQL - Error: insert track into database " + data.filepath);
    	else
    		throw QString ("SQL - Error: update track in database " + data.filepath);
    }
    return 0;
}



