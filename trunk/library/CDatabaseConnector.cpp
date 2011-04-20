#include "library/CDatabaseConnector.h"
#include <QFile>
#include <QDebug>

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
    this -> m_database = QSqlDatabase::addDatabase("QSQLITE2");
    this -> m_database.setDatabaseName(this->m_databaseContainerFile);
    return this -> m_database.open();
}

CDatabaseConnector::~CDatabaseConnector() {

}
