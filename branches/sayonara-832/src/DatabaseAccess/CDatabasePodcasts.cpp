
/* CDatabasePodcasts.cpp */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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

/*
 * CDatabasePodcasts.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: luke
 */

#include "DatabaseAccess/CDatabaseConnector.h"

#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <stdlib.h>
#include <QVariant>
#include <QObject>
#include <QSqlError>

using namespace std;


bool CDatabaseConnector::getAllPodcasts(QMap<QString, QString> & podcasts) {

    podcasts.clear();

    if (!this -> _database->isOpen())
         this -> _database->open();

    QSqlQuery q (*_database);
    q.prepare("SELECT name, url FROM savedpodcasts;");

    if (!q.exec())
        return false;

    else{
        while(q.next()) {
            QString name = q.value(0).toString();
            QString url = q.value(1).toString();

            podcasts[name] = url;
        }
    }

    return true;
}



bool CDatabaseConnector::deletePodcast(QString name) {
    if (!this -> _database->isOpen())
             this -> _database->open();

    QSqlQuery q (*_database);
    q.prepare("DELETE FROM savedpodcasts WHERE name = :name;" );
    q.bindValue(":name", name);
    if(!q.exec()) {
        qDebug() << "Could not delete podcast " << name;
        return false;
    }

    qDebug() << "podcast " << name << " successfully deleted";

    return true;
}




bool CDatabaseConnector::addPodcast(QString name, QString url) {
    if (!this -> _database->isOpen())
             this -> _database->open();

    QSqlQuery q (*_database);
    q.prepare("INSERT INTO savedpodcasts (name, url) VALUES (:name, :url); " );
    q.bindValue(":name", name);
    q.bindValue(":url", url);

    if(!q.exec()) {
        qDebug() << "Could not add podcast " << name << ", " << url;
        return false;
    }

    qDebug() << "podcast " << name << ", " << url << " successfully added";
    return true;
}

