/* GUI_BroadcastSetup.cpp */

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



#include "GUI_BroadcastSetup.h"
#include "HelperStructs/Helper.h"


GUI_BroadcastSetup::GUI_BroadcastSetup(QWidget *parent) :
	SayonaraDialog(parent),
	Ui::GUI_BroadcastSetup()
{
	setupUi(this);
}

void GUI_BroadcastSetup::accept(){

	bool new_active = cb_active->isChecked();
	bool new_prompt = cb_prompt->isChecked();
	int new_port = sb_port->value();

	bool old_active = _settings->get(Set::BroadCast_Active);
	bool old_prompt = _settings->get(Set::Broadcast_Prompt);
	int old_port = _settings->get(Set::Broadcast_Port);

	if(old_active != new_active){
		_settings->set(Set::BroadCast_Active, new_active);
	}

	if(old_prompt != new_prompt){
		_settings->set(Set::Broadcast_Prompt, new_prompt);
	}

	if(old_port != new_port){
		_settings->set(Set::Broadcast_Port, new_port);
	}

	QDialog::accept();
}

void GUI_BroadcastSetup::language_changed(){
	retranslateUi(this);
}

void GUI_BroadcastSetup::reject(){
	QDialog::reject();
}

void GUI_BroadcastSetup::show(){

	cb_active->setChecked( _settings->get(Set::BroadCast_Active) );
	cb_prompt->setChecked( _settings->get(Set::Broadcast_Prompt) );
	sb_port->setValue( _settings->get(Set::Broadcast_Port) );

	QDialog::show();
}
