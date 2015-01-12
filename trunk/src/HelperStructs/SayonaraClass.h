/* SayonaraClass.h */

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



#ifndef SAYONARACLASS_H
#define SAYONARACLASS_H

#include <QWidget>
#include <QDialog>
#include <QMainWindow>
#include "Settings/Settings.h"


class SayonaraClass {

	protected:
		Settings* _settings;

	public:
		SayonaraClass()
		{
			_settings = Settings::getInstance();
		}

		virtual ~SayonaraClass(){

		}
};

class SayonaraDialog : public QDialog, protected SayonaraClass {

	Q_OBJECT

private slots:
	virtual void language_changed(){}
	virtual void skin_changed(){}

public:
	SayonaraDialog(QWidget* parent=0) :
		QDialog(parent),
		SayonaraClass()

	{
		REGISTER_LISTENER(Set::Player_Language, language_changed);
		REGISTER_LISTENER(Set::Player_Style, skin_changed);
	}

	virtual ~SayonaraDialog(){

	}

};

class SayonaraWidget : public QWidget, protected SayonaraClass {

	Q_OBJECT

private slots:
	virtual void language_changed(){}
	virtual void skin_changed(){}

public:
	SayonaraWidget(QWidget* parent=0) :
		QWidget(parent),
		SayonaraClass()

	{
		REGISTER_LISTENER(Set::Player_Language, language_changed);
		REGISTER_LISTENER(Set::Player_Style, skin_changed);
	}

	virtual ~SayonaraWidget(){

	}
};


class SayonaraMainWindow : public QMainWindow, protected SayonaraClass {

	Q_OBJECT

private slots:
	virtual void language_changed(){}
	virtual void skin_changed(){}

public:
	SayonaraMainWindow(QWidget* parent=0) :
		QMainWindow(parent),
		SayonaraClass()

	{
		REGISTER_LISTENER(Set::Player_Language, language_changed);
		REGISTER_LISTENER(Set::Player_Style, skin_changed);
	}

	virtual ~SayonaraMainWindow(){

	}
};



#endif // SAYONARACLASS_H
