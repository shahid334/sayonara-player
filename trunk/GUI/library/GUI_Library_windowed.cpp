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

#define SEL_ARTISTS 0
#define SEL_ALBUMS 1
#define SEL_TRACKS 2

#include "GUI/ui_GUI_Library_windowed.h"

#include "GUI/MyColumnHeader.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/library/models/LibraryItemModelTracks.h"
#include "GUI/library/delegates/LibraryItemDelegateTracks.h"
#include "GUI/library/models/LibraryItemModelAlbums.h"
#include "GUI/library/delegates/LibraryItemDelegateAlbums.h"
#include "GUI/library/delegates/LibraryItemDelegateArtists.h"
#include "GUI/library/models/LibraryItemModelArtists.h"
#include "GUI/library/InfoBox/GUILibraryInfoBox.h"
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
#include "HelperStructs/Style.h"

#include <QList>
#include <QTimer>
#include <QCursor>
#include <QDebug>
#include <QPoint>
#include <QMouseEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QPalette>
#include <QBrush>
#include <QScrollBar>
#include <QItemSelectionModel>
#include <QHeaderView>






#define INFO_IMG_SIZE 220


using namespace std;

GUI_Library_windowed::GUI_Library_windowed(QWidget* parent) : QWidget(parent) {

	this->ui = new Ui::Library_windowed();
	this->ui->setupUi(this);

    _settings = CSettingsStorage::getInstance();



	_lib_info_dialog = new GUI_Library_Info_Box(this);


    QList<ColumnHeader> track_columns;
    QList<ColumnHeader> album_columns;
    QList<ColumnHeader> artist_columns;

    ColumnHeader t_h0("#", true, Sort::TrackNumAsc, Sort::TrackNumDesc, 25);
    ColumnHeader t_h1("Title", false, Sort::TrackTitleAsc, Sort::TrackTitleDesc, 0.4, 200);
    ColumnHeader t_h2("Artist", true, Sort::TrackArtistAsc, Sort::TrackArtistDesc, 0.3, 160);
    ColumnHeader t_h3("Album", true, Sort::TrackAlbumAsc, Sort::TrackAlbumDesc, 0.3, 160);
    ColumnHeader t_h4("D#", true, Sort::TrackDiscnumberAsc, Sort::TrackDiscnumberDesc, 25);
    ColumnHeader t_h5("Year", true, Sort::TrackYearAsc, Sort::TrackYearDesc, 50);
    ColumnHeader t_h6("Dur.", true, Sort::TrackLenghtAsc, Sort::TrackLengthDesc, 50);
    ColumnHeader t_h7("Bitrate", true, Sort::TrackBitrateAsc, Sort::TrackBitrateDesc, 75);
    ColumnHeader t_h8("Filesize", true, Sort::TrackSizeAsc, Sort::TrackSizeDesc, 75);

    ColumnHeader al_h0("#", true, Sort::NoSorting, Sort::NoSorting, 20);
    ColumnHeader al_h1("Album", false, Sort::AlbumNameAsc, Sort::AlbumNameDesc, 1.0, 160);
    ColumnHeader al_h2("Duration", true, Sort::AlbumDurationAsc, Sort::AlbumDurationDesc, 90);
    ColumnHeader al_h3("#Tracks", true, Sort::AlbumTracksAsc, Sort::AlbumTracksDesc, 80);
    ColumnHeader al_h4("Year", true, Sort::AlbumYearAsc, Sort::AlbumYearDesc, 50);

    ColumnHeader ar_h0("#", true, Sort::NoSorting, Sort::NoSorting, 20);
    ColumnHeader ar_h1("Artist", false, Sort::ArtistNameAsc, Sort::ArtistNameDesc, 1.0, 160 );
    ColumnHeader ar_h2("#Tracks", true, Sort::ArtistTrackcountAsc, Sort::ArtistTrackcountDesc, 80);

    track_columns  << t_h0  << t_h1  << t_h2  << t_h3  << /*t_h4  <<*/ t_h5  << t_h6  << t_h7 << t_h8;
    album_columns  << al_h0 << al_h1 << al_h2 << al_h3 << al_h4;
    artist_columns << ar_h0 << ar_h1 << ar_h2;

    _shown_cols_tracks = _settings->getLibShownColsTitle();
    _shown_cols_artist = _settings->getLibShownColsArtist();
    _shown_cols_albums = _settings->getLibShownColsAlbum();


	QList<int> sorting = _settings->getLibSorting();

    _sort_albums = (Sort::SortOrder) sorting[1];
    _sort_artists = (Sort::SortOrder) sorting[0];
    _sort_tracks = (Sort::SortOrder) sorting[2];



    this->_album_model = new LibraryItemModelAlbums(album_columns);
    this->_album_delegate = new LibraryItemDelegateAlbums(_album_model, this->ui->lv_album);
	this->_artist_model = new LibraryItemModelArtists(artist_columns);
    this->_artist_delegate = new LibraryItemDelegateArtists(_artist_model, this->ui->lv_artist);
    this->_track_model = new LibraryItemModelTracks(track_columns);
    this->_track_delegate = new LibraryItemDelegateTracks(_track_model, this->ui->tb_title);


	this->ui->tb_title->setModel(_track_model);
    this->ui->tb_title->setItemDelegate(_track_delegate);
	this->ui->tb_title->setAlternatingRowColors(true);
	this->ui->tb_title->setDragEnabled(true);
    this->ui->tb_title->set_table_headers(track_columns, _sort_tracks);
    this->ui->tb_title->rc_header_menu_init(_shown_cols_tracks);


	this->ui->lv_artist->setModel(_artist_model);
	this->ui->lv_artist->setItemDelegate(_artist_delegate);
	this->ui->lv_artist->setAlternatingRowColors(true);
	this->ui->lv_artist->setDragEnabled(true);
    this->ui->lv_artist->set_table_headers(artist_columns, _sort_artists);
    this->ui->lv_artist->rc_header_menu_init(_shown_cols_artist);

	this->ui->lv_album->setModel(this->_album_model);
	this->ui->lv_album->setItemDelegate(this->_album_delegate);
	this->ui->lv_album->setAlternatingRowColors(true);
	this->ui->lv_album->setDragEnabled(true);
    this->ui->lv_album->set_table_headers(album_columns, _sort_albums);
    this->ui->lv_album->rc_header_menu_init(_shown_cols_albums);

	_discmenu = 0;
	_timer = new QTimer(this);



	connect(_timer, SIGNAL(timeout()), this, SLOT(timer_timed_out()));


	connect(this->ui->lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_dbl_clicked(const QModelIndex & )));
    connect(this->ui->lv_album, SIGNAL(sig_all_selected()), this, SLOT(album_pressed()));
    connect(this->ui->lv_album, SIGNAL(pressed(const QModelIndex & )), this, SLOT(album_pressed(const QModelIndex & )));
    connect(this->ui->lv_album, SIGNAL(clicked(const QModelIndex & )), this, SLOT(album_released(const QModelIndex & )));
	connect(this->ui->lv_album, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(album_middle_clicked(const QPoint&)));
    connect(this->ui->lv_album, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_album_changed(Sort::SortOrder)));
    connect(this->ui->lv_album, SIGNAL(sig_columns_changed(QStringList&)), this, SLOT(columns_album_changed(QStringList&)));
    connect(this->ui->lv_album, SIGNAL(sig_edit_clicked()), this, SLOT(edit_album()));
    connect(this->ui->lv_album, SIGNAL(sig_info_clicked()), this, SLOT(info_album()));
    connect(this->ui->lv_album, SIGNAL(sig_delete_clicked()), this, SLOT(delete_album()));
    connect(this->ui->lv_album, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next()));
    connect(this->ui->lv_album, SIGNAL(sig_no_disc_menu()), this, SLOT(delete_menu()));

	connect(this->ui->tb_title, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_dbl_clicked(const QModelIndex & )));
    connect(this->ui->tb_title, SIGNAL(sig_all_selected ()), this, SLOT(track_pressed()));
    connect(this->ui->tb_title, SIGNAL(pressed ( const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex&)));
    connect(this->ui->tb_title, SIGNAL(clicked(const QModelIndex & )), this, SLOT(track_pressed(const QModelIndex & )));
	connect(this->ui->tb_title, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(tracks_middle_clicked(const QPoint&)));
    connect(this->ui->tb_title, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_title_changed(Sort::SortOrder)));
    connect(this->ui->tb_title, SIGNAL(sig_columns_changed(QStringList&)), this, SLOT(columns_title_changed(QStringList&)));
    connect(this->ui->tb_title, SIGNAL(sig_edit_clicked()), this, SLOT(edit_tracks()));
    connect(this->ui->tb_title, SIGNAL(sig_info_clicked()), this, SLOT(info_tracks()));
    connect(this->ui->tb_title, SIGNAL(sig_delete_clicked()), this, SLOT(delete_tracks()));
    connect(this->ui->tb_title, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_tracks()));


	connect(this->ui->lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_dbl_clicked(const QModelIndex & )));
    connect(this->ui->lv_artist, SIGNAL(sig_all_selected()), this, SLOT(artist_pressed()));
	connect(this->ui->lv_artist, SIGNAL(pressed(const QModelIndex & )), this, SLOT(artist_pressed(const QModelIndex & )));
    connect(this->ui->lv_artist, SIGNAL(clicked(const QModelIndex & )), this, SLOT(artist_pressed(const QModelIndex & )));
	connect(this->ui->lv_artist, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(artist_middle_clicked(const QPoint&)));
    connect(this->ui->lv_artist, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_artist_changed(Sort::SortOrder)));
    connect(this->ui->lv_artist, SIGNAL(sig_columns_changed(QStringList&)), this, SLOT(columns_artist_changed(QStringList&)));
    connect(this->ui->lv_artist, SIGNAL(sig_edit_clicked()), this, SLOT(edit_artist()));
    connect(this->ui->lv_artist, SIGNAL(sig_info_clicked()), this, SLOT(info_artist()));
    connect(this->ui->lv_artist, SIGNAL(sig_delete_clicked()), this, SLOT(delete_artist()));
    connect(this->ui->lv_artist, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next()));

    

    this->ui->btn_clear->setIcon(QIcon(Helper::getIconPath() + "broom.png"));
    this->ui->btn_info->setIcon(QIcon(Helper::getIconPath() + "info.png"));

    connect(this->ui->btn_clear, SIGNAL( clicked()), this, SLOT(clear_button_pressed()));
    connect(this->ui->btn_info, SIGNAL(clicked()), _lib_info_dialog, SLOT(psl_refresh()));
    connect(this->ui->le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));
    connect(ui->le_search, SIGNAL(returnPressed()), this, SLOT(return_pressed()));
    connect(this->ui->combo_searchfilter, SIGNAL(currentIndexChanged(int)), this, SLOT(searchfilter_changed(int)));

    bool show_only_tracks = _settings->getLibShowOnlyTracks();
    this->ui->lv_artist->setVisible(!show_only_tracks);
    this->ui->lv_album->setVisible(!show_only_tracks);

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

void GUI_Library_windowed::set_info_dialog(GUI_InfoDialog *dialog){
    _info_dialog = dialog;
}

void GUI_Library_windowed::show_only_tracks(bool b){

	this->ui->lv_artist->setVisible(!b);
	this->ui->lv_album->setVisible(!b);
}


void GUI_Library_windowed::resizeEvent(QResizeEvent* e){

	Q_UNUSED(e);

    this->ui->lv_album->set_col_sizes();
    this->ui->lv_artist->set_col_sizes();
    this->ui->tb_title->set_col_sizes();


}

void  GUI_Library_windowed::columns_album_changed(QStringList& list){
    _shown_cols_albums = list;
    _settings->setLibShownColsAlbum(list);


}


void  GUI_Library_windowed::columns_artist_changed(QStringList& list){
    _shown_cols_artist = list;
    _settings->setLibShownColsArtist(list);
}


void  GUI_Library_windowed::columns_title_changed(QStringList & list){
    _shown_cols_tracks = list;
    _settings->setLibShownColsTitle(list);
}



void GUI_Library_windowed::fill_library_tracks(MetaDataList& v_metadata){

    this->ui->tb_title->fill_metadata(v_metadata);

    if(_info_dialog)
		_info_dialog->setMetaData(v_metadata);

    this->ui->lv_artist->set_mimedata(v_metadata, "tracks", true);
    this->ui->lv_album->set_mimedata(v_metadata, "tracks", true);
}


void GUI_Library_windowed::fill_library_albums(AlbumList& albums){

   this->ui->lv_album->fill_albums(albums);
}


void GUI_Library_windowed::fill_library_artists(ArtistList& artists){

    this->ui->lv_artist->fill_artists(artists);
}



void GUI_Library_windowed::artist_pressed(const QModelIndex& idx){

    this->_info_dialog->set_tag_edit_visible(true);
    QList<int> idx_list_int;

    idx_list_int = ui->lv_artist->calc_selections();

    emit sig_artist_pressed(idx_list_int);
}

void GUI_Library_windowed::artist_released(const QModelIndex& idx){}


void GUI_Library_windowed::disc_pressed(int disc){
    emit sig_disc_pressed(disc);
}

void GUI_Library_windowed::album_pressed(const QModelIndex& idx){
    _info_dialog->set_tag_edit_visible(true);
    QList<int> idx_list_int;
    idx_list_int = ui->lv_album->calc_selections();

    QList<int> discnumbers = _album_model->get_discnumbers(idx);

    if(discnumbers.size() > 1 && idx_list_int.size() == 1 ){
	delete_menu();
	_discmenu = new DiscPopupMenu(ui->lv_album, discnumbers);
	
	connect(_discmenu, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));
	_timer->start(300);
    } 
    
    emit sig_album_pressed(idx_list_int);
}


void GUI_Library_windowed::album_released(const QModelIndex& idx){
    album_pressed(idx);
    delete_menu();
    
}



void GUI_Library_windowed::track_pressed(const QModelIndex& idx){

    this->_info_dialog->set_tag_edit_visible(true);
    QList<int> idx_list_int;

    idx_list_int = ui->tb_title->calc_selections();
    emit sig_track_pressed(idx_list_int);

}

void GUI_Library_windowed::track_released(const QModelIndex&){}



void GUI_Library_windowed::track_info_available(const MetaDataList& v_md){

    this->ui->tb_title->set_mimedata(v_md, "tracks", false);
	if(_info_dialog)
		_info_dialog->setMetaData(v_md);
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


void GUI_Library_windowed::sortorder_artist_changed(Sort::SortOrder s){
    _sort_artists = s;
    emit sig_sortorder_changed(_sort_artists, _sort_albums, _sort_tracks);
}


void GUI_Library_windowed::sortorder_album_changed(Sort::SortOrder s){
    _sort_albums = s;
    emit sig_sortorder_changed(_sort_artists, _sort_albums, _sort_tracks);
}


void GUI_Library_windowed::sortorder_title_changed(Sort::SortOrder s){
    _sort_tracks = s;
    emit sig_sortorder_changed(_sort_artists, _sort_albums, _sort_tracks);
}


void GUI_Library_windowed::clear_button_pressed(){

	this->ui->le_search->setText("");
	text_line_edited("", true);
}

void GUI_Library_windowed::return_pressed(){

    text_line_edited(this->ui->le_search->text(), true);
}

void GUI_Library_windowed::text_line_edited(const QString& search, bool force_emit){

    if(!force_emit && !_settings->getLibLiveSheach()) return;

    QList<int> lst;
    _album_model->set_selected(lst);
    _track_model->set_selected(lst);
    _artist_model->set_selected(lst);

    if(search.toLower() == "f:"){
        this->ui->combo_searchfilter->setCurrentIndex(0);
        this->ui->le_search->setText("");
    }

    else if(search.toLower() == "g:") {
        this->ui->combo_searchfilter->setCurrentIndex(1);
        this->ui->le_search->setText("");
    }

    else if(search.toLower() == "p:") {
        this->ui->combo_searchfilter->setCurrentIndex(2);
        this->ui->le_search->setText("");
    }

	Filter filter;
	int idx = this->ui->combo_searchfilter->currentIndex();
	switch(idx){
		case 0:	filter.by_searchstring = BY_FULLTEXT; break;
        case 1: filter.by_searchstring = BY_GENRE; break;
        case 2: filter.by_searchstring = BY_FILENAME; break;

		default: filter.by_searchstring = BY_FULLTEXT; break;
	}

    if(search.size() < 3)
		filter.cleared = true;

    else{
        filter.filtertext = QString("%") + search + QString("%");
        filter.cleared = false;
    }

	_cur_searchfilter = filter;

    emit sig_filter_changed(filter);
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
        success_string = "Importing was successful"; _settings->getPlayerStyle();
	}

	else success_string = "Importing failed";

	//QMessageBox::information(NULL, "Information", success_string );
	library_changed();
}

void GUI_Library_windowed::delete_menu(){
	if(!_discmenu)return;
	
	_discmenu->hide();
	_discmenu->close();
		
    	disconnect(_discmenu, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));
	delete _discmenu;
	_discmenu = 0;
	
}

void GUI_Library_windowed::timer_timed_out(){
	_timer->stop();
	if(!_discmenu) return;

	QPoint p = QCursor::pos();
	_discmenu->popup(p);
	
	
}

void GUI_Library_windowed::change_skin(bool b){
if(!_album_delegate || !_artist_delegate || !_track_delegate) return;
    this->_album_delegate->set_skin(b);
    this->_artist_delegate->set_skin(b);
    this->_track_delegate->set_skin(b);


}
