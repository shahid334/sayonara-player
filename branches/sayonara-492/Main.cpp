/* Main.cpp */

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


/*
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */



#include "application.h"

#include <QApplication>
#include <QStringList>
#include <QDebug>

#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

int check_for_another_instance_unix(){

	int pid = -1;

	QDir dir("/proc");
	dir.cd(".");
	QStringList lst = dir.entryList(QDir::Dirs);
	int n_instances = 0;

	foreach(QString dirname, lst){
		bool ok;
		int tmp_pid = dirname.toInt(&ok);
		if(!ok) continue;

		dir.cd(dirname);

		QFile f(dir.absolutePath() + QDir::separator() + "cmdline");
		f.open(QIODevice::ReadOnly);
		if(!f.isOpen()) {
			dir.cd("..");
			continue;
		}

		QString str = f.readLine();
		f.close();

		if(str.contains("sayonara", Qt::CaseInsensitive)){
			if(pid == -1 || tmp_pid < pid) pid = tmp_pid;
			n_instances ++;
			if(n_instances > 1) return pid;
		}
		dir.cd("..");
	}

	return 0;
}


void printHelp(){
	qDebug() << "sayonara <list>";
	qDebug() << "<list> can consist of either files or directories or both";

}


int main(int argc, char *argv[]){
    CDatabaseConnector::getInstance()->init_settings_storage();
    CSettingsStorage* settings = CSettingsStorage::getInstance();
    bool success = CDatabaseConnector::getInstance()->load_settings();
	
    if(!success) {
	qDebug() << "Database Error: Could not load settings";
	return 0;
    }

#ifdef Q_OS_UNIX
	if(settings->getAllowOnlyOneInstance()){
		int pid = check_for_another_instance_unix();
		if(pid > 0) {
			qDebug() << "another instance is already running";
			QString kill_cmd = "kill -s 28 " + QString::number(pid);
			int success = system(kill_cmd.toLocal8Bit());
			Q_UNUSED(success);
			return 0;
		}
	}
#endif

   	if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")){
		QDir().mkdir(QDir::homePath() + QDir::separator() +  "/.Sayonara");
	}

		QApplication app (argc, argv);
            app.setApplicationName("Sayonara");
            app.setWindowIcon(QIcon(Helper::getIconPath() + "logo.png"));


		QStringList params;
		for(int i=1; i<argc; i++){
			QString param(argv[i]);
			params.push_back(param);
		}

        Application application(&app, params.size());
		if(!application.is_initialized()) return 0;
		application.setFiles2Play(params);

        app.exec();

        return 0;
}


