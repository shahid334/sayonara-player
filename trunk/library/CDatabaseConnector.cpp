#include "library/CDatabaseConnector.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Equalizer_presets.h"
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>
#include <CSettingsStorage.h>

using namespace std;


CDatabaseConnector* CDatabaseConnector::getInstance(){
	static CDatabaseConnector instance;
	return &instance;
}

CDatabaseConnector::CDatabaseConnector() :

    m_createScriptFileName ("createDB.sql"),
    m_databaseContainerFile (CSettingsStorage::getInstance()->getDBFileName())
{

    if (this -> isExistent()==false) {
        qDebug() << "Database not existent";
    }
}

bool CDatabaseConnector::isExistent() {
    QFile f (this->m_databaseContainerFile);
    bool r = f.exists();
    if (r == true) {
        f.close();
        r = openDatabase ();
        if (r) {
            m_database.close();
        }
    }
    return r;
}

bool CDatabaseConnector::createDB () {
	return true;
}

bool CDatabaseConnector::openDatabase () {
	if(m_database.isOpen()) m_database.close();
    this -> m_database = QSqlDatabase::addDatabase("QSQLITE",this->m_databaseContainerFile);
    this -> m_database.setDatabaseName(this->m_databaseContainerFile);
    bool e = this -> m_database.open();
    if (!e) {
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
    }


    return e;

}

CDatabaseConnector::~CDatabaseConnector() {

	 if (!this -> m_database.isOpen()) {
		 this->m_database.open();
	 }

	this -> store_settings_eq();
	this -> store_settings_lastfm();


    if (this -> m_database.isOpen()) {
        this -> m_database.close();
    }
}


bool CDatabaseConnector::load_settings(){
	 this -> load_settings_lastfm();
	 this -> load_settings_eq();
	 return true;
}

bool CDatabaseConnector::store_settings(){
	if(!m_database.isOpen()) m_database.open();
	this->store_settings_lastfm();
	this->store_settings_eq();
	return true;
}



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

// FIXME: artist könnte hochkommata enthalten (Guns 'n' roses)
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

// FIXME: artist könnte hochkommata enthalten (Guns 'n' roses)
int CDatabaseConnector::insertArtistIntoDatabase (const QString & artist) {
    QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO artists (name) values (?);");
    q.addBindValue(QVariant(artist));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << q.executedQuery();
        throw QString ("SQL - Error: insertArtistIntoDatabase " + artist);
    }
    return this -> getArtistID (artist);
}

// FIXME: album könnte hochkommata enthalten (sackcloth 'n' Ashes)
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

// FIXME: album könnte hochkommata enthalten (sackcloth 'n' Ashes)
int CDatabaseConnector::insertAlbumIntoDatabase (const QString & album) {
    QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO albums (name) values (?);");
    q.bindValue(0,QVariant(album));
    if (!q.exec()) {
        throw QString ("SQL - Error: insertAlbumIntoDatabase " + album);
    }
    return this -> getAlbumID (album);
}


// FIXME: was ist, wenn hier eine variable hochkommata aufweist? Bitte in den Variablen ' durch '' ersetzen
int CDatabaseConnector::insertTrackIntoDatabase (const MetaData & data, int artistID, int albumID) {
    QSqlQuery q (this -> m_database);
    q.prepare("insert into Tracks (filename,albumID,artistID,title,year,length,track) values (:filename,:albumID,:artistID,:title,:year,:length,:track)");
    q.bindValue(":filename",QVariant(data.filepath));
    q.bindValue(":albumID",QVariant(albumID));
    q.bindValue(":artistID",QVariant(artistID));
    q.bindValue(":length",QVariant(data.length_ms));
    q.bindValue(":year",QVariant(data.year));
    q.bindValue(":title",QVariant(data.title));
    q.bindValue(":track",QVariant(data.track_num));
    if (!q.exec()) {
        throw QString ("SQL - Error: insertTrackIntoDatabase " + data.filepath);
    }
    return 0;
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


// TODO: (Prio mittel) noch eine Funktion, in der nur alle tracks eines vectors von artists ausgegeben werden
// TODO: (Prio mittel) noch eine Funktion, in der nur alle tracks eines vectors von alben ausgegeben werden
// TODO: (Prio niedrig) noch eine Funktion, in der nur alle tracks ausgegeben werden, auf die ein suchstring passt (suchstring könnte für album, artist und track gelten)
int CDatabaseConnector::getTracksFromDatabase (std::vector<MetaData> & returndata) {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    MetaData data;
    try {
        QSqlQuery q (this -> m_database);
        q.prepare("select filename,albumID,artistID,title,year,length,track from tracks");
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
    return 0;
}


bool CDatabaseConnector::storeMetadata (std::vector<MetaData> & in)  {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    int artistID = -1, albumID = -1;

    m_database.transaction();
    foreach (MetaData data, in) {

    	try {
            //first check if we know the artist and its id

            artistID = this -> getArtistID(data.artist);
            if (artistID == -1) {
                artistID = insertArtistIntoDatabase(data.artist);
            }

            albumID = this -> getAlbumID(data.album);
            if (albumID == -1) {
                albumID = insertAlbumIntoDatabase( data.album);
            }
            this -> insertTrackIntoDatabase (data,artistID,albumID);
        }
        catch (QString ex) {
            qDebug() << "Error during inserting of metadata into database";
            qDebug() << ex;
            QSqlError er = this -> m_database.lastError();
            qDebug() << er.driverText();
            qDebug() << er.databaseText();
            qDebug() << er.databaseText();
        }

    }
    m_database.commit();
    return true;
}







void CDatabaseConnector::getAllArtists(vector<Artist>& result){
	 if (!this -> m_database.isOpen())
				        this -> m_database.open();

		try {
			QSqlQuery q (this -> m_database);
			q.prepare(QString("SELECT ") +
						"artists.artistID, " +
						"artists.name, " +
						"count(albums.albumid), " +
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
				artist.num_albums = q.value(2).toInt();
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

void CDatabaseConnector::getAllAlbums(vector<Album>& result){
	 if (!this -> m_database.isOpen())
			        this -> m_database.open();

	try {
		QSqlQuery q (this -> m_database);
		q.prepare(QString("SELECT ") +
					"albums.albumID, " +
					"albums.name, " +
					"SUM(tracks.length) / 1000, " +
					"count(tracks.trackid), " +
					"max(tracks.year), " +
					"group_concat(artists.name) " +
					"FROM albums, Tracks, artists " +
					"WHERE Tracks.albumID = albums.albumID and artists.artistid = tracks.artistid " +
					"GROUP BY albums.albumID, albums.name " +
					"ORDER BY albums.name;");

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

	m_database.close();
}

void CDatabaseConnector::getAllArtistsByAlbum(int album, vector<Artist>& result){
	 if (!this -> m_database.isOpen())
					        this -> m_database.open();

			try {
				QSqlQuery q (this -> m_database);
				q.prepare(QString("SELECT ") +
							"artists.artistID, " +
							"artists.name, " +
							"count(albums.albumid), " +
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
					artist.num_albums = q.value(2).toInt();
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

			m_database.close();


}

void CDatabaseConnector::getAllAlbumsByArtist(int artist, vector<Album>& result, QString filter){
	 if (!this -> m_database.isOpen())
		this -> m_database.open();

	try {
		QSqlQuery q (this -> m_database);
		QString querytext = QString("SELECT ") +
				"albums.albumID, " +
				"albums.name, " +
				"SUM(tracks.length) / 1000, " +
				"count(tracks.trackid), " +
				"max(tracks.year), " +
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


		querytext += QString("GROUP BY albums.albumID, albums.name ") +
				"ORDER BY albums.name;";

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

	m_database.close();
}


void CDatabaseConnector::getAllTracksByAlbum(int album, vector<MetaData>& returndata, QString filter){
	  if (!this -> m_database.isOpen())
	        this -> m_database.open();
	    MetaData data;
	    try {
	        QSqlQuery q (this -> m_database);
	        QString querytext = QString("select filename,albumID,artistID,title,year,length,track from tracks where albumid=:albumid ");


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

	    m_database.close();
}












void CDatabaseConnector::getAllTracksByArtist(int artist, vector<MetaData>& returndata, QString filter){
	if (!this -> m_database.isOpen())
		this -> m_database.open();

	MetaData data;
	try {
		QSqlQuery q (this -> m_database);
		QString querytext = QString("select filename,albumID,artistID,title,year,length,track from tracks where artistID = :artist_id ");

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

	m_database.close();
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
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.name AS album, artists.name " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND tracks.title LIKE :search_in_title " +
				"UNION " +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.name AS album, artists.name " +
					"FROM tracks, albums, artists " +
					"WHERE tracks.albumid = albums.albumid AND tracks.artistid = artists.artistid AND albums.name LIKE :search_in_album " +
				"UNION  " +
				"SELECT  " +
					"tracks.title, tracks.length, tracks.year, tracks.filename, tracks.track AS track, albums.name AS album, artists.name " +
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
			data.album = q.value(5).toString().trimmed();
			data.artist = q.value(6).toString().trimmed();

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
}

void CDatabaseConnector::getAllAlbumsBySearchString(QString search, vector<Album>& result){
	if (!this -> m_database.isOpen())
				this -> m_database.open();
			Album album;

			try {
				QSqlQuery q (this -> m_database);
				QString query;
				query = QString("SELECT * FROM ( ") +
							"SELECT " +
								"albums.albumid as albumid, albums.name as name, COUNT(tracks.trackid), SUM(tracks.length)/1000, MAX(tracks.year), group_concat(artists.name) " +
								"FROM albums, artists, tracks " +
								"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.name LIKE :search_in_album " +
								"GROUP BY albums.albumid, albums.name " +
							"UNION " +
							"SELECT  " +
								"albums.albumid, albums.name, COUNT(tracks.trackid), SUM(tracks.length)/1000, MAX(tracks.year), group_concat(artists.name) " +
								"FROM albums, artists, tracks " +
								"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.title LIKE :search_in_title " +
								"GROUP BY albums.albumid, albums.name " +
							"UNION " +
							"SELECT " +
								"albums.albumid, albums.name, COUNT(tracks.trackid), SUM(tracks.length)/1000, MAX(tracks.year), group_concat(artists.name) " +
								"FROM albums, artists, tracks " +
								"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.name LIKE :search_in_artist " +
								"GROUP BY albums.albumid, albums.name " +
						") " +
						"GROUP BY albumid, name ORDER BY name;";

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

void CDatabaseConnector::getAllArtistsBySearchString(QString search, vector<Artist>& result){


	if (!this -> m_database.isOpen())
				this -> m_database.open();
			Artist artist;

			try {
				QSqlQuery q (this -> m_database);
				QString query;
				query = QString("SELECT * FROM ( ") +
						"SELECT " +
					"			artists.artistid as artistid, artists.name as name, COUNT(tracks.trackid), COUNT(albums.albumid) " +
					"			FROM albums, artists, tracks " +
					"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.name LIKE :search_in_artist " +
					"			GROUP BY artists.artistid, artists.name " +
					"		UNION " +
					"		SELECT  " +
					"			artists.artistid, artists.name, COUNT(tracks.trackid), COUNT(albums.albumid) " +
					"			FROM albums, artists, tracks " +
					"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.name LIKE :search_in_album " +
					"			GROUP BY artists.artistid, artists.name " +
					"		UNION " +
					"		SELECT  " +
					"			artists.artistid, artists.name, COUNT(tracks.trackid), COUNT(albums.albumid) " +
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
					artist.num_albums = q.value(3).toInt();

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



void CDatabaseConnector::load_settings_lastfm() {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    QString username, password;
    try {
        QSqlQuery q (this -> m_database);
        q.prepare("select value from settings where key ='LastFM_login';");
        if (!q.exec()) {
            throw QString ("SQL - Error: fillSettingsStorage");
        }

		if(q.next()){
			QStringList list = q.value(0).toString().split(',');
			if(list.size() == 2){
				username = list[0];
				password = list[1];
			}
		}

		CSettingsStorage::getInstance()->setLastFMNameAndPW(username,password);
    }

    catch (QString ex) {
        qDebug() << "Error during inserting of metadata into database";
        qDebug() << ex;
    }
}


void CDatabaseConnector::store_settings_lastfm () {

    try {
        QString username, password;
        CSettingsStorage::getInstance()->getLastFMNameAndPW(username,password);
        QSqlQuery q (this -> m_database);
        q.prepare("select value from settings where key = 'LastFM_login'");
        if (!q.exec()) {
            throw QString ("SQL - Error: storeSettingsFromStorage");
        }
        if (!q.next()) {
            q.prepare("update settings set value='dummy,dummy' WHERE key='LastFM_login'");
            if (!q.exec()) {
                throw QString ("SQL - Error: storeSettingsFromStorage");
            }
        }
        q.prepare("UPDATE settings set value=? WHERE key='LastFM_login'");
        q.addBindValue(QVariant(username+","+password));

        if (!q.exec()) {
            throw QString ("SQL - Error: storeSettingsFromStorage");
        }
    }
    catch (QString ex) {
        qDebug() << "Error during inserting of metadata into database";
        qDebug() << ex;
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
        qDebug() << er.databaseText();
    }
}


void CDatabaseConnector::store_settings_eq(){

	 try {

	        vector<EQ_Setting> vec;
	        CSettingsStorage::getInstance()->getEqualizerSettings(vec);
	        QString str2insert = "";
	        for(uint i=0; i<vec.size(); i++){
	        	if(vec[i].name == "Custom"){
	        		str2insert = vec[i].toString();
	        		break;
	        	}
	        }

	        QSqlQuery q (this -> m_database);
	        q.prepare("select value from settings where key = 'EQ_pr_custom'");

	        if (!q.exec()) {
	            throw QString ("SQL - Error: store settings equalizer (database not ready)");
	        }

	        if (!q.next() || str2insert == "") {
	            q.prepare("insert into settings  values('EQ_pr_custom', ?);");
	            q.addBindValue(str2insert);

	            if (!q.exec()) {
	                throw QString ("SQL - Error: storeSettingsFromStorage");
	            }

	            return;
	        }

	        if(str2insert != ""){
				q.prepare("UPDATE settings set value=? WHERE key='EQ_pr_custom'");
				q.addBindValue(str2insert);

				if (!q.exec()) {
					throw QString ("SQL - Error: storeSettingsFromStorage");
				}
				else{
					qDebug() << "inserted successfully";
				}
	        }
	    }

	    catch (QString ex) {
	        qDebug() << "Error during inserting of equalizer into database";
	        qDebug() << ex;
	    }
}

void CDatabaseConnector::load_settings_eq(){

	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	vector<EQ_Setting> result;
    try{
		QSqlQuery q (this -> m_database);
		q.prepare("select value from settings where key like 'EQ_pr_%';");

		if (!q.exec()) {
			throw QString (q.lastError().text());
		}

		while (q.next()) {
		   EQ_Setting eq_setting;
		   QString row = q.value(0).toString();
		   eq_setting.parseFromString(row);
		   result.push_back(eq_setting);
		}

		CSettingsStorage::getInstance()->setEqualizerSettings(result);
    }
    catch(QString ex){
    	qDebug() << "Error fetching EQ Presets (" << ex << ")";
    }
}
