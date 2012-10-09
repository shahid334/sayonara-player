/* GUI_PlaylistChooser.cpp */

/* Copyright (C) 2012  Lucio Carreras
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


#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/globals.h"

#include <vector>

#include <QWidget>
#include <QDockWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>

#include <ui_GUI_PlaylistChooser.h>

using namespace std;


GUI_PlaylistChooser::GUI_PlaylistChooser(QWidget* parent) : QDockWidget(parent) {

	_cur_idx = -1;


	this->ui = new Ui::PlaylistChooser();
	this->ui->setupUi(this);

	this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->ui->btn_save_as->setIcon(QIcon(Helper::getIconPath() + "save_as.png"));
	this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "delete.png"));

	this->ui->btn_save->setToolTip("Save");
	this->ui->btn_save_as->setToolTip("Save as");
	this->ui->btn_delete->setToolTip("Delete");


	connect(this->ui->btn_apply, SIGNAL(pressed()), this, SLOT(apply_button_pressed()));

	connect(this->ui->btn_save, SIGNAL(pressed()), this, SLOT(save_button_pressed()));
	connect(this->ui->btn_save_as, SIGNAL(pressed()), this, SLOT(save_as_button_pressed()));
	connect(this->ui->btn_delete, SIGNAL(pressed()), this, SLOT(delete_button_pressed()));

    hide();

}

GUI_PlaylistChooser::~GUI_PlaylistChooser() {

}



void GUI_PlaylistChooser::all_playlists_fetched(QMap<int, QString>& mapping){

	this->ui->combo_playlistchooser->clear();
	this->ui->combo_playlistchooser->addItem("<empty>", -1);

	QList<int> keys = mapping.keys();
	foreach(int key, keys){
		QString name = mapping.value(key);
		this->ui->combo_playlistchooser->addItem(name, key);
	}

	if(_cur_idx < this->ui->combo_playlistchooser->count() && _cur_idx >= 0)
		this->ui->combo_playlistchooser->setCurrentIndex(_cur_idx);

	connect(this->ui->combo_playlistchooser, SIGNAL(currentIndexChanged(int)), this, SLOT(playlist_selected(int)));
}



void GUI_PlaylistChooser::save_button_pressed(){

	if(_cur_idx >= this->ui->combo_playlistchooser->count() || _cur_idx < 0) return;

	int val = this->ui->combo_playlistchooser->itemData(_cur_idx).toInt();
	if(val >= 0 && _cur_idx > 0 ){
		QMessageBox dialog;

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Warning);
		dialog.setText("<b>Overwrite?</b>");
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		dialog.setDefaultButton(QMessageBox::No);
		dialog.setInformativeText("Are you sure?");
		int answer = dialog.exec();
		dialog.close();

		if(answer == QMessageBox::Yes)
			emit sig_save_playlist(val);
	}

	else if(val == -1)
		save_as_button_pressed();

	else return;
}



void GUI_PlaylistChooser::save_as_button_pressed(){

	bool ok;
	QString pl_name = QInputDialog::getText(0, "Please choose a name", "Playlist name", QLineEdit::Normal, "", &ok);

	if(pl_name.size() > 0 && ok)
		emit sig_save_playlist(pl_name);
}


void GUI_PlaylistChooser::delete_button_pressed(){
	QMessageBox dialog;

	dialog.setFocus();
	dialog.setIcon(QMessageBox::Warning);
	dialog.setText("<b>Delete?</b>");
	dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	dialog.setDefaultButton(QMessageBox::No);
	dialog.setInformativeText("Are you sure?");
	int answer = dialog.exec();
	dialog.close();

	if(_cur_idx < this->ui->combo_playlistchooser->count() && _cur_idx != -1){
		int val = this->ui->combo_playlistchooser->itemData(_cur_idx).toInt();
		if(val >= 0 && answer == QMessageBox::Yes)
			emit sig_delete_playlist(val);
	}

}


void GUI_PlaylistChooser::playlist_selected(int idx){

	_cur_idx = idx;
	if(_cur_idx >= this->ui->combo_playlistchooser->count() || _cur_idx == -1) return;

	int val = this->ui->combo_playlistchooser->itemData(idx).toInt();
	bool val_bigger_zero = (val >= 0);

	this->ui->btn_delete->setEnabled(val_bigger_zero);
	this->ui->btn_save->setEnabled(val_bigger_zero);
	this->ui->btn_apply->setEnabled(val_bigger_zero);

	if(val_bigger_zero)
		emit sig_playlist_chosen(val);

	else
		emit sig_clear_playlist();
}


void GUI_PlaylistChooser::apply_button_pressed(){

	if(_cur_idx >= this->ui->combo_playlistchooser->count() || _cur_idx == -1) return;
	int val = this->ui->combo_playlistchooser->itemData(_cur_idx).toInt();

	if(val >= 0)
		emit sig_playlist_chosen(val);
}


void GUI_PlaylistChooser::closeEvent ( QCloseEvent * event ){

    event->ignore();
    hide();
    close();
	emit sig_closed();
}

