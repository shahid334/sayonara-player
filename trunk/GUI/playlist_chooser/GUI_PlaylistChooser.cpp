/*
 * GUIPlaylistChooser.cpp
 *
 *  Created on: Jan 2, 2012
 *      Author: luke
 */

#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

#include <vector>

#include <QWidget>
#include <QDockWidget>
#include <QInputDialog>
#include <QDebug>

#include <ui_GUI_PlaylistChooser.h>

using namespace std;


GUI_PlaylistChooser::GUI_PlaylistChooser(QWidget* parent) : QDockWidget(parent) {

	this->ui = new Ui::PlaylistChooser();
	this->ui->setupUi(this);

	this->ui->btn_save->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->ui->btn_save_as->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->ui->btn_delete->setIcon(QIcon(Helper::getIconPath() + "close.png"));


	connect(this->ui->btn_apply, SIGNAL(pressed()), this, SLOT(apply_button_pressed()));

	connect(this->ui->btn_save, SIGNAL(pressed()), this, SLOT(save_button_pressed()));
	connect(this->ui->btn_save_as, SIGNAL(pressed()), this, SLOT(save_as_button_pressed()));
	connect(this->ui->btn_delete, SIGNAL(pressed()), this, SLOT(delete_button_pressed()));
	connect(this->ui->combo_playlistchooser, SIGNAL(currentIndexChanged(int)), this, SLOT(playlist_selected(int)));

}

GUI_PlaylistChooser::~GUI_PlaylistChooser() {

}



void GUI_PlaylistChooser::all_playlists_fetched(QMap<int, QString>& mapping){
	int cur_idx = this->ui->combo_playlistchooser->currentIndex();

	this->ui->combo_playlistchooser->clear();
	this->ui->combo_playlistchooser->addItem("Please choose", -1);

	QList<int> keys = mapping.keys();
	foreach(int key, keys){
		QString name = mapping.value(key);
		this->ui->combo_playlistchooser->addItem(name, key);
	}

	this->ui->combo_playlistchooser->setCurrentIndex(cur_idx);
}



void GUI_PlaylistChooser::save_button_pressed(){

	int cur_idx = this->ui->combo_playlistchooser->currentIndex();

	int val = this->ui->combo_playlistchooser->itemData(cur_idx).toInt();
	if(val >= 0 && cur_idx >0 ){
		qDebug() << "Ui: save playlist";
		emit sig_save_playlist(val);
	}

	else save_as_button_pressed();
}



void GUI_PlaylistChooser::save_as_button_pressed(){

	bool ok;
	QString pl_name = QInputDialog::getText(0, "Please choose a name", "Playlist name", QLineEdit::Normal, "", &ok);

	if(pl_name.size() > 0 && ok) emit sig_save_playlist(pl_name);
}


void GUI_PlaylistChooser::delete_button_pressed(){
	int cur_idx = this->ui->combo_playlistchooser->currentIndex();
	int val = this->ui->combo_playlistchooser->itemData(cur_idx).toInt();
	if(val >= 0)
		emit sig_delete_playlist(val);

}


void GUI_PlaylistChooser::playlist_selected(int idx){

	int val = this->ui->combo_playlistchooser->itemData(idx).toInt();
	bool val_bigger_zero = (val >= 0);

	this->ui->btn_delete->setEnabled(val_bigger_zero);
	this->ui->btn_save->setEnabled(val_bigger_zero);
	this->ui->btn_apply->setEnabled(val_bigger_zero);

	if(val_bigger_zero)
			emit sig_playlist_chosen(val);
}


void GUI_PlaylistChooser::apply_button_pressed(){

	int cur_idx = this->ui->combo_playlistchooser->currentIndex();
	int val = this->ui->combo_playlistchooser->itemData(cur_idx).toInt();

	if(val >= 0)
		emit sig_playlist_chosen(val);

}



void GUI_PlaylistChooser::closeEvent(QCloseEvent* e){

	emit sig_closed();
	this->close();
}

