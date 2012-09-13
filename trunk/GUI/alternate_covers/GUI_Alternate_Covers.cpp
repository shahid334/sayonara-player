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
#include "CoverLookup/CoverLookup.h"

#include <ui_GUI_Alternate_Covers.h>
#include <QPixmap>
#include <QString>
#include <QDebug>
#include <QFileSystemWatcher>



GUI_Alternate_Covers::GUI_Alternate_Covers() {



	this->ui = new Ui::GUI_Alternate_Covers();
	this->ui->setupUi(this);

	this->_model = new AlternateCoverItemModel();
	this->_delegate = new AlternateCoverItemDelegate();

	QString path = Helper::getSayonaraPath() + QDir::separator() + "tmp";
	QDir dir(path);
	if (!dir.exists()) {
		dir.mkpath(".");
	}

	QStringList paths;
	paths << path;

	_watcher= new QFileSystemWatcher(paths);

	this->ui->tv_images->setModel(_model);
	this->ui->tv_images->setItemDelegate(_delegate);

	_class_name = "Alternate Covers";





	_cov_lookup = new CoverLookup(_class_name);




	connect(this->ui->btn_save, SIGNAL(pressed()), this, SLOT(save_button_pressed()));
	connect(this->ui->btn_cancel, SIGNAL(pressed()), this, SLOT(cancel_button_pressed()));
	connect(this->ui->btn_search_album, SIGNAL(pressed()), this, SLOT(search_button_pressed()));
	connect(this->ui->tv_images, SIGNAL(pressed(const QModelIndex& )), this, SLOT(cover_pressed(const QModelIndex& )));
	connect(this->_cov_lookup, SIGNAL(sig_multi_covers_found(QString)), this, SLOT(covers_there(QString)));
	connect(this->_watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));
	connect(this->_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(tmp_folder_changed(const QString&)));



}


GUI_Alternate_Covers::~GUI_Alternate_Covers() {


}


void GUI_Alternate_Covers::start(const QString& artist, const QString& album){

	this->ui->le_search->setText(artist + " " + album);
	this->show();


}


void GUI_Alternate_Covers::fill_covers(){

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



void GUI_Alternate_Covers::search_button_pressed(){

	QString searchstring = this->ui->le_search->text();
	if(searchstring.size() < 3) return;

	QStringList filters;
	filters << "*.jpg";
	filters << "*.png";
	filters << "*.gif";
	QDir dir(Helper::getSayonaraPath() + QDir::separator() + "tmp");
	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);
	QStringList file_list = dir.entryList();
	foreach(QString filename, file_list){

		QFile file(dir.absoluteFilePath(filename));
		file.remove();
	}

	_cov_lookup->search_images_by_searchstring(searchstring, 10);

}

void GUI_Alternate_Covers::cover_pressed(const QModelIndex& idx){
	int row = idx.row();
	int col = idx.column();

	_cur_idx = row * 5 + col;
}


void GUI_Alternate_Covers::covers_there(QString classname){

	if(classname != _class_name) return;

	QStringList filters;
	QStringList tmp;

	filters << "*.jpg";
	filters << "*.png";
	filters << "*.gif";

	QDir dir(Helper::getSayonaraPath() + QDir::separator() + "tmp");

	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);

	tmp = dir.entryList();
	int n_rows= tmp.size() / _model->columnCount() + 1;

	if (tmp.size() % _model->columnCount() == 0) n_rows--;
	_model->removeRows(0, _model-> rowCount());

	_model->insertRows(0, n_rows);

	int row = 0;
	int col = 0;
	int i=0;
	foreach (QString f, tmp) {
		col = i % _model->columnCount();
		row = i / _model->columnCount();

		this->ui->tv_images->setColumnWidth(col, 100);
		this->ui->tv_images->setRowHeight(row,100);

	    QString filename = dir.absoluteFilePath(f);

	    _model->setData(_model->index(row, col), filename, Qt::EditRole);
	    i++;
	}
}


void GUI_Alternate_Covers::tmp_folder_changed(const QString& directory){

	qDebug() << "file system changed ";
	QStringList filters;
		QStringList tmp;

		filters << "*.jpg";
		filters << "*.png";
		filters << "*.gif";

		QDir dir(directory);

		dir.setFilter(QDir::Files);
		dir.setNameFilters(filters);

		tmp = dir.entryList();
		int n_rows= tmp.size() / _model->columnCount() + 1;

		if (tmp.size() % _model->columnCount() == 0) n_rows--;
		_model->removeRows(0, _model-> rowCount());

		_model->insertRows(0, n_rows);

		int row = 0;
		int col = 0;
		int i=0;
		foreach (QString f, tmp) {
			col = i % _model->columnCount();
			row = i / _model->columnCount();

			this->ui->tv_images->setColumnWidth(col, 100);
			this->ui->tv_images->setRowHeight(row,100);

		    QString filename = dir.absoluteFilePath(f);

		    _model->setData(_model->index(row, col), filename, Qt::EditRole);
		    i++;
		}
}





