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
#include "HelperStructs/Style.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "GUI/alternate_covers/AlternateCoverItemDelegate.h"
#include "GUI/alternate_covers/AlternateCoverItemModel.h"
#include "CoverLookup/CoverLookup.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <ui_GUI_Alternate_Covers.h>
#include <QDialog>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QFileSystemWatcher>



GUI_Alternate_Covers::GUI_Alternate_Covers(QWidget* parent, QString calling_class) : QDialog(parent){

	this->ui = new Ui::AlternateCovers();
	this->ui->setupUi(this);

	_watcher= new QFileSystemWatcher(paths);
    _db = CDatabaseConnector::getInstance();

    connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_pressed()));
    connect(this->ui->btn_cancel, SIGNAL(clicked()), this, SLOT(cancel_pressed()));
    connect(this->ui->btn_no_cover, SIGNAL(clicked()), this, SLOT(no_cover_pressed()));


}


GUI_Alternate_Covers::~GUI_Alternate_Covers() {

	delete ui;
}


void GUI_Alternate_Covers::search_for_artist_image(QString artist_name, QString target_filename){
    ui->lab_title->setText("Alternative Image for artist");
   _target_cover_filename = target_filename;

    show();
}

void GUI_Alternate_Covers::search_for_sampler_image(QString album_name, QString target_filename){

    ui->lab_title->setText("Alternative Cover for " + album_name + " by Various artists");
    _target_cover_filename = target_filename;

    show();
}

void GUI_Alternate_Covers::search_for_album_image(QString artist_name, QString album_name, QString target_filename){
    ui->lab_title->setText("Alternative Cover for " + album_name + " by " + artist_name);
    _target_cover_filename = target_filename;
    show();
}


void GUI_Alternate_Covers::search_for_album_image(int album_id, QString target_filename){

    Album album = _db->getAlbumByID(album_id);
    if(album.is_sampler){
        ui->lab_title->setText("Alternative Cover for " + album_name + " by Various artists");

    }

    else if(album.artists.size() > 0){
        ui->lab_title->setText("Alternative Cover for " + album.name + " by " + album.artists[0]);
    }

    else{
        ui->lab_title->setText("Alternative Cover for " + album.name + " by Unknown artist");
    }

    _target_cover_filename = target_filename;

    show();

}

void GUI_Alternate_Covers::cover_chosen(QString filename){
    _sel_cover_filename = filename;
}


void GUI_Alternate_Covers::save_pressed(){
    if(_sel_cover_filename.size() == 0) return;
    if(_target_cover_filename.size() == 0) return;

    QFile f(_sel_cover_filename);
    f.copy(_target_cover_filename);

    emit sig_cover_found();


}

void GUI_Alternate_Covers::cancel_pressed(){
    this->close();
}

void GUI_Alternate_Covers::no_cover_pressed(){

}

