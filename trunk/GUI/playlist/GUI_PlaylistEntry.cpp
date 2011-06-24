/*
 * GUI_PlaylistEntry.cpp
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#include "GUI/playlist/GUI_PlaylistEntry.h"

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

	if(album == "") return;
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


