/* Setting.h */

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



#ifndef SETTING_H
#define SETTING_H

#include <typeinfo>
#include "Settings/SettingKey.h"
#include "Settings/SettingConverter.h"
#include "Settings/SettingNotifier.h"
#include "DatabaseAccess/CDatabaseConnector.h"

// Every setting needs a key and a value
// The SK::SettingKey is only used inside the setting mechanism

class AbstrSetting{

	protected:
		AbstrSetting(){}
		AbstrSetting(const AbstrSetting&){}

	protected:
		SK::SettingKey	_key;
		QString			_db_key;


	protected:

		AbstrSetting(SK::SettingKey key, const char* db_key){
			_key = key;
			_db_key = db_key;
		}

	public:

		virtual ~AbstrSetting(){}

		QString get_db_key() const {
			return _db_key;
		}

		SK::SettingKey get_key() const {
			return _key;
		}

		/* Pure virtual function for DB load/save */
		virtual void load_db(const CDatabaseConnector* db)=0;
		virtual void store_db(const CDatabaseConnector* db)=0;
};


template< typename T,
		 template <typename Arg> class SC = SettingConverter >

class Setting : public AbstrSetting
{

	private:
		Setting();
		Setting(const Setting&);

		T _val;
		T _default_val;
        bool _db_setting;


	public:

		/* Constructor */
		template<typename SK::SettingKey S>
        Setting(const SettingKey<T, S> key, const char* db_key, T def) :
			AbstrSetting(S, db_key)
		{
			Q_UNUSED(key);
			_default_val = def;
			_val = def;
            _db_setting = true;
		}

        template<typename SK::SettingKey S>
        Setting(const SettingKey<T, S> key, T def) :
            AbstrSetting(S, "")
        {
            Q_UNUSED(key);
            _default_val = def;
            _val = def;
            _db_setting = false;
        }


		/* Destructor */
		virtual ~Setting(){

		}


		/* Load setting from DB */
		virtual void load_db(const CDatabaseConnector* db){

            if(!_db_setting) return;

			QString s;
			bool success = db->load_setting(_db_key, s);

			if(!success){
				qDebug() << "Setting " << _db_key << ": Not found. Use default value...";
				_val = _default_val;
				qDebug() << "Load Setting " << _db_key << ": " << SC<T>::cvt_to_string(_val);
				return;
			}

			success = SC<T>::cvt_from_string(s, _val);
			if(!success){
				qDebug() << "Setting " << _db_key << ": Cannot convert. Use default value...";
				_val = _default_val;
			}

			qDebug() << "Load Setting " << _db_key << ": " << SC<T>::cvt_to_string(_val);
		}

		/* Save setting to DB */
		virtual void store_db(const CDatabaseConnector* db){

            if(!_db_setting) return;

            QString s = SC<T>::cvt_to_string(_val);
			db->store_setting(_db_key, s);
			qDebug() << "Store Setting " << _db_key << ": " << s;
		}

		/* ... */
		T getValue() const {
			return _val;
		}

		/* ... */
		T getDefaultValue() const {
			return _default_val;
		}

		/* ... */
		bool setValue(const T& val){

			if( _val == val ){
				return false;
			}

			_val = val;
			return true;
		}
};

#endif // SETTING_H
