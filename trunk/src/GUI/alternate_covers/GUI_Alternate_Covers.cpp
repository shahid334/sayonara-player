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
#include "GUI/ui_GUI_Alternate_Covers.h"
#include "CoverLookup/CoverLookup.h"
#include "HelperStructs/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QDialog>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QFileSystemWatcher>
#include <unistd.h>



GUI_Alternate_Covers::GUI_Alternate_Covers(QWidget* parent, QString calling_class) : QDialog(parent){

	this->ui = new Ui::AlternateCovers();
	this->ui->setupUi(this);

    QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
    if(QFile::exists(lib_path))
        _last_path = lib_path;
    else _last_path = QDir::homePath();


    _cov_fetch_thread = 0;

	_calling_class = calling_class;
	_class_name = "Alternate Covers";
	_cur_idx = -1;

    _model = new AlternateCoverItemModel(this);
    _delegate = new AlternateCoverItemDelegate(this);

    this->ui->tv_images->setModel(_model);
    this->ui->tv_images->setItemDelegate(_delegate);
    _cov_fetch_thread = 0;

    _tmp_dir = Helper::getSayonaraPath() + "tmp";
	QDir dir(_tmp_dir);
	if (!dir.exists()) {
		dir.mkpath(".");
	}

	QStringList paths;
	paths << _tmp_dir;

    _watcher= new QFileSystemWatcher(paths);

	connect(this->ui->btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
	connect(this->ui->btn_cancel, SIGNAL(clicked()), this, SLOT(cancel_button_pressed()));
	connect(this->ui->btn_search, SIGNAL(clicked()), this, SLOT(search_button_pressed()));
	connect(this->ui->tv_images, SIGNAL(pressed(const QModelIndex& )), this, SLOT(cover_pressed(const QModelIndex& )));
    connect(this->_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
    connect(this->_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));

    connect(this->ui->btn_file, SIGNAL(clicked()), this, SLOT(open_file_dialog()));

}


GUI_Alternate_Covers::~GUI_Alternate_Covers() {
	delete _model;
	delete _delegate;
	delete ui;
}

void GUI_Alternate_Covers::changeSkin(bool dark){

}

void GUI_Alternate_Covers::language_changed(){
    this->ui->retranslateUi(this);
}

void GUI_Alternate_Covers::start(QString searchstring, QString target_filename, QString dir){

    if(dir.size() > 0){
        _last_path = dir;
    }
    _no_album = true;
    _target_filename = target_filename;

    QString old_searchstring = ui->le_search->text();
    ui->le_search->setText(searchstring);
    ui->lab_title->setText(searchstring);

    _filelist.clear();
    update_model();

    this->show();
}



void GUI_Alternate_Covers::save_button_pressed(){

	if(_cur_idx == -1) return;

	int row = _cur_idx / _model->columnCount();
	int col = _cur_idx % _model->columnCount();

	QModelIndex idx = _model->index(row, col);

	if(!idx.isValid()){
		qDebug() << "index not valid";
		return;
	}

	QString src_filename = _model->data(idx, Qt::WhatsThisRole).toString().split(',')[0];
	QFile file(src_filename);

	if(!file.exists()) {
		QMessageBox::warning(this, tr("Information"), tr("This cover does not exist") );
		return;
	}

	bool success = true;
    QString cover_token;

    if(_no_album){

        cover_token = _target_filename;

        QFile f2(cover_token);
        if(f2.exists()) f2.remove();
        success = file.copy(cover_token);
    }

    // should never happen
    else if(_search_for_album){
		if(_album.is_sampler ){

            cover_token = Helper::get_cover_path(_album.artists[0], _album.name);
			foreach(QString artist, _album.artists){
                QString ct = Helper::get_cover_path(artist, _album.name);
                if(QFile::exists(ct)) QFile::remove(ct);
                success &= file.copy(ct);
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


	if(success) {

		emit sig_covers_changed(_calling_class, cover_token);
	}

	else QMessageBox::warning(this, tr("Information"), tr("Some error appeared when updating cover") );
}


void GUI_Alternate_Covers::cancel_button_pressed(){

    for(int r=0; r<2; r++){
        for(int c=0; c<5; c++){
            QModelIndex idx = _model->index(r, c);
            _model->setData(idx, "0,0", Qt::EditRole);
        }
    }


    hide();
    close();
}


void GUI_Alternate_Covers::search_button_pressed(){

    if(_cov_fetch_thread && _cov_fetch_thread->isRunning()) {
        _cov_fetch_thread->set_run(false);
        ui->btn_search->setText(tr("Search"));
        return;
    }

    ui->btn_search->setText(tr("Stop"));

    _cur_idx = -1;
    _filelist.clear();
    update_model();

	QString searchstring = this->ui->le_search->text();
    QString url = Helper::calc_google_image_search_adress(searchstring);


    _blocked = false;
    _cov_fetch_thread = new CoverFetchThread(0, 1, url, 20);

    connect(_cov_fetch_thread, SIGNAL(destroyed()), this, SLOT(cft_destroyed()));
    connect(_cov_fetch_thread, SIGNAL(finished()), _cov_fetch_thread, SLOT(deleteLater()));

	QStringList filters;
	filters << "*.jpg";
	filters << "*.png";
	filters << "*.gif";

    disconnect(this->_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
    disconnect(this->_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));

	QDir dir(_tmp_dir);
	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);
	QStringList file_list = dir.entryList();
	foreach(QString filename, file_list){

		QFile file(dir.absoluteFilePath(filename));
		file.remove();
	}

    connect(this->_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
    connect(this->_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));

    _cov_fetch_thread->start();

}

void GUI_Alternate_Covers::cft_destroyed(){

    _cov_fetch_thread = 0;
    this->ui->btn_search->setText(tr("Search"));
}

void GUI_Alternate_Covers::update_model(){


    _model->removeRows(0, _model->rowCount());
    _model->insertRows(0, 2);

    QList<int> lst;
    lst << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9;

    for(int i=0; i<_filelist.size(); i++){

        QString str = _filelist[i];

        QString str_tmp = str.right(str.size() - (str.lastIndexOf(QDir::separator()) + 1));
        str_tmp = str_tmp.left(str_tmp.size() - 4);
        str_tmp.replace("img_", "");
        int number = str_tmp.toInt() - 1;
        if(number > 9) break;
        if(lst.contains(number))lst.removeAt(lst.indexOf(number));

        int row = number / _model->columnCount();
        int col = number % _model->columnCount();
        QModelIndex idx = _model->index(row, col);

        this->ui->tv_images->setColumnWidth(col, 100);
        this->ui->tv_images->setRowHeight(row,100);

        if(number == _cur_idx) str.append(",1");
        else str.append(",0");

		_model->setData(idx, str, Qt::EditRole);
	}



    if(ui->rb_online->isChecked()){

        if(lst.isEmpty() && _cov_fetch_thread) {
            _cov_fetch_thread->set_run(false);
            _blocked = false;
        }
    }

    else {
        ui->btn_search->setText(tr("Search"));
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

	ui->btn_search->setText(tr("Search"));
}


void GUI_Alternate_Covers::tmp_folder_changed(const QString& directory){

    if(_blocked ) return;
    _blocked = true;

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

    _blocked = false;
}


void GUI_Alternate_Covers::open_file_dialog(){


    if(_cov_fetch_thread && _cov_fetch_thread->isRunning()) {
        _cov_fetch_thread->set_run(false);
        return;
    }

    QDir dir(Helper::getSayonaraPath() + "tmp");
    QStringList entrylist;
    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

    dir.setFilter(QDir::Files);
    dir.setNameFilters(filters);

    entrylist = dir.entryList();

    foreach (QString f, entrylist)
        QFile::remove(dir.absoluteFilePath(f));


    QStringList lst = QFileDialog::getOpenFileNames(this,
                                  tr("Open image files"),
                                  _last_path,
                                  filters.join(" "));


    int i=1;
    foreach(QString f, lst){
        QImage img(f);


        QString filename =Helper::getSayonaraPath() + "tmp/img_" + QString::number(i) + ".jpg" ;
        _filelist << filename;
        img.save(filename);
        i++;
    }

    update_model();
}


