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
#include <iostream>



#include "application.h"

#include <QApplication>

#include <string>
#include <vector>
#include <iostream>


using namespace std;


void printHelp(){
	qDebug() << "sayonara [-g]";
	qDebug() << "-g\tuse gstreamer instead of phonon";
}



int main(int argc, char *argv[]){

		if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")){
			QDir().mkdir(QDir::homePath() + QDir::separator() +  "/.Sayonara");
		}

		QApplication app (argc, argv);
			app.setApplicationName("Sayonara");
			app.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

            Application application(&app);

        app.exec();

        return 0;
}


