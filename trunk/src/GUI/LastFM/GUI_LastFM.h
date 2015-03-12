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


#include <QString>
#include "GUI/ui_GUI_LastFM_Dialog.h"
#include "HelperStructs/SayonaraClass.h"

class GUI_LastFM : public SayonaraDialog, private Ui_GUI_LastFM_Dialog{

	Q_OBJECT

public:
	GUI_LastFM(QWidget* parent=0);
	GUI_LastFM(QWidget* parent, QString username, QString password);

	virtual ~GUI_LastFM();

private slots:
	void save_button_pressed();
	void clear_session_pressed();
	void cb_correct_id3_toggled(bool);
	void cb_activate_toggled(bool);


public slots:
	void show_win();

private:
	void init();
	void setLFMActive(bool b);

protected:
	virtual void language_changed();

public:
	void set_username_and_password(QString username, QString password);

};

#endif /* GUI_LASTFM_H_ */
