
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
#include "HelperStructs/MetaData/MetaData.h"

#include <QFile>
#include <QSqlQuery>
#include <QMap>

bool CDatabaseConnector::getAllPlaylistSkeletons(QList<CustomPlaylistSkeleton>& skeletons, CDatabaseConnector::PlaylistChooserType type, SortOrderPlaylists sortorder) {

	skeletons.clear();

	QString sortorder_str;
	switch(sortorder){
		case IDAsc:
			sortorder_str = " ORDER BY playlists.playlistID ASC ";
			break;
		case IDDesc:
			sortorder_str = " ORDER BY playlists.playlistID DESC ";
			break;
		case NameAsc:
			sortorder_str = " ORDER BY playlists.playlist ASC ";
			break;
		case NameDesc:
			sortorder_str = " ORDER BY playlists.playlist DESC ";
			break;
		default:
			break;
	}

	QString type_clause;
	switch(type){
		case CDatabaseConnector::OnlyTemporary:
			type_clause = " AND playlists.temporary = 1 ";
			break;
		case CDatabaseConnector::OnlyNoTemporary:
			type_clause = " AND playlists.temporary = 0 ";
			break;
		default:
			break;

	}

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q(*_database);

	QString querytext = QString() +

			"SELECT "
			"playlists.playlistID, "
			"playlists.playlist, "
			"playlists.temporary "

			"FROM playlists, playlisttotracks "
			"WHERE playlists.playlistid = playlisttotracks.playlistid " + type_clause +
			"GROUP BY playlists.playlistID, playlists.playlist " + sortorder_str +
			";";

	q.prepare(querytext);

	if(!q.exec()) {
		show_error("Cannot fetch all playlists", q);
		return false;
	}

	while(q.next()) {
		CustomPlaylistSkeleton skeleton;
		skeleton.id = q.value(0).toInt();
		skeleton.name =	q.value(1).toString();
		skeleton.is_temporary =	(q.value(2).toInt() == 1) ? true : false;
		skeletons << skeleton;
	}

	return true;
}

bool CDatabaseConnector::getPlaylistSkeletonById(CustomPlaylistSkeleton& skeleton){
	DB_RETURN_NOT_OPEN_BOOL(_database);

	if(skeleton.id < 0){
		qDebug() << "Cannot fetch playlist -1";
		return false;
	}

	QSqlQuery q(*_database);

	QString querytext = QString() +

			"SELECT "
			"playlists.playlistID, "
			"playlists.playlist, "
			"playlists.temporary "
			"FROM playlists "
			"WHERE playlists.playlistid = :playlist_id "
			"GROUP BY playlists.playlistID, playlists.playlist "
			"ORDER BY playlists.playlist;";

	q.prepare(querytext);
	q.bindValue(":playlist_id", skeleton.id);

	if(!q.exec()) {
		show_error("Cannot fetch all playlists", q);
		return false;
	}

	if(q.next()) {
		skeleton.id = q.value(0).toInt();
		skeleton.name =	q.value(1).toString();
		skeleton.is_temporary =	(q.value(2).toInt() == 1) ? true : false;
		return true;
	}

	return false;
}

bool CDatabaseConnector::getPlaylistById(CustomPlaylist& pl) {

	if(!getPlaylistSkeletonById(pl)){
		qDebug() << "Get playlist by id: cannot fetch skeleton id " << pl.id;
		return false;
	}

	DB_RETURN_NOT_OPEN_BOOL(_database);

	pl.tracks.clear();

	QSqlQuery q(*_database);

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
            "tracks.TrackID 	AS trackID, "		// 10
			"playlistToTracks.filepath AS filepath "       // 11
			"FROM tracks, albums, artists, playlists, playlisttotracks "
			"WHERE playlists.playlistID = :playlist_id "
			"AND playlists.playlistID = playlistToTracks.playlistID "
			"AND playlistToTracks.trackID = tracks.trackID "
			"AND tracks.albumID = albums.albumID "
			"AND tracks.artistID = artists.artistID "
            "ORDER BY playlistToTracks.position ASC; ";

	q.prepare(querytext);
	q.bindValue(":playlist_id", pl.id);

	if (!q.exec()) {
		show_error(QString("Cannot get tracks for playlist ") + QString::number(pl.id), q);
	}

	else{
        while (q.next()) {
            MetaData data;
			data.set_filepath(q.value(0).toString());
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
        }
    }

	QString querytext2 = QString()+
			"SELECT "
			"playlisttotracks.filepath AS filepath, "
			"playlisttotracks.position AS position "
			"FROM playlists, playlisttotracks "
			"WHERE playlists.playlistID = :playlist_id "
			"AND playlists.playlistID =  playlistToTracks.playlistID "
			"AND playlistToTracks.trackID <= 0 "
			"ORDER BY playlistToTracks.position ASC; ";

    QSqlQuery q2(*_database);
    q2.prepare(querytext2);
	q2.bindValue(":playlist_id", pl.id);

    if(!q2.exec()) {
		show_error(QString("Playlist by id: Cannot fetch playlist ") + QString::number(pl.id), q2);
        return false;
    }

    while (q2.next()) {

        int position = q2.value(1).toInt();

        MetaData data;
		data.set_filepath( q2.value(0).toString() );
        data.id = -1;
        data.is_extern = true;

		for(int row=0; row<=pl.tracks.size(); row++) {
            if( row >= position) {
                pl.tracks.insert(row, data);
                break;
            }
        }
    }

	return true;
}

// negative, if error
// nonnegative else
int CDatabaseConnector::getPlaylistIdByName(const QString& name) {

	DB_RETURN_NOT_OPEN_INT(_database);

	QSqlQuery q(*_database);

	q.prepare("SELECT playlistid FROM playlists WHERE playlist = :playlist_name;");
	q.bindValue(":playlist_name", name);

	if(!q.exec()) {
		show_error(QString("Playlist by name: Cannot fetch playlist ") + name, q);
		return -1;
	}

	else {
		  if(q.next()) {
		      return q.value(0).toInt();
		  }

		  return -1;
	}
}


bool CDatabaseConnector::insertTrackIntoPlaylist(const MetaData& md, int playlist_id, int pos) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);

	QString query_string = QString("INSERT INTO playlisttotracks ") +
                            "(trackid, playlistid, position, filepath) " +
							"VALUES " +
                            "(:track_id, :playlist_id, :position, :filepath);";


	q.prepare(query_string);
	q.bindValue(":track_id", md.id);
	q.bindValue(":playlist_id", playlist_id);
	q.bindValue(":position", pos);
	q.bindValue(":filepath", md.filepath());

	if (!q.exec()) {
		show_error("Cannot insert track into playlist", q);
		return false;
	}

	return true;
}


// returns id if everything ok
// negative otherwise
int CDatabaseConnector::createPlaylist(QString playlist_name, bool temporary) {

	DB_RETURN_NOT_OPEN_INT(_database);

	int temporary_int = (temporary) ? 1 : 0;

	QString query_string = "INSERT INTO playlists (playlist, temporary) VALUES (:playlist_name, :temporary);";

	QSqlQuery q(*_database);

	q.prepare(query_string);
	q.bindValue(":playlist_name", QVariant(playlist_name));
	q.bindValue(":temporary", QVariant(temporary_int));

	if(!q.exec()) {
		show_error("Cannot create playlist", q);
		return -1;
	}

	return getPlaylistIdByName(playlist_name);

}

bool CDatabaseConnector::updatePlaylistTempState(int playlist_id, bool temporary){

	DB_RETURN_NOT_OPEN_BOOL(_database);

	int temporary_int = (temporary) ? 1 : 0;

	QSqlQuery q(*_database);
	QString query_string = QString("UPDATE playlists SET temporary=:temporary WHERE playlistID = :playlist_id;");
	q.prepare(query_string);
	q.bindValue(":playlist_id", playlist_id);
	q.bindValue(":temporary", temporary_int);

	if(!q.exec()){
		show_error("Cannot update playlist", q);
		return false;
	}

	return true;
}

bool CDatabaseConnector::storePlaylist(const MetaDataList& vec_md, QString playlist_name, bool temporary) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	int playlist_id = getPlaylistIdByName(playlist_name);
	if(playlist_name.isEmpty()){
		qDebug() << "Warning! Try to save empty playlist";
	}

	if(playlist_id >= 0) {
		emptyPlaylist(playlist_id);
	}

	else {
		playlist_id = createPlaylist(playlist_name, temporary);
		if( playlist_id < 0) {
			return false;
		}
	}

	_database->transaction();

	// fill playlist
    for(int i=0; i<vec_md.size(); i++) {

        const MetaData& md = vec_md[i];

		bool success = insertTrackIntoPlaylist(md, playlist_id, i);

		if( !success ) {
			_database->rollback();
			return false;
		}
	}

	_database->commit();

	return true;
}



bool CDatabaseConnector::storePlaylist(const MetaDataList& vec_md, int playlist_id, bool temporary) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	CustomPlaylist pl;
	pl.id = playlist_id;

	bool success = getPlaylistById(pl);
	if(!success){
		qDebug() << "Store: Cannot fetch playlist: " << pl.id;
		return false;
	}

	if( !pl.name.isEmpty() ) {
		emptyPlaylist(playlist_id);
	}

	else {

		playlist_id = createPlaylist(pl.name, temporary);

		if( playlist_id < 0) {
			return false;
		}
	}

	_database->transaction();

	// fill playlist
    for(int i=0; i<vec_md.size(); i++) {

        const MetaData& md = vec_md.at(i);

		bool success = insertTrackIntoPlaylist(md, playlist_id, i);

		if( !success ) {
			_database->rollback();
			return false;
		}
	}

	_database->commit();

	return true;
}

bool CDatabaseConnector::emptyPlaylist(int playlist_id) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q(*_database);
	QString querytext = QString("DELETE FROM playlistToTracks WHERE playlistID = :playlist_id;");
	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);

	if(!q.exec()) {
		show_error("DB: Playlist cannot be cleared", q);
		return false;
	}

	return true;
}


bool CDatabaseConnector::deletePlaylist(int playlist_id) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	emptyPlaylist(playlist_id);

	QSqlQuery q(*_database);
	QString querytext = QString("DELETE FROM playlists WHERE playlistID = :playlist_id;");

	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);

	if(!q.exec()){
		show_error(QString("Cannot delete playlist ") + QString::number(playlist_id), q);
		return false;
	}

	return true;
}


bool CDatabaseConnector::deleteTrackFromPlaylists(int track_id) {

	DB_TRY_OPEN(_database);
	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q(*_database);
	QString querytext = QString("DELETE FROM playlistToTracks WHERE trackID = :track_id;");

	q.prepare(querytext);
	q.bindValue(":track_id", track_id);

	if(!q.exec()){
		show_error(QString("Cannot delete track ") + QString::number(track_id) + "from playlist", q);
		return false;
	}

	return true;
}
