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
#include "LyricLookup/LyricLookup.h"

#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/playlist/PlaylistItemModel.h"
#include "GUI/playlist/PlaylistItemDelegate.h"

#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QFileDialog>
#include <QScrollBar>
#include <QMacStyle>
#include <QAbstractListModel>
#include <iostream>
#include <vector>
#include <QStyleFactory>
#include <QPaintDevice>
#include <QPainter>
#include <QMessageBox>
#include <QTextEdit>

class QPaintEngine;
class QPaintDevice;


using namespace std;
// CTOR
GUI_Playlist::GUI_Playlist(QWidget *parent) :
	QWidget(parent)
	 {
	this->ui = new Ui::Playlist_Window();


	ui->setupUi(this);
	initGUI();

	//this->ui->btn_dummy->setVisible(false);

	_pli_model = new PlaylistItemModel();
	_pli_delegate = new PlaylistItemDelegate(this->ui->listView);


	inner_drag_drop = false;
	_show_lyrics = false;
	this->ui->listView->setDragEnabled(true);


	_playlist_mode = CSettingsStorage::getInstance()->getPlaylistMode();
	this->ui->btn_append->setChecked(_playlist_mode.append);
	this->ui->btn_repAll->setChecked(_playlist_mode.repAll);
	this->ui->btn_dynamic->setChecked(_playlist_mode.dynamic);
	this->ui->btn_shuffle->setChecked(_playlist_mode.shuffle);


	//this->ui->pl_progress_bar->hide();

	this->ui->listView->setModel(_pli_model);
	this->ui->listView->setItemDelegate(_pli_delegate);
	this->ui->listView->setSelectionRectVisible(true);
	this->ui->listView->setAlternatingRowColors(true);
	this->ui->listView->setMovement(QListView::Free);

	this->_text = new QTextEdit(this->ui->listView);
	this->_text->setAcceptRichText(true);
	this->_text->hide();


	this->ui->btn_import->setVisible(false);


	this->connect(this->ui->btn_clear, SIGNAL(released()), this, SLOT(clear_playlist_slot()));
	//this->connect(this->ui->btn_dummy, SIGNAL(released()), this, SLOT(dummy_pressed()));

	this->connect(this->ui->btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_lyrics, SIGNAL(toggled(bool)), this, SLOT(lyric_button_toggled(bool)));
	this->connect(this->ui->btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));

	this->connect(this->ui->listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(pressed(const QModelIndex&)));
	this->connect(this->ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(double_clicked(const QModelIndex &)));
	this->connect(this->ui->listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(released(const QModelIndex &)));

	this->connect(this->ui->btn_import, SIGNAL(clicked()), this, SLOT(import_button_clicked()));
	//this->connect(this->ui->btn_dummy, SIGNAL(released()), this, SLOT(dummy_pressed()));

	// we need a reason for refreshing the list
	QStringList empty_list;
	emit sound_files_dropped(empty_list);

	this->setAcceptDrops(true);
	_parent = parent;
	_total_secs = 0;

	_cur_selected_row = -1;
	_cur_playing_row = -1;

	check_for_library_path();

	int style = CSettingsStorage::getInstance()->getPlayerStyle();
	bool dark = (style == 1);
	change_skin(dark);
}


// DTOR
GUI_Playlist::~GUI_Playlist() {

}



void GUI_Playlist::library_path_changed(QString path){
	Q_UNUSED(path);
	check_for_library_path();
}

void GUI_Playlist::check_for_library_path(){
	QString libraryPath = CSettingsStorage::getInstance()->getLibraryPath();

	if(libraryPath.size() == 0 || !QFile::exists(libraryPath)){
		this->ui->btn_dynamic->setEnabled(false);
		this->ui->btn_dynamic->setToolTip("Please set library path first");
	}

	else{
		this->ui->btn_dynamic->setEnabled(true);
		this->ui->btn_dynamic->setToolTip("Dynamic playing");
	}
}

void GUI_Playlist::dummy_pressed(){


}


void GUI_Playlist::lyric_button_toggled(bool on){

	this->parentWidget()->setFocus();

	if(on){
		QVariant data = this->_pli_model->data(_pli_model->index(_cur_playing_row, 0), Qt::WhatsThisRole);
		QStringList lst = data.toStringList();
		MetaData md;

		md.fromStringList(lst);
		qDebug() << "artist = " << md.artist << "; song = " << md.title;


		LyricLookup ll;
		QString lyrics = ll.find_lyrics(md.artist, md.title);
		lyrics = lyrics.trimmed();

		QString title = QString("<font size=\"5\" color=\"#F3841A\"><b>") + md.artist + " - " + md.title + "</b></font><br /><br />";
		QString text_data = title + lyrics ;//+ "</center>";
		QSize size = this->ui->listView->size();
		size.setWidth(size.width() + this->ui->listView->verticalScrollBar()->width());

		_text->setAcceptRichText(true);
		_text->setText(title + lyrics);
		_text->setLineWrapColumnOrWidth(this->ui->listView->width() - 10);
		_text->setLineWrapMode(QTextEdit::FixedPixelWidth);
		_text->setMinimumSize(size);
		_text->setMaximumSize(size);
		_text->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		_text->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		_text->show();

		this->ui->listView->verticalScrollBar()->hide();

		_show_lyrics = true;
	}

	else{
		_show_lyrics = false;
		_text->setPlainText("");
		_text->hide();

		this->ui->listView->verticalScrollBar()->show();
	}

}



// SLOT: switch between dark & light skin
void GUI_Playlist::change_skin(bool dark){

	if(dark){
		QString table_style = Style::get_tv_style();
		QString scrollbar_style = Style::get_v_scrollbar_style();

		this->ui->lab_totalTime->setStyleSheet("background-color: " + Style::get_player_back_color() + ";");
		this->ui->listView->setStyleSheet(table_style);
		this->ui->listView->verticalScrollBar()->setStyleSheet(scrollbar_style);


		QString btn_style = Style::get_btn_style(6);

		this->ui->btn_append->setStyleSheet(btn_style);
		this->ui->btn_clear->setStyleSheet(btn_style);
		this->ui->btn_dynamic->setStyleSheet(btn_style);
		this->ui->btn_repAll->setStyleSheet(btn_style);
		this->ui->btn_shuffle->setStyleSheet(btn_style);
		this->ui->btn_lyrics->setStyleSheet(btn_style);
	}

	else {
		this->ui->lab_totalTime->setStyleSheet("");
		this->ui->listView->setStyleSheet("");
		this->ui->listView->verticalScrollBar()->setStyleSheet("");

		QString btn_style = Style::get_btn_style(7);

		this->ui->btn_append->setStyleSheet(btn_style);
		this->ui->btn_clear->setStyleSheet(btn_style);
		this->ui->btn_dynamic->setStyleSheet(btn_style);
		this->ui->btn_repAll->setStyleSheet(btn_style);
		this->ui->btn_shuffle->setStyleSheet(btn_style);
		this->ui->btn_lyrics->setStyleSheet(btn_style);
	}
}

// SLOT: maybe we wanna use the progress bar
// atm this function is not used
void GUI_Playlist::update_progress_bar(int percent){

	Q_UNUSED(percent);

}

// SLOT: fill all tracks in v_metadata into playlist
// the current track should be highlighted
void GUI_Playlist::fillPlaylist(vector<MetaData>& v_metadata, int cur_play_idx){


	_pli_model->removeRows(0, _pli_model->rowCount());
	_pli_model->insertRows(0, v_metadata.size());
	_total_secs = 0;
	int idx = 0;

	if(_cur_playing_row != cur_play_idx && !_text->isHidden()){
		_cur_playing_row = cur_play_idx;
		lyric_button_toggled(true);
	}

	_cur_playing_row = cur_play_idx;

	this->ui->btn_import->setVisible(false);
	for(vector<MetaData>::iterator it = v_metadata.begin(); it != v_metadata.end(); it++){

		if(it->is_extern) this->ui->btn_import->setVisible(true);

		QModelIndex model_idx = _pli_model->index(idx, 0);

		int min, sek;
		Helper::cvtSecs2MinAndSecs(it->length_ms / 1000, &min, &sek);
		_total_secs += (min * 60 + sek);

		QString time_str = Helper::cvtSomething2QString(min, 2) + ":" + Helper::cvtSomething2QString(sek, 2);
		QStringList str4Playlist = it->toStringList();



		if(idx == _cur_playing_row) str4Playlist.push_back("1");
		else str4Playlist.push_back("0");

		str4Playlist.push_back("0");
		qDebug() << "insert " << str4Playlist;

		_pli_model->setData(model_idx, (const QVariant&) str4Playlist, Qt::EditRole);

		idx++;
	}

	set_total_time_label();

	// nur ein test TODO: REMOVE ME
	//emit playlist_filled(v_metadata);
}

// private SLOT: clear button pressed
void GUI_Playlist::clear_playlist_slot(){
	this->ui->lab_totalTime->setText("Total Time: 0m 0s");
	this->ui->btn_import->setVisible(false);
	_pli_model->removeRows(0, _pli_model->rowCount());
	_cur_playing_row = -1;
	_cur_selected_row = -1;
	qDebug() << "Ui:: send clear playlist";
	emit clear_playlist();
}

// private SLOT: save button pressed
void GUI_Playlist::save_playlist_slot(){

	QString file2Save =QFileDialog::getSaveFileName(this, tr("Choose filename to save"), QDir::homePath(), QString("*.m3u"));
	if(!file2Save.endsWith(".m3u"))
		file2Save.append(".m3u");
	emit save_playlist(file2Save);

}

// private SLOT: playlist item pressed (init drag & drop)
void GUI_Playlist::pressed(const QModelIndex& index){

	if(!index.isValid() || index.row() < 0 || index.row() >= _pli_model->rowCount()) return;

	_cur_selected_row = index.row();

	QList<QVariant> list2send;
	QStringList metadata = this->_pli_model->data(index, Qt::WhatsThisRole).toStringList();
	if(metadata.size() == 0) return;

	list2send.push_back(metadata);

	QMimeData* mime = new QMimeData();
	mime->setProperty("data_type", DROP_TYPE_TRACKS);
	mime->setProperty("data", list2send);

	this->ui->listView->set_mime_data(mime);
	if(index.row() == _cur_selected_row)
		inner_drag_drop = true;

	else inner_drag_drop = false;

}

void GUI_Playlist::released(const QModelIndex& index){
	inner_drag_drop = false;

	if(!index.isValid() || index.row() < 0 || index.row() >= _pli_model->rowCount()) return;

		this->ui->listView->set_mime_data(NULL);
		inner_drag_drop = false;
}


// private SLOT: track chosen (change track)
void GUI_Playlist::double_clicked(const QModelIndex & index){

	if(!index.isValid() || index.row() < 0 || index.row() >= _pli_model->rowCount()) return;

	clear_drag_lines(index.row());

	for(int i=0; i<index.model()->rowCount(); i++){

		QModelIndex tmp_idx = index.model()->index(i, 0);
		QStringList str4Playlist = index.model()->data(tmp_idx, Qt::WhatsThisRole).toStringList();

		if(i == index.row())
			str4Playlist[str4Playlist.length()-2] = "1";	// paint as marked

		else
			str4Playlist[str4Playlist.length()-2] = "0";

		_pli_model->setData(tmp_idx, (const QVariant&) str4Playlist, Qt::EditRole);
	}

	int new_row = index.row();

	if(new_row < 0 || new_row >= _pli_model->rowCount() ) return;

	emit selected_row_changed(new_row);
	_cur_playing_row = new_row;
}



// SLOT: if the current track has has changed
// by playlist
void GUI_Playlist::track_changed(int new_row){

	if(new_row < 0) return;

	QModelIndex index = _pli_model->index(new_row, 0);
	if(_cur_playing_row != new_row && !_text->isHidden()){
		_cur_playing_row = new_row;
		lyric_button_toggled(true);
	}

	_cur_playing_row = new_row;

	for(int i=0; i<_pli_model->rowCount(); i++){

		QModelIndex tmp_idx = _pli_model->index(i, 0);
		QStringList str4Playlist = index.model()->data(tmp_idx, Qt::WhatsThisRole).toStringList();

		do{
			if(i == index.row() && str4Playlist.size() >= 3)
				str4Playlist[str4Playlist.size()-2] = QString("1");

			else if(str4Playlist.size() >= 3)
				str4Playlist[str4Playlist.size()-2] = QString("0");

			else break;

			_pli_model->setData(tmp_idx, (const QVariant&) str4Playlist, Qt::EditRole);

		} while(0);
	}

	this->ui->listView->scrollTo(index);

}

// private SLOT: rep1, repAll, shuffle or append has changed
void GUI_Playlist::playlist_mode_changed_slot(){

	this->parentWidget()->setFocus();

	_playlist_mode.rep1 = this->ui->btn_rep1->isChecked();
	_playlist_mode.repAll = this->ui->btn_repAll->isChecked();
	_playlist_mode.shuffle = this->ui->btn_shuffle->isChecked();
	_playlist_mode.append = this->ui->btn_append->isChecked();
	_playlist_mode.dynamic = this->ui->btn_dynamic->isChecked();

	emit playlist_mode_changed(_playlist_mode);
	emit save_playlist("bla");
}




// initialize gui
// maybe the button state (pressed/unpressed) should be loaded from db here
void GUI_Playlist::initGUI(){

	QString icon_path = Helper::getIconPath();

	this->ui->btn_append->setIcon(QIcon(icon_path + "append.png"));
	this->ui->btn_rep1->setIcon(QIcon(icon_path + "rep1.png"));
	this->ui->btn_rep1->setVisible(false);
	this->ui->btn_repAll->setIcon(QIcon(icon_path + "repAll.png"));
	this->ui->btn_dynamic->setIcon(QIcon(icon_path + "dynamic.png"));
	this->ui->btn_shuffle->setIcon(QIcon(icon_path + "shuffle.png"));
	this->ui->btn_clear->setIcon(QIcon(icon_path + "broom.png"));
	this->ui->btn_import->setIcon(QIcon(icon_path + "import.png"));
	this->ui->btn_lyrics->setIcon(QIcon(icon_path + "lyrics.png"));

}


// we start the drag action, all lines has to be cleared
void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event){
	int row = this->ui->listView->indexAt(_last_known_drag_pos).row();
	clear_drag_lines(row);
}



// remove the black line under the titles
void GUI_Playlist::clear_drag_lines(int row){

	for(int i=row-3; i<=row+3; i++){

		QModelIndex idx = this->_pli_model->index(i, 0);
		if(!idx.isValid() || idx.row() < 0 || idx.row() >= _pli_model->rowCount())
			continue;

		QStringList list = _pli_model->data(this->_pli_model->index(i, 0), Qt::WhatsThisRole).toStringList();

		// set the flag, that no black line should be painted
		if(list.size() > 1)
			list[list.length()-1] = QString("0");

		_pli_model->setData(this->_pli_model->index(i, 0), (const QVariant&) list, Qt::EditRole);

	}
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

	int scrollbar_pos = this->ui->listView->verticalScrollBar()->sliderPosition();
	int y_event =  event->pos().y();
	int y_playlist = this->ui->listView->y();


	// scroll up
	if(y_event <= y_playlist+10){

		if(scrollbar_pos >= 1)
			this->ui->listView->scrollTo(this->_pli_model->index(scrollbar_pos-1, 0));
	}

	// scroll down
	else if(y_event >= y_playlist + this->ui->listView->height()-10){

		int num_steps = this->ui->listView->height() / 30;
		QModelIndex idx = _pli_model->index(scrollbar_pos+num_steps, 0);

		if(idx.isValid()) this->ui->listView->scrollTo(idx);
		if(idx.row() == -1) this->ui->listView->scrollToBottom();
	}



	int row = this->ui->listView->indexAt(pos).row();

	if(row <= -1) row = _pli_model->rowCount()-1;
	else if(row > 0) row--;

	if( (row == _cur_selected_row && inner_drag_drop) ||
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
	QStringList list = _pli_model->data(cur_idx, Qt::WhatsThisRole).toStringList();
	list[list.length()-1] = QString("1");

	_pli_model->setData(cur_idx, (const QVariant&) list, Qt::EditRole);

}


// finally drop it
void GUI_Playlist::dropEvent(QDropEvent* event){

	if(!event->mimeData()) return;

	QPoint pos = event->pos();
	QModelIndex idx = this->ui->listView->indexAt(pos);

	int row = idx.row();

	if(row == -1){
		row = this->_pli_model->rowCount();
	}

	clear_drag_lines(row);

	if(inner_drag_drop){
		inner_drag_drop = false;
		if( (row-1) == _cur_selected_row){
			event->ignore();

			return;
		}

		if(_cur_selected_row < row ) {
			row--;
		}

		remove_cur_selected_row();
	}



	QString text = event->mimeData()->text();

	if(text.startsWith("file://")){

		QStringList pathlist = text.split('\n');
		QStringList file_paths;

		if(pathlist.size() > 1) pathlist.removeLast();

		for(int i=0; i<pathlist.size(); i++){
			QString path =  pathlist.at(i).right(pathlist.at(i).length() - 7).trimmed();
			path = path.replace("%20", " ");
			if(QFile::exists(path)){

				if(path.endsWith(".mp3") ||
						path.endsWith(".ogg") ||
						path.endsWith(".wav") ||
						path.endsWith(".flac") ||
						path.endsWith(".m4u") ||
						path.endsWith(".wma") ||
						path.endsWith(".aac"))
					file_paths.push_back(path);

				else if(path.at(path.length()-4) != '.'){ // directory
					emit directory_dropped(path, row);
					return;
				}
			}
		}


		if(file_paths.size() > 0){
			vector<MetaData> v_metadata;
			for(int i=0; i<file_paths.size(); i++){
				v_metadata.push_back(ID3::getMetaDataOfFile(file_paths[i]));
			}

			emit dropped_tracks(v_metadata, row);

			return;
		}

		return;
	} // extern drop end


	int event_type = event->mimeData()->property("data_type").toInt();
	if(row == -1) row = _pli_model->rowCount();

	QList<QVariant> list = event->mimeData()->property("data").toList();
	if(row < _cur_playing_row) _cur_playing_row += list.size();

	if(event_type == DROP_TYPE_TRACKS){
		vector<MetaData> v_md4Playlist;
		for(int i=0; i<list.size(); i++){

			QStringList md_stringlist = list.at(i).toStringList();

			MetaData md;
			md.fromStringList(md_stringlist);
			v_md4Playlist.push_back(md);
		}

		emit dropped_tracks(v_md4Playlist, row);
	}
}


void GUI_Playlist::set_total_time_label(){


	int secs, mins, hrs;
			Helper::cvtSecs2MinAndSecs(_total_secs, &mins, &secs);
			hrs = mins / 60;
			mins = mins % 60;

			QString playlist_string = QString::number(this->_pli_model->rowCount());
			if(this->_pli_model->rowCount() == 1) playlist_string += " Track - ";
			else playlist_string += " Tracks - ";


			if(hrs > 0) playlist_string += QString::number(hrs) + "h ";

			playlist_string += 	Helper::cvtNum2String(mins, 2) +
								"m " +
								Helper::cvtNum2String(secs, 2) +
								"s";



			this->ui->lab_totalTime->setText(playlist_string);

}

void GUI_Playlist::remove_cur_selected_row(){
	if(_pli_model->rowCount() > 1){
		if(_cur_selected_row < _cur_playing_row) _cur_playing_row --;

		emit row_removed(_cur_selected_row);
		_cur_selected_row = -1;
	}

	else {
		clear_playlist_slot();
		_cur_selected_row = -1;
	}
}

void GUI_Playlist::keyPressEvent(QKeyEvent* e){

	QWidget::keyPressEvent(e);
	int key = e->key();

	if(key == 16777223 && _cur_selected_row < _pli_model->rowCount() && _cur_selected_row >= 0){

		remove_cur_selected_row();
	}
}


void GUI_Playlist::edit_id3_but_pressed(){
	emit edit_id3_signal();
}

void GUI_Playlist::last_fm_logged_in(bool success){

	this->ui->btn_dynamic->setEnabled(success);
}


void GUI_Playlist::import_button_clicked(){
	emit sig_import_to_library(false);
}

void GUI_Playlist::import_result(bool success){

	this->ui->btn_import->setVisible(!success);

}
