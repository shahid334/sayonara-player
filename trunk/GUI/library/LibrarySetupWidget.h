/* LibrarySetupWidget.h */

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
 * LibrarySetupWidget.h
 *
 *  Created on: Apr 27, 2011
 *      Author: luke
 */

#ifndef LIBRARYSETUPWIDGET_H_
#define LIBRARYSETUPWIDGET_H_

#include <QDialog>
#include <QObject>
#include <ui_GUI_LibrarySetup_Widget.h>

class LibrarySetupWidget : public QWidget, private Ui::LibrarySetup{

	Q_OBJECT

public:

	LibrarySetupWidget(QWidget* parent=0);
	virtual ~LibrarySetupWidget();

	signals:
		void libpath_changed(QString);

private slots:
		void save_button_clicked(bool b=true);
		void choose_button_clicked(bool b=true);
/*
public slots:
		void show();*/

	private:
		Ui::LibrarySetup* ui;

};

#endif /* LIBRARYSETUPWIDGET_H_ */
