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





