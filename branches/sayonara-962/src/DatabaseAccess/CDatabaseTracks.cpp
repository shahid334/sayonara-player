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
#include "HelperStructs/Helper.h"

#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QDir>

#include <cstdlib>
#include <QElapsedTimer>

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
    "tracks.discnumber AS discnumber, " \
    "tracks.rating AS rating " \
    "FROM tracks " \
    "INNER JOIN albums ON tracks.albumID = albums.albumID " \
	"INNER JOIN artists ON tracks.artistID = artists.artistID " \
	"WHERE filetype is null "



bool CDatabaseConnector::db_fetch_tracks(QSqlQuery& q, MetaDataList& result) {

	result.clear();

	if (!q.exec()) {
		show_error("Cannot fetch tracks from database", q);
		return false;
	}

	if(!q.last()){
		return true;
	}

	int i=0;
	int n_rows = q.at() + 1;

	result.resize(n_rows);

	for(bool is_element = q.first(); is_element; is_element = q.next(), i++){

		MetaData& data = result[i];

		data.id = 		 q.value(0).toInt();
		data.title = 	 q.value(1).toString();
		data.length_ms = q.value(2).toInt();
		data.year = 	 q.value(3).toInt();
		data.bitrate = 	 q.value(4).toInt();
		data.set_filepath(q.value(5).toString());
		data.track_num = q.value(6).toInt();
		data.album_id =  q.value(7).toInt();
		data.artist_id = q.value(8).toInt();
		data.album = 	 q.value(9).toString().trimmed();
		data.artist = 	 q.value(10).toString().trimmed();
		data.genres =	 q.value(11).toString().split(",");
		data.filesize =  q.value(12).toInt();
		data.discnumber = q.value(13).toInt();
		data.rating = q.value(14).toInt();
	}

	return true;
}


QString CDatabaseConnector::append_track_sort_string(QString querytext, SortOrder sort) {


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
    else if(sort == TrackRatingAsc) querytext += QString(" ORDER BY rating ASC;");
    else if(sort == TrackRatingDesc) querytext += QString(" ORDER BY rating DESC;");


	else querytext += ";";

	return querytext;
}


bool CDatabaseConnector::getMultipleTracksByPath(QStringList& paths, MetaDataList& v_md) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

    _database->transaction();

	for(const QString& path : paths) {
        MetaData md = getTrackByPath(path);
        v_md.push_back(md);
    }

    _database->commit();

	return (v_md.size() == paths.size());
}


MetaData CDatabaseConnector::getTrackByPath(const QString& path) {

	DB_TRY_OPEN(_database);

	MetaDataList vec_data;

	QSqlQuery q (*_database);

	QString querytext = TRACK_SELECTOR + " AND tracks.filename = :filename;";
	q.prepare(querytext);
    q.bindValue(":filename", path);

	MetaData md;
	md.id = -1;
	md.set_filepath(path);

	if(!db_fetch_tracks(q, vec_data)) return md;

    if(vec_data.size() == 0) {
        md.is_extern = true;
        return md;
    }

    return vec_data[0];
}

MetaData CDatabaseConnector::getTrackById(int id) {

	DB_TRY_OPEN(_database);

	MetaDataList vec_data;

	QSqlQuery q (*_database);
	QString querytext = TRACK_SELECTOR + " AND tracks.trackID = :track_id;";

	q.prepare(querytext);
	q.bindValue(":track_id", QVariant(id));

	MetaData md;
	md.id = -1;

	if(!db_fetch_tracks(q, vec_data)) return md;

    if(vec_data.size() == 0) {
        md.is_extern = true;
        return md;
    }
	return vec_data[0];
}


bool CDatabaseConnector::getTracksFromDatabase (MetaDataList & returndata, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);

	QString querytext = append_track_sort_string(TRACK_SELECTOR, sort);
	q.prepare(querytext);

	return db_fetch_tracks(q, returndata);
}

bool CDatabaseConnector::getAllTracksByAlbum(int album, MetaDataList& returndata, Filter filter, SortOrder sort, int discnumber) {

	bool success;
	QList<int> list;

	MetaDataList mdlist;
	list << album;
	returndata.clear();

	success = getAllTracksByAlbum(list, mdlist, filter, sort);
	
	if(discnumber < 0) returndata = mdlist;
	
	foreach(const MetaData& md, mdlist) {
		if(discnumber != md.discnumber) continue;
		returndata.push_back(md);
	}

	return success;
}

bool CDatabaseConnector::getAllTracksByAlbum(QList<int> albums, MetaDataList& returndata, Filter filter, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	QString querytext = TRACK_SELECTOR;

	if(albums.size() == 0) return false;

	else if(albums.size() == 1) {
		querytext += "AND tracks.albumid=:albumid ";
	}

	else {
		querytext += "AND (tracks.albumid=:albumid ";
		for(int i=1; i<albums.size(); i++) {
			querytext += "OR tracks.albumid=:albumid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	if( !filter.filtertext.isEmpty() ) {


		switch(filter.by_searchstring) {
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
                                    "	WHERE t2.cissearch LIKE :filter1 "+

									"	UNION SELECT t3.trackid "+
									"	FROM tracks t3, albums a2 "+
                                    "	WHERE a2.albumid = t3.albumid AND a2.cissearch LIKE :filter2 "+

									"	UNION SELECT t4.trackid " +
									"	FROM tracks t4, albums a3, artists ar2" +
									"	WHERE t4.albumid = a3.albumid " +
									"	AND t4.artistid = ar2.artistid " +
                                    "	AND ar2.cissearch LIKE :filter3 "

								") ";
				break;


		}

	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":albumid", QVariant(albums[0]));
	for(int i=1; i<albums.size(); i++) {
		q.bindValue(QString(":albumid_") + QString::number(i), albums[i]);
	}

	if(filter.filtertext.length() > 0) {
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring) {
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

	return db_fetch_tracks(q, returndata);

}

bool CDatabaseConnector::getAllTracksByArtist(int artist, MetaDataList& returndata, Filter filter, SortOrder sort) {

	QList<int> list;
	list << artist;
	return getAllTracksByArtist(list, returndata, filter, sort);
}

bool CDatabaseConnector::getAllTracksByArtist(QList<int> artists, MetaDataList& returndata, Filter filter, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	QString querytext = TRACK_SELECTOR;

	if(artists.size() == 0){
		return false;
	}

	if(artists.size() == 1) {
		querytext += "AND tracks.artistid=:artistid ";
	}

	else {
		querytext += "AND (tracks.artistid=:artistid ";
		for(int i=1; i<artists.size(); i++) {
			querytext += "OR tracks.artistid=:artistid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	if(filter.filtertext.length() > 0 ) {
		switch(filter.by_searchstring) {

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
                            "	WHERE t2.cissearch LIKE :filter1 "
							"	UNION SELECT t3.trackID "
							"	FROM tracks t3, albums a2"
                            "	WHERE a2.albumID = t3.albumID AND a2.cissearch LIKE :filter2 "
							"	UNION SELECT t4.trackID "
							"	FROM tracks t4, albums a3, artists ar2"
                            "	WHERE t4.albumid = a3.albumid AND t4.artistID = ar2.artistID AND ar2.cissearch LIKE :filter3 "
						") ";
				break;
		}
		// consider the case, that the search string may fit to the title
		// union the case that the search string may fit to the album

	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":artist_id", QVariant(artists[0]));
	for(int i=1; i<artists.size(); i++) {
		q.bindValue(QString(":artistid_") + QString::number(i), artists[i]);
	}

	if(filter.filtertext.length() > 0 ) {
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.by_searchstring) {
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

	return db_fetch_tracks(q, returndata);
}


bool CDatabaseConnector::getAllTracksBySearchString(Filter filter, MetaDataList& result, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	QString querytext;

	switch(filter.by_searchstring) {

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
                            "SELECT tracks.trackID FROM tracks WHERE tracks.cissearch LIKE :search_in_title " +
                            "UNION "+
                            "SELECT tracks.trackID FROM tracks INNER JOIN albums ON tracks.albumID = albums.albumID AND albums.cissearch LIKE :search_in_album "
                            "UNION "+
                            "SELECT tracks.trackID FROM tracks INNER JOIN artists ON tracks.artistID = artists.artistID AND artists.cissearch LIKE :search_in_artist "
                            ") ";

			break;
	}




	querytext = append_track_sort_string(querytext, sort);
	q.prepare(querytext);


	switch(filter.by_searchstring) {

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


	return db_fetch_tracks(q, result);
}

bool CDatabaseConnector::deleteTrack(int id){

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	QString querytext = QString("DELETE FROM tracks WHERE trackID = :track_id;");

	q.prepare(querytext);
	q.bindValue(":track_id", id);

	if (!q.exec()) {
		show_error(QString("Cannot delete track") + QString::number(id), q);
		return false;
	}

	deleteFromAllPlaylistChooser(id);

	return true;
}


bool CDatabaseConnector::deleteTracks(const QList<int>& ids){

	 int n_files = 0;
	 bool success;

	_database->transaction();

		for(const int& id : ids){
			if( deleteTrack(id) ){
				n_files++;
			};
		}

	success = _database->commit();

	return success && (n_files == ids.size());
}


bool CDatabaseConnector::deleteTracks(const MetaDataList& v_md) {

	int success = 0;

	_database->transaction();

		for(const MetaData& md : v_md){
			if( deleteTrack(md.id) ){
				success++;
			};
		}

	_database->commit();

	return (success == v_md.size());
}

bool CDatabaseConnector::deleteInvalidTracks(){

	bool success;

	MetaDataList v_md;
	QMap<QString, int> map;
	QList<int> to_delete;
	MetaDataList v_md_update;

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q(*_database);


	if(!getTracksFromDatabase(v_md)){
		qDebug() << "Cannot get tracks from db";
		return false;
	}

	int idx = 0;
	for(const MetaData& md : v_md){

		if(map.contains(md.filepath())){
			qDebug() << "found double path: " << md.filepath();
			int old_idx = map[md.filepath()];
			to_delete << md.id;
			v_md_update << v_md[old_idx];
		}

		else{
			map.insert(md.filepath(), idx);
		}

		idx++;
	}

	qDebug() << "Will delete " << to_delete;
	success = deleteTracks(to_delete);
	qDebug() << "delete tracks " << success;

	success = deleteTracks(v_md_update);
	qDebug() << "delete other tracks " << success;

	success = storeMetadata(v_md_update);
	qDebug() << "update tracks " << success;

	return false;
}


bool CDatabaseConnector::updateTrack(const MetaData& md) {

	if(md.id == -1) return false;

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);

	q.prepare("UPDATE Tracks "
			  "SET albumID=:albumID, "
				   "artistID=:artistID, "
				   "title=:title, "
				   "year=:year, "
				   "length=:length, "
				   "bitrate=:bitrate, "
				   "track=:track, "
				   "genre=:genre, "
				   "filesize=:filesize, "
				   "discnumber=:discnumber, "
				   "cissearch=:cissearch, "
				   "rating=:rating "
				"WHERE TrackID = :trackID;");

	q.bindValue(":albumID",QVariant(md.album_id));
	q.bindValue(":artistID",QVariant(md.artist_id));
	q.bindValue(":title",QVariant(md.title));
	q.bindValue(":track",QVariant(md.track_num));
	q.bindValue(":length", QVariant(md.length_ms));
	q.bindValue(":bitrate", QVariant(md.bitrate));
	q.bindValue(":year",QVariant(md.year));
	q.bindValue(":trackID", QVariant(md.id));
	q.bindValue(":genre", QVariant(md.genres.join(",")));
	q.bindValue(":filesize", QVariant(md.filesize));
	q.bindValue(":discnumber", QVariant(md.discnumber));
	q.bindValue(":rating", QVariant(md.rating));
	q.bindValue(":cissearch", QVariant(md.title.toLower()));

	if (!q.exec()) {
		show_error(QString("Cannot update track ") + md.filepath(), q);
		return false;
	}

	return true;
}

bool CDatabaseConnector::updateTracks(const MetaDataList& lst) {

	bool success;
	int n_files = 0;

    _database->transaction();
	for(const MetaData& md : lst){

		if(updateTrack(md)){
			n_files++;
		}
	}

	success = _database->commit();

	return success && (n_files == lst.size());
}

int CDatabaseConnector::insertTrackIntoDatabase (const MetaData & md, int artistID, int albumID) {

	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q (*_database);

	MetaData md_tmp = getTrackByPath( md.filepath() );

	if( md_tmp.id >= 0 ) {
		MetaData track_copy = md;
		track_copy.id = md_tmp.id;
		track_copy.artist_id = artistID;
		track_copy.album_id = albumID;

		if( updateTrack(track_copy) ){
			return md_tmp.id;
		}
	}

	qDebug() << "insert new track: " << md.filepath();

	QString querytext = QString("INSERT INTO tracks ") +
                "(filename,albumID,artistID,title,year,length,track,bitrate,genre,filesize,discnumber,cissearch) " +
				"VALUES "+
                "(:filename,:albumID,:artistID,:title,:year,:length,:track,:bitrate,:genre,:filesize,:discnumber,:cissearch); ";

	q.prepare(querytext);

	q.bindValue(":filename", md.filepath());
	q.bindValue(":albumID", albumID);
	q.bindValue(":artistID",artistID);
	q.bindValue(":length", md.length_ms);
	q.bindValue(":year", md.year);
	q.bindValue(":title", md.title);
	q.bindValue(":track", md.track_num);
	q.bindValue(":bitrate", md.bitrate);
	q.bindValue(":genre", md.genres.join(","));
	q.bindValue(":filesize", md.filesize);
	q.bindValue(":discnumber", md.discnumber);
	q.bindValue(":cissearch", md.title.toLower());

    if (!q.exec()) {

		show_error(QString("Cannot insert track into database ") + md.filepath(), q);
		return -1;
    }

	return getTrackByPath(md.filepath()).id;
}



