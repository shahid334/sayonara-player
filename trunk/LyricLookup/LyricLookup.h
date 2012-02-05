/* LyricLookup.h */

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


/*
 * LyricLookup.h
 *
 *  Created on: May 21, 2011
 *      Author: luke
 */

#ifndef LYRICLOOKUP_H_
#define LYRICLOOKUP_H_

#define WIKIA 0
#define OLDIELYRICS 1
#define LYRICSKEEPER 2
#define METROLYRICS 3
#define ASKLYRICS 4

#include "LyricLookup/LyricServer.h"
#include <QStringList>
#include <QString>
#include <QList>

class LyricLookup {
public:
	LyricLookup();
	virtual ~LyricLookup();

	QString find_lyrics(QString artist, QString song, int srv=0);
	QStringList getServers();
private:
	void init_server_list();

	QList<ServerTemplate> _server_list;
	int	_cur_server;

	QString calc_url(QString artist, QString song);
	bool parse_webpage(QString& dst);
};

#endif /* LYRICLOOKUP_H_ */
