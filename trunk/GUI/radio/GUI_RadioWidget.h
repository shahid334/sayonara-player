/* GUI_RadioWidget.h */

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
 * GUIRadioWidget.h
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#ifndef GUIRADIOWIDGET_H_
#define GUIRADIOWIDGET_H_

#include <ui_GUI_RadioWidget.h>
#include <QDockWidget>
#include <QWidget>

class GUI_RadioWidget : public QDockWidget, private Ui::RadioWidget{

Q_OBJECT

public:
	GUI_RadioWidget(QWidget* parent=0);
	virtual ~GUI_RadioWidget();


	signals:
		void listen_clicked(const QString&, bool artist);

	private slots:
		void start_listen();
		void radio_button_changed();



	private:

		Ui::RadioWidget* _ui;

};

#endif /* GUIRADIOWIDGET_H_ */
