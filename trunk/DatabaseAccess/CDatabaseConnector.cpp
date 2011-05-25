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
    	qDebug() << "DatabaseConnector database cannot be opened!";
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

