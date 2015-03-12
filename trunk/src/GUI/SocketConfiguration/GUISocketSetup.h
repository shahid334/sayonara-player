/* GUISocketSetup.h

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 3, 2012 
 *
 */

#ifndef GUISOCKETSETUP_H_
#define GUISOCKETSETUP_H_

#include "HelperStructs/SayonaraClass.h"
#include "GUI/ui_GUI_SocketSetup.h"


class GUI_SocketSetup: public SayonaraDialog, private Ui::SocketSetupDialog {

	Q_OBJECT

public slots:
    void show_win();


private slots:
	void _sl_start_changed(int);
	void _sl_ok_pressed();

public:
	GUI_SocketSetup(QWidget* parent=0);
	virtual ~GUI_SocketSetup();

protected:
	virtual void language_changed();


private:
	int 					_socket_from;
	int 					_socket_to;
	bool					_is_activated;




};

#endif /* GUISOCKETSETUP_H_ */
