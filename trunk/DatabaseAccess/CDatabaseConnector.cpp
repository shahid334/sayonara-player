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
#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>
#include <HelperStructs/CSettingsStorage.h>

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
	qDebug() << "Open database: " << f.fileName();
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
    	qDebug() << "DatabaseConnector database cannot be opened!";
        QSqlError er = this -> m_database.lastError();
        qDebug() << er.driverText();
        qDebug() << er.databaseText();
    }

    else{
    	apply_fixes();
    }


    return e;

}

CDatabaseConnector::~CDatabaseConnector() {

    if (this -> m_database.isOpen()) {
        this -> m_database.close();
    }
}

bool CDatabaseConnector::apply_fixes(){

	if (!this -> m_database.isOpen())
	        this -> m_database.open();

	if (!this -> m_database.isOpen()) return false;

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


	return true;
}
