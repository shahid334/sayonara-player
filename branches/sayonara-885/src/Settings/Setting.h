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


template<typename T,
		 template <typename Arg> class SC = SettingConverter >

class Setting : public AbstrSetting
{

	private:
		Setting();
		Setting(const Setting&);

		T _val;
		T _default_val;


	public:

		/* Constructor */
		template<typename SK::SettingKey S>
		Setting(const SettingKey<T, S> key, const char* db_key, T def) :
			AbstrSetting(S, db_key)
		{
			Q_UNUSED(key);
			_default_val = def;
			_val = def;
		}


		/* Destructor */
		virtual ~Setting(){

		}


		/* Load setting from DB */
		virtual void load_db(const CDatabaseConnector* db){
			bool success;
			QString s;
			success = db->load_setting(_db_key, s);
			if(!success){
				_val = _default_val;
				return;
			}

			success = SC<T>::cvt_from_string(s, _val);
			if(!success){
				_val = _default_val;
			}
		}

		/* Save setting to DB */
		virtual void store_db(const CDatabaseConnector* db){

			QString s = SC<T>::cvt_to_string(_val);
			db->store_setting(_db_key, s);
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
		void setValue(const T& val){
			_val = val;
		}
};

#endif // SETTING_H
