/* GUI_PlaylistEntrySmall.cpp */

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
 * GUI_PlaylistEntrySmall.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#include "GUI/playlist/GUI_PlaylistEntrySmall.h"

#include <QFrame>


GUI_PlaylistEntrySmall::GUI_PlaylistEntrySmall(QWidget* parent) : QFrame(parent){


	this->ui = new PlaylistEntrySmall();
	this->ui->setupUi(this);



}

GUI_PlaylistEntrySmall::~GUI_PlaylistEntrySmall() {
	// TODO Auto-generated destructor stub
}

void GUI_PlaylistEntrySmall::setArtist(QString artist){

	this->ui->lab_artist->setText(artist);

}


void GUI_PlaylistEntrySmall::setAlbum(QString album){

}

void GUI_PlaylistEntrySmall::setTitle(QString title){

	this->ui->lab_title->setText(title);
}


void GUI_PlaylistEntrySmall::setTime(QString time){
	this->ui->lab_time->setText(time);

}


void GUI_PlaylistEntrySmall::setBackgroundColorPlaying(){

	this->setStyleSheet("background-color: #C0C0C0;");

}


void GUI_PlaylistEntrySmall::setBackgroundColorNotPlaying(){

	this->setStyleSheet("background-color: transparent;");
}


void GUI_PlaylistEntrySmall::setWidth(int width){

	this->resize(width, 30);


}


