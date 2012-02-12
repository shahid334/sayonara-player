/*
 * CDatabaseStreams.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: luke
 */

#include "DatabaseAccess/CDatabaseConnector.h"

#include <vector>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>

using namespace std;


bool CDatabaseConnector::getAllStreams(QMap<QString, QString> & streams){

	if (!this -> m_database.isOpen())
		 this -> m_database.open();

	QSqlQuery q (this -> m_database);
	q.prepare("SELECT name, url FROM savedstreams;");

	if (!q.exec())
		return false;

	else{
		while(q.next()){
			QString name = q.value(0).toString();
			QString url = q.value(1).toString();

			streams[name] = url;
		}
	}

	return true;
}

bool CDatabaseConnector::deleteStream(QString name){
	if (!this -> m_database.isOpen())
			 this -> m_database.open();

	QSqlQuery q (this -> m_database);
	q.prepare("DELETE FROM savedstreams WHERE name = :name;" );
	q.bindValue(":name", name);
	if(!q.exec()) {
		qDebug() << "Could not delete stream " << name;
		return false;
	}

	qDebug() << "stream " << name << " successfully deleted";

	return true;
}



bool CDatabaseConnector::addStream(QString name, QString url){
	if (!this -> m_database.isOpen())
			 this -> m_database.open();

	QSqlQuery q (this -> m_database);
	q.prepare("INSERT INTO savedstreams (name, url) VALUES (:name, :url); " );
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		qDebug() << "Could not add stream " << name << ", " << url;
		return false;
	}

	qDebug() << "stream " << name << ", " << url << " successfully added";
	return true;
}

