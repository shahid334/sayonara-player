/* GUI_Speed.cpp */

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



#include "GUI_Speed.h"

GUI_Speed::GUI_Speed(QString name, QWidget *parent) :
	PlayerPlugin(name, parent)
{
	ui = new Ui::GUI_Speed();
	ui->setupUi(this);

	connect(ui->sli_speed, SIGNAL(sliderMoved(int)), this, SLOT(slider_changed(int)));
	connect(ui->cb_active, SIGNAL(toggled(bool)), this, SLOT(active_changed(bool)));
}


void GUI_Speed::slider_changed(int val){
	float val_f = val / 100.0f;
	ui->lab_speed->setText(QString::number(val_f, 'f', 2));
	emit sig_speed_changed(val_f);
}


void GUI_Speed::active_changed(bool b){

	if(!b){
		emit sig_speed_changed(-1.0f);
	}

	else {
		emit sig_speed_changed( ui->sli_speed->value() / 100.0f );
	}
}
