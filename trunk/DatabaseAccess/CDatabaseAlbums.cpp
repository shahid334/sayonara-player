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





int CDatabaseConnector::getAlbumID (const QString & album)  {
    QSqlQuery q (this -> m_database);
    int albumID = -1;
    q.prepare("select albumID from albums where name == ?;");
    q.addBindValue(QVariant(album));
    if (!q.exec()) {
        throw QString ("SQL - Error: getAlbumID" + album);
    }
    if (q.next()) {
        albumID = q.value(0).toInt();
    }
    return albumID;
}


int CDatabaseConnector::getMaxAlbumID(){
	 if (!this -> m_database.isOpen())
				        this -> m_database.open();

	 int max_id = -1;
		try {
			QSqlQuery q (this -> m_database);
			q.prepare("SELECT MAX(albumID) FROM albums;");

			if (!q.exec()) {
				throw QString ("SQL - Error: Could not get max albumID from DB");
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

Album CDatabaseConnector::getAlbumByID(const int& id){
	 if (!this -> m_database.isOpen())
				        this -> m_database.open();

		try {
			QSqlQuery q (this -> m_database);
			QString querytext = QString("SELECT ") +
					"albums.albumID, " +
					"albums.name as name, " +
					"SUM(tracks.length) / 1000, " +
					"count(tracks.trackid), " +
					"max(tracks.year) as year, " +
					"group_concat(artists.name) " +
					"FROM albums, tracks, artists " +
					"WHERE albums.albumID = :id AND tracks.albumID = albums.albumID AND artists.artistID = tracks.artistID;";


			q.prepare(querytext);
			q.bindValue(":id", QVariant(id));

			if (!q.exec()) {
				throw QString ("SQL - Error: Could not get all albums from database");
			}

			Album album;
			while (q.next()) {
				album.id = q.value(0).toInt();
				album.name = q.value(1).toString().trimmed();
				album.length_sec = q.value(2).toInt();
				album.num_songs = q.value(3).toInt();
				album.year = q.value(4).toInt();
				QStringList artistList = q.value(5).toString().split(',');
				artistList.removeDuplicates();
				album.artists = artistList;
				album.is_sampler = (artistList.size() > 1);
			}

			return album;


		}
		catch (QString ex) {
			qDebug() << "SQL - Error: getAlbumsFromDatabase";
			qDebug() << ex;
			QSqlError er = this -> m_database.lastError();
			qDebug() << er.driverText();
			qDebug() << er.databaseText();
			qDebug() << er.databaseText();
		}
}

QString CDatabaseConnector::getAlbumName (const int & id) {
    QSqlQuery q (this -> m_database);
    QString a;
    if (id!= -1) {
        q.prepare("select name from albums where albumid == ?;");
        q.addBindValue(QVariant(id));
        if (!q.exec()) {
            throw QString ("SQL - Error: getAlbumName " + id);
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


void CDatabaseConnector::getAllAlbums(vector<Album>& result, QString sort_order){
	 if (!this -> m_database.isOpen())
			        this -> m_database.open();

	try {
		QSqlQuery q (this -> m_database);
		QString querytext = QString("SELECT ") +
				"albums.albumID, " +
				"albums.name as name, " +
				"SUM(tracks.length) / 1000, " +
				"count(tracks.trackid), " +
				"max(tracks.year) as year, " +
				"group_concat(artists.name) " +
				"FROM albums, Tracks, artists " +
				"WHERE Tracks.albumID = albums.albumID and artists.artistid = tracks.artistid " +
				"GROUP BY albums.albumID, albums.name";


		if(sort_order == "name asc") querytext += QString(" ORDER BY name ASC;");
		else if(sort_order == "name desc") querytext += QString(" ORDER BY name DESC;");
		else if(sort_order == "year asc")querytext += QString(" ORDER BY year ASC;");
		else if(sort_order == "year desc")querytext += QString(" ORDER BY year DESC;");

		q.prepare(querytext);

		if (!q.exec()) {
			throw QString ("SQL - Error: Could not get all albums from database");
		}

		Album album;
		while (q.next()) {
			album.id = q.value(0).toInt();
			album.name = q.value(1).toString().trimmed();
			album.length_sec = q.value(2).toInt();
			album.num_songs = q.value(3).toInt();
			album.year = q.value(4).toInt();
			QStringList artistList = q.value(5).toString().split(',');
			artistList.removeDuplicates();
			album.artists = artistList;
			album.is_sampler = (artistList.size() > 1);

			result.push_back(album);
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



void CDatabaseConnector::getAllAlbumsByArtist(int artist, vector<Album>& result, QString filter, QString sort_order){
	 if (!this -> m_database.isOpen())
		this -> m_database.open();

	try {
		QSqlQuery q (this -> m_database);
		QString querytext = QString("SELECT ") +
				"albums.albumID, " +
				"albums.name as name, " +
				"SUM(tracks.length) / 1000, " +
				"count(tracks.trackid), " +
				"max(tracks.year) as year, " +
				"group_concat(artists.name) " +
				"FROM albums, tracks, artists " +
				"WHERE tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid AND artists.artistid = :artist_id ";

		if(filter.length() > 0){
				querytext += QString("AND tracks.trackid IN ( ") +
										"SELECT t2.trackid " +
										"FROM tracks t2 "+
										"WHERE t2.title LIKE :filter1 "+

										"UNION SELECT t3.trackid "+
										"FROM tracks t3, albums "+
										"WHERE albums.albumid = t3.albumid AND albums.name LIKE :filter2 "+

										"UNION SELECT t4.trackid " +
										"FROM tracks t4, albums, artists " +
										"WHERE t4.albumid = albums.albumid AND t4.artistid = artists.artistid AND artists.name LIKE :filter3 "

									") ";
		}


		querytext += QString("GROUP BY albums.albumID, albums.name ");
		if(sort_order == "name asc") querytext += QString(" ORDER BY name ASC;");
		else if(sort_order == "name desc") querytext += QString(" ORDER BY name DESC;");
		else if(sort_order == "year asc")querytext += QString(" ORDER BY year ASC;");
		else if(sort_order == "year desc")querytext += QString(" ORDER BY year DESC;");



		q.prepare(querytext);
		q.bindValue(":artist_id", QVariant(artist));
		if(filter.length() > 0){
			q.bindValue(":filter1", QVariant(filter));
			q.bindValue(":filter2", QVariant(filter));
			q.bindValue(":filter3", QVariant(filter));
		}


		if (!q.exec()) {
			throw QString ("SQL - Error: Could not get all albums from database");
		}

		Album album;
		while (q.next()) {
			album.id = q.value(0).toInt();
			album.name = q.value(1).toString().trimmed();
			album.length_sec = q.value(2).toInt();
			album.num_songs = q.value(3).toInt();
			album.year = q.value(4).toInt();
			QStringList artistList = q.value(5).toString().split(',');
			artistList.removeDuplicates();
			album.artists = artistList;
			album.is_sampler = (artistList.size() > 1);

			result.push_back(album);
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


void CDatabaseConnector::getAllAlbumsBySearchString(QString search, vector<Album>& result, QString sort_order){
	if (!this -> m_database.isOpen())
				this -> m_database.open();
			Album album;

			try {
				QSqlQuery q (this -> m_database);
				QString query;
				query = QString("SELECT * FROM ( ") +
							"SELECT " +
								"albums.albumid as albumid, albums.name as name, COUNT(tracks.trackid), SUM(tracks.length)/1000, MAX(tracks.year) as myear, group_concat(artists.name) " +
								"FROM albums, artists, tracks " +
								"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.name LIKE :search_in_album " +
								"GROUP BY albums.albumid, albums.name " +
							"UNION " +
							"SELECT  " +
								"albums.albumid, albums.name, COUNT(tracks.trackid), SUM(tracks.length)/1000, MAX(tracks.year) as myear, group_concat(artists.name) " +
								"FROM albums, artists, tracks " +
								"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.title LIKE :search_in_title " +
								"GROUP BY albums.albumid, albums.name " +
							"UNION " +
							"SELECT " +
								"albums.albumid, albums.name, COUNT(tracks.trackid), SUM(tracks.length)/1000, MAX(tracks.year) as myear, group_concat(artists.name) " +
								"FROM albums, artists, tracks " +
								"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.name LIKE :search_in_artist " +
								"GROUP BY albums.albumid, albums.name " +
						") " +
						"GROUP BY albumid, name";

				if(sort_order == "name asc") query += QString(" ORDER BY name ASC;");
				else if(sort_order == "name desc") query += QString(" ORDER BY name DESC;");
				else if(sort_order == "year asc")query += QString(" ORDER BY myear ASC;");
				else if(sort_order == "year desc")query += QString(" ORDER BY myear DESC;");


				q.prepare(query);
				q.bindValue(":search_in_title",QVariant(search));
				q.bindValue(":search_in_album",QVariant(search));
				q.bindValue(":search_in_artist",QVariant(search));

				if (!q.exec()) {
					throw QString ("SQL - Error: getTracksFromDatabase cannot execute search album query" );
				}

				while (q.next()) {

					album.id = q.value(0).toInt();
					album.name = q.value(1).toString().trimmed();
					album.num_songs = q.value(2).toInt();
					album.length_sec = q.value(3).toInt();
					album.year = q.value(4).toInt();
					album.artists = QStringList();
					QStringList artistList = q.value(5).toString().split(',');
					artistList.removeDuplicates();
					album.artists = artistList;
					album.is_sampler = (artistList.size() > 1);

					result.push_back(album);
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





int CDatabaseConnector::insertAlbumIntoDatabase (const QString & album) {
	if (!this -> m_database.isOpen())
					this -> m_database.open();

	QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO albums (name) values (:album);");
    q.bindValue(":album", QVariant(album));
    if (!q.exec()) {
        throw QString ("SQL - Error: insertAlbumIntoDatabase " + album);
    }
    return this -> getAlbumID (album);
}


int CDatabaseConnector::insertAlbumIntoDatabase (const Album & album) {

	 if (!this -> m_database.isOpen())
		this -> m_database.open();

	QSqlQuery q (this -> m_database);
    try{
    	q.prepare("INSERT INTO albums (albumid, name) values (:id, :name);");
    	q.bindValue(":id", QVariant(album.id));
    	    q.bindValue(":name", QVariant(album.name));
    	    if (!q.exec()) {
    	        throw QString ("SQL - Error: insertAlbumIntoDatabase " + album.name);
    	    }
    	    return this -> getAlbumID (album.name);
    }

    catch (QString ex) {
    		qDebug() << "SQL - Error: insert albums into database";
    		qDebug() << ex;
    		QSqlError er = this -> m_database.lastError();
    		qDebug() << er.driverText();
    		qDebug() << er.databaseText();
    		qDebug() << er.databaseText();
    		return -1;
    	}


}

