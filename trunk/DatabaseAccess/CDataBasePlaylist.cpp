
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
#include <QMap>
#include <QSqlError>

using namespace std;

bool CDatabaseConnector::getAllPlaylists(QMap<int, QString>& mapping){

	if (!this -> m_database.isOpen())
			this -> m_database.open();

	if (!this -> m_database.isOpen()) return false;

	QSqlQuery q(this->m_database);

	QString querytext = QString("SELECT playlists.playlistID, playlists.playlist ") +
			"FROM playlists, playlisttotracks " +
			"WHERE playlists.playlistid = playlisttotracks.playlistid " +
			"GROUP BY playlists.playlistID, playlists.playlist;";
	q.prepare(querytext);
	if(!q.exec()){
		return false;
	}

	while(q.next()){
		int playlist_id = q.value(0).toInt();
		QString playlist_name = q.value(1).toString();
		mapping[playlist_id] = playlist_name;
	}

	return true;
}

bool CDatabaseConnector::getPlaylistById(int playlist_id, CustomPlaylist& pl){

	if (!this -> m_database.isOpen())
		this -> m_database.open();

	if (!this -> m_database.isOpen()) return false;

	pl.tracks.clear();
	pl.num_tracks = 0;
	pl.length = 0;
	pl.id = playlist_id;
	pl.name = getPlaylistNameById(playlist_id);

	QSqlQuery q(this->m_database);

	QString querytext = QString("SELECT ") +
			"tracks.filename 	AS filename, "		// 0
			"albums.name		AS albumName, "		// 1
			"tracks.albumID		AS albumID, " 		// 2
			"tracks.artistID	AS artistID, "		// 3
			"artists.name		AS artistName, "	// 4
			"tracks.title		AS title, "			// 5
			"tracks.year		AS year, "			// 6
			"tracks.length		AS length, "		// 7
			"tracks.track		AS track, "			// 8
			"tracks.bitrate		AS bitrate, "		// 9
			"tracks.TrackID 	AS trackID "		// 10
			"FROM tracks, albums, artists, playlists, playlisttotracks WHERE " +
			"playlists.playlistID = :playlist_id AND playlists.playlistID = playlistToTracks.playlistID AND " +
			"playlistToTracks.trackID = tracks.trackID AND "
			"tracks.albumID = albums.albumID AND tracks.artistID = artists.artistID; ";


	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);


	if (!q.exec()) {
		return false;
	}

	while (q.next()) {
		MetaData data;
		data.filepath =  q.value(0).toString();
		data.album = 	 q.value(1).toString().trimmed();
		data.album_id =  q.value(2).toInt();
		data.artist_id = q.value(3).toInt();
		data.artist = 	 q.value(4).toString().trimmed();
		data.title = 	 q.value(5).toString();
		data.year = 	 q.value(6).toInt();
		data.length_ms = q.value(7).toInt();
		data.track_num = q.value(8).toInt();
		data.bitrate = 	 q.value(9).toInt();
		data.id = 		 q.value(10).toInt();
		data.is_extern = false;

		pl.tracks.push_back(data);
		pl.num_tracks++;
		pl.length += (data.length_ms / 1000);
	}

	return true;

}

// negative, if error
// nonnegative else
int CDatabaseConnector::getPlaylistIdByName(QString playlist_name){
	if (!this -> m_database.isOpen())
			this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	QSqlQuery q(this->m_database);

	q.prepare("SELECT playlistid FROM playlists WHERE playlist = :playlist_name;");
	q.bindValue(":playlist_name", QVariant(playlist_name));
	if(!q.exec()){
		return -2;
	}

	else {
		  if(q.next()){
		      return q.value(0).toInt();
		  }

		  return -1;
	}
}

QString CDatabaseConnector::getPlaylistNameById(int playlist_id){
	if (!this -> m_database.isOpen())
				this -> m_database.open();

		if (!this -> m_database.isOpen()) return "";

		QSqlQuery q(this->m_database);

		q.prepare("SELECT playlistname FROM playlists WHERE playlistid = :playlistid;");
		q.bindValue(":playlistid", QVariant(playlist_id));
		if(!q.exec()){
			return "";
		}

		else {
			  if(q.next()){
			      return q.value(0).toString();
			  }

			  return "";
		}
}


bool CDatabaseConnector::insertTrackIntoPlaylist(MetaData& md, int playlist_id, int pos){

	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return false;

	QSqlQuery q (this -> m_database);
	qDebug() << "insert track: " << md.id << " into playlist " << playlist_id << " at " << pos;

	QString query_string = QString("INSERT INTO playlisttotracks ") +
							"(trackid, playlistid, position) " +
							"VALUES " +
							"(:track_id, :playlist_id, :position);";

	qDebug() <<

	q.prepare(query_string);
	q.bindValue(":track_id", QVariant(md.id));
	q.bindValue(":playlist_id", QVariant(playlist_id));
	q.bindValue(":position", QVariant(pos));

	if (!q.exec()) {
		qDebug() << "DB: cannot insert track into playlist";
		return false;
	}

	return true;
}


// returns 0 if everything ok
// negative otherwise
int CDatabaseConnector::createPlaylist(QString playlist_name){


	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	QSqlQuery q(this->m_database);
	qDebug() << "DB: create playlist " << playlist_name;
	QString query_string = "INSERT INTO playlists (playlist) VALUES (:playlist_name);";
	q.prepare(query_string);
	q.bindValue(":playlist_name", QVariant(playlist_name));
	if(!q.exec()){
		qDebug() << "DB: Cannot create playlist";
		return -1;
	}

	else
		return getPlaylistIdByName(playlist_name);

}


bool CDatabaseConnector::storePlaylist(const vector<MetaData>& vec_md, QString playlist_name){
	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return false;

	// create playlist if neccessary

	int playlist_id = getPlaylistIdByName(playlist_name);

	if(playlist_id >= 0){
		emptyPlaylist(playlist_id);
	}

	else {
		playlist_id = createPlaylist(playlist_name);
		if( playlist_id < 0){
			qDebug() << "playlist " << playlist_id << " could not be created";
			return false;
		}

		else
			qDebug() << "playlist " << playlist_id << " created";

	}


	m_database.transaction();

	// fill playlist
	for(uint i=0; i<vec_md.size(); i++){

		MetaData md = vec_md.at(i);
		bool success = insertTrackIntoPlaylist(md, playlist_id, i);
		if( !success ){
			qDebug() << "DB: Error during inserting track into playlist.. aborting";
			m_database.rollback();
			return false;
		}
	}

	m_database.commit();

	return true;

}



bool CDatabaseConnector::storePlaylist(const vector<MetaData>& vec_md, int playlist_id){
	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return false;

	// create playlist if neccessary

	QString playlist_name = getPlaylistNameById(playlist_id);


	if(playlist_name.size() >= 0){
		emptyPlaylist(playlist_id);
	}

	else {
		playlist_id = createPlaylist(playlist_name);
		if( playlist_id < 0){
			qDebug() << "playlist " << playlist_id << " could not be created";
			return false;
		}

		else{
			qDebug() << "playlist " << playlist_id << " created";
		}
	}


	m_database.transaction();

	// fill playlist
	for(uint i=0; i<vec_md.size(); i++){

		MetaData md = vec_md.at(i);
		bool success = insertTrackIntoPlaylist(md, playlist_id, i);
		if( !success ){
			qDebug() << "DB: Error during inserting track into playlist.. aborting";
			m_database.rollback();
			return false;
		}
	}

	m_database.commit();

	return true;

}

bool CDatabaseConnector::emptyPlaylist(int playlist_id){
	if (!this -> m_database.isOpen())
			this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	QSqlQuery q(this->m_database);
	QString querytext = QString("DELETE FROM playlistToTracks WHERE playlistID = :playlist_id;");
	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);

	if(!q.exec()){
		qDebug() << "DB: Playlist cannot be cleared";
		return false;
	}

	return true;
}


bool CDatabaseConnector::deletePlaylist(int playlist_id){
	if (!this -> m_database.isOpen())
			this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	emptyPlaylist(playlist_id);

	QSqlQuery q(this->m_database);
	QString querytext = QString("DELETE FROM playlists WHERE playlistID = :playlist_id;");
	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);
	return q.exec();

}
