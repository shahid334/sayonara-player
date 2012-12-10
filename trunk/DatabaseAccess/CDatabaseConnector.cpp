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
        m_database.close();

    else
        qDebug() << "Could not open Database";


    return success;
}

bool CDatabaseConnector::createDB () {

        bool ret = false;
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
    CSettingsStorage* s = CSettingsStorage::getInstance();
    if(s)
        return true;
    else
        return false;
}

bool CDatabaseConnector::openDatabase () {

    if(m_database.isOpen()) m_database.close();

    m_database = QSqlDatabase::addDatabase("QSQLITE", _db_filename);
    m_database.setDatabaseName( _db_filename );
    bool e = m_database.open();
    if (!e) {
    	qDebug() << "DatabaseConnector database cannot be opened!";
        QSqlError er = m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
    }

    else{
        qDebug() << "Apply fixes";
    	apply_fixes();
    }

    return e;
}

CDatabaseConnector::~CDatabaseConnector() {

	qDebug() << "close database... ";
    if (this -> m_database.isOpen()) {
        this -> m_database.close();
    }
}

bool CDatabaseConnector::apply_fixes(){

    DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_BOOL(m_database);

	QSqlQuery q (this->m_database);
	QString querytext = "SELECT position FROM playlisttotracks;";
	q.prepare(querytext);
	if(!q.exec()){

		QSqlQuery q2 (this -> m_database);
		querytext = "DROP TABLE playlisttotracks;";
		q2.prepare(querytext);
		q2.exec();

		QSqlQuery q3(this->m_database);
		querytext = QString("CREATE TABLE playlisttotracks ") +
				"( "
				"	trackid INTEGER, "
				"	playlistid INTEGER, "
				"	position INTEGER, "
				"	FOREIGN KEY (trackid) REFERENCES tracks(trackid), "
				"	FOREIGN KEY (playlistid) REFERENCES playlists(playlistid) "
				");";
		q3.prepare(querytext);
		q3.exec();
	}


	// save internet streams
	QSqlQuery q4 (this->m_database);
	querytext = "SELECT * FROM savedstreams;";
	q4.prepare(querytext);
	if(!q4.exec()){
		QSqlQuery q5(this->m_database);
		querytext = QString("CREATE TABLE savedstreams ") +
				"( " +
				"	name VARCHAR(255) PRIMARY KEY, " +
				"	url VARCHAR(255) " +
				");";
		q5.prepare(querytext);
		q5.exec();
    }

    QSqlQuery q6(this->m_database);
    querytext = "ALTER table tracks ADD genre varchar;";
    q6.prepare(querytext);
    q6.exec();

	return true;
}
