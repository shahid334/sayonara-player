#ifndef _EQUALIZER_PRESETS_
#define _EQUALIZER_PRESETS_

#include <QString>
#include <QStringList>

struct EQ_Setting{
	
	
	QList<double> settings;
	QString name;

	EQ_Setting(){

	}
	
	void parseFromString(QString str){

		settings.clear();

		QStringList list = str.split(',');
		name = list.at(0);

		for(int i=1; i<list.size(); i++){
			settings.append(list.at(i).toDouble());
		}
	}


	QString toString(){

		QString str = name;
		for(int i=0; i<settings.size(); i++){
			str += QString(",") + QString::number(settings[i]);
		}

		return str;
	}
};

#endif
