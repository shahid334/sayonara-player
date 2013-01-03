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

	_calling_class = calling_class;
	_class_name = "Alternate Covers";
	_cov_lookup = new CoverLookup(_class_name);
	_cur_idx = -1;

	_model = new AlternateCoverItemModel();
	_delegate = new AlternateCoverItemDelegate();

	this->ui->tv_images->setModel(_model);
	this->ui->tv_images->setItemDelegate(_delegate);

	_tmp_dir = Helper::getSayonaraPath() + QDir::separator() + "tmp";
	QDir dir(_tmp_dir);
	if (!dir.exists()) {
		dir.mkpath(".");
	}

	QStringList paths;
	paths << _tmp_dir;

	_watcher= new QFileSystemWatcher(paths);

	connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
	connect(this->ui->btn_cancel, SIGNAL(clicked()), this, SLOT(cancel_button_pressed()));
    connect(this->ui->btn_no_cover, SIGNAL(clicked()), this, SLOT(no_cover_pressed()));
	connect(this->ui->btn_search, SIGNAL(clicked()), this, SLOT(search_button_pressed()));
	connect(this->ui->tv_images, SIGNAL(pressed(const QModelIndex& )), this, SLOT(cover_pressed(const QModelIndex& )));
	connect(this->_cov_lookup, SIGNAL(sig_multi_covers_found(QString, int)), this, SLOT(covers_there(QString, int)));
	connect(this->_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
	connect(this->_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));

}


GUI_Alternate_Covers::~GUI_Alternate_Covers() {
	delete _model;
	delete _delegate;
	delete ui;
}

void GUI_Alternate_Covers::changeSkin(bool dark){

}

void GUI_Alternate_Covers::cleanup(){

	 _cov_lookup->terminate_thread();

	_model->removeRows(0, _model->rowCount());
	_filelist.clear();

	ui->pb_progress->setTextVisible(false);
	ui->pb_progress->setVisible(false);
	ui->btn_search->setText("Search");
}


void GUI_Alternate_Covers::start(QString searchstring, QString target_filename){

	cleanup();

    _no_album = true;
    _target_filename = target_filename;

    this->ui->le_search->setText(searchstring);
    this->ui->lab_title->setText(searchstring);

    this->show();

}


void GUI_Alternate_Covers::start(int album_artist_id, bool search_for_album){

	cleanup();

    _no_album = false;
    _search_for_album = search_for_album;

    if(search_for_album){
		_album = CDatabaseConnector::getInstance()->getAlbumByID(album_artist_id);

		if(_album.is_sampler){
			this->ui->le_search->setText(_album.name);
			this->ui->lab_title->setText(_album.name + " by various artists");
		}

		else if(_album.artists.size() == 0){
			this->ui->le_search->setText(_album.name);
			this->ui->lab_title->setText(_album.name + " by unknown artist");
		}

		else{
			this->ui->le_search->setText(_album.artists[0] + " " + _album.name);
			this->ui->lab_title->setText(_album.name + " by " + _album.artists[0]);
		}
	}

	else{
		_artist = CDatabaseConnector::getInstance()->getArtistByID(album_artist_id);
		this->ui->le_search->setText(_artist.name);
		this->ui->lab_title->setText(_artist.name);
	}

	this->show();
}


void GUI_Alternate_Covers::delete_token(){

    QString cover_token;

    if(_no_album){
        cover_token = _target_filename;
    }

    else if(_search_for_album){
        if(_album.is_sampler ){

            foreach(QString artist, _album.artists){
                cover_token = Helper::get_cover_path(artist, _album.name);
                QFile::remove(cover_token);
            }

            return;

        }

        else if(_album.artists.size() == 0)
            cover_token = Helper::get_cover_path("", _album.name);


        else
            cover_token = Helper::get_cover_path(_album.artists[0], _album.name);

    }

    else
        cover_token = Helper::get_artist_image_path(_artist.name);

    QFile::remove(cover_token);

}


QString GUI_Alternate_Covers::copy_and_save(QString src_filename){

    QFile file(src_filename);

    if(!file.exists()) {
        QMessageBox::warning(this, "Information", "This cover does not exist... Sorry" );
        return "";
    }

    QString cover_token;
    bool success = true;

    if(_no_album){

        cover_token = _target_filename;
        if(QFile::exists(cover_token)) QFile::remove(cover_token);
        success &= file.copy(cover_token);
    }


    else if(_search_for_album){
        if(_album.is_sampler ){

            foreach(QString artist, _album.artists){
                cover_token = Helper::get_cover_path(artist, _album.name);
                if(QFile::exists(cover_token)) QFile::remove(cover_token);
                success &= file.copy(cover_token);
            }
        }

        else if(_album.artists.size() == 0){
            cover_token = Helper::get_cover_path("", _album.name);
            if(QFile::exists(cover_token)) QFile::remove(cover_token);
            success = file.copy(cover_token);
        }

        else{
            cover_token = Helper::get_cover_path(_album.artists[0], _album.name);
            if(QFile::exists(cover_token)) QFile::remove(cover_token);
            success = file.copy(cover_token);
        }
    }

    else{
        cover_token = Helper::get_artist_image_path(_artist.name);
        if(QFile::exists(cover_token)) QFile::remove(cover_token);
        success = file.copy(cover_token);
    }


    if(!success) cover_token = "";
    return cover_token;
}

void GUI_Alternate_Covers::save_button_pressed(){

	if(_cur_idx == -1) return;

	int row = _cur_idx / _model->columnCount();
	int col = _cur_idx % _model->columnCount();

	QModelIndex idx = _model->index(row, col);

	QString src_filename = _model->data(idx, Qt::WhatsThisRole).toString().split(',')[0];
    QString cover_token = copy_and_save(src_filename);

    if(cover_token.size() > 0) {

        emit sig_covers_changed(_calling_class);
		close();
	}

	else QMessageBox::warning(this, "Information", "Some error appeared when updating cover" );
}


void GUI_Alternate_Covers::cancel_button_pressed(){

	close();
}


void GUI_Alternate_Covers::no_cover_pressed(){

    delete_token();

    emit sig_no_cover();

    close();
}

void GUI_Alternate_Covers::remove_old_files(){
	QStringList filters;
	filters << "*.jpg" << "*.png" << "*.gif";
	Helper::remove_files_in_directory(_tmp_dir, filters);
}

void GUI_Alternate_Covers::search_button_pressed(){

    _cur_idx = -1;
    _cov_lookup->terminate_thread();
    remove_old_files();

	if(ui->btn_search->text().compare("Stop") == 0){

		ui->btn_search->setText("Search");
        ui->pb_progress->setVisible(false);
		return;
	}

	QString searchstring = this->ui->le_search->text();

	ui->btn_search->setText("Stop");
	ui->pb_progress->setValue(0);
	ui->pb_progress->setTextVisible(true);
	ui->pb_progress->setVisible(true);

	_cov_lookup->search_images_by_searchstring(searchstring, 10, _search_for_album);
}

void GUI_Alternate_Covers::update_model(){

    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, 2);

    for(int i=0; i<_filelist.size(); i++){

        QString str = _filelist[i];
        QString str_tmp = str.right(str.size() - (str.lastIndexOf(QDir::separator()) + 1));
        str_tmp = str_tmp.left(str_tmp.size() - 4);
        str_tmp.replace("image_", "");
        int number = str_tmp.toInt() - 1;

        int row = number / _model->columnCount();
        int col = number % _model->columnCount();
        QModelIndex idx = _model->index(row, col);

        this->ui->tv_images->setColumnWidth(col, 100);
        this->ui->tv_images->setRowHeight(row,100);

        if(number == _cur_idx) str.append(",1");
        else str.append(",0");

		_model->setData(idx, str, Qt::EditRole);
	}

}

void GUI_Alternate_Covers::cover_pressed(const QModelIndex& idx){
	int row = idx.row();
	int col = idx.column();

	_cur_idx = row * _model->columnCount() + col;

    update_model();
}


void GUI_Alternate_Covers::covers_there(QString classname, int n_covers){

	if(classname != _class_name) return;

    _filelist.clear();

	QDir dir(_tmp_dir);
	QStringList entrylist;

	QStringList filters;
		filters << "*.jpg";
		filters << "*.png";
		filters << "*.gif";

	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);

	entrylist = dir.entryList();

	foreach (QString f, entrylist)
		_filelist << dir.absoluteFilePath(f);

    update_model();

	ui->pb_progress->setVisible(false);
	ui->btn_search->setText("Search");
}


void GUI_Alternate_Covers::tmp_folder_changed(const QString& directory){

	_filelist.clear();

	QDir dir(directory);
	QStringList entrylist;
	QStringList filters;
		filters << "*.jpg";
		filters << "*.png";
		filters << "*.gif";

	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);

	entrylist = dir.entryList();

	foreach (QString f, entrylist)
		_filelist << dir.absoluteFilePath(f);


    update_model();

	ui->pb_progress->setTextVisible(false);
	ui->pb_progress->setVisible(true);
	ui->pb_progress->setValue(_filelist.size() * 10);

	if(ui->pb_progress->value() == 100)
		ui->pb_progress->setVisible(false);

}





