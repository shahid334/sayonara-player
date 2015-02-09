

/* CDatabaseBookmarks.cpp */

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

#include "DatabaseAccess/CDatabaseConnector.h"

#include <QFile>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

bool CDatabaseConnector::searchBookmarks(int track_id, QMap<quint32, QString>& bookmarks) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	bookmarks.clear();

	QSqlQuery q (*_database);
	q.prepare("SELECT name, timeidx FROM savedbookmarks WHERE trackid=:trackid;");
	q.bindValue(":trackid", track_id);

	if (!q.exec()){
		show_error( QString("Could not fetch bookmarks for track ") + QString::number(track_id), q);
		return false;
	}

	while(q.next()) {
		QString name = q.value(0).toString();
		quint32 bm = q.value(1).toUInt();

		bookmarks.insert(bm, name);
	}

	return true;
}


bool CDatabaseConnector::insertBookmark(int track_id, quint32 time, QString name) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	q.prepare("INSERT INTO savedbookmarks (trackid, name, timeidx) VALUES(:trackid, :name, :timeidx);");
	q.bindValue(":trackid", track_id);
	q.bindValue(":name", name);
	q.bindValue(":timeidx", time);

	if (!q.exec()){
		show_error("Cannot insert bookmarks", q);
		return false;
	}

	return true;
}


bool CDatabaseConnector::removeBookmark(int track_id, quint32 time) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	q.prepare("DELETE FROM savedbookmarks WHERE trackid=:trackid AND timeidx=:timeidx;");
	q.bindValue(":trackid", track_id);
	q.bindValue(":timeidx", time);

	if (!q.exec()){
		show_error("Cannot remove bookmark", q);
		return false;
	}

	return true;
}


bool CDatabaseConnector::removeAllBookmarks(int track_id) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QSqlQuery q (*_database);
	q.prepare("DELETE FROM savedbookmarks WHERE trackid=:trackid;");
	q.bindValue(":trackid", track_id);

	if (!q.exec()){
		show_error("Cannot remove all bookmarks", q);
		return false;
	}

	return true;
}

