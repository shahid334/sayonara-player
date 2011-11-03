/* PlaylistMode.h */

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


#ifndef PLAYLISTMODE_H_
#define PLAYLISTMODE_H_


#include <iostream>
#include <QList>
#include <QVariant>
#include <QStringList>

using namespace std;

struct Playlist_Mode{

	bool				rep1;
	bool				repAll;
	bool				repNone;
	bool				append;
	bool				shuffle;
	bool				dynamic;

	Playlist_Mode(){
		rep1 = false;
		repAll = false;
		repNone = true;
		append = false;
		shuffle = false;

	}

	void print(){
		cout << "rep1 = " << rep1 << ", "
			<< "repAll = " << repAll << ", "
			<< "repNone = " << repNone << ", "
			<< "append = " << append <<", "
			<< "dynamic = " << dynamic << ","
			<< endl;
	}

	QString toString(){
		QString str;
		str += (append ? "1" : "0")  + QString(",");
		str += (repAll ? "1" : "0")  + QString(",");
		str += (rep1 ? "1" : "0")  + QString(",");
		str += (repNone ? "1" : "0")  + QString(",");
		str += (shuffle ? "1" : "0")  + QString(",");
		str += (dynamic ? "1" : "0");

		return str;
	}

	void fromString(QString str){
		QStringList list = str.split(',');
		if(list.size() != 6) return;

		append = list[0].toInt() == 1;
		repAll = list[1].toInt() == 1;
		rep1 = list[2].toInt() == 1;
		repNone = list[3].toInt() == 1;
		shuffle = list[4].toInt() == 1;
		dynamic = list[5].toInt() == 1;

	}




};

#endif
