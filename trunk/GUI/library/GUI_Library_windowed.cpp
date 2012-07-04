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
#include "HelperStructs/WebAccess.h"


#include "CoverLookup/CoverLookup.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "StreamPlugins/LastFM/LastFM.h"

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

#define INFO_IMG_SIZE 220


using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {

	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

	_db = CDatabaseConnector::getInstance();

	_album_msg_box = 0;
	this->_sort_albums = "name asc";
	this->_sort_artists = "name asc";
	this->_sort_tracks = "artist asc";

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

	connect(this->ui->lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_dbl_clicked(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_dbl_clicked(const QModelIndex & )));
	connect(this->ui->tb_title, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_dbl_clicked(const QModelIndex & )));

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

	Q_UNUSED(idx);

	_selected_artists.clear();
	_selected_albums.clear();

	QModelIndexList idx_list = this->ui->lv_artist->selectionModel()->selectedRows();

	vector<MetaData> v_md,  v_md_acc;
	vector<Album> v_albums_acc, v_albums;


	foreach(QModelIndex model_idx, idx_list){
		Artist artist = _v_artists.at(model_idx.row());
		_selected_artists.push_back(artist.id);
		_db->getAllTracksByArtist(artist.id, v_md, _cur_searchstring, _sort_tracks);
		_db->getAllAlbumsByArtist(artist.id, v_albums, _cur_searchstring, _sort_albums);
	}

	if(idx_list.size() == 1) {
		v_albums_acc = v_albums;
		v_md_acc = v_md;
	}
	else{
		for(uint i=0; i<v_md.size(); i++)
			v_md_acc.push_back(v_md[i]);

		for(uint i=0; i<v_albums.size(); i++)
			v_albums_acc.push_back(v_albums[i]);
	}


	fill_library_albums(v_albums_acc);
	fill_library_tracks(v_md_acc);

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

	Q_UNUSED(idx);

	_selected_albums.clear();

	QModelIndexList idx_list = this->ui->lv_album->selectionModel()->selectedRows();
	vector<MetaData> v_md;
	vector<MetaData> v_md_acc;

	foreach(QModelIndex model_idx, idx_list){
		Album album = _v_albums.at(model_idx.row());
		_selected_albums.push_back(album.id);
		_db->getAllTracksByAlbum(album.id, v_md, _cur_searchstring, _sort_tracks);
	}

	if(idx_list.size() == 1) v_md_acc = v_md;
	else{
		for(uint i=0; i<v_md.size(); i++){
			v_md_acc.push_back(v_md[i]);
		}
	}

	fill_library_tracks(v_md_acc);

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

	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows();

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



void GUI_Library_windowed::album_dbl_clicked(const QModelIndex & idx){

	int album_id = _v_albums.at(idx.row()).id;

	vector<MetaData> vec;

	_db->getAllTracksByAlbum(album_id, vec, _cur_searchstring, _sort_tracks);
	emit sig_album_chosen(vec);

	_everything_loaded = false;
}

void GUI_Library_windowed::artist_dbl_clicked(const QModelIndex & idx){

	int artist_id = _v_artists.at(idx.row()).id;

	vector<MetaData> vec;

	_db->getAllTracksByArtist(artist_id, vec, _cur_searchstring, _sort_tracks);
	emit sig_artist_chosen(vec);

	vec.clear();
	_everything_loaded = false;

}

void GUI_Library_windowed::track_dbl_clicked(const QModelIndex& idx){

	if(!idx.isValid()) return;

	vector<MetaData> vec;
	vec.push_back(_v_metadata.at(idx.row()));
	emit sig_track_chosen(vec);
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

	_selected_albums.clear();
	_selected_artists.clear();

	_cur_searchstring = "";

	this->ui->le_search->clear();
	_everything_loaded = false;
	text_line_edited(" ");

}

void GUI_Library_windowed::refresh(){

	vector<Album> vec_albums;
	vector<MetaData> vec_tracks;
	vector<Artist> vec_artists;

	_db->getTracksFromDatabase(vec_tracks, _sort_tracks);
	_db->getAllAlbums(vec_albums, _sort_albums);
	_db->getAllArtists(vec_artists, _sort_artists);

	fill_library_artists(vec_artists);
	fill_library_albums(vec_albums);
	fill_library_tracks(vec_tracks);

	_everything_loaded = true;

	if(this->ui->le_search->text().size() > 0)
		text_line_edited(this->ui->le_search->text());

}

void GUI_Library_windowed::text_line_edited(const QString& search){

	if( (search.length() < 3 && _everything_loaded) ) {
		_cur_searchstring = "";
		return;
	}

	vector<Album> vec_albums;
	vector<MetaData> vec_tracks;
	vector<Artist> vec_artists;

	if(search.length() < 3){

		_db->getTracksFromDatabase(vec_tracks, _sort_tracks);
		_db->getAllAlbums(vec_albums, _sort_albums);
		_db->getAllArtists(vec_artists, _sort_artists);
		fill_library_artists(vec_artists);
		fill_library_albums(vec_albums);
		fill_library_tracks(vec_tracks);

		_selected_albums.clear();
		_selected_artists.clear();

		_everything_loaded = true;
		_cur_searchstring = "";

		return;
	}

	// searchstring.size >= 3

	_everything_loaded = false;
	_cur_searchstring = QString("%") + search + "%";

	_db->getAllTracksBySearchString(_cur_searchstring, vec_tracks, _sort_tracks);
	fill_library_tracks(vec_tracks);

	_db->getAllAlbumsBySearchString(_cur_searchstring, vec_albums, _sort_albums);
	fill_library_albums(vec_albums);

	_db->getAllArtistsBySearchString(_cur_searchstring, vec_artists, _sort_artists);
	fill_library_artists(vec_artists);
}


void GUI_Library_windowed::change_skin(bool dark){


	if(dark){

		QString table_style = Style::get_tv_style(dark);
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

		QString table_style = Style::get_tv_style(dark);

		this->ui->lv_album->setStyleSheet(table_style);
		this->ui->lv_artist->setStyleSheet(table_style);
		this->ui->tb_title->setStyleSheet(table_style);

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

		_db->getAllAlbumsBySearchString(_cur_searchstring, vec_albums, _sort_albums);
	}

	else if(_selected_artists.size() > 0){

		foreach(int artist_id, _selected_artists){
			vector<Album> vec_albums_tmp;
			_db->getAllAlbumsByArtist(artist_id, vec_albums_tmp, _cur_searchstring, _sort_albums);
			for(uint i=0; i<vec_albums_tmp.size(); i++){
				vec_albums.push_back(vec_albums_tmp[i]);
			}
		}
	}

	else{
		_db->getAllAlbums(vec_albums, _sort_albums);
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

		_db->getAllArtistsBySearchString(_cur_searchstring, vec_artists, _sort_artists);
	}

	else{
		_db->getAllArtists(vec_artists, _sort_artists);
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

		// searchstring, no album selected, no artist selected
		if(!_cur_searchstring.isEmpty() && _selected_albums.size() == 0 && _selected_artists.size() == 0){
			_db->getAllTracksBySearchString( _cur_searchstring, vec_md, _sort_tracks);
		}

		// possible searchstring, album selected
		else if( _selected_albums.size() > 0 ){


			foreach(int album_id, _selected_albums){
				vector<MetaData> tmp_md;
				_db->getAllTracksByAlbum(album_id, tmp_md, _cur_searchstring, _sort_tracks);
				for(uint i=0; i<tmp_md.size(); i++){
					vec_md.push_back(tmp_md[i]);
				}
			}
		}

		// possible searchstring, artist selected
		else if( _selected_artists.size() > 0 ){

			foreach(int artist_id, _selected_artists){
				vector<MetaData> tmp_md;
				_db->getAllTracksByArtist(artist_id, tmp_md, _cur_searchstring, _sort_tracks);
				for(uint i=0; i<tmp_md.size(); i++){
					vec_md.push_back(tmp_md[i]);
				}
			}
		}

		// no album, no artist, no searchstring
		else{
			_db->getTracksFromDatabase(vec_md, _sort_tracks);
		}

		fill_library_tracks(vec_md);
}




void GUI_Library_windowed::reloading_library(int percent){
	if(percent == -2){
		this->ui->lab_status->setText("<b>(Delete missing items...)</b>");
	}

	else if(percent == -1)
		this->ui->lab_status->setText("<b>(Load tracks from harddisk...)</b>");

	else this->ui->lab_status->setText("<b>(Reloading: " + QString::number(percent) + QString("\%)") + QString("</b>"));
}

void GUI_Library_windowed::reloading_library_finished(){
	this->ui->lab_status->setText("");
}

void GUI_Library_windowed::library_should_be_reloaded(){
	//this->ui->btn_reload->setVisible(true);
}

void GUI_Library_windowed::reload_library_slot(){

	this->ui->btn_reload->setVisible(false);
	emit sig_reload_library();
}




void GUI_Library_windowed::edit_album(){

	emit sig_data_for_id3_change(_v_metadata);
}

void GUI_Library_windowed::apply_cover_to_entire_album(){

}


/* TODO: clean me up, it cannot be that hard to solve the sampler issue */
void GUI_Library_windowed::info_album(){

	LastFM* lfm = LastFM::getInstance();
	if(_selected_albums.size() == 0) return;

	if(_album_msg_box) delete _album_msg_box;
	_album_msg_box = new QMessageBox();

	QPixmap pm;
	QString cover_path = "";
	bool	cover_found;


	// info string
	QString msg_text, year_string;
	int min_year = 120000;
	int max_year = -120000;
	bool unknown_year = false;
	int num_songs = 0;
	qint64 length = 0;
	QString first_album_name, first_album_artist;
	int first_album_id;
	int num_albums = 0;
	int album_playcount = 0;

	foreach(int album_id, _selected_albums){

		QString artist;
		Album album =  _db->getAlbumByID(album_id);

		num_songs += album.num_songs;
		length += album.length_sec;

		if(album.year < min_year && album.year != 0) min_year = album.year;
		if(album.year > max_year && album.year != 0) max_year = album.year;
		if(album.year == 0) unknown_year = true;

		artist = album.artists[0];
		if(album.is_sampler)
			artist = "Various";

		album_playcount += lfm->get_album_info(artist, album.name).toInt();

		if(num_albums == 0){
			first_album_artist = artist;
			first_album_name = album.name;
			first_album_id = album.id;
			cover_path = Helper::get_cover_path(artist, album.name);
		}
		num_albums ++;

		msg_text += "<b>" + artist + ": " + album.name + "<b><br />";
	}

	msg_text += "<br />";

	if(_selected_albums.size() > 1){
		cover_path = Helper::getIconPath() + "append.png";
	}

	// album paths
	QStringList album_paths;
	QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
	foreach(MetaData md, _v_metadata){

		QString filepath = md.filepath;
		filepath = filepath.replace(lib_path, "<b>${ML}</b>");
		filepath = filepath.left(filepath.lastIndexOf(QDir::separator()));
		if(!album_paths.contains(filepath, Qt::CaseInsensitive)){
			album_paths.push_back(filepath);
		}
	}


	if(min_year != max_year){
		if(min_year < 3000)
			year_string += QString::number(min_year);
		if(max_year > 0) year_string += QString(" - ") + QString::number(max_year);
	}
	else{
		if(min_year > 0)
			year_string = QString::number(min_year);
	}
	if(unknown_year) year_string += ", Unknown";

	QString info_text = QString::number(num_songs) + " tracks<br />" +
						"<b>Playing time:</b> " + Helper::cvtMsecs2TitleLengthString(length * 1000) + "<br />" +
						"<b>Year:</b> " + year_string + "<br />";

			if(album_playcount > 0)
						info_text += "<b>LastFM playcount:</b> " + QString::number(album_playcount) + "<br />";

			info_text += "<br />";


	foreach(QString album_path, album_paths){
		info_text += album_path + "<br />";
	}

	pm = QPixmap(cover_path);
	cover_found = !pm.isNull();

	if(cover_found){
		pm = pm.scaledToWidth(INFO_IMG_SIZE, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);
	}

	else{

		cover_path = Helper::getIconPath() + "append.png";
		QPixmap pm_tmp = QPixmap(cover_path);
		pm_tmp = pm_tmp.scaledToWidth(INFO_IMG_SIZE, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm_tmp);


		MetaData md;
		md.album_id = first_album_id;
		md.album = first_album_name;
		md.artist = first_album_artist;

		emit sig_search_cover(md);
	}

	_album_msg_box->setText(msg_text);
	_album_msg_box->setInformativeText(info_text);
	_album_msg_box->exec();
	_album_msg_box->close();

}


void GUI_Library_windowed::delete_album(){

	if(_selected_albums.size() == 0) return;
	QStringList file_list;

	foreach(int album_id, _selected_albums){
		vector<MetaData> vec_md;
		_db->getAllTracksByAlbum(album_id, vec_md, _cur_searchstring, _sort_tracks);
		deleteSomeTracks(vec_md);
	}
}


void GUI_Library_windowed::edit_artist(){
	emit sig_data_for_id3_change(_v_metadata);
}


void GUI_Library_windowed::info_artist(){


	LastFM* lfm = LastFM::getInstance();

	QModelIndexList idx_list = this->ui->lv_artist->selectionModel()->selectedRows();

	if(idx_list.size() <= 0 || !idx_list.at(0).isValid()) return;

	QString msg_text, info_text;
	int num_albums = 0;
	int num_songs = 0;
	int num_artists = 0;
	bool various_artists = (_selected_artists.size() > 1);
	QString first_artist_name;
	QString cover_path = "";
	int num_plays = 0;
	foreach(int artist_id, _selected_artists){
		Artist artist = _db->getArtistByID(artist_id);
		msg_text += "<b>" + artist.name + "</b><br />";

		num_albums += artist.num_albums;
		num_songs += artist.num_songs;
		num_plays += lfm->get_artist_info(artist.name).toInt();

		if(num_artists == 0){
			first_artist_name = artist.name;
		}
	}

	msg_text += QString::number(num_albums) + " albums<br />";
	msg_text += QString::number(num_songs) + " tracks<br />";
	msg_text += QString::number(num_plays) + " LastFM plays";


	QStringList artist_paths;
	QString lib_path = CSettingsStorage::getInstance()->getLibraryPath();
	foreach(MetaData md, _v_metadata){

		QString filepath = md.filepath;
		filepath = filepath.replace(lib_path, "<b>${ML}</b>");
		filepath = filepath.left(filepath.lastIndexOf(QDir::separator()));
		if(!artist_paths.contains(filepath, Qt::CaseInsensitive)){
			artist_paths.push_back(filepath);
		}
	}

	foreach(QString path, artist_paths){
		info_text += QString(path) + "<br/>";
	}

	QImage img;

	if(various_artists){
		cover_path = Helper::getIconPath() + "append.png";
	}

	else {
		cover_path = Helper::get_artist_image_path(first_artist_name);
	}

	QPixmap pm = QPixmap(cover_path);
	bool cover_found = !pm.isNull();

	if(_album_msg_box)
		delete _album_msg_box;

	_album_msg_box = new QMessageBox();


	if(cover_found){
		pm = pm.scaledToWidth(INFO_IMG_SIZE, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);
	}

	emit sig_search_artist_image(first_artist_name);

	_album_msg_box->setText(msg_text);
	_album_msg_box->setInformativeText(info_text);
	_album_msg_box->exec();
	_album_msg_box->close();

}


void GUI_Library_windowed::delete_artist(){

	if(_selected_artists.size() == 0) return;

	foreach(int artist_id, _selected_artists){
		vector<MetaData> vec_md;
		_db->getAllTracksByArtist(artist_id, vec_md, _cur_searchstring, _sort_tracks);
		deleteSomeTracks(vec_md);
	}
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

	emit sig_data_for_id3_change(vec_md);
}

void GUI_Library_windowed::info_tracks(){
	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);

	LastFM* lfm = LastFM::getInstance();
	if(_album_msg_box)
		delete _album_msg_box;
	_album_msg_box = new QMessageBox();

	int playcount = 0;
	if(idx_list.size() == 0 ) return;
	else if(idx_list.size() == 1 && idx_list.at(0).isValid()){
		MetaData md =  _v_metadata[idx_list.at(0).row()];
		QString title = md.title;

		_album_msg_box->setText(QString("<b>") + title + "</b>");

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

		QMap<QString, QString> info;
		MetaData md_tmp;
			md_tmp.artist = artist;
			md_tmp.title = md.title;
		lfm->get_track_info(md_tmp, info, false);
		/*if(!info[LFM_TAG_USERPLAYCOUNT].isNull() && !info[LFM_TAG_USERPLAYCOUNT].isEmpty() ){
			playcount += info[LFM_TAG_USERPLAYCOUNT].toInt();
		}*/

		QString info_text = QString("<b>Artist:</b>" ) + artist + "<br />" +
				"<b>Album:</b> " + album + " ("+ track_num +")<br />" +
				"<b>Length:</b> " + length + QString("<br />");

		if(year != "-1"){
				info_text += "<b>Year:</b> " + year + QString("<br />");
		}

		if(bitrate != "-1"){
			info_text += "<b>Bitrate:</b> " + bitrate + QString("<br />");
		}

		if(playcount > 0){
			info_text += "<b>LastFM plays:</b> " + QString::number(playcount) + QString("<br />");
		}


		_album_msg_box->setInformativeText( info_text + "<br />" + filepath );

		if(QFile::exists(Helper::get_cover_path(artist, album))){
			QPixmap pm = QPixmap(Helper::get_cover_path(artist, album));
			pm = pm.scaledToWidth(INFO_IMG_SIZE, Qt::SmoothTransformation);
			_album_msg_box->setIconPixmap(pm);
			_album_msg_box->repaint();
		}

		else {
			emit sig_search_cover(md);
		}
	}

	else if(idx_list.size() > 1){

		QString album = "";
		QString artist = "";
		QString year = "";
		QString bitrate = "";

		QString tracks = QString::number(idx_list.size()) + " tracks";

		_album_msg_box->setText(tracks);

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

			QMap<QString, QString> info;
			MetaData md_tmp;
				md_tmp.artist = artist;
				md_tmp.title = md.title;
			lfm->get_track_info(md_tmp, info, false);
			/*if( !info[LFM_TAG_USERPLAYCOUNT].isNull() && !info[LFM_TAG_USERPLAYCOUNT].isEmpty() ){
				playcount += info[LFM_TAG_USERPLAYCOUNT].toInt();
			}*/
		}

		QString info_text = "<b>Artist:</b> " + artist + "<br />" +
							"<b>Album:</b> " + album;
		if(year != "-1"){
			info_text += "<br /><b>Year:</b> " + year;
		}


		if(bitrate != "-1"){
			info_text += "<br /><b>Bitrate:</b> " + bitrate;
		}

		if(playcount >= 0){
			info_text += "<br /><b>LastFM plays:</b> " + QString::number(playcount);
		}

		_album_msg_box->setInformativeText(info_text);
		QString cover_path;
		if(artist != "various artists" && album != "various albums"){
			cover_path = Helper::get_cover_path(artist, album);
			if(!QFile::exists(cover_path)) {
				emit sig_search_cover( _v_metadata[idx_list.at(0).row()]);
				cover_path = Helper::getIconPath() + "append.png";
			}

		}
		else {
			cover_path = Helper::getIconPath() + "append.png";
		}

		QPixmap pm = QPixmap(cover_path);
		pm = pm.scaledToWidth(INFO_IMG_SIZE, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);
	}

	_album_msg_box->exec();
	_album_msg_box->close();


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

			_db->deleteTracks(vec_md);

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


void GUI_Library_windowed::cover_changed(QString path){
	if(!_album_msg_box || path.size() == 0) return;

	if(!QFile::exists(path)){
		qDebug() << path << " does not exist";
	}

	QPixmap pm = QPixmap(path);
	if(!pm.isNull()){
		pm = pm.scaledToWidth(INFO_IMG_SIZE, Qt::SmoothTransformation);
		_album_msg_box->setIconPixmap(pm);
	}
}


void GUI_Library_windowed::library_changed(){

	QString search_string = _cur_searchstring;

	if(search_string.size() > 2){
		search_string = search_string.remove(0,1);
		search_string = search_string.remove(search_string.size() -1, 1);
	}

	clear_button_pressed();

	if(search_string.size() > 0){
		this->ui->le_search->setText(search_string);
		text_line_edited(search_string);
	}

	if(_selected_artists.size() > 0){
		for(uint i=0; i<_v_artists.size(); i++){
			for(int j=0; j<_selected_artists.size(); j++){
				if(_selected_artists[j] == _v_artists[i].id){
					QModelIndex idx = this->_artist_model->index(i, 0);
					this->ui->lv_artist->selectRow(i);
					artist_pressed(idx);
					break;
				}
			}

		}
	}

	if(_selected_albums.size() > 0){
		for(uint i=0; i<_v_albums.size(); i++){
			for(int j=0; j<_selected_albums.size(); j++){

				if(_selected_albums[j] == _v_albums[i].id){
					QModelIndex idx = this->_album_model->index(i, 0);
					this->ui->lv_album->selectRow(i);
					album_pressed(idx);
					break;
				}
			}
		}
	}

}


void GUI_Library_windowed::import_result(bool success){

	QString success_string;
	if(success){
		success_string = "Importing was successful";
	}

	else success_string = "Importing failed";

	QMessageBox::information(NULL, "Information", success_string );
	library_changed();
}
