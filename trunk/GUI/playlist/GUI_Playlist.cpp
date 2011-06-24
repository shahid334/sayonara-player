/*
 * GUI_Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */


#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"

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

GUI_Playlist::GUI_Playlist(QWidget *parent) :
	QWidget(parent)
	 {
	this->ui = new Ui::Playlist_Window();

	ui->setupUi(this);
	initGUI();

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
	this->connect(this->ui->btn_save_playlist, SIGNAL(released()), this, SLOT(save_playlist_slot()));

	this->connect(this->ui->btn_rep1, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_repAll, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->btn_shuffle, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));

	this->connect(this->ui->btn_append, SIGNAL(released()), this, SLOT(playlist_mode_changed_slot()));
	this->connect(this->ui->listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(selected_row_changed(const QModelIndex&)));
	this->connect(this->ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(current_row_changed(const QModelIndex &)));

	this->connect(this->ui->but_EditID3, SIGNAL(released()), this, SLOT(edit_id3_but_pressed()));

	this->setAcceptDrops(true);
	_parent = parent;
	_total_secs = 0;

	_cur_selected_row = -1;
	_cur_playing_row = -1;

}


GUI_Playlist::~GUI_Playlist() {
	// TODO Auto-generated destructor stub
}


void GUI_Playlist::update_progress_bar(int percent){

	Q_UNUSED(percent);
	/*if(percent > 0) this->ui->pl_progress_bar->show();
	if(percent == 100) this->ui->pl_progress_bar->hide();
	this->ui->pl_progress_bar->setValue(percent);*/
}

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
	emit playlist_filled(v_metadata);
}

void GUI_Playlist::clear_playlist_slot(){
	this->ui->lab_totalTime->setText("Total Time: 0m 0s");
	_pli_model->removeRows(0, _pli_model->rowCount());
	_cur_playing_row = -1;
	_cur_selected_row = -1;
	emit clear_playlist();
}

void GUI_Playlist::save_playlist_slot(){

	QString file2Save =QFileDialog::getSaveFileName(this, tr("Choose filename to save"), QDir::homePath(), QString("*.m3u"));
	if(!file2Save.endsWith(".m3u"))
		file2Save.append(".m3u");
	emit save_playlist(file2Save);

}

void GUI_Playlist::selected_row_changed(const QModelIndex& index){

	_cur_selected_row = index.row();
	QDrag* drag = new QDrag(this);
	QMimeData* mime = new QMimeData();

	QList<QVariant> list2send;

	QStringList metadata = this->_pli_model->data(index, Qt::WhatsThisRole).toStringList();
	list2send.push_back(metadata);


	mime->setProperty("data_type", DROP_TYPE_TRACKS);
	mime->setProperty("data", (QVariant) list2send);
	drag->setMimeData(mime);
	inner_drag_drop = true;

	drag->exec();




}

void GUI_Playlist::current_row_changed(const QModelIndex & index){

	if(index.isValid()){

		for(int i=0; i<index.model()->rowCount(); i++){

			QModelIndex tmp_idx = index.model()->index(i, 0);
			QStringList str4Playlist = index.model()->data(tmp_idx, Qt::WhatsThisRole).toStringList();




			if(i == index.row())
				str4Playlist[str4Playlist.length()-2] = "1";

			else
				str4Playlist[str4Playlist.length()-2] = "0";


			_cur_playing_row = index.row();

			_pli_model->setData(tmp_idx, (const QVariant&) str4Playlist, Qt::EditRole);
		}

		int new_row = index.row();
		current_row_changed(new_row);
	}
}

void GUI_Playlist::current_row_changed(int new_row){

	if(new_row < 0) return;

	emit selected_row_changed(new_row);
	_cur_playing_row = new_row;
}


void GUI_Playlist::track_changed(int new_row){

	if(new_row < 0) return;

	QModelIndex index = _pli_model->index(new_row, 0);
	_cur_playing_row = new_row;

	for(int i=0; i<_pli_model->rowCount(); i++){

		QModelIndex tmp_idx = _pli_model->index(i, 0);
		QStringList str4Playlist = index.model()->data(tmp_idx, Qt::WhatsThisRole).toStringList();

				str4Playlist.removeLast();


				if(i == index.row())
					str4Playlist.push_back("1");

				else
					str4Playlist.push_back("0");

		_pli_model->setData(tmp_idx, (const QVariant&) str4Playlist, Qt::EditRole);
	}

	this->ui->listView->scrollTo(index);

}


void GUI_Playlist::playlist_mode_changed_slot(){

	_playlist_mode.rep1 = this->ui->btn_rep1->isChecked();
	_playlist_mode.repAll = this->ui->btn_repAll->isChecked();
	_playlist_mode.shuffle = this->ui->btn_shuffle->isChecked();
	_playlist_mode.append = this->ui->btn_append->isChecked();

	emit playlist_mode_changed(_playlist_mode);
}



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


void GUI_Playlist::initGUI(){


	this->ui->btn_append->setIcon(QIcon(Helper::getIconPath() + "append.png"));
	this->ui->btn_rep1->setIcon(QIcon(Helper::getIconPath() + "rep1.png"));
	this->ui->btn_repAll->setIcon(QIcon(Helper::getIconPath() + "repAll.png"));
	this->ui->btn_shuffle->setIcon(QIcon(Helper::getIconPath() + "shuffle.png"));
	this->ui->btn_clear->setIcon(QIcon(Helper::getIconPath() + "broom.png"));
	this->ui->btn_save_playlist->setIcon(QIcon(Helper::getIconPath() + "save.png"));
	this->ui->but_EditID3->setIcon(QIcon(Helper::getIconPath() + "edit.png"));

}


void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event){

	QPoint pos = event->pos();
		int row = this->ui->listView->indexAt(pos).row();

	if(this->_pli_model->index(row-2, 0).isValid()){
			QStringList list = _pli_model->data(this->_pli_model->index(row-2, 0), Qt::WhatsThisRole).toStringList();
			list[list.length()-1] = QString("0");
			_pli_model->setData(this->_pli_model->index(row-2, 0), (const QVariant&) list, Qt::EditRole);
		}


	if(this->_pli_model->index(row, 0).isValid()){
				QStringList list = _pli_model->data(this->_pli_model->index(row, 0), Qt::WhatsThisRole).toStringList();
				list[list.length()-1] = QString("0");
				_pli_model->setData(this->_pli_model->index(row, 0), (const QVariant&) list, Qt::EditRole);
			}




	if(this->_pli_model->index(row-1, 0).isValid() && (row-1) != _cur_selected_row){
		QStringList list = _pli_model->data(this->_pli_model->index(row-1, 0), Qt::WhatsThisRole).toStringList();
		list[list.length()-1] = QString("1");
		_pli_model->setData(this->_pli_model->index(row-1, 0), (const QVariant&) list, Qt::EditRole);
	}

}



void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event){


	int y_event =  event->pos().y();
	int y_playlist = this->ui->listView->geometry().topLeft().y();


	int scroll_bar_pos = this->ui->listView->verticalScrollBar()->sliderPosition();



	if(y_event <= y_playlist+20){

		if(scroll_bar_pos >= 1)
			this->ui->listView->scrollTo(this->_pli_model->index(scroll_bar_pos-1, 0));
	}

	else if(y_event >= y_playlist + this->ui->listView->height()-20){

		int num_steps = this->ui->listView->height() / 30;
		QModelIndex idx = this->_pli_model->index(scroll_bar_pos+num_steps, 0);
		if(idx.isValid()) this->ui->listView->scrollTo(idx);
	}

	if(event->mimeData() != NULL )
		event->acceptProposedAction();

}


void GUI_Playlist::dropEvent(QDropEvent* event){
	QPoint pos = event->pos();
	int row = this->ui->listView->indexAt(pos).row();

 	if(inner_drag_drop && (row-1) == _cur_selected_row){
		qDebug() << "ignore event";
 		event->ignore();
		inner_drag_drop = false;
		return;
	}

	else if(inner_drag_drop){
		qDebug() << "Remove current selected row";
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

			emit sound_files_dropped(file_paths);
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
