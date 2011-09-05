#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
//#include "HelperStructs/Equalizer_presets.h"
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>
//#include <CSettingsStorage.h>




int CDatabaseConnector::getMaxArtistID(){
	 if (!this -> m_database.isOpen())
				        this -> m_database.open();

	 int max_id = -1;
		try {
			QSqlQuery q (this -> m_database);
			q.prepare("SELECT MAX(artistID) FROM artists;");

			if (!q.exec()) {
				throw QString ("SQL - Error: Could not get max artistID from DB");
			}


			while (q.next()) {
				max_id = q.value(0).toInt();
			}

			return max_id;


		}
		catch (QString ex) {
			qDebug() << ex;
			QSqlError er = this -> m_database.lastError();
			return max_id;
		}
}


int CDatabaseConnector::getArtistID (const QString & artist)  {
    QSqlQuery q (this -> m_database);
    int artistID = -1;
    q.prepare("select artistID from artists where name == ?;");
    q.addBindValue(QVariant(artist));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << artistID << " " << q.executedQuery();
        throw QString ("SQL - Error: get Artist ID" + artist);
    }
    if (q.next()) {
        artistID = q.value(0).toInt();
    }
    return artistID;
}



QString CDatabaseConnector::getArtistName (const int & id) {
    QSqlQuery q (this -> m_database);
    QString a;
    if (id!=-1) {
        q.prepare("select name from artists where artistid == ?;");
        q.addBindValue(QVariant (id));
        if (!q.exec()) {
            throw QString ("SQL - Error: getArtistName " + id);
        }
        if (q.next()) {
            a = q.value(0).toString().trimmed();
        }
    }
    else {
        qDebug() << "For some reason there is a -1 input of " << Q_FUNC_INFO;
    }
    return a;
}





void CDatabaseConnector::getAllArtists(vector<Artist>& result){
	 if (!this -> m_database.isOpen())
				        this -> m_database.open();

		try {
			QSqlQuery q (this -> m_database);
			q.prepare(QString("SELECT ") +
						"artists.artistID, " +
						"artists.name, " +
						"group_concat(albums.albumid), " +
						"count(tracks.trackid) "
						"FROM Tracks, artists, albums " +
						"WHERE Tracks.albumID = albums.albumID and artists.artistid = tracks.artistid " +
						"GROUP BY artists.artistID, artists.name " +
						"ORDER BY artists.name;");

			if (!q.exec()) {
				throw QString ("SQL - Error: Could not get all artists from database");
			}

			Artist artist;
			while (q.next()) {
				artist.id = q.value(0).toInt();
				artist.name = q.value(1).toString().trimmed();

				 QStringList list = q.value(2).toString().split(',');
				 list.removeDuplicates();
				artist.num_albums = list.size();
				artist.num_songs = q.value(3).toInt();

				result.push_back(artist);
			}


		}
		catch (QString ex) {
			qDebug() << "SQL - Error: getTracksFromDatabase";
			qDebug() << ex;
			QSqlError er = this -> m_database.lastError();
			qDebug() << er.driverText();
			qDebug() << er.databaseText();
			qDebug() << er.databaseText();
		}

}





void CDatabaseConnector::getAllArtistsByAlbum(int album, vector<Artist>& result){
	 if (!this -> m_database.isOpen())
					        this -> m_database.open();

			try {
				QSqlQuery q (this -> m_database);
				q.prepare(QString("SELECT ") +
							"artists.artistID, " +
							"artists.name, " +
							"group_concat(albums.albumid), " +
							"count(tracks.trackid) "
							"FROM Tracks, artists, albums " +
							"WHERE Tracks.albumID = albums.albumID and artists.artistid = tracks.artistid " +
							"AND albums.albumid=" + QString::number(album) + " " +
							"GROUP BY artists.artistID, artists.name " +
							"ORDER BY artists.name;");

				if (!q.exec()) {
					throw QString ("SQL - Error: Could not get all artists from database");
				}

				Artist artist;
				while (q.next()) {
					artist.id = q.value(0).toInt();
					artist.name = q.value(1).toString().trimmed();

					 QStringList list = q.value(2).toString().split(',');
					list.removeDuplicates();
					artist.num_albums = list.size();
					artist.num_songs = q.value(3).toInt();

					result.push_back(artist);
				}


			}
			catch (QString ex) {
				qDebug() << "SQL - Error: getTracksFromDatabase";
				qDebug() << ex;
				QSqlError er = this -> m_database.lastError();
				qDebug() << er.driverText();
				qDebug() << er.databaseText();
				qDebug() << er.databaseText();

			}




}




int CDatabaseConnector::insertArtistIntoDatabase (const QString & artist) {
	if (!this -> m_database.isOpen())
					this -> m_database.open();

	QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO artists (name) values (:artist);");
    q.bindValue(":artist", QVariant(artist));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << q.executedQuery();
        throw QString ("SQL - Error: insertArtistIntoDatabase " + artist);
    }
    return this -> getArtistID (artist);
}



int CDatabaseConnector::insertArtistIntoDatabase (const Artist & artist) {
	 if (!this -> m_database.isOpen())
				        this -> m_database.open();

	QSqlQuery q (this -> m_database);
	try{
		q.prepare("INSERT INTO artists (artistid, name) values (:id, :name);");
		q.bindValue(":id", QVariant(artist.id));
		q.bindValue(":name", QVariant(artist.name));
		if (!q.exec()) {
			qDebug()<< q.lastQuery() << q.executedQuery();
			throw QString ("SQL - Error: insertArtistIntoDatabase " + artist.name);
		}
	}



   catch (QString ex) {
   		qDebug() << "SQL - Error: insert albums into database";
   		qDebug() << ex;
   		QSqlError er = this -> m_database.lastError();
   		qDebug() << er.driverText();
   		qDebug() << er.databaseText();
   		qDebug() << er.databaseText();
   	}

    return this -> getArtistID (artist.name);
}




void CDatabaseConnector::getAllArtistsBySearchString(QString search, vector<Artist>& result){


	if (!this -> m_database.isOpen())
				this -> m_database.open();
			Artist artist;

			try {
				QSqlQuery q (this -> m_database);
				QString query;
				query = QString("SELECT * FROM ( ") +
						"SELECT " +
					"			artists.artistid as artistid, artists.name as name, COUNT(tracks.trackid), group_concat(albums.albumid) " +
					"			FROM albums, artists, tracks " +
					"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.name LIKE :search_in_artist " +
					"			GROUP BY artists.artistid, artists.name " +
					"		UNION " +
					"		SELECT  " +
					"			artists.artistid, artists.name, COUNT(tracks.trackid), group_concat(albums.albumid) " +
					"			FROM albums, artists, tracks " +
					"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.name LIKE :search_in_album " +
					"			GROUP BY artists.artistid, artists.name " +
					"		UNION " +
					"		SELECT  " +
					"			artists.artistid, artists.name, COUNT(tracks.trackid), group_concat(albums.albumid) " +
					"			FROM albums, artists, tracks " +
					"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.title LIKE :search_in_title " +
					"			GROUP BY artists.artistid, artists.name " +
					"	)  " +
					"	GROUP BY artistid, name ORDER BY name; ";

				q.prepare(query);
				q.bindValue(":search_in_title",QVariant(search));
				q.bindValue(":search_in_album",QVariant(search));
				q.bindValue(":search_in_artist",QVariant(search));
				if (!q.exec()) {
					throw QString ("SQL - Error: getTracksFromDatabase cannot execute search artist query" );
				}

				while (q.next()) {

					artist.id = q.value(0).toInt();
					artist.name = q.value(1).toString().trimmed();
					artist.num_songs = q.value(2).toInt();
					 QStringList list = q.value(3).toString().split(',');
					 list.removeDuplicates();
					 artist.num_albums = list.size();

					result.push_back(artist);
				}
			}


			catch (QString ex) {
				qDebug() << "SQL - Error: getTracksFromDatabase";
				qDebug() << ex;
				QSqlError er = this -> m_database.lastError();
				qDebug() << er.driverText();
				qDebug() << er.databaseText();
				qDebug() << er.databaseText();
			}
}
