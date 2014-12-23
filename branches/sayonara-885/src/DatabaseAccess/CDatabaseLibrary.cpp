/* CDatabaseLibrary.cpp */

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


void CDatabaseConnector::deleteTracksAlbumsArtists() {

	DB_RETURN_NOT_OPEN_VOID(_database);

	_database->transaction();

	bool err = false;

	for(int i=0; i<3; i++) {
		QSqlQuery q (*_database);

		if(i==0) q.prepare("delete from tracks;");
		else if(i==1) q.prepare("delete from artists;");
		else if(i==2) q.prepare("delete from albums;");

		if( !q.exec() ){
			show_error("Cannot delete all tracks, albums and artists");
			err = true;
		}
	}

	if(err){
		_database->commit();
	}
	else {
		_database->rollback();
	}
}



bool CDatabaseConnector::storeMetadata (MetaDataList & v_md)  {

	DB_RETURN_NOT_OPEN_BOOL(_database);

    int artistID = -1, albumID = -1;

    _database->transaction();

    foreach (MetaData data, v_md) {

		//first check if we know the artist and its id
		albumID = this -> getAlbumID(data.album);

		if (albumID == -1) {
			albumID = insertAlbumIntoDatabase((QString) data.album);
		}

		artistID = this -> getArtistID(data.artist);
		if (artistID == -1) {
			artistID = insertArtistIntoDatabase((QString) data.artist);
		}

		insertTrackIntoDatabase (data,artistID,albumID);

    }

    return _database->commit();
}

