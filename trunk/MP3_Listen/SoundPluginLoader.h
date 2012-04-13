/*
 * SoundPluginLoader.h
 *
 *  Created on: Apr 13, 2012
 *      Author: luke
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
