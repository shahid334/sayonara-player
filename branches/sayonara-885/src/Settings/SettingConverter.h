#ifndef SETTINGCONVERTER_H
#define SETTINGCONVERTER_H

#include <QString>
#include <QSize>
#include <QPoint>
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/Equalizer_presets.h"


template<typename T>
class SettingConverter{
public:
	static QString cvt_to_string(const T& val){
		return val.toString();
	}

	static bool cvt_from_string(QString val, T& ret){
		ret = T::fromString(val);
		return true;
	}
};

template<>
class SettingConverter<int>{
public:
	static QString cvt_to_string(const int& val){
		return QString::number(val);
	}

	static bool cvt_from_string(QString val, int& i){
		bool ok;
		i = val.toInt(&ok);

		return ok;
	}
};


template<>
class SettingConverter<QStringList>{
public:
	static QString cvt_to_string(const QStringList& val){
		return val.join(",");
	}

	static bool cvt_from_string(QString val, QStringList& lst){
		lst = val.split(",");
		return true;
	}
};


template<>
class SettingConverter<bool>{
public:
	static QString cvt_to_string(const bool& val){
		if(val) return QString("true");
		else return QString("false");
	}

	static bool cvt_from_string(QString val, bool& b){
		if( val.compare("true", Qt::CaseInsensitive) == 0 ||
			val.toInt() == 1)
		{
			b = true;
		}

		else
		{
			b = false;
		}

		return true;
	}
};


template<>
class SettingConverter<QString>{
public:
	static QString cvt_to_string(const QString& val){
		QString ret(val);
		return ret;
	}

	static bool cvt_from_string(QString val, QString& b){
		b = val;
		return true;
	}
};

template<>
class SettingConverter<QSize>{
public:
	static QString cvt_to_string(const QSize& val){
		return QString::number(val.width()) + "," + QString::number(val.height());
	}

	static bool cvt_from_string(QString val, QSize& sz){

		bool ok;
		int width, height;

		QStringList lst = val.split(",");

		if(lst.size() < 2) return false;

		width = lst[0].toInt(&ok);

		if(!ok) return false;
		height = lst[1].toInt(&ok);
		if(!ok) return false;

		sz.setWidth(width);
		sz.setHeight(height);

		return true;
	}
};


template<>
class SettingConverter<QPoint>{
public:
	static QString cvt_to_string(const QPoint& val){
		return QString::number(val.x()) + "," + QString::number(val.y());
	}

	static bool cvt_from_string(QString val, QPoint& sz){

		bool ok;
		int x, y;

		QStringList lst = val.split(",");

		if(lst.size() < 2) return false;

		x = lst[0].toInt(&ok);

		if(!ok) return false;
		y = lst[1].toInt(&ok);
		if(!ok) return false;

		sz.setX(x);
		sz.setY(y);

		return true;
	}
};


template<typename T>
class SettingConverter< QList<T> >{

public:
	static QString cvt_to_string(const QList<T>& val){

		SettingConverter<T> sc;
		QStringList lst;

		foreach(T v, val){
			lst << sc.cvt_to_string(v);
		}

		return lst.join(",");
	}

	static bool cvt_from_string(QString val, QList<T>& ret){

		SettingConverter<T> sc;
		ret.clear();
		QStringList lst = val.split(",");

		foreach(QString l, lst){
			T v;
			sc.cvt_from_string(l, v);
			ret << v;
		}

		return true;
	}
};

#endif // SETTINGCONVERTER_H
