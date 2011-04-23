#include "library/CDatabaseConnector.h"
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QSqlError>

CDatabaseConnector::CDatabaseConnector(QObject *parent) :
    QObject(parent),
    m_createScriptFileName ("createDB.sql"),
    m_databaseContainerFile ("player.db")
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

}

int CDatabaseConnector::getArtistID (const QString & artist)  {
    QSqlQuery q (this -> m_database);
    int artistID = -1;
    q.prepare("select artistID from artists where name == '" + artist +"';");
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << artistID << " " << q.executedQuery();
        throw QString (Q_FUNC_INFO)+ q.lastError().databaseText();
    }
    if (q.next()) {
        artistID = q.value(0).toInt();
    }
    return artistID;
}

int CDatabaseConnector::insertArtistIntoDatabase (const QString & artist) {
    QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO artists (name) values (?);");
    q.addBindValue(QVariant(artist));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << q.executedQuery();
        throw QString (Q_FUNC_INFO) + q.lastError().databaseText();
    }
    return this -> getArtistID (artist);
}

int CDatabaseConnector::getAlbumID (const QString & album)  {
    QSqlQuery q (this -> m_database);
    int albumID = -1;
    q.prepare("select albumID from albums where name == '" + album + "';");
    if (!q.exec()) {
        throw QString (Q_FUNC_INFO) + q.lastError().databaseText();
    }
    if (q.next()) {
        albumID = q.value(0).toInt();
    }
    return albumID;
}

int CDatabaseConnector::insertAlbumIntoDatabase (const QString & album) {
    QSqlQuery q (this -> m_database);
    q.prepare("INSERT INTO albums (name) values (?);");
    q.bindValue(0,QVariant(album));
    if (!q.exec()) {
        throw QString (Q_FUNC_INFO) + q.lastError().databaseText();
    }
    return this -> getAlbumID (album);
}

int CDatabaseConnector::insertTrackIntoDatabase (const MetaData & data, int artistID, int albumID) {
    QSqlQuery q (this -> m_database);
    q.prepare("insert into Tracks (filename,albumID,artistID,title,year,length) values (:filename,:albumID,:artistID,:title,:year,:length)");
    q.bindValue(":filename",QVariant(data.filepath));
    q.bindValue(":albumID",QVariant(albumID));
    q.bindValue(":length",QVariant(data.length_ms));
    q.bindValue(":year",QVariant(data.year));
    q.bindValue(":title",QVariant(data.title));
    if (!q.exec()) {
        qDebug()<< q.lastQuery() << artistID << " " << q.executedQuery();
        throw QString (Q_FUNC_INFO + QString (__LINE__));
    }
    return 0;
}

QString CDatabaseConnector::getAlbumName (const int & id) {
    QSqlQuery q (this -> m_database);
    QString a;
    q.prepare("select name from albums where albumid == '" + QString::number(id) + "';");
    if (!q.exec()) {
        throw QString (Q_FUNC_INFO) + q.lastError().databaseText();
    }
    if (q.next()) {
        a = q.value(0).toString();
    }
    return a;
}

QString CDatabaseConnector::getArtistName (const int & id) {
    QSqlQuery q (this -> m_database);
    QString a;
    q.prepare("select name from artists where artistid == '" + QString::number(id) + "';");
    if (!q.exec()) {
        throw QString (Q_FUNC_INFO) + q.lastError().databaseText();
    }
    if (q.next()) {
        a = q.value(0).toString();
    }
    return a;
}



int CDatabaseConnector::getTracksFromDatabase (std::vector<MetaData> & returndata) {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    MetaData data;
    try {
        QSqlQuery q (this -> m_database);
        q.prepare("select filename,albumID,artistID,title,year,length from tracks");
        if (!q.exec()) {
        }
        while (q.next()) {
            data.filepath = q.value(0).toString();
            data.album = this -> getAlbumName(q.value(1).toInt());
            data.artist = this -> getArtistName(q.value(2).toInt());
            data.title = q.value(3).toString();
            data.year = q.value(4).toInt();
            data.length_ms = q.value(5).toInt();
            returndata.push_back(data);
        }
    }
    catch (QString ex) {
        qDebug() << "Error during inserting of metadata into database";
        qDebug() << ex;
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
    }
}


bool CDatabaseConnector::storeMetadata (std::vector<MetaData> & in)  {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    int artistID = -1, albumID = -1;

//            foreign key (genreID) REFERENCES genres (genreID),
//            foreign key (lyricID) REFERENCES lyrics (lyricID)
//            foreign key (albumID) REFERENCES albums (albumID),
//            foreign key (path) REFERENCES directories (path),
//            foreign key (artistID) REFERENCES artists (artistID)

//    q.     QSqlQuery query("SELECT * FROM artist");
//    int fieldNo = query.record().indexOf("country");
//    while (query.next()) {
//        QString country = query.value(fieldNo).toString();
//        doSomething(country);
//    }

    try {
        foreach (MetaData data, in) {
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




//            data.




        }
    }
    catch (QString ex) {
        qDebug() << "Error during inserting of metadata into database";
        qDebug() << ex;
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
    }
    return true;
}

