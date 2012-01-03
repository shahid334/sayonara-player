/* GUI_Library_windowed.cpp */

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
 * GUI_Library_windowed.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/library/LibraryItemModelTracks.h"
#include "GUI/library/LibraryItemDelegateTracks.h"
#include "GUI/library/LibraryItemModelAlbums.h"
#include "GUI/library/LibraryItemDelegateAlbums.h"
#include "GUI/library/LibraryItemDelegateArtists.h"
#include "GUI/library/LibraryItemModelArtists.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/CSettingsStorage.h"

#include "CoverLookup/CoverLookup.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/tagedit/GUI_TagEdit.h"

#include "ui_GUI_Library_windowed.h"

#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QPalette>
#include <QBrush>
#include <QScrollBar>

#include <vector>


using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {



	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);


	_album_msg_box = 0;
	this->_sort_albums = "name asc";
	this->_sort_artists = "name asc";
	this->_sort_tracks = "artist asc";

	_selected_artist = -1;
	_selected_album = -1;

	_selected_artist_name = "";
	_selected_album_name = "";


	_everything_loaded = false;


	this->_album_model = new LibraryItemModelAlbums();
	this->_album_delegate = new LibraryItemDelegateAlbums(this->ui->lv_album);
	this->_artist_model = new LibraryItemModelArtists();
	this->_artist_delegate = new LibraryItemDelegateArtists(this->ui->lv_artist);
	this->_track_model = new LibraryItemModelTracks();
	this->_track_delegate = new LibraryItemDelegateTracks(this->ui->tb_title);

	this->ui->tb_title->setModel(this->_track_model);
	this->ui->tb_title->setItemDelegate(this->_track_delegate);
	this->ui->lv_album->setModel(this->_album_model);
	this->ui->lv_album->setItemDelegate(this->_album_delegate);
	this->ui->lv_artist->setModel(this->_artist_model);
	this->ui->lv_artist->setItemDelegate(this->_artist_delegate);



	this->ui->lv_album->setDragEnabled(true);
	this->ui->lv_artist->setDragEnabled(true);
	this->ui->tb_title->setDragEnabled(true);

	this->ui->btn_clear->setIcon(QIcon(Helper::getIconPath() + "broom.png"));


	this->ui->lv_artist->setAlternatingRowColors(true);

	this->ui->btn_reload->setIcon(QIcon(Helper::getIconPath() + "reload.png"));
	this->ui->btn_reload->setVisible(false);

	init_menues();

	connect(this->ui->btn_clear, SIGNAL( clicked()), this, SLOT(clear_button_pressed()));
	connect(this->ui->btn_reload, SIGNAL( clicked()), this, SLOT(reload_library_slot()));
	connect(this->ui->le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));

	connect(this->ui->lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_chosen(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_chosen(const QModelIndex & )));
	connect(this->ui->tb_title, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_chosen(const QModelIndex & )));

	connect(this->ui->tb_title, SIGNAL(pressed ( const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex&)));
	connect(this->ui->lv_album, SIGNAL(pressed(const QModelIndex & )), this, SLOT(album_pressed(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(pressed(const QModelIndex & )), this, SLOT(artist_pressed(const QModelIndex & )));

	connect(this->ui->lv_album, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(show_album_context_menu(const QPoint&)));
	connect(this->ui->lv_artist, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(show_artist_context_menu(const QPoint&)));
	connect(this->ui->tb_title, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(show_track_context_menu(const QPoint&)));

	connect(this->ui->lv_album->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_albums_by_column(int)));
	connect(this->ui->lv_artist->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_artists_by_column(int)));
	connect(this->ui->tb_title->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_tracks_by_column(int)));

	int style = CSettingsStorage::getInstance()->getPlayerStyle();
	bool dark = (style == 1);
	change_skin(dark);

}

GUI_Library_windowed::~GUI_Library_windowed() {
	// TODO Auto-generated destructor stub
}


void GUI_Library_windowed::init_menues(){
	_right_click_menu = new QMenu(this);
	_info_action = new QAction("Info", this);
	_edit_action = new QAction("Edit", this);
	_delete_action = new QAction("Delete", this);

	_right_click_menu->addAction(_info_action);
	_right_click_menu->addAction(_edit_action);
	_right_click_menu->addAction(_delete_action);

}


void GUI_Library_windowed::fill_library_tracks(vector<MetaData>& v_metadata){

	_v_metadata.clear();
	_v_metadata = v_metadata;

	this->_track_model->removeRows(0, this->_track_model->rowCount());
	this->_track_model->insertRows(0, v_metadata.size());

	for(uint i=0; i<v_metadata.size(); i++){

		QModelIndex idx = _track_model->index(i, 0);
		QStringList data = v_metadata.at(i).toStringList();
		this->_track_model->setData(idx, data, Qt::EditRole);
	}


}

void GUI_Library_windowed::fill_library_albums(vector<Album>& albums){

	_v_albums.clear();
	_v_albums = albums;



	this->_album_model->removeRows(0, this->_album_model->rowCount());
	this->_album_model->insertRows(0, albums.size());

	for(uint i=0; i<albums.size(); i++){
		QModelIndex idx = this->_album_model->index(i, 1);

		QStringList data = albums.at(i).toStringList();
		this->_album_model->setData(idx, data, Qt::EditRole );
	}

}

void GUI_Library_windowed::fill_library_artists(vector<Artist>& artists){

	_v_artists.clear();
	_v_artists = artists;

	this->_artist_model->removeRows(0, this->_artist_model->rowCount());
	this->_artist_model->insertRows(0, artists.size());

	for(uint i=0; i<artists.size(); i++){

		QModelIndex idx = this->_artist_model->index(i, 0);

		QStringList data = artists.at(i).toStringList();

		this->_artist_model->setData(idx, data, Qt::EditRole );
	}

}


void GUI_Library_windowed::resizeEvent(QResizeEvent* e){

	Q_UNUSED(e);

	QSize tmpSize = this->ui->tb_title->size();
	int width = tmpSize.width() -20;
	int resisable_content = width - (50 + 50 + 60 + 25 );


	if(width > 600){
		this->ui->tb_title->setColumnWidth(0, 25);
		this->ui->tb_title->setColumnWidth(1, resisable_content * 0.40);
		this->ui->tb_title->setColumnWidth(2, resisable_content * 0.30);
		this->ui->tb_title->setColumnWidth(3, resisable_content * 0.30);
		this->ui->tb_title->setColumnWidth(4, 50);
		this->ui->tb_title->setColumnWidth(5, 50);
		this->ui->tb_title->setColumnWidth(6, 60);
		this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);


	}

	else{
		this->ui->tb_title->setColumnWidth(0, 25);
		this->ui->tb_title->setColumnWidth(1, (width-25) * 0.40);
		this->ui->tb_title->setColumnWidth(2, (width-25) * 0.30);
		this->ui->tb_title->setColumnWidth(3, (width-25) * 0.30);
		this->ui->tb_title->setColumnWidth(4, 50);
		this->ui->tb_title->setColumnWidth(5, 50);
		this->ui->tb_title->setColumnWidth(6, 60);
		this->ui->tb_title->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


	}

	this->ui->lv_album->setColumnWidth(0, 20);

	this->ui->lv_album->setColumnWidth(0, 20);
	this->ui->lv_album->setColumnWidth(1, this->ui->lv_album->width() - 100);
	this->ui->lv_album->setColumnWidth(2, 40);

	this->ui->lv_artist->setColumnWidth(0, 20);
	this->ui->lv_artist->setColumnWidth(1, this->ui->lv_artist->width() - 100);
	this->ui->lv_artist->setColumnWidth(2, 50);
	//this->ui->lv_artist->setColumnWidth(1, this->ui->lv_artist->width() - 20);
}


void GUI_Library_windowed::artist_pressed(const QModelIndex& idx){



	int artist_id = _v_artists.at(idx.row()).id;
	_selected_artist = artist_id;
	_selected_artist_name = _v_artists.at(idx.row()).name;

	_selected_album = -1;
	_selected_album_name = QString("");


	Artist artist = _v_artists.at(idx.row());
	vector<MetaData> vec_tracks;
	vector<Album> vec_albums;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	if(this->ui->le_search->text().length() == 0){
		db->getAllTracksByArtist(artist_id, vec_tracks, _cur_searchstring, _sort_tracks);
		db->getAllAlbumsByArtist(artist_id, vec_albums, _cur_searchstring, _sort_tracks);
	}
	else {
		db->getAllTracksByArtist(artist_id, vec_tracks, _cur_searchstring, _sort_tracks) ;
		db->getAllAlbumsByArtist(artist_id, vec_albums, _cur_searchstring, _sort_albums);
	}


	fill_library_albums(vec_albums);
	fill_library_tracks(vec_tracks);


	QMimeData* mime = new QMimeData();

	QList<QVariant> list2send;

		for(int i=0; i<this->_track_model->rowCount(); i++){
			QStringList metadata = _v_metadata.at(i).toStringList();
			list2send.push_back(metadata);
		}

		mime->setProperty("data_type", DROP_TYPE_TRACKS);
		mime->setProperty("data", (QVariant) list2send);
		this->ui->lv_artist->set_mime_data(mime);


}

void GUI_Library_windowed::album_pressed(const QModelIndex& idx){

	int album_id = _v_albums.at(idx.row()).id;
	_selected_album = album_id;
	Album album = _v_albums.at(idx.row());
	vector<MetaData> vec_tracks;
	vector<Artist> vec_artists;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	if(this->ui->le_search->text().length() == 0){
		db->getAllTracksByAlbum(album_id, vec_tracks, _cur_searchstring, _sort_tracks);
	}

	else {
		db->getAllTracksByAlbum(album_id, vec_tracks, _cur_searchstring, _sort_tracks);
	}

	fill_library_tracks(vec_tracks);


	QMimeData* mime = new QMimeData();

	QList<QVariant> list2send;

	for(int i=0; i<this->_track_model->rowCount(); i++){
		QStringList metadata = _v_metadata.at(i).toStringList();
		list2send.push_back(metadata);
	}

	mime->setProperty("data_type", DROP_TYPE_TRACKS);
	mime->setProperty("data", (QVariant) list2send);

	this->ui->lv_album->set_mime_data(mime);

}

void GUI_Library_windowed::track_pressed(const QModelIndex& idx){

	if(!idx.isValid()) return;

	QMimeData* mime = new QMimeData();

	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);

	QList<QVariant> list2send;

	for(int i=0; i<idx_list.size(); i++){
		int row = idx_list.at(i).row();

		QStringList metadata = _v_metadata.at(row).toStringList();
		list2send.push_back(metadata);
	}

	mime->setProperty("data_type", DROP_TYPE_TRACKS);
	mime->setProperty("data", (QVariant) list2send);
	this->ui->tb_title->set_mime_data(mime);
}



void GUI_Library_windowed::album_chosen(const QModelIndex & idx){

	int album_id = _v_albums.at(idx.row()).id;

	vector<MetaData> vec;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->getAllTracksByAlbum(album_id, vec, _cur_searchstring, _sort_tracks);
	emit album_chosen_signal(vec);

	_everything_loaded = false;
}

void GUI_Library_windowed::artist_chosen(const QModelIndex & idx){

	int artist_id = _v_artists.at(idx.row()).id;

	vector<MetaData> vec;
	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->getAllTracksByArtist(artist_id, vec, _cur_searchstring, _sort_tracks);
	emit artist_chosen_signal(vec);

	vec.clear();
	_everything_loaded = false;

}

void GUI_Library_windowed::track_chosen(const QModelIndex& idx){

	if(!idx.isValid()) return;

	vector<MetaData> vec;
	vec.push_back(_v_metadata.at(idx.row()));
	emit track_chosen_signal(vec);
}



void GUI_Library_windowed::show_artist_context_menu(const QPoint& p){



	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_artist()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_artist()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_artist()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_artist()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_artist()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_artist()));

}

void GUI_Library_windowed::show_album_context_menu(const QPoint& p){
	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_album()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_album()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_album()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_album()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_album()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_album()));


}

void GUI_Library_windowed::show_track_context_menu(const QPoint& p){

	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_tracks()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_tracks()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_tracks()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_tracks()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_tracks()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_tracks()));
}


void GUI_Library_windowed::clear_button_pressed(){
	_selected_album = -1;
	_selected_artist = -1;

	_selected_artist_name = "";
	_selected_album_name = "";

	_cur_searchstring = "";

	this->ui->le_search->clear();
	_everything_loaded = false;
	text_line_edited(" ");

}

void GUI_Library_windowed::refresh(){

	CDatabaseConnector* db = CDatabaseConnector::getInstance();

	vector<Album> vec_albums;
	vector<MetaData> vec_tracks;
	vector<Artist> vec_artists;


	db->getTracksFromDatabase(vec_tracks, _sort_tracks);
	db->getAllAlbums(vec_albums, _sort_albums);
	db->getAllArtists(vec_artists, _sort_artists);
	fill_library_artists(vec_artists);
	fill_library_albums(vec_albums);
	fill_library_tracks(vec_tracks);
	_everything_loaded = true;

	if(this->ui->le_search->text().size() > 0)
		text_line_edited(this->ui->le_search->text());

}

void GUI_Library_windowed::text_line_edited(const QString& search){

	if( (search.length() < 3 && _everything_loaded) ) return;

	vector<Album> vec_albums;
	vector<MetaData> vec_tracks;
	vector<Artist> vec_artists;

	if(search.length() < 3){


		CDatabaseConnector* db = CDatabaseConnector::getInstance();
		db->getTracksFromDatabase(vec_tracks, _sort_tracks);
		db->getAllAlbums(vec_albums, _sort_albums);
		db->getAllArtists(vec_artists, _sort_artists);
		fill_library_artists(vec_artists);
		fill_library_albums(vec_albums);
		fill_library_tracks(vec_tracks);

		_selected_album = -1;
		_selected_artist = -1;

		_selected_artist_name = "";
		_selected_album_name = "";

		_everything_loaded = true;

		return;

	}

	_everything_loaded = false;
	_cur_searchstring = QString("%") + search + "%";

	CDatabaseConnector* db = CDatabaseConnector::getInstance();
	db->getAllTracksBySearchString(_cur_searchstring, vec_tracks, _sort_tracks);
	fill_library_tracks(vec_tracks);

	db->getAllAlbumsBySearchString(_cur_searchstring, vec_albums, _sort_albums);
	fill_library_albums(vec_albums);

	db->getAllArtistsBySearchString(_cur_searchstring, vec_artists, _sort_artists);
	fill_library_artists(vec_artists);



}


void GUI_Library_windowed::change_skin(bool dark){


	if(dark){

		QString table_style = Style::get_tv_style();
		QString header_style = Style::get_tv_header_style();
		QString v_scrollbar_style = Style::get_v_scrollbar_style();
		QString h_scrollbar_style = Style::get_h_scrollbar_style();

		this->ui->lv_album->setStyleSheet(table_style);
		this->ui->lv_artist->setStyleSheet(table_style);
		this->ui->tb_title->setStyleSheet(table_style);

		this->ui->btn_clear->setStyleSheet(Style::get_btn_style());

		this->ui->tb_title->setShowGrid(false);
		this->ui->lv_album->setShowGrid(false);
		this->ui->lv_artist->setShowGrid(false);

		this->ui->tb_title->horizontalHeader()->setStyleSheet(header_style);
		this->ui->lv_album->horizontalHeader()->setStyleSheet(header_style);
		this->ui->lv_artist->horizontalHeader()->setStyleSheet(header_style);

		this->ui->lv_artist->verticalScrollBar()->setStyleSheet(v_scrollbar_style);
		this->ui->lv_album->verticalScrollBar()->setStyleSheet(v_scrollbar_style);
		this->ui->tb_title->verticalScrollBar()->setStyleSheet(v_scrollbar_style);

		this->ui->lv_artist->horizontalScrollBar()->setStyleSheet(h_scrollbar_style);
		this->ui->lv_album->horizontalScrollBar()->setStyleSheet(h_scrollbar_style);
		this->ui->tb_title->horizontalScrollBar()->setStyleSheet(h_scrollbar_style);
	}


	else {
		this->ui->lv_album->setStyleSheet("");
		this->ui->lv_artist->setStyleSheet("");
		this->ui->tb_title->setStyleSheet("");

		this->ui->tb_title->setShowGrid(true);
		this->ui->lv_album->setShowGrid(true);
		this->ui->lv_artist->setShowGrid(true);

		this->ui->tb_title->horizontalHeader()->setStyleSheet("");
		this->ui->lv_album->horizontalHeader()->setStyleSheet("");
		this->ui->lv_artist->horizontalHeader()->setStyleSheet("");

		this->ui->lv_artist->verticalScrollBar()->setStyleSheet("");
		this->ui->lv_album->verticalScrollBar()->setStyleSheet("");
		this->ui->tb_title->verticalScrollBar()->setStyleSheet("");

		this->ui->lv_artist->horizontalScrollBar()->setStyleSheet("");
		this->ui->lv_album->horizontalScrollBar()->setStyleSheet("");
		this->ui->tb_title->horizontalScrollBar()->setStyleSheet("");
	}
}


QString GUI_Library_windowed::getTotalTimeString(Album& album){
	int secs, mins, hrs;
	Helper::cvtSecs2MinAndSecs(album.length_sec, &mins, &secs);
	hrs = mins / 60;
	mins = mins % 60;

	QString str = "";

	if(hrs > 0) str += QString::number(hrs) + "h ";

	str += 	Helper::cvtNum2String(mins) +
			"m " +
			Helper::cvtNum2String(secs) +
			"s";



	return str;


}



void GUI_Library_windowed::id3_tags_changed(){


	refresh();
}



void GUI_Library_windowed::sort_albums_by_column(int col){

	if(col == 1){
		if(_sort_albums == "name asc") _sort_albums = "name desc";
		else _sort_albums = "name asc";
	}

	if(col == 2){
		if(_sort_albums == "year asc") _sort_albums = "year desc";
		else _sort_albums = "year asc";
	}

	vector<Album> vec_albums;

	if(!_cur_searchstring.isEmpty()){

		CDatabaseConnector::getInstance()->getAllAlbumsBySearchString(_cur_searchstring, vec_albums, _sort_albums);
	}

	else if(_selected_artist != -1){
		CDatabaseConnector::getInstance()->getAllAlbumsByArtist(_selected_artist, vec_albums, _cur_searchstring, _sort_albums);
	}

	else{
		CDatabaseConnector::getInstance()->getAllAlbums(vec_albums, _sort_albums);
	}

	fill_library_albums(vec_albums);
}


void GUI_Library_windowed::sort_artists_by_column(int col){

	if(col == 1){
		if(_sort_artists == "name asc") _sort_artists = "name desc";
		else _sort_artists = "name asc";
	}

	if(col == 2){
		if(_sort_artists == "trackcount asc") _sort_artists = "trackcount desc";
		else _sort_artists = "trackcount asc";
	}

	vector<Artist> vec_artists;

	if(!_cur_searchstring.isEmpty()){

		CDatabaseConnector::getInstance()->getAllArtistsBySearchString(_cur_searchstring, vec_artists, _sort_artists);
	}

	else{
		CDatabaseConnector::getInstance()->getAllArtists(vec_artists, _sort_artists);
	}

	fill_library_artists(vec_artists);
}

void GUI_Library_windowed::sort_tracks_by_column(int col){

		if(col == COL_TRACK_NUM){
			if(_sort_tracks == "track asc") _sort_tracks = "track desc";
			else _sort_tracks = "track asc";
		}

		else if(col == COL_TITLE){
			if(_sort_tracks == "title asc") _sort_tracks = "title desc";
			else _sort_tracks = "title asc";
		}

		else if(col == COL_ALBUM){
			if(_sort_tracks == "album asc") _sort_tracks = "album desc";
			else _sort_tracks = "album asc";
		}

		else if(col == COL_ARTIST){
			if(_sort_tracks == "artist asc") _sort_tracks = "artist desc";
			else _sort_tracks = "artist asc";
		}

		else if(col == COL_YEAR){
			if(_sort_tracks == "year asc") _sort_tracks = "year desc";
			else _sort_tracks = "year asc";
		}

		else if(col == COL_LENGTH){
			if(_sort_tracks == "length asc") _sort_tracks = "length desc";
			else _sort_tracks = "length asc";
		}


		else if(col == COL_BITRATE){
			if(_sort_tracks == "bitrate asc") _sort_tracks = "bitrate desc";
			else _sort_tracks = "bitrate asc";
		}



		vector<MetaData> vec_md;

		if(!_cur_searchstring.isEmpty() && _selected_album == -1 && _selected_artist == -1){
			CDatabaseConnector::getInstance()->getAllTracksBySearchString( _cur_searchstring, vec_md, _sort_tracks);
		}

		else if(_selected_album != -1){
			CDatabaseConnector::getInstance()->getAllTracksByAlbum(_selected_album, vec_md, _cur_searchstring, _sort_tracks);
		}

		else if(_selected_artist != -1){
			CDatabaseConnector::getInstance()->getAllTracksByArtist(_selected_artist, vec_md, _cur_searchstring, _sort_tracks);
		}

		else{
			CDatabaseConnector::getInstance()->getTracksFromDatabase(vec_md, _sort_tracks);
		}


		fill_library_tracks(vec_md);





}




void GUI_Library_windowed::reloading_library(int percent){
	if(percent < 0)
		this->ui->lab_status->setText("<b>(Reloading...)</b>");
	else this->ui->lab_status->setText("<b>(Reloading: " + QString::number(percent) + QString("\%)") + QString("</b>"));
}

void GUI_Library_windowed::reloading_library_finished(){
	this->ui->lab_status->setText("Music Library");
}

void GUI_Library_windowed::library_should_be_reloaded(){
	this->ui->btn_reload->setVisible(true);
}

void GUI_Library_windowed::reload_library_slot(){

	this->ui->btn_reload->setVisible(false);
	emit reload_library();
}




void GUI_Library_windowed::edit_album(){

	emit data_for_id3_change(_v_metadata);
}

void GUI_Library_windowed::apply_cover_to_entire_album(){
	QPixmap pixmap = QPixmap(Helper::get_cover_path(_album_of_interest.artists[0], _album_of_interest.name));
	for(int i=0; i<_album_of_interest.artists.size(); i++){
		pixmap.save(Helper::get_cover_path(_album_of_interest.artists[i], _album_of_interest.name));
	}
}

void GUI_Library_windowed::info_album(){

	int num_artists;
	QPushButton* apply = 0;
	QPixmap pm;
	QString sel_artist = "";
	QString cover_path = "";
	bool	cover_found;


	if(_album_msg_box) delete _album_msg_box;
	_album_msg_box = new QMessageBox();


	QModelIndexList idx_list = this->ui->lv_album->selectionModel()->selectedRows(1);
	if(idx_list.size() <= 0 || !idx_list.at(0).isValid()) return;

	Album album;
	QStringList album_str_list = _album_model->data(idx_list.at(0), Qt::WhatsThisRole).toStringList();
	album.fromStringList( album_str_list );
	_album_of_interest = CDatabaseConnector::getInstance()->getAlbumByID(album.id);
	num_artists = _album_of_interest.artists.size();



	QString artist = _album_of_interest.artists[0];
	if(_album_of_interest.artists.size() > 1){
		artist = "Various artists";
	}

	vector<MetaData> tracks;
		CDatabaseConnector::getInstance()->getAllTracksByAlbum(_album_of_interest.id, tracks, _cur_searchstring, this->_sort_albums);
		QStringList album_paths;
		foreach(MetaData md, tracks){

			QString filepath = md.filepath;
			QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
			filepath = filepath.replace(lib_path, ".");
			filepath = filepath.left(filepath.lastIndexOf(QDir::separator()));
			if(!album_paths.contains(filepath, Qt::CaseInsensitive)){
				album_paths.push_back(filepath);
			}
		}


	_album_msg_box->setText(QString("<b>") + artist + " - " + album.name + " (" + QString::number(_album_of_interest.artists.size()) + ")</b>");

	QString year =  QString::number(_album_of_interest.year);
	if(_album_of_interest.year == 0) year = "Unknown";
	QString info_text = QString::number(_album_of_interest.num_songs) + " tracks\n" +
						"Playing time: " + Helper::cvtMsecs2TitleLengthString(_album_of_interest.length_sec * 1000) + "\n" +
						"Year: " + year + "\n\n";

	foreach(QString album_path, album_paths){
		info_text += album_path + "\n";
	}


	if(_album_of_interest.is_sampler){
		cover_path = Helper::get_cover_path("", _album_of_interest.name);
		_album_of_interest.artists.clear();
		_album_of_interest.artists.push_back("");
	}

	else
		cover_path = Helper::get_cover_path(_album_of_interest.artists[0], _album_of_interest.name);

	pm = QPixmap(cover_path);
	cover_found = !pm.isNull();


	if(cover_found){

		if(_album_of_interest.is_sampler){
			apply = _album_msg_box->addButton("Apply cover to entire album", QMessageBox::ActionRole);
			connect(apply, SIGNAL(pressed()), this, SLOT(apply_cover_to_entire_album()));
		}

		pm = pm.scaledToWidth(150, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);

	}

	else{
		qDebug() << Q_FUNC_INFO << " No cover found...";
		MetaData md;
		md.album_id = _album_of_interest.id;
		md.album = _album_of_interest.name;
		md.artist = _album_of_interest.artists[0];

		emit search_cover(md);
	}

	_album_msg_box->setInformativeText(info_text);
	_album_msg_box->exec();
	_album_msg_box->close();

}


void GUI_Library_windowed::delete_album(){

	if(_selected_album == -1) return;
	QStringList file_list;
	vector<MetaData> vec_md;
	CDatabaseConnector::getInstance()->getAllTracksByAlbum(_selected_album, vec_md, _cur_searchstring, _sort_tracks);

	deleteSomeTracks(vec_md);
}


void GUI_Library_windowed::edit_artist(){
	emit data_for_id3_change(_v_metadata);
}


void GUI_Library_windowed::info_artist(){

	if(_album_msg_box) delete _album_msg_box;
		_album_msg_box = new QMessageBox();
	QModelIndexList idx_list = this->ui->lv_artist->selectionModel()->selectedRows(1);
	if(idx_list.size() <= 0 || !idx_list.at(0).isValid()) return;

	Artist artist;
	QStringList artist_str_list = _artist_model->data(idx_list.at(0), Qt::WhatsThisRole).toStringList();
	artist.fromStringList( artist_str_list );


	vector<MetaData> tracks;
	CDatabaseConnector::getInstance()->getAllTracksByArtist(artist.id, tracks, _cur_searchstring, _sort_tracks);
	QStringList artist_paths;
	foreach(MetaData md, tracks){

		QString filepath = md.filepath;
		QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
		filepath = filepath.replace(lib_path, ".");
		filepath = filepath.left(filepath.lastIndexOf(QDir::separator()));
		if(!artist_paths.contains(filepath, Qt::CaseInsensitive)){
			artist_paths.push_back(filepath);
		}
	}

	QString info_text = QString::number(artist.num_albums) + " albums\n" +
						QString::number(artist.num_songs) + " songs\n\n";

	foreach(QString path, artist_paths){
		info_text += QString(path + "\n");
	}

	int rnd_album = rand() % _v_albums.size();

	_album_of_interest = _v_albums[rnd_album];
	_album_of_interest.artists.clear();
	_album_of_interest.artists.push_back(artist.name);
	_album_of_interest.name = _v_albums[rnd_album].name;

	QString cover_path = Helper::get_cover_path(artist.name, _album_of_interest.name);
	if(!QFile::exists(cover_path)){
		qDebug() << "Cover path " << cover_path << " does not exist";
	}

	QPixmap pm = QPixmap(cover_path);
	bool cover_found = !pm.isNull();

	if(cover_found){
		pm = pm.scaledToWidth(150, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);
	}

	else{
		qDebug() << Q_FUNC_INFO << " No cover found...";
		MetaData md;

		md.album_id = _album_of_interest.id;
		md.album = _album_of_interest.name;
		md.artist = artist.name;

		emit search_cover(md);
	}

	_album_msg_box->setInformativeText(info_text);
	_album_msg_box->exec();
	_album_msg_box->close();

}


void GUI_Library_windowed::delete_artist(){

	if(_selected_artist == -1) return;
	vector<MetaData> vec_md;
	CDatabaseConnector::getInstance()->getAllTracksByArtist(_selected_artist, vec_md, _cur_searchstring, _sort_tracks);
	deleteSomeTracks(vec_md);

}


void GUI_Library_windowed::edit_tracks(){
	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);
	vector<MetaData> vec_md;

	for(int i=0; i<idx_list.size(); i++){
		int row = idx_list[i].row();
		if(row < (int) _v_metadata.size()){
			vec_md.push_back(_v_metadata[row]);
		}
	}

	emit data_for_id3_change(vec_md);
}

void GUI_Library_windowed::info_tracks(){
	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);
	QMessageBox box;

	if(idx_list.size() == 0 ) return;
	else if(idx_list.size() == 1 && idx_list.at(0).isValid()){
		MetaData md =  _v_metadata[idx_list.at(0).row()];
		QString title = md.title;

		box.setText(QString("<b>") + title + "</b>");

		QString artist = md.artist;
		QString album = md.album;
		QString length = Helper::cvtMsecs2TitleLengthString(md.length_ms);
		QString year = QString::number(md.year);
		if(md.year == 0) year = "Unknown";
		QString bitrate = QString::number(md.bitrate / 1000) + " kBit/s";
		QString filepath = md.filepath;
		QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
		filepath = filepath.replace(lib_path, QString(lib_path + "<br />"));
		filepath = filepath.left(filepath.lastIndexOf(QDir::separator()));
		QString track_num = QString::number(md.track_num);


		box.setInformativeText(	QString("<b>Artist:</b>" ) + artist + "<br />" +
								"<b>Album:</b> " + album + " ("+ track_num +")<br />" +
								"<b>Length:</b> " + length + QString("<br />") +
								"<b>Year:</b> " + year + QString("<br />") +
								"<b>Bitrate:</b> " + bitrate + QString("<br /><br />") +
								filepath);


		QPixmap pm = QPixmap(Helper::get_cover_path(artist, album));
		pm = pm.scaledToWidth(150, Qt::SmoothTransformation);
		box.setIconPixmap(pm);
	}

	else if(idx_list.size() > 1){

		QString album = "";
		QString artist = "";
		QString year = "";
		QString bitrate = "";

		QString tracks = QString::number(idx_list.size()) + " tracks";

		box.setText(tracks);

		for(int i=0; i<idx_list.size(); i++){
			QModelIndex idx = idx_list[i];
			if(!idx.isValid()) continue;

			MetaData md =  _v_metadata[idx_list.at(i).row()];
			if(album == "") album = md.album;
			if(artist == "") artist = md.artist;
			if(year == "" && md.year != 0) year = QString::number(md.year);
			if(bitrate == "") bitrate = QString::number(md.bitrate);

			if(album != md.album) album = "various albums";
			if(artist != md.artist) artist = "various artists";
			if(year != QString::number(md.year)) year = "-1";
			if(bitrate != QString::number(md.bitrate)) bitrate = "-1";
		}

		QString info_text = "<b>Artist:</b> " + artist + "<br />" +
							"<b>Album:</b> " + album;
		if(year != "-1"){
			info_text += "<br /><b>Year:</b> " + year;
		}


		if(bitrate != "-1"){
			info_text += "<br /><b>Bitrate:</b> " + bitrate;
		}

		box.setInformativeText(info_text);
		if(artist != "various artists" && album != "various albums"){

			QPixmap pm = QPixmap(Helper::get_cover_path(artist, album));
			pm = pm.scaledToWidth(150, Qt::SmoothTransformation);
			box.setIconPixmap(pm);
		}
	}

	box.exec();
	box.close();


}

void GUI_Library_windowed::delete_tracks(){


	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);

	vector<MetaData> vec_md;
	foreach(QModelIndex idx, idx_list){
		int row = idx.row();
		vec_md.push_back(_v_metadata.at(row));
	}

	deleteSomeTracks(vec_md);

}



void GUI_Library_windowed::deleteSomeTracks(vector<MetaData>& vec_md){
	QMessageBox dialog;
	QString tl = this->ui->le_search->text();

	dialog.setFocus();
	dialog.setIcon(QMessageBox::Warning);
	dialog.setText("<b>Warning!</b>");
	dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	dialog.setDefaultButton(QMessageBox::No);

	QStringList file_list;
	foreach(MetaData md, vec_md){
		file_list.push_back(md.filepath);
	}


	int num_files_to_delete = vec_md.size();
	dialog.setInformativeText(	QString("You are about to delete ") +
								QString::number(num_files_to_delete) +
								" files!\nContinue?" );

	int answer = dialog.exec();
	dialog.close();

	int count_failure = 0;

	switch(answer){
		case QMessageBox::Yes:

			CDatabaseConnector::getInstance()->deleteTracks(vec_md);

			foreach(QString filename, file_list){
				QFile file(filename);
				if( !file.remove() )
					count_failure ++;
			}

			if(tl.size() > 0)
				text_line_edited(tl);

			else
				refresh();


			break;

		case QMessageBox::No:
			return;

		default: return;
	}


	QMessageBox answerbox;
	QString text;

	answerbox.setText("Info");
	answerbox.setIcon(QMessageBox::Information);

	if(!count_failure){
		text = "All files could be removed";
	}

	else{
		text = QString::number(count_failure) +
				" of " + QString::number(file_list.size()) +
				" files could not be removed!";
	}


	answerbox.setInformativeText(text);
	answerbox.exec();
	answerbox.close();

}


void GUI_Library_windowed::cover_changed(bool success){
	if(!_album_msg_box || !success) return;

	QString cover_path = Helper::get_cover_path(_album_of_interest.artists[0], _album_of_interest.name);

	if(!QFile::exists(cover_path)){
		qDebug() << cover_path << " does not exist";
	}
	QPixmap pm = QPixmap(cover_path);
	if(!pm.isNull()){

		/*if(_album_of_interest.is_sampler){
			QPushButton* apply = _album_msg_box->addButton("Apply cover to entire album", QMessageBox::ActionRole);
			connect(apply, SIGNAL(pressed()), this, SLOT(apply_cover_to_entire_album()));
		}*/

		pm = pm.scaledToWidth(150, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);
	}
}
