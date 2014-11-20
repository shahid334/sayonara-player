/* Settings.h */

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


#ifndef Settings_H
#define Settings_H

#include <QString>
#include <QThread>
#include <QPair>
#include <QSize>
#include <QPoint>

#include <HelperStructs/Equalizer_presets.h>
#include <HelperStructs/PlaylistMode.h>
#include <HelperStructs/MetaData.h>

#include "Settings/Setting.h"


class Settings : public QObject
{

private:
	Settings();

	QString _db_file;
	QString _version;

	AbstrSetting* _settings[SK::Num_Setting_Keys + 1];


public:

	static Settings* getInstance();
	virtual ~Settings ();

	/* get all settings (used by database) */
	AbstrSetting**	get_settings();


	/* before you wanna access a setting you have to register it */
	void register_setting(AbstrSetting* s);


	/* checks if all settings are registered */
	bool check_settings();


	/* get a setting, defined by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	T get(const SettingKey<T,S> k){
		Q_UNUSED(k);
		Setting<T>* s = (Setting<T>*) _settings[(int) S];
		return s->getValue();
	}


	/* set a setting, define by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	void set(SettingKey<T,S> key, const T& val){

		Setting<T>* s = (Setting<T>*) _settings[(int) S];

		if( s->setValue(val)) {
			SettingNotifier< SettingKey<T, S> >* sn = SettingNotifier< SettingKey<T, S> >::getInstance();
			sn->val_changed();
		}
	}
};


#endif // Settings_H
