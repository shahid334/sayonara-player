/* GUI_Alternate_Covers.cpp */

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
 * GUI_Alternate_Covers.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "GUI/alternate_covers/AlternateCoverItemDelegate.h"
#include "GUI/alternate_covers/AlternateCoverItemModel.h"

#include <ui_GUI_Alternate_Covers.h>
#include <QPixmap>



GUI_Alternate_Covers::GUI_Alternate_Covers() {
	this->ui = new Ui::GUI_Alternate_Covers();
	this->ui->setupUi(this);

	this->_model = new AlternateCoverItemModel();
	this->_delegate = new AlternateCoverItemDelegate();

	this->ui->tv_images->setModel(_model);
	this->ui->tv_images->setItemDelegate(_delegate);

	connect(this->ui->btn_save, SIGNAL(pressed()), this, SLOT(save_button_pressed()));
	connect(this->ui->btn_cancel, SIGNAL(pressed()), this, SLOT(cancel_button_pressed()));
	connect(this->ui->btn_search_album, SIGNAL(pressed()), this, SLOT(search_album_button_pressed()));
	connect(this->ui->btn_search_artist, SIGNAL(pressed()), this, SLOT(search_artist_button_pressed()));
	connect(this->ui->tv_images, SIGNAL(pressed(const QModelIndex& )), this, SLOT(cover_pressed(const QModelIndex& )));


}


GUI_Alternate_Covers::~GUI_Alternate_Covers() {
	_pixmaps.clear();
}


void GUI_Alternate_Covers::start(const QString& artist, const QString& album){

	this->show();
	this->ui->le_search->setText(album);

}


void GUI_Alternate_Covers::fill_covers(){

	_model->removeRows(0, _model->rowCount());
	_model->insertRows(0, _pixmaps.size() / 5 + 1);

	for(int i=0; i<_pixmaps.size(); i++){

		int row = i / 5;
		int col = i % 5;

		QModelIndex idx = _model->index(row, col);
		_model->setData(idx, i, Qt::EditRole);
	}
}

void GUI_Alternate_Covers::new_cover_found(const QPixmap& pixmap){

	_pixmaps.push_back(pixmap);

}


void GUI_Alternate_Covers::save_button_pressed(){

	this->close();

}

void GUI_Alternate_Covers::cancel_button_pressed(){

	this->close();
}

void GUI_Alternate_Covers::search_album_button_pressed(){
	if(this->ui->le_search->text().size() < 3) return;
	emit search_artist_image(this->ui->le_search->text());

}

void GUI_Alternate_Covers::search_artist_button_pressed(){

	if(this->ui->le_search->text().size() < 3) return;
	emit search_album_image(this->ui->le_search->text());

}

void GUI_Alternate_Covers::cover_pressed(const QModelIndex& idx){
	int row = idx.row();
	int col = idx.column();

	_cur_idx = row * 5 + col;
}





