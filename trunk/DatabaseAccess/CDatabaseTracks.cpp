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

MetaData CDatabaseConnector::getTrackById(int id){
	if (!this -> m_database.isOpen())
			this -> m_database.open();

		MetaData data;
		try {
			QSqlQuery q (this -> m_database);
			QString querytext = QString("select filename,albumID,artistID,title,year,length,track,bitrate,TrackID from tracks where TrackID = :track_id ");

			q.prepare(querytext);
			q.bindValue(":track_id", QVariant(id));


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
				return data;
			}
		}
		catch (QString& ex) {
			qDebug() << "SQL - Error: getTracksFromDatabase";
			qDebug() << ex;
			QSqlError er = this -> m_database.lastError();
			qDebug() << er.driverText();
			qDebug() << er.databaseText();
			qDebug() << er.databaseText();
			return data;
		}

		return data;


}


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
		  catch (QString& ex) {
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


int CDatabaseConnector::getTracksFromDatabase (std::vector<MetaData> & returndata, QString sort) {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    MetaData data;
    try {
        QSqlQuery q (this -> m_database);

        QString querytext = QString("SELECT ") +
        		"tracks.filename, " +			// 0
        		"tracks.albumID, " +			// 1
        		"albums.name, " +				// 2
        		"tracks.artistID, " +			// 3
        		"artists.name, " +				// 4
        		"tracks.title, " +				// 5
        		"tracks.year, " +				// 6
        		"tracks.length, " +				// 7
        		"tracks.track, " +				// 8
        		"tracks.bitrate, " +			// 9
        		"tracks.TrackID " +				// 10
        		"FROM tracks, albums, artists WHERE " +
        		"tracks.albumID = albums.albumID AND tracks.artistID = artists.artistID ";


        if(sort == "artist asc") querytext += QString(" ORDER BY artists.name ASC, tracks.track;");
		else if(sort == "artist desc") querytext += QString(" ORDER BY artists.name DESC, tracks.track;");
		else if(sort == "album asc") querytext += QString(" ORDER BY albums.name ASC, tracks.track;");
		else if(sort == "album desc") querytext += QString(" ORDER BY albums.name DESC, tracks.track;");
		else if(sort == "title asc") querytext += QString(" ORDER BY tracks.title ASC;");
		else if(sort == "title desc") querytext += QString(" ORDER BY tracks.title DESC;");
		else querytext += ";";


        q.prepare(querytext);


        if (!q.exec()) {
            throw QString ("SQL - Error: getTracksFromDatabase cammot execute query");
        }
        int albumID = -1;
        int artistID = -1;
        while (q.next()) {

			data.filepath =  q.value(0).toString();
			data.album_id =  q.value(1).toInt();
			data.album = 	 q.value(2).toString().trimmed();
			data.artist_id = q.value(3).toInt();
			data.artist = 	 q.value(4).toString().trimmed();
			data.title = 	 q.value(5).toString();
			data.year = 	 q.value(6).toInt();
			data.length_ms = q.value(7).toInt();
			data.track_num = q.value(8).toInt();
			data.bitrate = 	 q.value(9).toInt();
			data.id = 		 q.value(10).toInt();
            data.is_extern = false;

            returndata.push_back(data);
        }
    }
    catch (QString& ex) {
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





void CDatabaseConnector::getAllTracksByAlbum(int album, vector<MetaData>& returndata, QString filter, QString sort){
	  if (!this -> m_database.isOpen())
	        this -> m_database.open();
	    MetaData data;
	    try {
	        QSqlQuery q (this -> m_database);
	        QString querytext = QString("SELECT ") +
	        		"tracks.filename, " +		// 0
	        		"albums.name, " +			// 1
	        		"tracks.albumID, " + 		// 2
	        		"tracks.artistID, " +		// 3
	        		"artists.name, " +			// 4
	        		"tracks.title, " +			// 5
	        		"tracks.year, " +			// 6
	        		"tracks.length, " +			// 7
	        		"tracks.track, " +			// 8
	        		"tracks.bitrate, " +		// 9
	        		"tracks.TrackID " +			// 10
	        		"FROM tracks, albums, artists " +
	        		"WHERE tracks.albumid=:albumid " +
	        		"AND tracks.albumid = albums.albumID " +
	        		"AND tracks.artistid = artists.artistID ";


	        if(filter.length() > 0 ){
				// consider the case, that the search string may fit to the title
				// union the case that the search string may fit to the album
	        	querytext += QString("AND tracks.trackid IN ( ") +
									"	SELECT t2.trackid " +
									"	FROM tracks t2 "+
									"	WHERE t2.title LIKE :filter1 "+

									"	UNION SELECT t3.trackid "+
									"	FROM tracks t3, albums a2 "+
									"	WHERE a2.albumid = t3.albumid AND a2.name LIKE :filter2 "+

									"	UNION SELECT t4.trackid " +
									"	FROM tracks t4, albums a3, artists ar2" +
									"	WHERE t4.albumid = a3.albumid " +
									"	AND t4.artistid = ar2.artistid " +
									"	AND ar2.name LIKE :filter3 "

								") ";



			}

	        if(sort == "artist asc") querytext += QString(" ORDER BY artists.name ASC, tracks.track;");
	        else if(sort == "artist desc") querytext += QString(" ORDER BY artists.name DESC, tracks.track;");
	        else if(sort == "album asc") querytext += QString(" ORDER BY albums.albumID ASC, tracks.track;");
	        else if(sort == "album desc") querytext += QString(" ORDER BY albums.albumID DESC, tracks.track;");
	        else if(sort == "title asc") querytext += QString(" ORDER BY tracks.title ASC;");
	        else if(sort == "title desc") querytext += QString(" ORDER BY tracks.title DESC;");
	        else querytext += ";";


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

	        while (q.next()) {


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

	            returndata.push_back(data);
	        }
	    }
	    catch (QString& ex) {
	        qDebug() << "SQL - Error: getTracksFromDatabase";
	        qDebug() << ex;
	        QSqlError er = this -> m_database.lastError();
	        qDebug() << er.driverText();
	        qDebug() << er.databaseText();
	        qDebug() << er.databaseText();
	    }

	   // m_database.close();
}




void CDatabaseConnector::getAllTracksByArtist(int artist, vector<MetaData>& returndata, QString filter, QString sort){
	if (!this -> m_database.isOpen())
		this -> m_database.open();

	qDebug() << "Get all tracks by artist";

	MetaData data;
	try {
		QSqlQuery q (this -> m_database);
		QString querytext = QString("SELECT ") +
						"tracks.filename, " +		// 0
						"albums.name, " +			// 1
						"tracks.albumID, " + 		// 2
						"tracks.artistID, " +		// 3
						"artists.name, " +			// 4
						"tracks.title, " +			// 5
						"tracks.year, " +			// 6
						"tracks.length, " +			// 7
						"tracks.track, " +			// 8
						"tracks.bitrate, " +		// 9
						"tracks.TrackID " +			// 10
						"FROM tracks, albums, artists " +
						"WHERE tracks.artistID=:artistID " +
						"AND tracks.albumID = albums.albumID " +
						"AND tracks.artistID = artists.artistID ";

		if(filter.length() > 0 ){
			// consider the case, that the search string may fit to the title
			// union the case that the search string may fit to the album
			querytext += QString("AND tracks.trackid IN ( ") +
								"	SELECT t2.trackid " +
								"	FROM tracks t2 "+
								"	WHERE t2.title LIKE :filter1 "+
								"	UNION SELECT t3.trackID "+
								"	FROM tracks t3, albums a2"+
								"	WHERE a2.albumID = t3.albumID AND a2.name LIKE :filter2 "+
								"	UNION SELECT t4.trackID " +
								"	FROM tracks t4, albums a3, artists ar2" +
								"	WHERE t4.albumid = a3.albumid AND t4.artistID = ar2.artistID AND ar2.name LIKE :filter3 "

							") ";


		}


	    if(sort == "artist asc") querytext += QString(" ORDER BY artists.name ASC, tracks.track;");
		else if(sort == "artist desc") querytext += QString(" ORDER BY artists.name DESC, tracks.track;");
		else if(sort == "album asc") querytext += QString(" ORDER BY albums.name ASC, tracks.track;");
		else if(sort == "album desc") querytext += QString(" ORDER BY albums.name DESC, tracks.track;");

		else if(sort == "title asc") querytext += QString(" ORDER BY tracks.title ASC;");
		else if(sort == "title desc") querytext += QString(" ORDER BY tracks.title DESC;");
		else querytext += ";";

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

		while (q.next()) {

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

			returndata.push_back(data);
		}
	}
	catch (QString& ex) {
		qDebug() << "SQL - Error: getTracksFromDatabase";
		qDebug() << ex;
		QSqlError er = this -> m_database.lastError();
		qDebug() << er.driverText();
		qDebug() << er.databaseText();
		qDebug() << er.databaseText();
	}

	//m_database.close();
}


void CDatabaseConnector::getAllTracksBySearchString(QString search, vector<MetaData>& result, QString sort){

	if (!this -> m_database.isOpen())
		this -> m_database.open();
	MetaData data;

	try {
		QSqlQuery q (this -> m_database);
		QString querytext;
		querytext = QString("SELECT * FROM ( ") +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.albumID, albums.name AS albumName, artists.artistID, artists.name AS artistName, tracks.bitrate,tracks.TrackID " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND tracks.title LIKE :search_in_title " +
				"UNION " +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.albumID, albums.name AS albumName, artists.artistID, artists.name AS artistName, tracks.bitrate,tracks.TrackID " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND albums.name LIKE :search_in_album " +
				"UNION  " +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.albumID, albums.name AS albumName, artists.artistID, artists.name AS artistName, tracks.bitrate,tracks.TrackID " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND artists.name LIKE :search_in_artist " +
				") ";


		if(sort == "artist asc") querytext += QString(" ORDER BY artistName ASC, track;");
		else if(sort == "artist desc") querytext += QString(" ORDER BY artistName DESC, track;");
		else if(sort == "album asc") querytext += QString(" ORDER BY albumName ASC, track;");
		else if(sort == "album desc") querytext += QString(" ORDER BY albumName DESC, track;");
		else if(sort == "title asc") querytext += QString(" ORDER BY title ASC;");
		else if(sort == "title desc") querytext += QString(" ORDER BY title DESC;");
		else querytext += ";";

		q.prepare(querytext);
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
			data.album = q.value(6).toString().trimmed();
			data.artist_id = q.value(7).toInt();
			data.artist = q.value(8).toString().trimmed();
			data.bitrate = q.value(9).toInt();
			data.id = q.value(10).toInt();
			data.is_extern = false;

			result.push_back(data);
		}
	}


	catch (QString& ex) {
		qDebug() << "SQL - Error: getTracksFromDatabase";
		qDebug() << ex;
		QSqlError er = this -> m_database.lastError();
		qDebug() << er.driverText();
		qDebug() << er.databaseText();
		qDebug() << er.databaseText();
	}

	//m_database.close();
}



int CDatabaseConnector::deleteTracks(vector<MetaData>& vec_tracks){
	if (!this -> m_database.isOpen())
			this -> m_database.open();

	int success = 0;

	m_database.transaction();

	for(int i=0; i<vec_tracks.size(); i++){

		int track_id = vec_tracks[i].id;

		try {
			QSqlQuery q (this -> m_database);
			QString querytext = QString("DELETE FROM tracks WHERE trackID = :track_id;");

			q.prepare(querytext);
			q.bindValue(":track_id", QVariant(track_id));


			if (!q.exec()) {
				throw QString ("SQL - Error: delete track from Database:  cannot execute query");
			}

			success++;
		}

		catch (QString& ex) {
			qDebug() << "SQL - Error: getTracksFromDatabase";
			qDebug() << ex;
			QSqlError er = this -> m_database.lastError();
			qDebug() << er.driverText();
			qDebug() << er.databaseText();
			qDebug() << er.databaseText();
		}
	}

	m_database.commit();

	return success;

}
