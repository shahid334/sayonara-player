#include "library/CDatabaseConnector.h"
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QSqlError>
#include <CSettingsStorage.h>

CDatabaseConnector::CDatabaseConnector(QObject *parent) :
    QObject(parent),
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
    this -> storeSettingsFromStorage();
    if (this -> m_database.isOpen()) {
        this -> m_database.close();
    }
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
    q.prepare("insert into Tracks (filename,albumID,artistID,title,year,length) values (:filename,:albumID,:artistID,:title,:year,:length)");
    q.bindValue(":filename",QVariant(data.filepath));
    q.bindValue(":albumID",QVariant(albumID));
    q.bindValue(":artistID",QVariant(artistID));
    q.bindValue(":length",QVariant(data.length_ms));
    q.bindValue(":year",QVariant(data.year));
    q.bindValue(":title",QVariant(data.title));
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
            a = q.value(0).toString();
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
            a = q.value(0).toString();
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
        q.prepare("select filename,albumID,artistID,title,year,length from tracks");
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
            data.title = q.value(3).toString();
            data.year = q.value(4).toInt();
            data.length_ms = q.value(5).toInt();
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
    foreach (MetaData data, in) {
        try {
            //first check if we know the artist and its id

            // TODO: Provisorischer FIX, ist das so in Ordnung? laufen tuts so
//            QString tmpArtist = data.artist.replace("'", "''");
            artistID = this -> getArtistID(data.artist);
            if (artistID == -1) {
                artistID = insertArtistIntoDatabase(data.artist);
            }

            // TODO: Provisorischer FIX, ist das so in Ordnung? laufen tuts so
//            QString tmpAlbum = data.album.replace("'", "''");
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
    return true;
}



void CDatabaseConnector::fillSettingsStorage () {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    QString username, password;
    try {
        QSqlQuery q (this -> m_database);
        q.prepare("select lastFMUserName,lastFMPassword from settings");
        if (!q.exec()) {
            throw QString ("SQL - Error: fillSettingsStorage");
        }
        if (q.next()) {
            username = q.value(0).toString();
            password = q.value(1).toString();
            CSettingsStorage::getInstance()->setLastFMNameAndPW(username,password);
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


void CDatabaseConnector::storeSettingsFromStorage () {
    if (!this -> m_database.isOpen())
        this -> m_database.open();
    try {
        QString username, password;
        CSettingsStorage::getInstance()->getLastFMNameAndPW(username,password);
        QSqlQuery q (this -> m_database);
        q.prepare("select lastFMUserName,lastFMPassword from settings");
        if (!q.exec()) {
            throw QString ("SQL - Error: storeSettingsFromStorage");
        }
        if (!q.next()) {
            q.prepare("insert into settings (lastFMUserName,lastFMPassword) values ('dummy','dummy');");
            if (!q.exec()) {
                throw QString ("SQL - Error: storeSettingsFromStorage");
            }
        }
        q.prepare("UPDATE settings set lastFMUserName = ?, lastFMPassword = ? where rowid == 1;");
        q.addBindValue(QVariant(username));
        q.addBindValue(QVariant(password));
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
