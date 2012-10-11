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
#include "GUI/library/GUILibraryInfoBox.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"


#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/CustomMimeData.h"


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


#define INFO_IMG_SIZE 220


using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent, GUI_InfoDialog* dialog) : QWidget(parent) {

	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

	_sort_albums = AlbumNameAsc;
	_sort_artists = ArtistNameAsc;
	_sort_tracks = TrackArtistAsc;

	_info_dialog = dialog;
	_lib_info_dialog = new GUI_Library_Info_Box(this);

	_mime_data_artist = 0;
	_mime_data_album = 0;
	_mime_data = 0;

	this->_album_model = new LibraryItemModelAlbums();
	this->_album_delegate = new LibraryItemDelegateAlbums(this->ui->lv_album);
	this->_artist_model = new LibraryItemModelArtists();
	this->_artist_delegate = new LibraryItemDelegateArtists(this->ui->lv_artist);
	this->_track_model = new LibraryItemModelTracks();
	this->_track_delegate = new LibraryItemDelegateTracks(this->ui->tb_title);

	this->ui->tb_title->setModel(this->_track_model);
	this->ui->tb_title->setItemDelegate(this->_track_delegate);
	this->ui->tb_title->setAlternatingRowColors(true);
	this->ui->tb_title->setDragEnabled(true);

	this->ui->lv_artist->setModel(this->_artist_model);
	this->ui->lv_artist->setItemDelegate(this->_artist_delegate);
	this->ui->lv_artist->setAlternatingRowColors(true);
	this->ui->lv_artist->setDragEnabled(true);

	this->ui->lv_album->setModel(this->_album_model);
	this->ui->lv_album->setItemDelegate(this->_album_delegate);
	this->ui->lv_album->setAlternatingRowColors(true);
	this->ui->lv_album->setDragEnabled(true);

	this->ui->btn_clear->setIcon(QIcon(Helper::getIconPath() + "broom.png"));
	this->ui->btn_info->setIcon(QIcon(Helper::getIconPath() + "info.png"));

	init_menues();

	connect(this->ui->btn_clear, SIGNAL( clicked()), this, SLOT(clear_button_pressed()));
	connect(this->ui->btn_info, SIGNAL(clicked()), _lib_info_dialog, SLOT(psl_refresh()));

	connect(this->ui->le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));

	connect(this->ui->lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_dbl_clicked(const QModelIndex & )));
	connect(this->ui->lv_album, SIGNAL(pressed(const QModelIndex & )), this, SLOT(album_pressed(const QModelIndex & )));
	connect(this->ui->lv_album, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(show_album_context_menu(const QPoint&)));
	connect(this->ui->lv_album->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_albums_by_column(int)));
	connect(this->ui->lv_album, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(album_middle_clicked(const QPoint&)));

	connect(this->ui->tb_title, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_dbl_clicked(const QModelIndex & )));
	connect(this->ui->tb_title, SIGNAL(pressed ( const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex&)));
	connect(this->ui->tb_title, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(show_track_context_menu(const QPoint&)));
	connect(this->ui->tb_title->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_tracks_by_column(int)));
	connect(this->ui->tb_title, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(tracks_middle_clicked(const QPoint&)));

	connect(this->ui->lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_dbl_clicked(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(pressed(const QModelIndex & )), this, SLOT(artist_pressed(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(context_menu_emitted(const QPoint&)), this, SLOT(show_artist_context_menu(const QPoint&)));
	connect(this->ui->lv_artist->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_artists_by_column(int)));
	connect(this->ui->lv_artist, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(artist_middle_clicked(const QPoint&)));

	connect(this->ui->combo_searchfilter, SIGNAL(currentIndexChanged(int)), this, SLOT(searchfilter_changed(int)));

	int style = CSettingsStorage::getInstance()->getPlayerStyle();
	bool dark = (style == 1);
	change_skin(dark);

    hide();

}

GUI_Library_windowed::~GUI_Library_windowed() {
	delete _album_model;
	delete _album_delegate;
	delete _artist_model;
	delete _artist_delegate;
	delete _track_model;
	delete _track_delegate;
	delete _lib_info_dialog;
}

void GUI_Library_windowed::init_menues(){
	_right_click_menu = new QMenu(this);

	_info_action = new QAction(QIcon(Helper::getIconPath() + "info.png"), "Info", this);
	_edit_action = new QAction(QIcon(Helper::getIconPath() + "lyrics.png"), "Edit", this);
	_delete_action = new QAction(QIcon(Helper::getIconPath() + "delete.png"), "Delete", this);
	_play_next_action = new QAction(QIcon(Helper::getIconPath() + "fwd_orange.png"), "Play next", this);

	_right_click_menu->addAction(_info_action);
	_right_click_menu->addAction(_edit_action);
	_right_click_menu->addAction(_delete_action);
	_right_click_menu->addAction(_play_next_action);
}

void GUI_Library_windowed::show_artist_context_menu(const QPoint& p){

	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_artist()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_artist()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_artist()));
	connect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_artist()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_artist()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_artist()));
	disconnect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next()));
}

void GUI_Library_windowed::show_album_context_menu(const QPoint& p){
	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_album()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_album()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_album()));
	connect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_album()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_album()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_album()));
	disconnect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next()));
}

void GUI_Library_windowed::show_track_context_menu(const QPoint& p){

	connect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_tracks()));
	connect(_info_action, SIGNAL(triggered()), this, SLOT(info_tracks()));
	connect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_tracks()));
	connect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next_tracks()));

	this->_right_click_menu->exec(p);

	disconnect(_edit_action, SIGNAL(triggered()), this, SLOT(edit_tracks()));
	disconnect(_info_action, SIGNAL(triggered()), this, SLOT(info_tracks()));
	disconnect(_delete_action, SIGNAL(triggered()), this, SLOT(delete_tracks()));
	disconnect(_play_next_action, SIGNAL(triggered()), this, SLOT(play_next_tracks()));
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
    this->ui->lv_artist->setColumnWidth(1, this->ui->lv_artist->width() - 120);
    this->ui->lv_artist->setColumnWidth(2, 60);
}



void GUI_Library_windowed::fill_library_tracks(MetaDataList& v_metadata){

	if(_info_dialog)
		_info_dialog->setMetaData(v_metadata);

	this->_track_model->removeRows(0, this->_track_model->rowCount());
	this->_track_model->insertRows(0, v_metadata.size());

	MetaDataList v_md;
	for(uint i=0; i<v_metadata.size(); i++){
		MetaData md = v_metadata[i];
		v_md.push_back(md);

		QModelIndex idx = _track_model->index(i, 0);

		this->_track_model->setData(idx, md.toVariant(), Qt::EditRole);

	}
	_mime_data_artist = new CustomMimeData();
	_mime_data_album = new CustomMimeData();

	_mime_data_artist->setMetaData(v_md);
	_mime_data_album->setMetaData(v_md);

	this->ui->lv_artist->set_mime_data(_mime_data_artist);
	this->ui->lv_album->set_mime_data(_mime_data_album);
}


void GUI_Library_windowed::fill_library_albums(AlbumList& albums){

	this->_album_model->removeRows(0, this->_album_model->rowCount());
	this->_album_model->insertRows(0, albums.size()); // fake "all albums row"

	QModelIndex idx;

	for(uint i=0; i<albums.size(); i++){
		idx = this->_album_model->index(i, 1);
		QStringList data = albums.at(i).toStringList();

		this->_album_model->setData(idx, data, Qt::EditRole );
	}
}


void GUI_Library_windowed::fill_library_artists(ArtistList& artists){

	this->_artist_model->removeRows(0, this->_artist_model->rowCount());
	this->_artist_model->insertRows(0, artists.size());

	QModelIndex idx;

	for(uint i=0; i<artists.size(); i++){

		idx = this->_artist_model->index(i, 0);
		QStringList data = artists.at(i).toStringList();

		this->_artist_model->setData(idx, data, Qt::EditRole );
	}
}




void GUI_Library_windowed::artist_pressed(const QModelIndex& idx){

	if(!idx.isValid()) return;

	QModelIndexList idx_list = this->ui->lv_artist->selectionModel()->selectedRows();
	QList<int> idx_list_int;

	foreach(QModelIndex model_idx, idx_list){
		idx_list_int.push_back(model_idx.row());
	}

	emit sig_artist_pressed(idx_list_int);
}


void GUI_Library_windowed::album_pressed(const QModelIndex& idx){

	if(!idx.isValid()) return;

	QModelIndexList idx_list = this->ui->lv_album->selectionModel()->selectedRows();
	QList<int> idx_list_int;

	foreach(QModelIndex model_idx, idx_list){
		idx_list_int.push_back(model_idx.row());
	}

	emit sig_album_pressed(idx_list_int);
}


void GUI_Library_windowed::track_info_available(const MetaDataList& v_md){

	_mime_data = new CustomMimeData();
	_mime_data->setMetaData(v_md);

	this->ui->tb_title->set_mime_data(_mime_data);
	if(_info_dialog)
		_info_dialog->setMetaData(v_md);
}


void GUI_Library_windowed::track_pressed(const QModelIndex& idx){

	if(!idx.isValid()) return;

	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows();
	QList<int> idx_list_int;

	foreach(QModelIndex  model_idx, idx_list){
		idx_list_int.push_back( model_idx.row() );
	}

	emit sig_track_pressed(idx_list_int);
}



void GUI_Library_windowed::album_dbl_clicked(const QModelIndex & idx){
	emit sig_album_dbl_clicked();
}

void GUI_Library_windowed::artist_dbl_clicked(const QModelIndex & idx){
	emit sig_artist_dbl_clicked();
}

void GUI_Library_windowed::track_dbl_clicked(const QModelIndex& idx){
	emit sig_track_dbl_clicked(idx.row());
}



void GUI_Library_windowed::clear_button_pressed(){


	this->ui->le_search->setText("");
	text_line_edited("", true);
}


void GUI_Library_windowed::text_line_edited(const QString& search, bool force_emit){

	Filter filter;
	int idx = this->ui->combo_searchfilter->currentIndex();
	switch(idx){
		case 0:	filter.by_searchstring = BY_FULLTEXT; break;
		case 1: filter.by_searchstring = BY_FILENAME; break;
		case 2: filter.by_searchstring = BY_GENRE; break;
		default: filter.by_searchstring = BY_FULLTEXT; break;
	}

    if(search.size() < 3)
		filter.cleared = true;

    else{
        filter.filtertext = QString("%") + search + QString("%");
        filter.cleared = false;

    }

	_cur_searchfilter = filter;
    sig_filter_changed(filter);
}

void GUI_Library_windowed::searchfilter_changed(int idx){
	Q_UNUSED(idx);
	text_line_edited(_cur_searchfilter.filtertext, true);
}



void GUI_Library_windowed::refresh(){

	text_line_edited(_cur_searchfilter.filtertext, true);
}


void GUI_Library_windowed::id3_tags_changed(){
	refresh();
}

template <typename T>
void switch_sorters(T& srcdst, T src1, T src2){
	if(srcdst == src1) srcdst = src2;
	else srcdst = src1;
}


void GUI_Library_windowed::sort_albums_by_column(int col){

	if(col == 1)
		switch_sorters(_sort_albums, AlbumNameAsc, AlbumNameDesc);

	if(col == 2)
		switch_sorters(_sort_albums, AlbumYearAsc, AlbumYearDesc);

	emit sig_sortorder_changed(_sort_artists, _sort_albums, _sort_tracks);
}


void GUI_Library_windowed::sort_artists_by_column(int col){

	if(col == 1)
		switch_sorters(_sort_artists, ArtistNameAsc, ArtistNameDesc);

	if(col == 2)
		switch_sorters(_sort_artists, ArtistTrackcountAsc, ArtistTrackcountDesc);

	emit sig_sortorder_changed(_sort_artists, _sort_albums, _sort_tracks);
}

void GUI_Library_windowed::sort_tracks_by_column(int col){


		if(col == COL_TRACK_NUM)
			switch_sorters(_sort_tracks, Sort::TrackNumAsc, Sort::TrackNumDesc);


		else if(col == COL_TITLE)
			switch_sorters(_sort_tracks, TrackTitleAsc, TrackTitleDesc);

		else if(col == COL_ALBUM)
			switch_sorters(_sort_tracks, TrackAlbumAsc, TrackAlbumDesc);

		else if(col == COL_ARTIST)
			switch_sorters(_sort_tracks, TrackArtistAsc, TrackArtistDesc);

		else if(col == COL_YEAR)
			switch_sorters(_sort_tracks, TrackYearAsc, TrackYearDesc);

		else if(col == COL_LENGTH)
			switch_sorters(_sort_tracks, TrackLenghtAsc, TrackLengthDesc);


		else if(col == COL_BITRATE)
			switch_sorters(_sort_tracks, TrackBitrateAsc, TrackBitrateDesc);

	emit sig_sortorder_changed(_sort_artists, _sort_albums, _sort_tracks);
}


void GUI_Library_windowed::reloading_library(QString& str){

	QString final_str = QString("<b>") + str + "</b>";
	this->ui->lab_status->setText(final_str);

}


void GUI_Library_windowed::reloading_library_finished(){
	this->ui->lab_status->setText("");
	refresh();
}



void GUI_Library_windowed::edit_album(){
	if(!_info_dialog) return;

	_info_dialog->setMode(INFO_MODE_ALBUMS);
	_info_dialog->show(TAB_EDIT);
}

void GUI_Library_windowed::edit_artist(){
	if(!_info_dialog) return;

	_info_dialog->setMode(INFO_MODE_ARTISTS);
	_info_dialog->show(TAB_EDIT);
}

void GUI_Library_windowed::edit_tracks(){
	if(!_info_dialog) return;

	_info_dialog->setMode(INFO_MODE_TRACKS);
	_info_dialog->show(TAB_EDIT);
}


void GUI_Library_windowed::info_album(){
	if(!_info_dialog) return;
	_info_dialog->setMode(INFO_MODE_ALBUMS);
	_info_dialog->show(TAB_INFO);
}

void GUI_Library_windowed::info_artist(){
	if(!_info_dialog) return;
	_info_dialog->setMode(INFO_MODE_ARTISTS);
	_info_dialog->show(TAB_INFO);
}

void GUI_Library_windowed::info_tracks(){
	if(!_info_dialog) return;
	_info_dialog->setMode(INFO_MODE_TRACKS);
	_info_dialog->show(TAB_INFO);
}

void GUI_Library_windowed::play_next(){

	emit sig_play_next_all_tracks();

}

void GUI_Library_windowed::play_next_tracks(){
	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);
	QList<int> lst;
	foreach(QModelIndex idx, idx_list){
		lst.push_back(idx.row());
	}

	emit sig_play_next_tracks(lst);
}


void GUI_Library_windowed::psl_delete_answer(QString answer){
	QMessageBox answerbox;

	answerbox.setText("Info");
	answerbox.setIcon(QMessageBox::Information);
	answerbox.setInformativeText(answer);

	answerbox.exec();
	answerbox.close();
	refresh();
}


void GUI_Library_windowed::delete_album(){

	int n_tracks = this->_track_model->rowCount();
	int answer = show_delete_dialog(n_tracks);

	if(answer){
		emit sig_delete_tracks(answer);
	}
}

void GUI_Library_windowed::delete_artist(){

	int n_tracks = this->_track_model->rowCount();
	int answer = show_delete_dialog(n_tracks);

	if(answer){
		emit sig_delete_tracks(answer);
	}
}

void GUI_Library_windowed::delete_tracks(){

	QModelIndexList idx_list = this->ui->tb_title->selectionModel()->selectedRows(0);
	QList<int> lst;
	foreach(QModelIndex idx, idx_list){
		lst.push_back(idx.row());
	}

	int answer = show_delete_dialog(lst.size());

	if(answer)
		emit sig_delete_certain_tracks(lst, answer);
}


int GUI_Library_windowed::show_delete_dialog(int n_tracks){

		QMessageBox dialog;
		QString tl = this->ui->le_search->text();
		QAbstractButton* clicked_button;
		QPushButton* only_library_button;

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Warning);
		dialog.setText("<b>Warning!</b>");
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		only_library_button = dialog.addButton("Only from library", QMessageBox::AcceptRole);
		dialog.setDefaultButton(QMessageBox::No);

		dialog.setInformativeText(	QString("You are about to delete ") +
									QString::number(n_tracks) +
									" files!\nContinue?" );

		int answer = dialog.exec();
		clicked_button = dialog.clickedButton();
		dialog.close();


		if(answer == QMessageBox::No)
			return 0;

		if(answer == QMessageBox::Yes)
			return 1;

		if(clicked_button->text() == only_library_button->text()){
			return 2;
		}

		return 0;
}

void GUI_Library_windowed::artist_middle_clicked(const QPoint& pt){
	play_next();
}


void GUI_Library_windowed::album_middle_clicked(const QPoint& pt){
	play_next();
}

void GUI_Library_windowed::tracks_middle_clicked(const QPoint& pt){
	play_next_tracks();
}


void GUI_Library_windowed::library_changed(){
	refresh();
}


void GUI_Library_windowed::import_result(bool success){

	QString success_string;
	if(success){
		success_string = "Importing was successful"; CSettingsStorage::getInstance()->getPlayerStyle();
	}

	else success_string = "Importing failed";

	//QMessageBox::information(NULL, "Information", success_string );
	library_changed();
}
