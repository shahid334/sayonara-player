/* GUI_Playlist.cpp */

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
 * GUI_Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */


#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/CDirectoryReader.h"

#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/playlist/PlaylistItemModel.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"

#include "StreamPlugins/LastFM/LastFM.h"
#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QFileDialog>
#include <QScrollBar>
#include <QMacStyle>
#include <QAbstractListModel>
#include <QStyleFactory>
#include <QPaintDevice>
#include <QPainter>
#include <QMessageBox>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QUrl>
#include <QFileInfo>


#include <iostream>

class QPaintEngine;
class QPaintDevice;


using namespace std;
// CTOR
GUI_Playlist::GUI_Playlist(QWidget *parent, GUI_InfoDialog* dialog) :
	QWidget(parent)
	 {

	_parent = parent;

	ui = new Ui::Playlist_Window();
	ui->setupUi(this);
	initGUI();


	CSettingsStorage* settings = CSettingsStorage::getInstance();
	bool small_playlist_items = settings->getShowSmallPlaylist();
    ui->listView->show_big_items(!small_playlist_items);

	_info_dialog = dialog;

    _radio_active = RADIO_OFF;

    _playlist_mode = settings->getPlaylistMode();

	ui->btn_append->setChecked(_playlist_mode.append);
	ui->btn_repAll->setChecked(_playlist_mode.repAll);
    ui->btn_dynamic->setChecked(_playlist_mode.dynamic);
	ui->btn_shuffle->setChecked(_playlist_mode.shuffle);
	ui->btn_numbers->setChecked(settings->getPlaylistNumbers());
	ui->btn_import->setVisible(false);

	check_dynamic_play_button();
	setAcceptDrops(true);

    connect(ui->btn_clear, SIGNAL(clicked()), this, SLOT(clear_playlist_slot()));

	connect(ui->btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));

    connect(ui->listView, SIGNAL(sig_metadata_dropped(const MetaDataList&,int)), this, SLOT(metadata_dropped(const MetaDataList&,int)));
    connect(ui->listView, SIGNAL(sig_rows_removed(const QList<int>&, bool)), this, SLOT(rows_removed(const QList<int>&, bool)));

    connect(ui->listView, SIGNAL(sig_edit_clicked()), this, SLOT(psl_edit_tracks()));
    connect(ui->listView, SIGNAL(sig_info_clicked()), this, SLOT(psl_info_tracks()));

    connect(ui->listView, SIGNAL(sig_selection_changed(MetaDataList&)), this, SLOT(selection_changed(MetaDataList&)));
    connect(ui->listView, SIGNAL(sig_double_clicked(int)), this, SLOT(double_clicked(int)));

    //connect(ui->btn_import, SIGNAL(clicked()), this, SLOT(import_button_clicked()));
	connect(ui->btn_numbers, SIGNAL(toggled(bool)), this, SLOT(btn_numbers_changed(bool)));
}


// DTOR
GUI_Playlist::~GUI_Playlist() {
	delete ui;
}

void GUI_Playlist::changeEvent(QEvent* e){
    e->accept();
}

void GUI_Playlist::resizeEvent(QResizeEvent* e){

    e->accept();
    this->ui->listView->update();
    this->ui->listView->reset();
}

void GUI_Playlist::language_changed(){

    this->ui->retranslateUi(this);
    set_total_time_label();
    check_dynamic_play_button();
}

// initialize gui
// maybe the button state (pressed/unpressed) should be loaded from db here
void GUI_Playlist::initGUI(){

	QString icon_path = Helper::getIconPath();

	ui->btn_append->setIcon(QIcon(icon_path + "append.png"));
	ui->btn_rep1->setIcon(QIcon(icon_path + "rep1.png"));
	ui->btn_rep1->setVisible(false);
	ui->btn_repAll->setIcon(QIcon(icon_path + "repAll.png"));
	ui->btn_dynamic->setIcon(QIcon(icon_path + "dynamic.png"));
	ui->btn_shuffle->setIcon(QIcon(icon_path + "shuffle.png"));
	ui->btn_clear->setIcon(QIcon(icon_path + "broom.png"));
	ui->btn_import->setIcon(QIcon(icon_path + "import.png"));
	ui->btn_numbers->setIcon(QIcon(icon_path + "numbers.png"));
}


void GUI_Playlist::library_path_changed(QString path){
    Q_UNUSED(path);
    check_dynamic_play_button();
}


void GUI_Playlist::check_dynamic_play_button(){

	QString libraryPath = CSettingsStorage::getInstance()->getLibraryPath();

	if(libraryPath.size() == 0 || !QFile::exists(libraryPath)){
	        ui->btn_dynamic->setToolTip(tr("Please set library path first"));
	}

	else{
        	ui->btn_dynamic->setToolTip(tr("Dynamic playing"));
	}
}

// Slot: comes from listview
void GUI_Playlist::metadata_dropped(const MetaDataList& v_md, int row){
    emit dropped_tracks(v_md, row);
}

// SLOT: fill all tracks in v_metadata into playlist
void GUI_Playlist::fillPlaylist(MetaDataList& v_metadata, int cur_play_idx, int radio_mode){

    ui->listView->fill(v_metadata, cur_play_idx);
    _total_msecs = 0;
    _radio_active = radio_mode;
    set_radio_active(radio_mode);

    foreach(MetaData md, v_metadata){
        _total_msecs += md.length_ms;
    }

    set_total_time_label();
}

// private SLOT: clear button pressed
void GUI_Playlist::clear_playlist_slot(){

    _total_msecs = 0;
    ui->lab_totalTime->setText(tr("Playlist empty"));
	ui->btn_import->setVisible(false);
    ui->listView->clear();

	emit clear_playlist();
}


// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::selection_changed(MetaDataList& v_md){

    _info_dialog->setMetaData(v_md);

    this->_info_dialog->set_tag_edit_visible(_radio_active == RADIO_OFF);
}


void GUI_Playlist::double_clicked(int row ){
    emit selected_row_changed(row);
}

void GUI_Playlist::track_changed(int row){
    ui->listView->set_current_track(row);
}


// private SLOT: rep1, repAll, shuffle or append has changed
void GUI_Playlist::playlist_mode_changed_slot(){

	this->parentWidget()->setFocus();

	_playlist_mode.rep1 = ui->btn_rep1->isChecked();
	_playlist_mode.repAll = ui->btn_repAll->isChecked();
	_playlist_mode.shuffle = ui->btn_shuffle->isChecked();
	_playlist_mode.append = ui->btn_append->isChecked();
	_playlist_mode.dynamic = ui->btn_dynamic->isChecked();

	emit playlist_mode_changed(_playlist_mode);
	emit save_playlist("bla");
}


void GUI_Playlist::psl_edit_tracks(){
	if(!_info_dialog) return;
	_info_dialog->setMode(INFO_MODE_TRACKS);
	_info_dialog->show(TAB_EDIT);
}

void GUI_Playlist::psl_info_tracks(){
	if(!_info_dialog) return;
	_info_dialog->setMode(INFO_MODE_TRACKS);
	_info_dialog->show(TAB_INFO);
}




void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event){
    event->accept();
}


void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event){
    event->accept();
}

void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event){
    if(event->pos().y() < this->ui->listView->y()) this->ui->listView->scrollUp();
    else if(event->pos().y() > this->ui->listView->y() + this->ui->listView->height()) this->ui->listView->scrollDown();

}


void GUI_Playlist::dropEvent(QDropEvent* event){
    this->ui->listView->dropEventFromOutside(event);
}



void GUI_Playlist::set_total_time_label(){

    QString text = "";

	if(_radio_active == RADIO_STATION){

        ui->lab_totalTime->setText(tr("Radio"));
		return;
	}

	ui->lab_totalTime->setContentsMargins(0, 2, 0, 2);

    int n_rows = ui->listView->get_num_rows();
    QString playlist_string = text + QString::number(n_rows);

    if(n_rows == 1)	playlist_string += tr(" Track - ");
    else playlist_string += tr(" Tracks - ");

    playlist_string += Helper::cvtMsecs2TitleLengthString(_total_msecs, false);

	ui->lab_totalTime->setText(playlist_string);
}



void GUI_Playlist::set_radio_active(int radio){

	_radio_active = radio;


	ui->btn_append->setVisible(radio == RADIO_OFF);
	ui->btn_dynamic->setVisible(radio == RADIO_OFF);
	ui->btn_repAll->setVisible(radio == RADIO_OFF);
	ui->btn_shuffle->setVisible(radio == RADIO_OFF);

    if(radio != RADIO_OFF){

        ui->listView->set_context_menu_actions(ENTRY_INFO);

    }

    else{

        ui->listView->set_context_menu_actions(ENTRY_INFO | ENTRY_REMOVE | ENTRY_EDIT);

    }

    ui->listView->set_drag_enabled(radio != RADIO_LFM);
}



void GUI_Playlist::psl_show_small_playlist_items(bool small_playlist_items){

    ui->listView->show_big_items(!small_playlist_items);
}


void GUI_Playlist::btn_numbers_changed(bool b){
	this->parentWidget()->setFocus();
	CSettingsStorage::getInstance()->setPlaylistNumbers(b);
	ui->listView->reset();
}


void GUI_Playlist::rows_removed(const QList<int>& lst, bool select_next_row){
    emit sig_rows_removed(lst, select_next_row);
}
