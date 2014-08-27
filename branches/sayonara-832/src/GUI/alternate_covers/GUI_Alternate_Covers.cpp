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
#include <QRegExp>



GUI_Alternate_Covers::GUI_Alternate_Covers(QWidget* parent, QString calling_class) :
	QDialog(parent),
	Ui::AlternateCovers()
{

	setupUi(this);



    QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
    if(QFile::exists(lib_path))
        _last_path = lib_path;
    else _last_path = QDir::homePath();

	_cl_alternative = 0;


	_cur_idx = -1;

    _model = new AlternateCoverItemModel(this);
    _delegate = new AlternateCoverItemDelegate(this);

	_is_searching = false;

	tv_images->setModel(_model);
	tv_images->setItemDelegate(_delegate);

	connect(btn_save, SIGNAL(clicked()), this, SLOT(save_button_pressed()));
	connect(btn_cancel, SIGNAL(clicked()), this, SLOT(cancel_button_pressed()));
	connect(btn_search, SIGNAL(clicked()), this, SLOT(search_button_pressed()));
	connect(tv_images, SIGNAL(pressed(const QModelIndex& )), this, SLOT(cover_pressed(const QModelIndex& )));
	connect(btn_file, SIGNAL(clicked()), this, SLOT(open_file_dialog()));
}


GUI_Alternate_Covers::~GUI_Alternate_Covers() {
	delete _model;
	delete _delegate;

	if(_cl_alternative) {
		_cl_alternative->stop();
	}

	delete_all_files();

}

void GUI_Alternate_Covers::changeSkin(bool dark) {

}

void GUI_Alternate_Covers::language_changed() {
	retranslateUi(this);
}

void GUI_Alternate_Covers::connect_and_start() {

	update_model();
	delete_all_files();

	connect(_cl_alternative, SIGNAL(sig_cover_found(const CoverLocation&)), this, SLOT(cl_new_cover(const CoverLocation&)));
	connect(_cl_alternative, SIGNAL(sig_finished(bool)), this, SLOT(cl_finished(bool)));

	_is_searching = true;
	btn_search->setText(tr("Stop"));

	_cl_alternative->start();

	show();
}


void GUI_Alternate_Covers::start(int album_id) {

    if(album_id < 0) return;

    Album album;
    CDatabaseConnector* db = CDatabaseConnector::getInstance();

    if( !db->getAlbumByID(album_id, album) ) return;

	start(album);
}

void GUI_Alternate_Covers::start(Album album) {

	le_search->setText(album.name + " " + Helper::get_major_artist(album.artists));
	_target_filename = CoverLocation::get_cover_location(album);
    _cl_alternative = new CoverLookupAlternative(this, album, 10);

	connect_and_start();
}


void GUI_Alternate_Covers::start(QString album_name, QString artist_name) {

	le_search->setText(album_name + " " + artist_name);
	_target_filename = CoverLocation::get_cover_location(album_name, artist_name);
    _cl_alternative = new CoverLookupAlternative(this, album_name, artist_name, 10);

	connect_and_start();
}

void GUI_Alternate_Covers::start(Artist artist) {

	le_search->setText(artist.name);
	_target_filename = CoverLocation::get_cover_location(artist);
    _cl_alternative = new CoverLookupAlternative(this, artist, 10);

	connect_and_start();

}


void GUI_Alternate_Covers::start(QString artist_name) {

	le_search->setText(artist_name);
	_target_filename = CoverLocation::get_cover_location(artist_name);
	_cl_alternative = new CoverLookupAlternative(this, artist_name, 10);

	connect_and_start();
}


CoverLocation GUI_Alternate_Covers::get_target_filename(){
	return _target_filename;
}


void GUI_Alternate_Covers::search_button_pressed() {

	if(_is_searching && _cl_alternative){
		_cl_alternative->stop();
		return;
	}

	_cl_alternative = new CoverLookupAlternative(this, le_search->text(), 10);
	connect_and_start();

}


void GUI_Alternate_Covers::save_button_pressed() {

	if(_cur_idx == -1) return;

	RowColumn rc = _model->cvt_2_row_col(_cur_idx);

	QModelIndex idx = _model->index(rc.row, rc.col);

	if(!idx.isValid()) {
		qDebug() << "index not valid";
		return;
	}

	QString src_filename = _model->data(idx, Qt::WhatsThisRole).toString().split(',')[0];
	QFile file(src_filename);

	if(!file.exists()) {
		QMessageBox::warning(this, tr("Information"), tr("This cover does not exist") );
		return;
	}

	QImage img(src_filename);
	img.save(_target_filename.cover_path);

	emit sig_cover_changed(true);
}


void GUI_Alternate_Covers::cancel_button_pressed() {

    hide();
    close();

	if(_cl_alternative) {
		_cl_alternative->stop();
	}

	delete_all_files();
}



void GUI_Alternate_Covers::cl_new_cover(const CoverLocation& cl) {

    int idx = 0;
	QModelIndex model_idx;
	QVariant var;


	_filelist << cl;

	QString dir, filename;
	Helper::split_filename(cl.cover_path, dir, filename);

	QString substr = filename.left(2);
    if(substr.endsWith("_")) {
		idx = filename.left(1).toInt();
    }

	RowColumn rc = _model->cvt_2_row_col(idx);
	bool valid = _model->is_valid(rc.row, rc.col);
	btn_save->setEnabled(valid);


	model_idx = _model->index(rc.row, rc.col);
	var.setValue(cl);


	_model->setData(model_idx, var);

}


void GUI_Alternate_Covers::cl_finished(bool b) {

	_is_searching = false;

	btn_search->setText(tr("Search"));
}



void GUI_Alternate_Covers::cover_pressed(const QModelIndex& idx) {

	_cur_idx = _model->cvt_2_idx(idx.row(), idx.column());

	bool valid = _model->is_valid(idx.row(), idx.column());

	btn_save->setEnabled(valid);

}


void GUI_Alternate_Covers::update_model() {
    _model->removeRows(0, _model->rowCount());
	_model->insertRows(0, _model->rowCount());

	for(int y=0; y<_model->rowCount(); y++) {
		for(int x=0; x<_model->columnCount(); x++) {

            QModelIndex idx = _model->index(y,x);
			QString sayonara_logo = Helper::getIconPath() + "logo.png";
			_model->setData(idx, sayonara_logo, Qt::EditRole);

        }
    }
}




void GUI_Alternate_Covers::open_file_dialog() {

	QDir dir( CSettingsStorage::getInstance()->getLibraryPath() );

    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);


	foreach (QString f, dir.entryList()) {
		QFile::remove(dir.absoluteFilePath(f));
	}

    QStringList lst = QFileDialog::getOpenFileNames(this,
                                  tr("Open image files"),
                                  _last_path,
                                  filters.join(" "));

	if(lst.size() == 0) return;

    update_model();

	int idx = 0;
    foreach(QString path, lst) {

		RowColumn rc = _model->cvt_2_row_col( idx );

		QModelIndex model_idx = _model->index(rc.row, rc.col);

		_model->setData(model_idx, path);

        idx ++;
    }
}


void GUI_Alternate_Covers::delete_all_files() {

	foreach(CoverLocation cl, _filelist) {
		if(!cl.valid) continue;
		QFile f(cl.cover_path);
		f.remove();
	}

	_filelist.clear();
}


void GUI_Alternate_Covers::closeEvent(QCloseEvent *e) {

	delete_all_files();
	QDialog::closeEvent(e);
}
