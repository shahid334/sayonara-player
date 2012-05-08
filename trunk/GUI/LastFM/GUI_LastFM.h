/* GUI_LastFM.h */

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
 * GUI_LastFM.h
 *
 *  Created on: Apr 21, 2011
 *      Author: luke
 */

#ifndef GUI_LASTFM_H_
#define GUI_LASTFM_H_

#include <QObject>
#include <QString>
#include <ui_GUI_LastFM_Widget.h>

class GUI_LastFM : public QWidget, private Ui_GUI_LastFM_Widget{

	Q_OBJECT

public:
	GUI_LastFM();
	GUI_LastFM(QString username, QString password);

	virtual ~GUI_LastFM();

	signals:
		void new_lfm_credentials(QString, QString);

	private slots:
		void save_button_pressed();
		void cb_correct_id3_toggled(bool);


	public slots:
		void show_win();

	private:
		Ui_GUI_LastFM_Widget* ui;


	public:
		void set_username_and_password(QString username, QString password);


};

#endif /* GUI_LASTFM_H_ */
