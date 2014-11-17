#ifndef SETTING_H
#define SETTING_H

#include <QString>
#include <QDebug>
#include <typeinfo>
#include "Settings/SettingKey.h"
#include "Settings/SettingConverter.h"
#include "DatabaseAccess/CDatabaseConnector.h"


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

		virtual ~AbstrSetting(){

		}

		QString get_db_key() const {
			return _db_key;
		}

		SK::SettingKey get_key() const {
			return _key;
		}

		virtual void load_db(CDatabaseConnector* db)=0;
		virtual void store_db(CDatabaseConnector* db)=0;

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

		template<typename SK::SettingKey S>
		Setting(const SettingKey<T, S> key, const char* db_key) :
			AbstrSetting(S, db_key)
		{
			Q_UNUSED(key);
		}

		template<typename SK::SettingKey S>
		Setting(const SettingKey<T, S> key, const char* db_key, T def) :
			Setting(key, db_key)
		{
			Q_UNUSED(key);

			_default_val = def;
			_val = def;
		}

		virtual ~Setting(){

		}

		T getValue() const {
			return _val;
		}

		T getDefaultValue() const {
			return _default_val;
		}

		void setValue(const T& val){
			_val = val;
		}


		virtual void load_db(CDatabaseConnector* db){
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

			qDebug() << "Load setting " << _db_key << ": " << typeid(_val).name() << " " << s << " (" << success << ")";
		}

		virtual void store_db(CDatabaseConnector* db){

			QString s = SC<T>::cvt_to_string(_val);
			qDebug() << "Store setting " << _db_key << " " << s;
			db->store_setting(_db_key, s);
		}

};

typedef Setting<int> IntSetting;
typedef Setting<bool> BoolSetting;
typedef Setting<QString> StringSetting;
typedef Setting<QStringList> StringListSetting;


#endif // SETTING_H
