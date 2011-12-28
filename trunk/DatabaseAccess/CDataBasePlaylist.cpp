
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

using namespace std;


int CDatabaseConnector::getPlaylistById(int playlist_id, vector<MetaData>& vec_md){

	return 0;
}

int CDatabaseConnector::getPlaylistIdByName(QString playlist_name){
	if (!this -> m_database.isOpen())
			this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	QSqlQuery q(this->m_database);

	q.prepare("SELECT playlistid FROM playlists WHERE playlist_name = :playlist_name");
	q.bindValue("playlist_name", QVariant(playlist_name));
	if(!q.exec){
		return -2;
	}

	else {
		  if(q.next())
		      return q.value(0).toString();
	}
}

QString CDatabaseConnector::getPlaylistNameById(int playlist_id){
	return "";
}



int CDatabaseConnector::createPlaylist(QString playlist_name){


	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	QSqlQuery q(this->m_database);

	QString query_string = "INSERT INTO playlists (playlist) VALUES (:playlist_name);";
	q.prepare(query_string);
	q.bindValue("playlist_name", QVariant(playlist_name));
	if(!q.exec()){
		return 0;
	}

	else
		return getPlaylistByName(playlist_name);

}

int CDatabaseConnector::insertTrackIntoPlaylist(MetaData& md, int playlist_id, int pos){

	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	QSqlQuery q (this -> m_database);

	QString query_string = "insert into playlisttotracks " +
							"(trackid, playlistid, positioninplaylist) " +
							"values " +
							"(:track_id, :playlist_id, :position);";
	q.prepare(query_string);
	q.bindValue("track_id", QVariant(md.id));
	q.bindValue("playlist_id", QVariant(playlist_id));
	q.bindValue("position", QVariant(pos));

	if (!q.exec()) {

		throw QString ("SQL - Error: insertTrackIntoDatabase " + data.filepath);
	}

	return 0;
}

int CDatabaseConnector::insertPlaylist(const vector<MetaData>& vec_md, QString playlist_name){
	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return -1;

	// create playlist if neccessary
	int playlist_id = getPlaylistIdByName();
	if(playlist_id < 0){
		playlist_id = createPlaylist(playlist_name);
		if( playlist_id < 0){
			throw QString ("SQL - Error: cannot create playlist: " + playlist_name);
			return 0;
		}
	}


	m_database.transaction();

	// fill playlist
	for(int i=0; i<vec_md.size(); i++){
		try{
			insertTrackIntoPlaylist(md, playlist_id, i);
		}

		catch (QString ex) {
			qDebug() << "Error during inserting track into playlist";
			qDebug() << ex;
			QSqlError er = this -> m_database.lastError();
		}
	}

	m_database.commit();

}

int CDatabaseConnector::deletePlaylist(int playlist_id){

}
