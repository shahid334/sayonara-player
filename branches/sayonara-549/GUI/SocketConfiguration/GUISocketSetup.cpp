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
#include "HelperStructs/CSettingsStorage.h"

#include <QDialog>


GUI_SocketSetup::GUI_SocketSetup(QWidget* parent) : QDialog(parent){

    this->ui = NULL;
    hide();

}

GUI_SocketSetup::~GUI_SocketSetup() {

}


void GUI_SocketSetup::_sl_start_changed(int val){

	if(val < 65525)
		this->ui->sb_increment->setValue(val + 10);

	else
		this->ui->sb_increment->setValue(65535);
}


void GUI_SocketSetup::_sl_ok_pressed(){
	_db->setSocketActivated(this->ui->cb_activate->isChecked());
	_db->setSocketFrom(this->ui->sb_start->value());
	_db->setSocketTo(this->ui->sb_increment->value());

    hide();
    close();
}

void GUI_SocketSetup::show_win(){

    if(this->ui == NULL){
        ui = new Ui::SocketSetupDialog();
        ui->setupUi(this);

        _db = CSettingsStorage::getInstance();
        _socket_from = _db->getSocketFrom();
        _socket_to = _db->getSocketTo();

        if(_socket_from == 0 ||  _socket_from > 65535) _socket_from = 1024;
        if(_socket_to == 0 ||  _socket_to > 65535) _socket_to = 1034;

        this->ui->cb_activate->setChecked(_db->getSocketActivated());
        this->ui->sb_start->setValue(_socket_from);
        this->ui->sb_increment->setValue(_socket_to);

        connect(this->ui->sb_start, SIGNAL(valueChanged(int)), this, SLOT(_sl_start_changed(int)));
        connect(this->ui->buttonBox, SIGNAL(accepted()), this, SLOT(_sl_ok_pressed()));
    }

    this->show();
}

