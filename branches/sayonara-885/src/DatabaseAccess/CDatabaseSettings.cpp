/* CDatabaseSettings.cpp */

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
#include "Settings/Settings.h"

#include <QFile>
#include <QDir>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>


bool CDatabaseConnector::load_settings() {

    DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_BOOL(_database);

	AbstrSetting** settings = Settings::getInstance()->get_settings();

	for(int i=0; i<SK::Num_Setting_Keys; i++){
		AbstrSetting* s = settings[i];
		if(!s) continue;

		s->load_db(this);
	}

    return true;
}

bool CDatabaseConnector::store_settings() {

    DB_TRY_OPEN(_database);
    DB_RETURN_NOT_OPEN_BOOL(_database);

	AbstrSetting** settings = Settings::getInstance()->get_settings();
	_database->transaction();

	for(int i=0; i<SK::Num_Setting_Keys; i++){
		AbstrSetting* s = settings[i];
		if(!s) continue;

		s->store_db(this);
	}

	_database->commit();
	return true;
}


bool CDatabaseConnector::load_setting(QString key, QString& tgt_value) const {

	QSqlQuery q (*_database);
	q.prepare("select value from settings where key = ?;");
	q.addBindValue(QVariant(key));

	if (!q.exec()) {
		qDebug() << "Cannot load setting " << key;
		qDebug() << _database->lastError();
		return false;
	}

	if(q.next()) {
		tgt_value = q.value(0).toString();
		return true;
	}

	return true;
}


bool CDatabaseConnector::store_setting(QString key, const QVariant& value) const {


	QSqlQuery q (*_database);
	q.prepare("select value from settings where key = :key;");
	q.bindValue(":key", key);

	if (!q.exec()) {
		qDebug() << "SQL Error (1): Cannot insert " << key;
		qDebug() << _database->lastError();
		return false;
	}

	if (!q.next()) {
		q.prepare("INSERT INTO settings VALUES(:key, :val);");
		q.bindValue(":key", key);
		q.bindValue(":value", value);
		if (!q.exec()) {
			qDebug() << "SQL Error (2): Cannot insert " << key;
			qDebug() << _database->lastError();
			return false;
		}

		else{
			qDebug() << "Inserted " << key << " first time";
		}
	}

	q.prepare("UPDATE settings set value=:value WHERE key=:key;");
	q.bindValue(":key", key);
	q.bindValue(":value", value);

	if (!q.exec()) {
		qDebug() << "SQL Error (3): Cannot insert " << key;
		qDebug() << _database->lastError();
		return false;
	}


	return true;

}

