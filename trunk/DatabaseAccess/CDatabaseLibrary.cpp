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






bool CDatabaseConnector::storeMetadata (std::vector<MetaData> & in)  {



	if (!this -> m_database.isOpen())
        this -> m_database.open();
    if(!this->m_database.isOpen()) {
    	qDebug() << "Datbase cannot be opened";
    	qDebug() << m_database.lastError();
    	return false;
    }


    int artistID = -1, albumID = -1;

    m_database.transaction();
    foreach (MetaData data, in) {

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


            this -> insertTrackIntoDatabase (data,artistID,albumID);
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

