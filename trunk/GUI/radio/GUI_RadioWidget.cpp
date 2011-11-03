/* GUI_RadioWidget.cpp */

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
 * GUIRadioWidget.cpp
 *
 *  Created on: Oct 22, 2011
 *      Author: luke
 */

#include "GUI/radio/GUI_RadioWidget.h"
#include <QWidget>
#include <QDockWidget>

GUI_RadioWidget::GUI_RadioWidget(QWidget* parent) : QDockWidget(parent)  {

	this->_ui = new Ui::RadioWidget( );
	this->_ui->setupUi(this);
	this->_ui->rb_artist->setChecked(true);

	connect(_ui->rb_artist, SIGNAL(released()), this, SLOT(radio_button_changed()));
	connect(_ui->rb_tag, SIGNAL(released()), this, SLOT(radio_button_changed()));

	connect(_ui->btn_listen, SIGNAL(released()), this, SLOT(start_listen()));


}

GUI_RadioWidget::~GUI_RadioWidget() {
	// TODO Auto-generated destructor stub
}


void GUI_RadioWidget::start_listen(){

	if(_ui->le_text->text().size() >= 3)
		emit listen_clicked(_ui->le_text->text(), _ui->rb_artist->isChecked());
}

void GUI_RadioWidget::radio_button_changed(){

	if(_ui->rb_artist->isChecked()){
		_ui->rb_tag->setChecked(false);
	}

	else _ui->rb_artist->setChecked(false);
}
