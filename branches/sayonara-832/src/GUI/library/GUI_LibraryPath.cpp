/* GUI_LibraryPath.cpp */

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



#include "GUI/library/GUI_LibraryPath.h"
#include "HelperStructs/Helper.h"
#include <QIcon>
#include <QDebug>


GUI_LibraryPath::GUI_LibraryPath(QWidget *parent) :
    QWidget(parent)
{
    ui = new Ui::GUI_SetLibrary();
    ui->setupUi(this);


    QIcon import(Helper::getIconPath() + "/import.png" );
    this->ui->btn_setLibrary->setIcon(import);

    connect(ui->btn_setLibrary, SIGNAL(clicked()), this, SLOT(btn_clicked()));
}

void GUI_LibraryPath::language_changed() {
    ui->retranslateUi(this);
}


void GUI_LibraryPath::btn_clicked(){

    emit sig_library_path_set();

}

void GUI_LibraryPath::resizeEvent(QResizeEvent* e){


    e->accept();
}


