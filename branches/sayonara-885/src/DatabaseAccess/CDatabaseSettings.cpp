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

    DB_RETURN_NOT_OPEN_BOOL(_database);

	Settings* _settings = Settings::getInstance();
	AbstrSetting** settings = _settings->get_settings();

	for(int i=0; i<SK::Num_Setting_Keys; i++){
		AbstrSetting* s = settings[i];
		if(!s) continue;

		s->load_db(this);
	}

    return true;
}

bool CDatabaseConnector::store_settings() {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	Settings* _settings = Settings::getInstance();
	AbstrSetting** settings = _settings->get_settings();
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

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	q.prepare("SELECT value FROM settings WHERE key = ?;");
	q.addBindValue(QVariant(key));

	if (!q.exec()) {
		show_error(QString("Cannot load setting ") + key);
		return false;
	}

	if(q.next()) {
		tgt_value = q.value(0).toString();
		return true;
	}

	return false;
}


bool CDatabaseConnector::store_setting(QString key, const QVariant& value) const {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	q.prepare("SELECT value FROM settings WHERE key = :key;");
	q.bindValue(":key", key);

	if (!q.exec()) {
		show_error(QString("Store setting: Cannot fetch setting ") + key);
		return false;
	}

	if (!q.next()) {
		q.prepare("INSERT INTO settings VALUES(:key, :val);");
		q.bindValue(":key", key);
		q.bindValue(":value", value);

		if (!q.exec()) {
			show_error(QString("Store setting: Cannot insert setting ") + key);
			return false;
		}

		qDebug() << "Inserted " << key << " first time";
	}

	q.prepare("UPDATE settings SET value=:value WHERE key=:key;");
	q.bindValue(":key", key);
	q.bindValue(":value", value);

	if (!q.exec()) {
		show_error(QString("Store setting: Cannot update setting ") + key);
		return false;
	}

	return true;
}

