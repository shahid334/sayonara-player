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
#include "HelperStructs/id3.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CustomMimeData.h"
#include "HelperStructs/CDirectoryReader.h"

#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/playlist/PlaylistItemModel.h"
#include "GUI/playlist/PlaylistItemDelegate.h"
#include "GUI/playlist/PlaylistItemDelegateSmall.h"
#include "GUI/playlist/PlaylistItemDelegateInterface.h"
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

	_info_dialog = dialog;

	_pli_model = new PlaylistItemModel();
	_pli_delegate = create_item_delegate(small_playlist_items);

	_inner_drag_drop = false;
    _radio_active = RADIO_OFF;

    _playlist_mode = settings->getPlaylistMode();

	ui->btn_append->setChecked(_playlist_mode.append);
	ui->btn_repAll->setChecked(_playlist_mode.repAll);
    ui->btn_dynamic->setChecked(_playlist_mode.dynamic);
	ui->btn_shuffle->setChecked(_playlist_mode.shuffle);
	ui->btn_numbers->setChecked(settings->getPlaylistNumbers());
	ui->btn_import->setVisible(false);

	check_dynamic_play_button();

	ui->listView->setDragEnabled(true);
	ui->listView->setModel(_pli_model);
	ui->listView->setItemDelegate(_pli_delegate);
	ui->listView->setSelectionRectVisible(true);
	ui->listView->setAlternatingRowColors(true);
	ui->listView->setMovement(QListView::Free);

	setAcceptDrops(true);

	int style = settings->getPlayerStyle();
	bool dark = (style == 1);
	change_skin(dark);

	init_menues();

    connect(ui->btn_clear, SIGNAL(clicked()), this, SLOT(clear_playlist_slot()));
	//this->connect(ui->btn_dummy, SIGNAL(released()), this, SLOT(dummy_pressed()));

	connect(ui->btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	connect(ui->btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));

	connect(ui->listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(pressed(const QModelIndex&)));
	connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(double_clicked(const QModelIndex &)));
	connect(ui->listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(released(const QModelIndex &)));
	connect(ui->listView, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(psl_show_context_menu(const QPoint&)));

	connect(ui->btn_import, SIGNAL(clicked()), this, SLOT(import_button_clicked()));
	connect(ui->btn_numbers, SIGNAL(toggled(bool)), this, SLOT(btn_numbers_changed(bool)));

	// we need a reason for refreshing the list
	QStringList empty_list;
	emit sound_files_dropped(empty_list);
}


// DTOR
GUI_Playlist::~GUI_Playlist() {
	delete _pli_delegate;
	delete _pli_model;

	delete ui;
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


void GUI_Playlist::init_menues(){

	_right_click_menu = new QMenu(this);
	_info_action = new QAction(QIcon(Helper::getIconPath() + "info.png"), "Info", this);
	_edit_action = new QAction(QIcon(Helper::getIconPath() + "lyrics.png"), "Edit", this);

	_right_click_menu->addAction(_info_action);
    _right_click_menu->addAction(_edit_action);
}



void GUI_Playlist::library_path_changed(QString path){
	Q_UNUSED(path);
	check_dynamic_play_button();
}

void GUI_Playlist::check_dynamic_play_button(){

	QString libraryPath = CSettingsStorage::getInstance()->getLibraryPath();

	if(libraryPath.size() == 0 || !QFile::exists(libraryPath)){
		ui->btn_dynamic->setToolTip("Please set library path first");
	}


	else{
		ui->btn_dynamic->setToolTip("Dynamic playing");
	}
}

void GUI_Playlist::dummy_pressed(){

}


// SLOT: switch between dark & light skin
void GUI_Playlist::change_skin(bool dark){
	Q_UNUSED(dark);
}


// SLOT: fill all tracks in v_metadata into playlist
// the current track should be highlighted
void GUI_Playlist::fillPlaylist(MetaDataList& v_metadata, int cur_play_idx, int radio_mode){

    _radio_active = radio_mode;
    _pli_model->removeRows(0, _pli_model->rowCount());

    if(v_metadata.size() == 0) return;
    _pli_model->insertRows(0, v_metadata.size());

    ui->btn_import->setVisible(false);
    set_radio_active(radio_mode);

    int idx = 0;
    qint64 total_msecs = 0;
    foreach(MetaData md, v_metadata){

    	QModelIndex model_idx = _pli_model->index(idx, 0);
		total_msecs += md.length_ms;

        if(md.is_extern)
			ui->btn_import->setVisible(true);

        md.pl_selected = false;
        md.pl_playing = (cur_play_idx == idx);

        _pli_model->setData(model_idx, md.toVariant(), Qt::EditRole);

        idx++;
	}

	set_total_time_label(total_msecs);
}

// private SLOT: clear button pressed
void GUI_Playlist::clear_playlist_slot(){

    ui->lab_totalTime->setText("Playlist empty");
	ui->btn_import->setVisible(false);

	_pli_model->removeRows(0, _pli_model->rowCount());
	_cur_selected_rows.clear();

	emit clear_playlist();
}


// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::pressed(const QModelIndex& index){

	if(!index.isValid() || index.row() < 0 || index.row() >= _pli_model->rowCount()) return;


	QModelIndexList idx_list = ui->listView->selectionModel()->selectedRows();
	MetaDataList v_md;

    // set all tracks not pressed
    foreach(int row, _cur_selected_rows){
        QModelIndex idx = _pli_model->index(row, 0);
        QVariant mdvariant = this->_pli_model->data(idx, Qt::WhatsThisRole);

        MetaData md;
        if(!MetaData::fromVariant(mdvariant, md)) continue;

        md.pl_selected = false;
        this->_pli_model->setData(idx, md.toVariant(), Qt::EditRole);
    }

    _cur_selected_rows.clear();

    // search for cur selected rows
    foreach(QModelIndex idx, idx_list){

		QVariant mdvariant = this->_pli_model->data(idx, Qt::WhatsThisRole);
		MetaData md;

		if(!MetaData::fromVariant(mdvariant, md)) continue;

        md.pl_selected = true;
        v_md.push_back(md);

        this->_pli_model->setData(idx, md.toVariant(), Qt::EditRole);
        if(_radio_active == RADIO_OFF){
            _cur_selected_rows.push_back(idx.row());
        }
	}

    if(_info_dialog){
        _info_dialog->setMetaData(v_md);
    }

    if(_radio_active == RADIO_OFF && v_md.size() > 0){

        CustomMimeData* mime = new CustomMimeData();
        mime->setText("tracks");
        mime->setMetaData(v_md);

        ui->listView->set_mime_data(mime);

        if(_cur_selected_rows.contains( index.row() ))
            _inner_drag_drop = true;

        else _inner_drag_drop = false;
    }

    else _inner_drag_drop = false;
}



void GUI_Playlist::released(const QModelIndex& index){

	_inner_drag_drop = false;

	if(!index.isValid() || index.row() < 0 || index.row() >= _pli_model->rowCount()) return;

	ui->listView->set_mime_data(NULL);
	_inner_drag_drop = false;
}


// private SLOT: track chosen (change track)
void GUI_Playlist::double_clicked(const QModelIndex & index){

	if(!index.isValid() || index.row() < 0 || index.row() >= _pli_model->rowCount()) return;

	clear_drag_lines(index.row());

	for(int i=0; i<index.model()->rowCount(); i++){

		QModelIndex tmp_idx = index.model()->index(i, 0);
		QVariant mdvariant = index.model()->data(tmp_idx, Qt::WhatsThisRole);
		MetaData md;
        if(!MetaData::fromVariant(mdvariant, md)) continue;

        if(i == index.row()){
            md.pl_selected = true;
            if(!md.is_disabled){
                md.pl_playing = true;
                emit selected_row_changed(i);
            }
        }

        else md.pl_selected = false;

		_pli_model->setData(tmp_idx, md.toVariant(), Qt::EditRole);
	}
}



// SLOT: if the current track has has changed
// by playlist
void GUI_Playlist::track_changed(int new_row){

    if(new_row < 0) return;
    if(new_row > _pli_model->rowCount()) return;

	QModelIndex index = _pli_model->index(new_row, 0);

    for(int i=0; i<_pli_model->rowCount(); i++){

		QModelIndex tmp_idx = _pli_model->index(i, 0);

        MetaData md;
        QVariant mdvariant = index.model()->data(tmp_idx, Qt::WhatsThisRole);
        if(!MetaData::fromVariant(mdvariant, md)) continue;

		md.pl_playing = (i == index.row());
		_pli_model->setData(tmp_idx, md.toVariant(), Qt::EditRole);
    }

	ui->listView->scrollTo(index);
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

void GUI_Playlist::psl_delete_tracks(){

}


void GUI_Playlist::psl_show_context_menu(const QPoint& p){

	connect(_edit_action, SIGNAL(triggered()), this, SLOT(psl_edit_tracks()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(psl_info_tracks()));
    _edit_action->setEnabled(_radio_active == RADIO_OFF);

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(psl_edit_tracks()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(psl_info_tracks()));
}





// remove the black line under the titles
void GUI_Playlist::clear_drag_lines(int row){

	for(int i=row-3; i<=row+3; i++){

		QModelIndex idx = this->_pli_model->index(i, 0);
		if(!idx.isValid() || idx.row() < 0 || idx.row() >= _pli_model->rowCount())
			continue;

		QVariant mdVariant = _pli_model->data(this->_pli_model->index(i, 0), Qt::WhatsThisRole);
		MetaData md;
		if(MetaData::fromVariant(mdVariant, md)){

			md.pl_dragged = false;
			_pli_model->setData(this->_pli_model->index(i, 0), md.toVariant(), Qt::EditRole);
		}

	}
}


// we start the drag action, all lines has to be cleared
void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event){

	Q_UNUSED(event);
	int row = ui->listView->indexAt(_last_known_drag_pos).row();
	clear_drag_lines(row);
}

// the drag comes, if there's data --> accept it
void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event){


    if(event->mimeData() != NULL )
		event->acceptProposedAction();
}

// we move the title
// scroll, if neccessary
// paint line
void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event){



	if( !event->mimeData() )  {
		qDebug() << "mime data not available";
		return;
	}

	QPoint pos = event->pos();
	_last_known_drag_pos = pos;

	int scrollbar_pos = ui->listView->verticalScrollBar()->sliderPosition();
	int y_event =  event->pos().y();
	int y_playlist = ui->listView->y();


	// scroll up
	if(y_event <= y_playlist+10){

		if(scrollbar_pos >= 1)
			ui->listView->scrollTo(this->_pli_model->index(scrollbar_pos-1, 0));
	}

	// scroll down
	else if(y_event >= y_playlist + ui->listView->height()-10){
		int num_steps = ui->listView->height() / _pli_delegate->rowHeight();
		QModelIndex idx = _pli_model->index(scrollbar_pos+num_steps, 0);

		if(idx.isValid()) ui->listView->scrollTo(idx);
		if(idx.row() == -1) ui->listView->scrollToBottom();
	}


	int row = ui->listView->indexAt(pos).row();

	if(row <= -1) row = _pli_model->rowCount()-1;
	else if(row > 0) row--;

	if( (_cur_selected_rows.contains(row) && _inner_drag_drop) ||
		row >= _pli_model->rowCount()){
			return;
	}

	clear_drag_lines(row);

	if(pos.y() <= y_playlist + 2){
		return;
	}

	QModelIndex cur_idx = _pli_model->index(row, 0);
	if( !cur_idx.isValid() ){

		return;
	}

	// paint line

	QVariant mdVariant = _pli_model->data(cur_idx, Qt::WhatsThisRole);
	MetaData md;
	if(!MetaData::fromVariant(mdVariant, md)) return;

	md.pl_dragged = true;
	_pli_model->setData(cur_idx, md.toVariant(), Qt::EditRole);

}

// finally drop it
void GUI_Playlist::dropEvent(QDropEvent* event){

	if(!event->mimeData()) return;

	// where did i drop?
	QPoint pos = event->pos();
	QModelIndex idx = ui->listView->indexAt(pos);
	int row = idx.row();
	if(row == -1){
		row = this->_pli_model->rowCount();
	}

	// remove line
	clear_drag_lines(row);


	if(_inner_drag_drop){
		_inner_drag_drop = false;
		if( _cur_selected_rows.contains(row-1) ){
			event->ignore();
			return;
		}

		if(_cur_selected_rows.first() < row ) {
			row -= _cur_selected_rows.size();
		}

		remove_cur_selected_rows();
	}

	const CustomMimeData* d = (const CustomMimeData*) event->mimeData();

	MetaDataList v_metadata;

	// extern
	if( d->hasUrls() ){
        qDebug() << "has urls";

		QStringList filelist;
		foreach(QUrl url, d->urls()){
            qDebug() << "Url: " << url;
				QString path;
				QString url_str = url.toString();
				path =  url_str.right(url_str.length() - 7).trimmed();
				path = path.replace("%20", " ");
				filelist.push_back(path);

		} // end foreach

		CDirectoryReader reader;
		reader.setFilter(Helper::get_soundfile_extensions());

		reader.getMetadataFromFileList(filelist, v_metadata);

		if(v_metadata.size() == 0) return;

		if(_radio_active == RADIO_OFF)
			emit dropped_tracks(v_metadata, row);
		else
			emit dropped_tracks(v_metadata, 0);

		return;
	}


    else if(d->hasHtml()){
        qDebug() << d->html();
    }

    else if(d->hasImage()){
        qDebug() << "image";
    }


    else if(d->hasText() && d->hasMetaData()){

        uint sz = d->getMetaData(v_metadata);
        if(sz == 0) return;

        if(row == -1) row = _pli_model->rowCount();

		if(_radio_active == RADIO_OFF)
			emit dropped_tracks(v_metadata, row);
		else
            emit dropped_tracks(v_metadata, 0);

    }

    else if(d->hasText()){
        qDebug() << d->text();
    }


    else {

    }
}


void GUI_Playlist::set_total_time_label(qint64 total_msecs){

    QString text = "";

	if(_radio_active == RADIO_STATION){
		ui->lab_totalTime->setText("Radio");
		return;
	}

	ui->lab_totalTime->setContentsMargins(0, 2, 0, 2);

    QString playlist_string = text + QString::number(this->_pli_model->rowCount());

    int n_rows = _pli_model->rowCount();
	if(n_rows == 1)	playlist_string += " Track - ";
	else playlist_string += " Tracks - ";

	playlist_string += Helper::cvtMsecs2TitleLengthString(total_msecs, false);

	ui->lab_totalTime->setText(playlist_string);
}

void GUI_Playlist::remove_cur_selected_rows(){

	if(_pli_model->rowCount() > 1){

		emit rows_removed(_cur_selected_rows);
	}

	else
		clear_playlist_slot();

	_cur_selected_rows.clear();
}

void GUI_Playlist::keyPressEvent(QKeyEvent* e){

	QWidget::keyPressEvent(e);
	int key = e->key();

    if(key == Qt::Key_Delete && _cur_selected_rows.size() > 0){
        qDebug() << "cur selected rows = " << _cur_selected_rows.size();
        remove_cur_selected_rows();
    }
}


void GUI_Playlist::import_button_clicked(){
    emit sig_import_to_library(false);
}

void GUI_Playlist::import_result(bool success){

	ui->btn_import->setVisible(!success);
    if(success)
        QMessageBox::information(this, "Import files", "All files could be imported");
    else
        QMessageBox::warning(this, "Import files", QString("Sorry, but tracks could not be imported <br />") +
                             "Please use the import function of the file menu<br /> or move tracks to library and use 'Reload library'");
}


void GUI_Playlist::set_radio_active(int radio){

	_radio_active = radio;


	ui->btn_append->setVisible(radio == RADIO_OFF);
	ui->btn_dynamic->setVisible(radio == RADIO_OFF);
	ui->btn_repAll->setVisible(radio == RADIO_OFF);
	ui->btn_shuffle->setVisible(radio == RADIO_OFF);

    if(radio != RADIO_OFF){
        ui->listView->set_drag_enabled(false);
        this->_right_click_menu->removeAction(_edit_action);
        this->_info_dialog->set_tag_edit_visible(false);
    }

    else if(!_right_click_menu->actions().contains(_edit_action)){
        ui->listView->set_drag_enabled(true);
        this->_right_click_menu->addAction(_edit_action);
        this->_info_dialog->set_tag_edit_visible(true);
    }

    else
        this->_info_dialog->set_tag_edit_visible(true);

}


PlaylistItemDelegateInterface* GUI_Playlist::create_item_delegate(bool small_playlist_items){

	PlaylistItemDelegateInterface* delegate = 0;
	if(small_playlist_items)
		delegate = new PlaylistItemDelegateSmall(ui->listView);
	else
		delegate = new PlaylistItemDelegate(ui->listView);

	return delegate;
}


void GUI_Playlist::psl_show_small_playlist_items(bool small_playlist_items){

	_pli_delegate = create_item_delegate(small_playlist_items);

	ui->listView->setItemDelegate(_pli_delegate);
	ui->listView->reset();
}


void GUI_Playlist::btn_numbers_changed(bool b){
	this->parentWidget()->setFocus();
	CSettingsStorage::getInstance()->setPlaylistNumbers(b);
	ui->listView->reset();

}
