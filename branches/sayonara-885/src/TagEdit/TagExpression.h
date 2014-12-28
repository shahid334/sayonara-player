/* TagExpression.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef TAGEXPRESSION_H
#define TAGEXPRESSION_H

#include <QMap>
#include <QString>
#include <QStringList>


#define TAG_NONE
#define TAG_TITLE QString("<t>")
#define TAG_ALBUM QString("<al>")
#define TAG_ARTIST QString("<ar>")
#define TAG_TRACK_NUM QString("<nr>")
#define TAG_YEAR QString("<y>")
#define TAG_DISC QString("<d>")

class TagExpression {


	QString _tag_str;
	QString _filepath;

	QMap<QString, QString> _cap_map;
	QMap<QString, QString> _tag_regex_map;
	bool _valid;

	QString escape_special_chars(const QString& str);
	QString calc_regex_string(const QStringList& splitted_tag_str);
	QStringList split_tag_string(const QString& tag_str);

	void update_tag(QString regex);



public:

	TagExpression();
	TagExpression(const QString& tag_str, const QString& filename);
	virtual ~TagExpression();


	void update_tag(QString tag_str, QString filename);
	bool is_valid();

	bool check_tag(QString tag, QString str);



	QMap<QString, QString> get_tag_val_map();


};

#endif // TAGEXPRESSION_H
