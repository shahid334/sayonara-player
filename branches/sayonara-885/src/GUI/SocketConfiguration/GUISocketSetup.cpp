/* GUISocketSetup.cpp

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

#include "GUI/ui_GUI_SocketSetup.h"
#include "GUI/SocketConfiguration/GUISocketSetup.h"
#include "Settings/Settings.h"

#include <QDialog>


GUI_SocketSetup::GUI_SocketSetup(QWidget* parent) :
	QDialog(parent) ,
	Ui::SocketSetupDialog(){

	setupUi(this);

	bool active;

	_db = Settings::getInstance();
	_socket_from = _db->get(Set::Socket_From);
	_socket_to =_db->get(Set::Socket_To);
	active = _db->get(Set::Socket_Active);

	if(_socket_from == 0 ||  _socket_from > 65535) _socket_from = 1024;
	if(_socket_to == 0 ||  _socket_to > 65535) _socket_to = 1034;

	cb_activate->setChecked( active );
	sb_start->setValue(_socket_from);
	sb_increment->setValue(_socket_to);

	connect(sb_start, SIGNAL(valueChanged(int)), this, SLOT(_sl_start_changed(int)));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(_sl_ok_pressed()));

    hide();

}

GUI_SocketSetup::~GUI_SocketSetup() {

}


void GUI_SocketSetup::_sl_start_changed(int val) {

	if(val < 65525)
		sb_increment->setValue(val + 10);

	else
		sb_increment->setValue(65535);
}


void GUI_SocketSetup::_sl_ok_pressed() {

	_db->set(Set::Socket_Active, cb_activate->isChecked());
	_db->set(Set::Socket_From, sb_start->value());
	_db->set(Set::Socket_To, sb_increment->value());

    hide();
    close();
}

void GUI_SocketSetup::show_win() {

	show();
}

void GUI_SocketSetup::language_changed() {

	retranslateUi(this);
}

