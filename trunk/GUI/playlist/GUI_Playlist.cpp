/*
 * GUI_Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */


#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/id3.h"

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

	this->ui->btn_dummy->setVisible(false);

	_pli_model = new PlaylistItemModel();
	_pli_delegate = new PlaylistItemDelegate(this->ui->listView);

	inner_drag_drop = false;

	this->ui->listView->setDragEnabled(true);


	//this->ui->pl_progress_bar->hide();

	this->ui->listView->setModel(_pli_model);
	this->ui->listView->setItemDelegate(_pli_delegate);
	this->ui->listView->setSelectionRectVisible(true);
	this->ui->listView->setAlternatingRowColors(true);
	this->ui->listView->setMovement(QListView::Free);

	this->connect(this->ui->btn_clear, SIGNAL(released()), this, SLOT(clear_playlist_slot()));
	//this->connect(this->ui->btn_save_playlist, SIGNAL(released()), this, SLOT(save_playlist_slot()));

	this->connect(this->ui->btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_dynamic, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));

	this->connect(this->ui->btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(pressed(const QModelIndex&)));
	this->connect(this->ui->listView, SIGNAL(released(const QModelIndex&)), this, SLOT(released(const QModelIndex&)));
	this->connect(this->ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(double_clicked(const QModelIndex &)));

	this->connect(this->ui->btn_dummy, SIGNAL(released()), this, SLOT(dummy_pressed()));

	// we need a reason for refreshing the list
	QStringList empty_list;
	emit sound_files_dropped(empty_list);

	this->setAcceptDrops(true);
	_parent = parent;
	_total_secs = 0;

	_cur_selected_row = -1;
	_cur_playing_row = -1;
}


void GUI_Playlist::dummy_pressed(){


}

void GUI_Playlist::similar_artists_available(const QStringList& artist_list){



}


// DTOR
GUI_Playlist::~GUI_Playlist() {

}

// SLOT: switch between dark & light skin
void GUI_Playlist::change_skin(bool dark){

	if(dark){
		this->ui->lab_totalTime->setStyleSheet("background-color: rgb(56, 56, 56); color: rgb(255, 255, 255);");
		this->ui->listView->setStyleSheet("background-color: rgb(48, 48, 48);  alternate-background-color: rgb(56,56,56);");
	}

	else {
		this->ui->lab_totalTime->setStyleSheet("");
		this->ui->listView->setStyleSheet("");
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

	_cur_playing_row = cur_play_idx;

	for(vector<MetaData>::iterator it = v_metadata.begin(); it != v_metadata.end(); it++){

		QModelIndex model_idx = _pli_model->index(idx, 0);

		int min, sek;
		Helper::cvtSecs2MinAndSecs(it->length_ms / 1000, &min, &sek);
		_total_secs += (min * 60 + sek);

		QString time_str = Helper::cvtSomething2QString(min, 2) + ":" + Helper::cvtSomething2QString(sek, 2);

		QStringList str4Playlist = it->toStringList();



		if(idx == _cur_playing_row) str4Playlist.push_back("1");
		else str4Playlist.push_back("0");

		str4Playlist.push_back("0");

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
	_pli_model->removeRows(0, _pli_model->rowCount());
	_cur_playing_row = -1;
	_cur_selected_row = -1;
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
	inner_drag_drop = true;


}

void GUI_Playlist::released(const QModelIndex& index){
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

	this->ui->btn_append->setIcon(QIcon(Helper::getIconPath() + "append.png"));
	this->ui->btn_rep1->setIcon(QIcon(Helper::getIconPath() + "rep1.png"));
	this->ui->btn_rep1->setVisible(false);
	this->ui->btn_repAll->setIcon(QIcon(Helper::getIconPath() + "repAll.png"));
	this->ui->btn_dynamic->setIcon(QIcon(Helper::getIconPath() + "dynamic.png"));
	this->ui->btn_shuffle->setIcon(QIcon(Helper::getIconPath() + "shuffle.png"));
	this->ui->btn_clear->setIcon(QIcon(Helper::getIconPath() + "broom.png"));

}


// we start the drag action, all lines has to be cleared
void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event){
	qDebug() << Q_FUNC_INFO;
	int row = this->ui->listView->indexAt(_last_known_drag_pos).row();
	clear_drag_lines(row);
}



// remove the black line under the titles
void GUI_Playlist::clear_drag_lines(int row){

	qDebug() << Q_FUNC_INFO;

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

	qDebug() << Q_FUNC_INFO;
	if(event->mimeData() != NULL )
		event->acceptProposedAction();
}



// we move the title
// scroll, if neccessary
// paint line
void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event){
	qDebug() << Q_FUNC_INFO;
	if( !event->mimeData() ) return;

	QPoint pos = event->pos();

	int scrollbar_pos = this->ui->listView->verticalScrollBar()->sliderPosition();
	int y_event =  event->pos().y();
	_last_known_drag_pos = pos;
	int y_playlist = this->ui->listView->y();


	// scroll up
	if(y_event <= y_playlist+10){

		if(scrollbar_pos >= 1)
			this->ui->listView->scrollTo(this->_pli_model->index(scrollbar_pos-1, 0));
	}

	// scroll down
	else if(y_event >= y_playlist + this->ui->listView->height()-10){

		int num_steps = this->ui->listView->height() / 30;
		QModelIndex idx = this->_pli_model->index(scrollbar_pos+num_steps, 0);
		if(idx.isValid()) this->ui->listView->scrollTo(idx);
		if(idx.row() == -1) this->ui->listView->scrollToBottom();
	}

	int row = this->ui->listView->indexAt(pos).row();

	if(row == -1) row = _pli_model->rowCount()-1;
	else if(row > 0) row--;

	int current = row-1;




	// delete all lines
	clear_drag_lines(current);


	// paint line
	if(	this->_pli_model->index(current, 0).isValid() &&
		(current) != _cur_selected_row &&
		current >= 0 &&
		current < this->_pli_model->rowCount() ){

		QStringList list = _pli_model->data(this->_pli_model->index(current, 0), Qt::WhatsThisRole).toStringList();
		list[list.length()-1] = QString("1");
		_pli_model->setData(this->_pli_model->index(current, 0), (const QVariant&) list, Qt::EditRole);
	}

}


// finally drop it
void GUI_Playlist::dropEvent(QDropEvent* event){

	if(!event->mimeData()) return;

	QPoint pos = event->pos();

	int row = this->ui->listView->indexAt(pos).row();

	if(row == -1){
		row = this->_pli_model->rowCount();
	}

	clear_drag_lines(row);


 	if(inner_drag_drop && (row-1) == _cur_selected_row){
		qDebug() << "ignore event";
 		event->ignore();
		inner_drag_drop = false;
		return;
	}

 	if(inner_drag_drop){
		qDebug() << "Remove current selected row";

		qDebug() << "Before: cur = " << _cur_selected_row << ", " << row;

		if(_cur_selected_row < row ) {
			qDebug() << "cur = " << _cur_selected_row << ", " << row;
			row--;
		}

		else qDebug() << "After: cur = " << _cur_selected_row << ", " << row;
		remove_cur_selected_row();
		inner_drag_drop = false;

	}


	qDebug() << "Dropped";

	//qDebug() << event->mimeData()->text();
	QString text = event->mimeData()->text();

	if(text.startsWith("file://")){

		QStringList pathlist = text.split('\n');
		QStringList file_paths;

		if(pathlist.size() > 1) pathlist.removeLast();

		for(int i=0; i<pathlist.size(); i++){
			QString path =  pathlist.at(i).right(pathlist.at(i).length() - 7).trimmed();
			//qDebug() << "path = " << path;
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
					emit directory_dropped(path);
					return;
				}
			}
		}


		if(file_paths.size() > 0){
			vector<MetaData> v_metadata;
			for(int i=0; i<file_paths.size(); i++){
				v_metadata.push_back(ID3::getMetaDataOfFile(file_paths[0]));
			}

			emit dropped_tracks(v_metadata, row);

			return;
		}

		return;
	}




	int event_type = event->mimeData()->property("data_type").toInt();
	//qDebug() << "event_type = " << event_type;
	if(row == -1) row = _pli_model->rowCount();
	else if(row > 0) row--;





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
