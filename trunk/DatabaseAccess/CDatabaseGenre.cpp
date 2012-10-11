/*
 * CDatabaseGenre.cpp
 *
 *  Created on: 11.10.2012
 *      Author: lugmair
 */
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/globals.h"

#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QObject>
#include <QSqlError>
#include <QList>

QString CDatabaseConnector::getGenreByID(int genreID){
	DB_TRY_OPEN(m_database);


	QSqlQuery q (this -> m_database);
	QString querytext = "SELECT name FROM genres WHERE genreID = :genreID";

	q.prepare(querytext);
	q.bindValue(":genreID", QVariant(genreID));

	if (!q.exec()) {
		return "Unknown";
	}

	if(q.next()){
		return q.value(0).toString();
	}

	return "Unknown";
}

int CDatabaseConnector::getGenreByName(QString genre){

	DB_TRY_OPEN(m_database);

	QSqlQuery q (this -> m_database);
	QString querytext = "SELECT genreID FROM genres WHERE name = :genreName";

	q.prepare(querytext);
	q.bindValue(":genreName", QVariant(genre));

	if (!q.exec()) {
		return -1;
	}

	if(q.next()){
		return q.value(0).toInt();
	}

	return -1;
}


int CDatabaseConnector::insertGenreIntoDatabase(QString genre){

	DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_INT(m_database);

	QSqlQuery q (this -> m_database);
	QString querytext = "INSERT INTO genres (name) VALUES(:genre)";

	q.prepare(querytext);
	q.bindValue(":genre", QVariant(genre));

	if (!q.exec()) {
		return -1;
	}

	return this->getGenreByName(genre);
}


bool CDatabaseConnector::insertGenreMappingsIntoDatabase(QList<int> genreIDs, int trackID){

    DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_BOOL(m_database);

    m_database.transaction();

    foreach(int genreID, genreIDs)
        insertGenreMappingIntoDatabase(genreID, trackID);

    return m_database.commit();

}


bool CDatabaseConnector::insertGenreMappingIntoDatabase(int genreID, int trackID){

    DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_BOOL(m_database);

    QSqlQuery q(this->m_database);

    QString querytext = "INSERT INTO track_genre_mapping (genreID, trackID) VALUES(:genreID, :trackID);";

    q.prepare(querytext);
    q.bindValue(":genreID", genreID);
    q.bindValue(":trackID", trackID);

    if(!q.exec()) return false;
    return true;

}


bool CDatabaseConnector::deleteGenreMappingByTrackID(int trackID){

    DB_TRY_OPEN(m_database);
    DB_RETURN_NOT_OPEN_BOOL(m_database);

    QSqlQuery q(this->m_database);

    QString querytext = "DELETE FROM track_genre_mapping WHERE trackID = :trackID";

    q.prepare(querytext);
    q.bindValue(":trackID", trackID);

    if(!q.exec()) return false;
    return true;
}
