/* LibrarySetupWidget.cpp */

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
 * LibrarySetupWidget.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: luke
 */

#include <QFileDialog>
#include <QWidget>
#include <QObject>
#include "GUI/library/SetupWidget/LibrarySetupWidget.h"

#include <ui_GUI_LibrarySetup_Widget.h>

LibrarySetupWidget::LibrarySetupWidget(QWidget* parent) : QWidget(parent) {
	this->ui = new Ui::LibrarySetup();

	this->ui->setupUi(this);
	connect(this->ui->btn_save, SIGNAL(clicked(bool)), this, SLOT(save_button_clicked(bool)));
	connect(this->ui->btn_choose, SIGNAL(clicked(bool)), this, SLOT(choose_button_clicked(bool)));

}

LibrarySetupWidget::~LibrarySetupWidget() {
	// TODO Auto-generated destructor stub
}

void LibrarySetupWidget::save_button_clicked(bool b){
	Q_UNUSED(b);
	if(this->ui->le_libpath->text().length() > 3){
		emit libpath_changed(this->ui->le_libpath->text());
		this->close();
	}
}

void LibrarySetupWidget::choose_button_clicked(bool b){
	Q_UNUSED(b)
	QString path = QFileDialog::getExistingDirectory(this, "Choose music library", QDir::homePath());
	this->ui->le_libpath->setText(path);

}/*

void LibrarySetupWidget::show(){
	this->show();
}*/
