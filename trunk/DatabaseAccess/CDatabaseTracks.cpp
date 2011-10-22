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

int CDatabaseConnector::updateTrack(MetaData& data){


	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	 if (!this -> m_database.isOpen()) return -1;

		QSqlQuery q (this -> m_database);
		try{
		q.prepare("update Tracks set albumID = :albumID, artistID = :artistID, title = :title, year = :year, track = :track WHERE TrackID = :trackID");

		q.bindValue(":albumID",QVariant(data.album_id));
		q.bindValue(":artistID",QVariant(data.artist_id));
		q.bindValue(":title",QVariant(data.title));
		q.bindValue(":track",QVariant(data.track_num));
		q.bindValue(":year",QVariant(data.year));
		q.bindValue(":trackID", QVariant(data.id));


		if (!q.exec()) {

			throw QString ("SQL - Error: insertTrackIntoDatabase " + data.filepath);
		}
		else{
			qDebug() << "updated (" << q.numRowsAffected() << ")";
		}
	}
		  catch (QString ex) {
		        qDebug() << "SQL - Error: getTracksFromDatabase";
		        qDebug() << ex;
		        QSqlError er = this -> m_database.lastError();
		        qDebug() << er.driverText();
		        qDebug() << er.databaseText();
		        qDebug() << er.databaseText();
		        return -1;
		    }


    return 0;

}




int CDatabaseConnector::insertTrackIntoDatabase (const MetaData & data, int artistID, int albumID) {


	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	 if (!this -> m_database.isOpen()) return -1;



	QSqlQuery q (this -> m_database);

	q.prepare("insert into Tracks (filename,albumID,artistID,title,year,length,track,bitrate) values (:filename,:albumID,:artistID,:title,:year,:length,:track,:bitrate)");
    q.bindValue(":filename",QVariant(data.filepath));
    q.bindValue(":albumID",QVariant(albumID));
    q.bindValue(":artistID",QVariant(artistID));
    q.bindValue(":length",QVariant(data.length_ms));
    q.bindValue(":year",QVariant(data.year));
    q.bindValue(":title",QVariant(data.title));
    q.bindValue(":track",QVariant(data.track_num));
    q.bindValue(":bitrate",QVariant(data.bitrate));


    if (!q.exec()) {

        throw QString ("SQL - Error: insertTrackIntoDatabase " + data.filepath);
    }
    return 0;
}


int CDatabaseConnector::getTracksFromDatabase (std::vector<MetaData> & returndata) {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    MetaData data;
    try {
        QSqlQuery q (this -> m_database);
        q.prepare("select filename,albumID,artistID,title,year,length,track,bitrate,TrackID from tracks");
        if (!q.exec()) {
            throw QString ("SQL - Error: getTracksFromDatabase cammot execute query");
        }
        int albumID = -1;
        int artistID = -1;
        while (q.next()) {
            albumID = q.value(1).toInt();
            artistID = q.value(2).toInt();
            data.filepath = q.value(0).toString();
            data.album = this -> getAlbumName(albumID);
            data.artist = this -> getArtistName(artistID);
            data.title = q.value(3).toString().trimmed();
            data.year = q.value(4).toInt();
            data.length_ms = q.value(5).toInt();
            data.track_num = q.value(6).toInt();
            data.bitrate = q.value(7).toInt();
            data.id = q.value(8).toInt();
            data.album_id = albumID;
            data.artist_id = artistID;
            data.is_extern = false;
            returndata.push_back(data);
        }
    }
    catch (QString ex) {
        qDebug() << "SQL - Error: getTracksFromDatabase";
        qDebug() << ex;
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
        qDebug() << er.databaseText();
        return -1;
    }

   // m_database.close();
    return 0;
}





void CDatabaseConnector::getAllTracksByAlbum(int album, vector<MetaData>& returndata, QString filter){
	  if (!this -> m_database.isOpen())
	        this -> m_database.open();
	    MetaData data;
	    try {
	        QSqlQuery q (this -> m_database);
	        QString querytext = QString("select filename,albumID,artistID,title,year,length,track, tracks.bitrate, TrackID from tracks where albumid=:albumid ");


	        if(filter.length() > 0 ){
				// consider the case, that the search string may fit to the title
				// union the case that the search string may fit to the album
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

	        querytext += QString(" order by albumid, track;");
	        q.prepare(querytext);
	        q.bindValue(":albumid", QVariant(album));
	        if(filter.length() > 0){
	        	q.bindValue(":filter1", QVariant(filter));
	        	q.bindValue(":filter2", QVariant(filter));
	        	q.bindValue(":filter3", QVariant(filter));
	        }


	        if (!q.exec()) {
	            throw QString ("SQL - Error: getTracksFromDatabase cannot execute query");
	        }
	        int albumID = -1;
	        int artistID = -1;
	        while (q.next()) {
	            albumID = q.value(1).toInt();
	            artistID = q.value(2).toInt();
	            data.filepath = q.value(0).toString();
	            data.album = this -> getAlbumName(albumID).trimmed();
	            data.artist = this -> getArtistName(artistID).trimmed();
	            data.title = q.value(3).toString();
	            data.year = q.value(4).toInt();
	            data.length_ms = q.value(5).toInt();
	            data.track_num = q.value(6).toInt();
	            data.bitrate = q.value(7).toInt();
	            data.id = q.value(8).toInt();
	            data.album_id = albumID;
	            data.artist_id = artistID;
	            data.is_extern = false;

	            returndata.push_back(data);
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

	   // m_database.close();
}












void CDatabaseConnector::getAllTracksByArtist(int artist, vector<MetaData>& returndata, QString filter){
	if (!this -> m_database.isOpen())
		this -> m_database.open();

	MetaData data;
	try {
		QSqlQuery q (this -> m_database);
		QString querytext = QString("select filename,albumID,artistID,title,year,length,track,bitrate,TrackID from tracks where artistID = :artist_id ");

		if(filter.length() > 0 ){
			// consider the case, that the search string may fit to the title
			// union the case that the search string may fit to the album
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

		querytext += QString("order by albumid, track;");
		qDebug() << "query = " << querytext;

		q.prepare(querytext);

		q.bindValue(":artist_id", QVariant(artist));
		if(filter.length() > 0 ){
			q.bindValue(":filter1", QVariant(filter));
			q.bindValue(":filter2", QVariant(filter));
			q.bindValue(":filter3", QVariant(filter));
		}



		if (!q.exec()) {
			throw QString ("SQL - Error: getTracksFromDatabase cannot execute query");
		}
		int albumID = -1;
		int artistID = -1;
		while (q.next()) {
			albumID = q.value(1).toInt();
			artistID = q.value(2).toInt();
			data.filepath = q.value(0).toString();
			data.album = this -> getAlbumName(albumID);
			data.artist = this -> getArtistName(artistID);
			data.title = q.value(3).toString();
			data.year = q.value(4).toInt();
			data.length_ms = q.value(5).toInt();
			data.track_num = q.value(6).toInt();
			data.bitrate = q.value(7).toInt();
			data.id = q.value(8).toInt();
			data.album_id = albumID;
			data.artist_id = artistID;
			data.is_extern = false;
			returndata.push_back(data);
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

	//m_database.close();
}


void CDatabaseConnector::getAllTracksBySearchString(QString search, vector<MetaData>& result){
	if (!this -> m_database.isOpen())
		this -> m_database.open();
	MetaData data;

	try {
		QSqlQuery q (this -> m_database);
		QString query;
		query = QString("SELECT * FROM ( ") +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.albumID AS album, artists.artistID, tracks.bitrate,tracks.TrackID " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND tracks.title LIKE :search_in_title " +
				"UNION " +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.albumID AS album, artists.artistID, tracks.bitrate,tracks.TrackID " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND albums.name LIKE :search_in_album " +
				"UNION  " +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.albumID AS album, artists.artistID, tracks.bitrate,tracks.TrackID " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND artists.name LIKE :search_in_artist " +
				") " +
			"ORDER BY album, track" +
			";";

		q.prepare(query);
		q.bindValue(":search_in_title",QVariant(search));
		q.bindValue(":search_in_album",QVariant(search));
		q.bindValue(":search_in_artist",QVariant(search));


		if (!q.exec()) {
			throw QString ("SQL - Error: getTracksFromDatabase cannot execute search track query" );
		}

		while (q.next()) {

			data.title = q.value(0).toString().trimmed();
			data.length_ms = q.value(1).toInt();
			data.year = q.value(2).toInt();
			data.filepath = q.value(3).toString();
			data.track_num = q.value(4).toInt();
			data.album_id = q.value(5).toInt();
			data.album = getAlbumName(data.album_id);
			data.artist_id = q.value(6).toInt();
			data.artist = getArtistName(data.artist_id);
			data.bitrate = q.value(7).toInt();
			data.id = q.value(8).toInt();
			data.is_extern = false;

			result.push_back(data);
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

	//m_database.close();
}
