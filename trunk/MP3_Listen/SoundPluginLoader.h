/* SoundPluginLoader.h */

/* Copyright (C) 2012  Lucio Carreras
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



#ifndef SOUNDPLUGINLOADER_H_
#define SOUNDPLUGINLOADER_H_

#include "MP3_Listen/Engine.h"

#include <QObject>
#include <QList>
#include <QString>

class SoundPluginLoader : public QObject {

	Q_OBJECT

private:
	int 			_cur_engine;
	QString			_cur_engine_name;
	QList<Engine*> 	_lst_engines;

	bool load_plugins(QString app_dir);

public:

	SoundPluginLoader(QString);
	virtual ~SoundPluginLoader();


	Engine* get_cur_engine();


signals:
	void sig_new_engine_set();

public slots:
	void psl_switch_engine(QString&);


};

#endif /* SOUNDPLUGINLOADER_H_ */
