/*
 * GUI_Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#include "GUI/GUI_Playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "GUI/Playlist/PlaylistItemModel.h"
#include "GUI/Playlist/PlaylistItemDelegate.h"

#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QFileDialog>
#include <QAbstractListModel>
#include <iostream>
#include <vector>



using namespace std;

GUI_Playlist::GUI_Playlist(QWidget *parent) :
	QWidget(parent)
	 {
	this->ui = new Ui::Playlist_Window();

	ui->setupUi(this);
	initGUI();

	_pli_model = new PlaylistItemModel();
	_pli_delegate = new PlaylistItemDelegate(this->ui->listView);

	this->ui->pl_progress_bar->hide();

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

	if(percent > 0) this->ui->pl_progress_bar->show();
	if(percent == 100) this->ui->pl_progress_bar->hide();
	this->ui->pl_progress_bar->setValue(percent);
}

void GUI_Playlist::fillPlaylist(vector<MetaData>& v_metadata){

	_pli_model->removeRows(0, _pli_model->rowCount());
	_pli_model->insertRows(0, v_metadata.size());
	_total_secs = 0;
	int idx = 0;

	for(vector<MetaData>::iterator it = v_metadata.begin(); it != v_metadata.end(); it++){

		QModelIndex model_idx = _pli_model->index(idx, 0);

		int min, sek;
		Helper::cvtSecs2MinAndSecs(it->length_ms / 1000, &min, &sek);
		_total_secs += (min * 60 + sek);

		QString time_str = Helper::cvtSomething2QString(min, 2) + ":" + Helper::cvtSomething2QString(sek, 2);


		QString str = 	it->artist +
						" - " +
						it->title +
						"      (" +
						 time_str +
						")";

		QString str4Playlist = 	it->artist + 			",\n" +
								"[" + it->album + "]" + ",\n" +
								it->title + 			",\n" +
								time_str + 				",\n";
								//"0";

		if(idx == _cur_playing_row) str4Playlist += "1";
		else str4Playlist += "0";


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
}

void GUI_Playlist::current_row_changed(const QModelIndex & index){

	if(index.isValid()){

		for(int i=0; i<index.model()->rowCount(); i++){

			QModelIndex tmp_idx = index.model()->index(i, 0);
			QString str4Playlist = index.model()->data(tmp_idx, Qt::WhatsThisRole).toString();

			if(i == index.row())
				str4Playlist.replace(str4Playlist.size()-1, 1, '1');

			else
				str4Playlist.replace(str4Playlist.size()-1, 1, '0');


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
		QString str4Playlist = _pli_model->data(tmp_idx, Qt::WhatsThisRole).toString();

		if(i == index.row())
			str4Playlist.replace(str4Playlist.size()-1, 1, '1');

		else
			str4Playlist.replace(str4Playlist.size()-1, 1, '0');

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
		this->ui->lab_totalTime->setStyleSheet("background-color: rgb(92, 92, 92);\ncolor: rgb(255, 255, 255);");
		this->ui->listView->setStyleSheet("background-color: rgb(255, 255, 255);");
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

}


void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event){

	event->acceptProposedAction();
}


void GUI_Playlist::dropEvent(QDropEvent* event){

	/*qDebug() << "Dropped";
	qDebug() << event->mimeData()->text();*/
	QString text = event->mimeData()->text();

	/*if(text.startsWith("file://")){
		QStringList pathlist = text.split('\n');
		QStringList file_paths;
		pathlist.removeLast();
		for(int i=0; i<pathlist.size(); i++){
			QString path =  pathlist.at(i).right(pathlist.at(i).length() - 7).trimmed();
			path = path.replace("%20", " ");
			if(QFile::exists(path)){

				if(path.endsWith(".mp3"))
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
	}*/

	QPoint pos = event->pos();
	int row = this->ui->listView->indexAt(pos).row();

	int event_type = event->mimeData()->property("data_type").toInt();
	qDebug() << "event_type = " << event_type;
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

			QString playlist_string = "Total Time: ";

			if(hrs > 0) playlist_string += QString::number(hrs) + "h ";

			playlist_string += 	Helper::cvtNum2String(mins, 2) +
								"m " +
								Helper::cvtNum2String(secs, 2) +
								"s";



			this->ui->lab_totalTime->setText(playlist_string);

}

void GUI_Playlist::keyPressEvent(QKeyEvent* e){

	QWidget::keyPressEvent(e);
	int key = e->key();

	if(key == 16777223 && _cur_selected_row < _pli_model->rowCount() && _cur_selected_row >= 0){

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
}
