/* GUI_PlaylistEntry.cpp */

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
 * GUI_PlaylistEntry.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#include "GUI/playlist/GUI_PlaylistEntry.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"

#include <QFrame>


GUI_PlaylistEntry::GUI_PlaylistEntry(QWidget* parent) : QFrame(parent){


	this->ui = new PlaylistEntry();
	this->ui->setupUi(this);



}

GUI_PlaylistEntry::~GUI_PlaylistEntry() {
	// TODO Auto-generated destructor stub
}

void GUI_PlaylistEntry::setArtist(QString artist){

	this->ui->lab_artist->setText(artist);

}


void GUI_PlaylistEntry::setAlbum(QString album){

	if(album == ""){
		this->ui->lab_album->setText("");
		return;
	}
	this->ui->lab_album->setText(QString(" [ ") + album + " ]");


}

void GUI_PlaylistEntry::setTitle(QString title){

	this->ui->lab_title->setText(title);
}


void GUI_PlaylistEntry::setTime(QString time){
	this->ui->lab_time->setText(time);

}


void GUI_PlaylistEntry::setBackgroundColorPlaying(){

	this->setStyleSheet("background-color: #C0C0C0;");

}


void GUI_PlaylistEntry::setBackgroundColorNotPlaying(){

	this->setStyleSheet("background-color: transparent;");
}


void GUI_PlaylistEntry::setWidth(int width){

	this->resize(width, 30);


}

void GUI_PlaylistEntry::setContent(MetaData& md, int idx){

	QString titlestr = md.title;
	if(CSettingsStorage::getInstance()->getPlaylistNumbers()){
		titlestr = QString::number(idx) + ". " + md.title;
	}
	this->ui->lab_title->setText(titlestr);


	this->ui->lab_artist->setText(md.artist);
	if(md.album == ""){
		this->ui->lab_album->setText("");
		return;
	}

	this->ui->lab_album->setText(QString(" [ ") + md.album.trimmed() + " ]");

	this->ui->lab_time->setText(Helper::cvtMsecs2TitleLengthString(md.length_ms));
}


