/* CDatabaseConnector.cpp */

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
#include "HelperStructs/Equalizer_presets.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QObject>
#include <QSqlError>
#include <HelperStructs/CSettingsStorage.h>
#include <HelperStructs/Helper.h>


#include <cstdlib>




CDatabaseConnector* CDatabaseConnector::getInstance(){
	static CDatabaseConnector instance;
	return &instance;
}

CDatabaseConnector::CDatabaseConnector()
{
    _db_filename = Helper::getSayonaraPath() + QDir::separator() + "player.db";

    bool success = isExistent();

    if ( !success ) {
        qDebug() << "Database not existent. Creating database";

        success = createDB();
    }

}

bool CDatabaseConnector::isExistent() {

    bool success;
    success = QFile::exists(_db_filename);
    if(!success){
        success = createDB();

        if(!success){
            qDebug() << "Database could not be created";
            return false;
        }

        else
            qDebug() << "Database created successfully";
    }

    success = openDatabase();

    if (success)
        _database->close();

    else
        qDebug() << "Could not open Database";


    return success;
}

bool CDatabaseConnector::createDB () {

        bool success;
        QDir dir = QDir::homePath();

        QString sayonara_path = Helper::getSayonaraPath();
        if(!QFile::exists(sayonara_path)){
            success = dir.mkdir(".Sayonara");
            qDebug() << "Could not create .Sayonara dir";
            if(!success) return false;
        }

        success = dir.cd(sayonara_path);

        //if ret is still not true we are not able to create the directory
        if(!success){
            qDebug() << "Could not change to .Sayonara dir";
            return false;
        }

        QString source_db_file = Helper::getSharePath() + QDir::separator() + "empty.db";
        QString target_db_file = _db_filename;
        success = QFile::exists(target_db_file);

        if(success) return true;

        if (!success) {

            qDebug() << "copy " <<  source_db_file << " to " << target_db_file;
            if (QFile::copy(source_db_file, target_db_file)) {
               qDebug() << "DB file has been copied to " <<   target_db_file;
               success = true;
            }

            else {
                qDebug() << "Fatal Error: could not copy DB file to " << target_db_file;
                success = false;
            }
        }

        return success;
}

bool CDatabaseConnector::init_settings_storage(){
    _settings = CSettingsStorage::getInstance();
    if(_settings){
	        
    	connect(_settings, SIGNAL(sig_save(QString, QVariant)), this, SLOT(store_setting(QString, QVariant)));
    	connect(_settings, SIGNAL(sig_save_all()), this, SLOT(store_settings()));
	return true;
    }

    else
        return false;
}

bool CDatabaseConnector::openDatabase () {
    _database = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", _db_filename));
    if(_database->isOpen()) _database->close();

    _database->setDatabaseName( _db_filename );
    bool e = _database->open();
    if (!e) {
    	qDebug() << "DatabaseConnector database cannot be opened!";
        QSqlError er = _database->lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
    }

    else{
        QSqlQuery q(*_database);
        QString text="PRAGMA case_sensitive_like=ON;\n"
                "PRAGMA cache_size=200000;\n"
                "PRAGMA page_size=4096;";
        q.prepare(text);
        qDebug() << "Case sensitive like: " << q.exec();
        _settings = CSettingsStorage::getInstance();
    	apply_fixes();
    }

    return e;
}
void CDatabaseConnector::closeDatabase(){
    qDebug() << "close database... ";
    if ( _database->isOpen()) {
         _database->close();
    }
    delete _database;

   
    QStringList connectionList = QSqlDatabase::connectionNames();
    for(int i = 0; i < connectionList.count(); ++i) {
        QSqlDatabase::removeDatabase(connectionList[i]);
    }
    

    _database = 0;

}


CDatabaseConnector::~CDatabaseConnector() {
    if(_database){

 	closeDatabase();
    }
 
}



bool CDatabaseConnector::check_and_insert_column(QString tablename, QString column, QString sqltype){

    DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_BOOL(_database);

    QSqlQuery q (*_database);
    QString querytext = "SELECT " + column + " FROM " + tablename + ";";
    q.prepare(querytext);



    if(!q.exec()){
        qDebug() << "DB: Could not find " << column << " in " << tablename << ": inserting it";

        QSqlQuery q2 (*_database);
        querytext = "ALTER TABLE " + tablename + " ADD COLUMN " + column + " " + sqltype + ";";
        q2.prepare(querytext);
        bool success = q2.exec();
        qDebug() << (success ? "Success" : "Fail");
        return success;
    }

    return true;
}

bool CDatabaseConnector::check_and_create_table(QString tablename, QString sql_create_str){

    DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_BOOL(_database);

    QSqlQuery q (*_database);
    QString querytext = "SELECT * FROM " + tablename + ";";
    q.prepare(querytext);

    if(!q.exec()){
        qDebug() << "DB: Table " << tablename << " does not exist: creating...";
        QSqlQuery q2 (*_database);
        q2.prepare(sql_create_str);
        return q2.exec();
    }

    return true;
}

bool CDatabaseConnector::updateAlbumCissearch(){

    AlbumList albums;
    getAllAlbums(albums);
    foreach(Album album, albums){
        QString str = "UPDATE albums SET cissearch=:cissearch WHERE albumID=:id;";
        QSqlQuery q(*_database);
        q.prepare(str);
        q.bindValue(":cissearch", album.name.toLower());
        q.bindValue(":id", album.id);

        q.exec();
    }

    return true;
}


bool CDatabaseConnector::updateArtistCissearch(){

    ArtistList artists;
    getAllArtists(artists);
    foreach(Artist artist, artists){
        QString str = "UPDATE artists SET cissearch=:cissearch WHERE artistID=:id;";
        QSqlQuery q(*_database);
        q.prepare(str);
        q.bindValue(":cissearch", artist.name.toLower());
        q.bindValue(":id", artist.id);
        q.exec();
    }

    return true;
}

bool CDatabaseConnector::updateTrackCissearch(){

    MetaDataList v_md;
    getTracksFromDatabase(v_md);
    foreach(MetaData md, v_md){
        updateTrack(md);
    }

    return true;
}

bool CDatabaseConnector::apply_fixes(){


    DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_BOOL(_database);

    int version = load_setting_int("version", 0);
    if(version == 3) return true;

    qDebug() << "Apply fixes";

    if(version < 1){

        check_and_insert_column("playlisttotracks", "position", "INTEGER");
        check_and_insert_column("playlisttotracks", "filepath", "VARCHAR(512)");
        check_and_insert_column("tracks", "genre", "VARCHAR(1024)");

        QString create_savedstreams = QString("CREATE TABLE savedstreams ") +
                    "( " +
                    "	name VARCHAR(255) PRIMARY KEY, " +
                    "	url VARCHAR(255) " +
                    ");";

        check_and_create_table("savedstreams", create_savedstreams);


        QString create_savedpodcasts = QString("CREATE TABLE savedpodcasts ") +
                    "( " +
                    "	name VARCHAR(255) PRIMARY KEY, " +
                    "	url VARCHAR(255) " +
                    ");";

        check_and_create_table("savedpodcasts", create_savedpodcasts);
    }

    if(version < 3){

        _database->transaction();

        bool success = true;
        success &= check_and_insert_column("tracks", "cissearch", "VARCHAR(512)");
        success &= check_and_insert_column("albums", "cissearch", "VARCHAR(512)");
        success &= check_and_insert_column("artists", "cissearch", "VARCHAR(512)");

        updateAlbumCissearch();
        updateArtistCissearch();
        updateTrackCissearch();

        _database->commit();


        QString create_vis_styles = QString("CREATE TABLE VisualStyles ") +
                "( " +
                "  name VARCHAR(255) PRIMARY KEY, " +
                "  col1 VARCHAR(20), " +
                "  col2 VARCHAR(20), " +
                "  col3 VARCHAR(20), " +
                "  col4 VARCHAR(20), " +
                "  nBinsSpectrum INTEGER, " +
                "  rectHeightSpectrum INTEGER, " +
                "  fadingStepsSpectrum INTEGER, " +
                "  horSpacingSpectrum INTEGER, " +
                "  vertSpacingSpectrum INTEGER, " +
                "  rectWidthLevel INTEGER, "
                "  horSpacingLevel INTEGER, " +
                "  verSpacingLevel INTEGER, " +
                "  fadingStepsLevel INTEGER " +
                ");";

        success = check_and_create_table("VisualStyles", create_vis_styles);
        if(success) store_setting("version", 3);
    }

	return true;
}
