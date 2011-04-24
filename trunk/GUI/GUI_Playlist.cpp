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

	this->connect(this->ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(current_row_changed(const QModelIndex &)));

	this->setAcceptDrops(true);
	_parent = parent;

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

	int idx = 0;

	qint64 playlist_time = 0;
	for(vector<MetaData>::iterator it = v_metadata.begin(); it != v_metadata.end(); it++){

		QModelIndex model_idx = _pli_model->index(idx, 0);

		//it->print();

		int min, sek;
		Helper::cvtSecs2MinAndSecs(it->length_ms / 1000, &min, &sek);
		playlist_time += (min * 60 + sek);


		QString time_str = Helper::cvtSomething2QString(min, 2) + ":" + Helper::cvtSomething2QString(sek, 2);


		QString str = 	it->artist +
						" - " +
						it->title +
						"      (" +
						 time_str +
						")";

		//QListWidgetItem* item = new QListWidgetItem(	str );
		//ui->_filelist->addItem(item);
		QString str4Playlist = 	it->artist + 			",\n" +
								"[" + it->album + "]" + ",\n" +
								it->title + 			",\n" +
								time_str + 				",\n" +
								"0";


		_pli_model->setData(model_idx, (const QVariant&) str4Playlist, Qt::EditRole);


		idx++;
	}

	int secs, mins, hrs;
	Helper::cvtSecs2MinAndSecs(playlist_time, &mins, &secs);
	hrs = mins / 60;
	mins = mins % 60;

	QString playlist_string = "Total Time: ";

	if(hrs > 0) playlist_string += QString::number(hrs) + "h ";

	playlist_string += 	QString::number(mins) +
						"m " +
						QString::number(secs) +
						"s";



	this->ui->lab_totalTime->setText(playlist_string);




}



void GUI_Playlist::clear_playlist_slot(){
	this->ui->lab_totalTime->setText("Total Time: 0m 0s");
	_pli_model->removeRows(0, _pli_model->rowCount());
	//this->ui->_filelist->clear();
	emit clear_playlist();
}

void GUI_Playlist::save_playlist_slot(){

	QString file2Save =QFileDialog::getSaveFileName(this, tr("Choose filename to save"), QDir::homePath(), QString("*.m3u"));
	if(!file2Save.endsWith(".m3u"))
		file2Save.append(".m3u");
	emit save_playlist(file2Save);

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

			_pli_model->setData(tmp_idx, (const QVariant&) str4Playlist, Qt::EditRole);
		}




		int new_row = index.row();


		current_row_changed(new_row);
	}
}



void GUI_Playlist::current_row_changed(int new_row){

	qDebug() << "Current row changed";
	//cout << "GUI: Current row changed to " << new_row << endl;
	/*for(int i=0; i<this->ui->_filelist->count(); i++){
			this->ui->_filelist->item(i)->setBackgroundColor(QColor::fromRgb(255, 255, 255));

	}*/
	if(new_row < 0) return;
	emit selected_row_changed(new_row);

}


void GUI_Playlist::track_changed(int new_row){

	qDebug() << "track changed";

	if(new_row < 0) return;

	QModelIndex index = _pli_model->index(new_row, 0);

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
		this->ui->lab_totalTime->setStyleSheet("color: rgb(0, 0, 0);");
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

	qDebug() << "Drag enter event";
	event->acceptProposedAction();


}


void GUI_Playlist::dropEvent(QDropEvent* event){

	QPoint pos = event->pos();
	QModelIndex idx = this->ui->listView->indexAt(pos);
	int row = idx.row();

	qDebug() << "Drop event";
	QString text = event->mimeData()->text();

	qDebug() << "row = " << row;


	QStringList title_list = event->mimeData()->property("title").toString().split("\n");
	QStringList artist_list = event->mimeData()->property("artist").toString().split("\n");
	QStringList album_list = event->mimeData()->property("album").toString().split("\n");
	QStringList length_list = event->mimeData()->property("length").toString().split("\n");

	if(title_list.size() != artist_list.size() || title_list.size() != album_list.size() || title_list.size() != length_list.size()) return;





	qDebug() << title_list.size()-1 << " inserted";

	if(row < 0) row = 0;
	if(row > 0) row--;
	_pli_model->insertRows(row, title_list.size()-1);

	for(int i=0; i<title_list.size()-1; i++){

		QString str4Playlist = 	artist_list.at(i) + ",\n" +
			"[" + album_list.at(i) + "]" + ",\n" +
			title_list.at(i) + 			",\n" +
			length_list.at(i) + ",\n" +
			"0";

		qDebug() << "insert " << str4Playlist;


		this->_pli_model->setData(this->_pli_model->index(row+i,0), str4Playlist, Qt::EditRole);



	}


}
