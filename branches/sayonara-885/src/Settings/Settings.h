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

#define REGISTER_LISTENER(setting_key, fn) \
	SettingNotifier<setting_key##_t>* v_##fn = SettingNotifier<setting_key##_t>::getInstance();\
	connect(v_##fn, SIGNAL(sig_value_changed()), this, SLOT( fn() ));

class AbstrSettingNotifier : public QObject{

	Q_OBJECT

	protected:
		AbstrSettingNotifier(QObject* parent=0){}

	signals:
		void sig_value_changed();
};

template < typename T >
class SettingNotifier : public AbstrSettingNotifier{

	private:
		SettingNotifier( QObject* parent=0 ) : AbstrSettingNotifier(parent){}
		SettingNotifier( const SettingNotifier& ){}

	public:

	static SettingNotifier< T >* getInstance(){
		static SettingNotifier< T > inst;
		return &inst;
	}

	void val_changed(){
		emit sig_value_changed();
	}
};


class Settings : public QObject
{

public:

	static Settings* getInstance();
	virtual ~Settings ();

	QString		get_db_filename ();
	QString		get_version();

	void		set_version(QString str);

	AbstrSetting** get_settings();

	void register_setting(AbstrSetting* s);
	bool check_settings();


	template<typename T, SK::SettingKey S>
	T get(const SettingKey<T,S> k){

		Setting<T>* s = (Setting<T>*) _settings[(int) S];
		return s->getValue();
	}


	template<typename T, SK::SettingKey S>
	void set(SettingKey<T,S> key, const T& val){

		Setting<T>* s = (Setting<T>*) _settings[(int) S];
		s->setValue(val);

		SettingNotifier< SettingKey<T, S> >* sn = SettingNotifier< SettingKey<T, S> >::getInstance();
		sn->val_changed();
	}

private:
	Settings();

	QString _db_file;
	QString _version;

	AbstrSetting* _settings[SK::Num_Setting_Keys + 1];
};


#endif // Settings_H
