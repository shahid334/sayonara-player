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
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>




void CDatabaseConnector::deleteTracksAlbumsArtists(){

		QSqlQuery q (this -> m_database);

		m_database.transaction();

		bool err = false;
		for(int i=0; i<3; i++){
			if(i==0) q.prepare("delete from tracks;");
			else if(i==1) q.prepare("delete from artists;");
			else if(i==2) q.prepare("delete from albums;");

			try{
				q.exec();
			}

			catch(QString ex){
				err = true;
				qDebug() << q.lastQuery();
				qDebug() << ex;
				qDebug() << q.executedQuery();

			}
		}

		 if(!err) m_database.commit();
}



bool CDatabaseConnector::storeMetadata (MetaDataList & v_md)  {

	DB_TRY_OPEN(m_database);

    int artistID = -1, albumID = -1;


    m_database.transaction();
    foreach (MetaData data, v_md) {

    	try {

    		 //first check if we know the artist and its id
    		 albumID = this -> getAlbumID(data.album);

			if (albumID == -1) {
				albumID = insertAlbumIntoDatabase((QString) data.album);
			}

            artistID = this -> getArtistID(data.artist);
            if (artistID == -1) {
                artistID = insertArtistIntoDatabase((QString) data.artist);
            }

            this -> insertTrackIntoDatabase (data,artistID,albumID, false);
        }

        catch (QString ex) {
            qDebug() << "Error during inserting of metadata into database";
            qDebug() << ex;
            QSqlError er = this -> m_database.lastError();
        }

    }
    m_database.commit();
    return true;
}

