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


#include "GUI/MyColumnHeader.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/library/model/LibraryItemModelTracks.h"
#include "GUI/library/model/LibraryItemModelArtists.h"
#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "GUI/library/delegate/LibraryItemDelegateTracks.h"
#include "GUI/library/delegate/LibraryItemDelegateAlbums.h"
#include "GUI/library/delegate/LibraryItemDelegateArtists.h"
#include "GUI/library/InfoBox/GUILibraryInfoBox.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"

#include "HelperStructs/Helper.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/CustomMimeData.h"
#include <QMessageBox>

#include "CoverLookup/CoverLookup.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "StreamPlugins/LastFM/LastFM.h"
#include "HelperStructs/Style.h"


#include <QTimer>
#include <QCursor>
#include <QPoint>
#include <QEvent>
#include <QMouseEvent>
#include <QPixmap>

#include <QPalette>
#include <QBrush>
#include <QScrollBar>
#include <QItemSelectionModel>
#include <QHeaderView>
#include <QFileDialog>


using namespace std;

GUI_Library_windowed::GUI_Library_windowed(CLibraryBase* library, GUI_InfoDialog* info_dialog, QWidget* parent) :
	SayonaraWidget(parent),
	Ui::Library_windowed()
{

	setupUi(this);

	_library = library;
	_info_dialog = info_dialog;

	_lib_info_dialog = new GUI_Library_Info_Box(this);

	_shown_cols_albums = _settings->get(Set::Lib_ColsAlbum);
	_shown_cols_artist = _settings->get(Set::Lib_ColsArtist);
	_shown_cols_tracks = _settings->get(Set::Lib_ColsTitle);

    _album_model = 0;
    _artist_model = 0;
    _track_model = 0;
    _album_delegate = 0;
    _artist_delegate = 0;
    _track_delegate = 0;

    init_headers();

	_discmenu = 0;
	_timer = new QTimer(this);
    setAcceptDrops(true);


    QAction* clear_search_action = new QAction("Clear search", this);
    QKeySequence sequence_clear(tr("Ctrl+."));
    clear_search_action->setShortcut(sequence_clear);
    clear_search_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(clear_search_action, SIGNAL(triggered()), btn_clear, SLOT(click()));
	connect(clear_search_action, SIGNAL(triggered()), le_search, SLOT(clear()));
	addAction(clear_search_action);

    QAction* search_action = new QAction("Search", this);
    QKeySequence sequence_search(tr("Ctrl+?"));

    search_action->setShortcut(sequence_search);
    search_action->setShortcutContext(Qt::WindowShortcut);
	connect(search_action, SIGNAL(triggered()), le_search, SLOT(setFocus()));
	addAction(search_action);


	connect(_library, SIGNAL(sig_all_artists_loaded(const ArtistList&)), this, SLOT(lib_fill_artists(const ArtistList&)));
	connect(_library, SIGNAL(sig_all_albums_loaded(const AlbumList&)), this, SLOT(lib_fill_albums(const AlbumList&)));
	connect(_library, SIGNAL(sig_all_tracks_loaded(const MetaDataList&)), this,	SLOT(lib_fill_tracks(const MetaDataList&)));
	connect(_library, SIGNAL(sig_track_mime_data_available(const MetaDataList&)), this, SLOT(track_info_available(const MetaDataList&)));
	connect(_library, SIGNAL(sig_delete_answer(QString)), this, SLOT(lib_delete_answer(QString)));
	connect(_library, SIGNAL(sig_reloading_library(const QString&)), this, SLOT(lib_reload(const QString&)));
	connect(_library, SIGNAL(sig_reload_library_finished()), this, SLOT(lib_reload_finished()));
	connect(_library, SIGNAL(sig_no_library_path()), this, SLOT(lib_no_lib_path()));


	connect(_timer, SIGNAL(timeout()), this, SLOT(timer_timed_out()));

	connect(lv_album, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(album_dbl_clicked(const QModelIndex & )));
	connect(lv_album, SIGNAL(sig_sel_changed(const QList<int> & )), this, SLOT(album_sel_changed(const QList<int>&)));
	connect(lv_album, SIGNAL(sig_released()), this, SLOT(album_released()));
	connect(lv_album, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(album_middle_clicked(const QPoint&)));
	connect(lv_album, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_album_changed(Sort::SortOrder)));
	connect(lv_album, SIGNAL(sig_columns_changed(QList<int>&)), this, SLOT(columns_album_changed(QList<int>&)));
	connect(lv_album, SIGNAL(sig_edit_clicked()), this, SLOT(edit_album()));
	connect(lv_album, SIGNAL(sig_info_clicked()), this, SLOT(info_album()));
	connect(lv_album, SIGNAL(sig_delete_clicked()), this, SLOT(delete_album()));
	connect(lv_album, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next()));
	connect(lv_album, SIGNAL(sig_append_clicked()), this, SLOT(append()));
	connect(lv_album, SIGNAL(sig_no_disc_menu()), this, SLOT(delete_menu()));
	connect(lv_album, SIGNAL(sig_import_files(const QStringList&)), this, SLOT(import_files(const QStringList&)));

	connect(tb_title, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(track_dbl_clicked(const QModelIndex & )));
	connect(tb_title, SIGNAL(sig_sel_changed(const QList<int> & )), this, SLOT(track_sel_changed(const QList<int>&)));
	connect(tb_title, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(tracks_middle_clicked(const QPoint&)));
	connect(tb_title, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_title_changed(Sort::SortOrder)));
	connect(tb_title, SIGNAL(sig_columns_changed(QList<int>&)), this, SLOT(columns_title_changed(QList<int>&)));
	connect(tb_title, SIGNAL(sig_edit_clicked()), this, SLOT(edit_tracks()));
	connect(tb_title, SIGNAL(sig_info_clicked()), this, SLOT(info_tracks()));
	connect(tb_title, SIGNAL(sig_delete_clicked()), this, SLOT(delete_tracks()));
	connect(tb_title, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next_tracks()));
	connect(tb_title, SIGNAL(sig_append_clicked()), this, SLOT(append_tracks()));
	connect(tb_title, SIGNAL(sig_import_files(const QStringList&)), this, SLOT(import_files(const QStringList&)));

	connect(lv_artist, SIGNAL(doubleClicked(const QModelIndex & )), this, SLOT(artist_dbl_clicked(const QModelIndex & )));
	connect(lv_artist, SIGNAL(sig_sel_changed(const QList<int> & )), this, SLOT(artist_sel_changed(const QList<int>&)));
	connect(lv_artist, SIGNAL(sig_middle_button_clicked(const QPoint&)), this, SLOT(artist_middle_clicked(const QPoint&)));
	connect(lv_artist, SIGNAL(sig_sortorder_changed(Sort::SortOrder)), this, SLOT(sortorder_artist_changed(Sort::SortOrder)));
	connect(lv_artist, SIGNAL(sig_columns_changed(QList<int>&)), this, SLOT(columns_artist_changed(QList<int>&)));
	connect(lv_artist, SIGNAL(sig_edit_clicked()), this, SLOT(edit_artist()));
	connect(lv_artist, SIGNAL(sig_info_clicked()), this, SLOT(info_artist()));
	connect(lv_artist, SIGNAL(sig_delete_clicked()), this, SLOT(delete_artist()));
	connect(lv_artist, SIGNAL(sig_play_next_clicked()), this, SLOT(play_next()));
	connect(lv_artist, SIGNAL(sig_append_clicked()), this, SLOT(append()));
	connect(lv_artist, SIGNAL(sig_import_files(const QStringList&)), this, SLOT(import_files(const QStringList&)));

	connect(btn_refresh, SIGNAL(clicked()), this, SLOT(refresh()));

	connect(btn_clear, SIGNAL( clicked()), this, SLOT(clear_button_pressed()));
	connect(btn_info, SIGNAL(clicked()), _lib_info_dialog, SLOT(psl_refresh()));
	connect(le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));
	connect(le_search, SIGNAL(returnPressed()), this, SLOT(return_pressed()));
	connect(combo_searchfilter, SIGNAL(currentIndexChanged(int)), this, SLOT(searchfilter_changed(int)));

	bool show_only_tracks =_settings->get(Set::Lib_OnlyTracks);

	lv_artist->setVisible(!show_only_tracks);
	lv_album->setVisible(!show_only_tracks);

	REGISTER_LISTENER(Set::Lib_OnlyTracks, _sl_show_only_tracks_changed);

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


void  GUI_Library_windowed::init_headers() {

    if(_album_model) delete _album_model;
    if(_artist_model) delete _artist_model;
    if(_track_model) delete _track_model;

    if(_album_delegate) delete _album_delegate;
    if(_artist_delegate) delete _artist_delegate;
    if(_track_delegate) delete _track_delegate;

    LibSortOrder so = _settings->get(Set::Lib_Sorting);

    QList<ColumnHeader> track_columns;
    QList<ColumnHeader> album_columns;
    QList<ColumnHeader> artist_columns;

    _header_names_tracks << "#" << tr("Title") << tr("Artist") << tr("Album") << tr("Year") << tr("Dur.") << tr("Bitrate") << tr("Filesize") << tr("Rating");
    _header_names_albums << "#" << tr("Album") << tr("Duration") << tr("#Tracks") << tr("Year") << tr("Rating");
    _header_names_artists << "#" << tr("Artist") << tr("#Tracks");

    ColumnHeader t_h0(_header_names_tracks[0], true, Sort::TrackNumAsc, Sort::TrackNumDesc, 25);
    ColumnHeader t_h1(_header_names_tracks[1], false, Sort::TrackTitleAsc, Sort::TrackTitleDesc, 0.4, 200);
    ColumnHeader t_h2(_header_names_tracks[2], true, Sort::TrackArtistAsc, Sort::TrackArtistDesc, 0.3, 160);
    ColumnHeader t_h3(_header_names_tracks[3], true, Sort::TrackAlbumAsc, Sort::TrackAlbumDesc, 0.3, 160);
    ColumnHeader t_h4(_header_names_tracks[4], true, Sort::TrackYearAsc, Sort::TrackYearDesc, 50);
    ColumnHeader t_h5(_header_names_tracks[5], true, Sort::TrackLenghtAsc, Sort::TrackLengthDesc, 50);
    ColumnHeader t_h6(_header_names_tracks[6], true, Sort::TrackBitrateAsc, Sort::TrackBitrateDesc, 75);
    ColumnHeader t_h7(_header_names_tracks[7], true, Sort::TrackSizeAsc, Sort::TrackSizeDesc, 75);
    ColumnHeader t_h8(_header_names_tracks[8], true, Sort::TrackRatingAsc, Sort::TrackRatingDesc, 50);

    ColumnHeader al_h0(_header_names_albums[0], true, Sort::NoSorting, Sort::NoSorting, 20);
    ColumnHeader al_h1(_header_names_albums[1], false, Sort::AlbumNameAsc, Sort::AlbumNameDesc, 1.0, 160);
    ColumnHeader al_h2(_header_names_albums[2], true, Sort::AlbumDurationAsc, Sort::AlbumDurationDesc, 90);
    ColumnHeader al_h3(_header_names_albums[3], true, Sort::AlbumTracksAsc, Sort::AlbumTracksDesc, 80);
    ColumnHeader al_h4(_header_names_albums[4], true, Sort::AlbumYearAsc, Sort::AlbumYearDesc, 50);
    ColumnHeader al_h5(_header_names_albums[5], true, Sort::AlbumRatingAsc, Sort::AlbumRatingDesc, 50);

    ColumnHeader ar_h0(_header_names_artists[0], true, Sort::NoSorting, Sort::NoSorting, 20);
    ColumnHeader ar_h1(_header_names_artists[1], false, Sort::ArtistNameAsc, Sort::ArtistNameDesc, 1.0, 160 );
    ColumnHeader ar_h2(_header_names_artists[2], true, Sort::ArtistTrackcountAsc, Sort::ArtistTrackcountDesc, 80);

    track_columns  << t_h0  << t_h1  << t_h2  << t_h3  <<t_h4  << t_h5  << t_h6 << t_h7 << t_h8;
    album_columns  << al_h0 << al_h1 << al_h2 << al_h3 << al_h4 << al_h5;
    artist_columns << ar_h0 << ar_h1 << ar_h2;

    _album_model = new LibraryItemModelAlbums(album_columns);
	_album_delegate = new LibraryItemDelegateAlbums(_album_model, lv_album, true);
    _artist_model = new LibraryItemModelArtists(artist_columns);
	_artist_delegate = new LibraryItemDelegateArtists(_artist_model, lv_artist);
    _track_model = new LibraryItemModelTracks(track_columns);
	_track_delegate = new LibraryItemDelegateTracks(_track_model, tb_title, true);

    connect(_album_delegate, SIGNAL(sig_rating_changed(int)), this, SLOT(album_rating_changed(int)));
    connect(_track_delegate, SIGNAL(sig_rating_changed(int)), this, SLOT(title_rating_changed(int)));

    tb_title->setModel(_track_model);
	tb_title->setAbstractModel((AbstractSearchTableModel*) _track_model);
	tb_title->setItemDelegate(_track_delegate);
	tb_title->setAlternatingRowColors(true);
	tb_title->setDragEnabled(true);
    tb_title->set_table_headers(track_columns, so.so_tracks);
	tb_title->rc_header_menu_init(_shown_cols_tracks);


	lv_artist->setModel(_artist_model);
	lv_artist->setAbstractModel((AbstractSearchTableModel*) _artist_model);
	lv_artist->setItemDelegate(_artist_delegate);
	lv_artist->setAlternatingRowColors(true);
	lv_artist->setDragEnabled(true);
    lv_artist->set_table_headers(artist_columns, so.so_artists);
	lv_artist->rc_header_menu_init(_shown_cols_artist);


	lv_album->setModel(_album_model);
	lv_album->setAbstractModel((AbstractSearchTableModel*) _album_model);
	lv_album->setItemDelegate(_album_delegate);
	lv_album->setAlternatingRowColors(true);
	lv_album->setDragEnabled(true);
    lv_album->set_table_headers(album_columns, so.so_albums);
	lv_album->rc_header_menu_init(_shown_cols_albums);

}

void GUI_Library_windowed::language_changed() {

	retranslateUi(this);

    _header_names_tracks.clear();
    _header_names_albums.clear();
    _header_names_artists.clear();


    _header_names_tracks << "#" << tr("Title") << tr("Artist") << tr("Album") << tr("Year") << tr("Dur.") << tr("Bitrate") << tr("Filesize");
    _header_names_albums << "#" << tr("Album") << tr("Duration") << tr("#Tracks") << tr("Year");
    _header_names_artists << "#" << tr("Artist") << tr("#Tracks");

    _album_model->set_new_header_names(_header_names_albums);
    _artist_model->set_new_header_names(_header_names_artists);
    _track_model->set_new_header_names(_header_names_tracks);
}


void GUI_Library_windowed::_sl_show_only_tracks_changed() {
	bool b = _settings->get(Set::Lib_OnlyTracks);

	lv_artist->setVisible(!b);
	lv_album->setVisible(!b);
}


void GUI_Library_windowed::resizeEvent(QResizeEvent* e) {

	Q_UNUSED(e);

	lv_album->set_col_sizes();
	lv_artist->set_col_sizes();
	tb_title->set_col_sizes();
}


void  GUI_Library_windowed::columns_album_changed(QList<int>& list) {
    _shown_cols_albums = list;
	_settings->set(Set::Lib_ColsAlbum, list);
}


void  GUI_Library_windowed::columns_artist_changed(QList<int>& list) {
    _shown_cols_artist = list;
	_settings->set(Set::Lib_ColsArtist, list);
}


void  GUI_Library_windowed::columns_title_changed(QList<int> & list) {
    _shown_cols_tracks = list;
	_settings->set(Set::Lib_ColsTitle, list);
}


void GUI_Library_windowed::lib_fill_tracks(const MetaDataList& v_metadata) {

	tb_title->fill<MetaDataList, MetaData>(v_metadata);

	lv_artist->set_mimedata(v_metadata, "tracks", true);
	lv_album->set_mimedata(v_metadata, "tracks", true);
}


void GUI_Library_windowed::lib_fill_albums(const AlbumList& albums) {

   lv_album->fill<AlbumList, Album>(albums);

}


void GUI_Library_windowed::lib_fill_artists(const ArtistList& artists) {

	lv_artist->fill<ArtistList, Artist>(artists);
}


void GUI_Library_windowed::refresh(){
	_library->refresh();
}

void GUI_Library_windowed::artist_sel_changed(const QList<int>& lst) {

	_info_dialog->setInfoMode(InfoDialogMode_Artists);

	_library->psl_selected_artists_changed(lst);
}

void GUI_Library_windowed::album_released() {
    if(_discmenu) {
        delete _discmenu;
        _discmenu = 0;
    }
}

void GUI_Library_windowed::album_sel_changed(const QList<int>& lst) {

	_info_dialog->setInfoMode(InfoDialogMode_Albums);
	//_info_dialog->set_tag_edit_visible(true);
    _timer->stop();
    if(lst.size() == 1) {
        QModelIndex idx = _album_model->index(lst[0], 0);
        QList<quint8> discnumbers = _album_model->get_discnumbers(idx);

        if(discnumbers.size() > 1 && lst.size() == 1 ) {
            delete_menu();
			_discmenu = new DiscPopupMenu(lv_album, discnumbers);

            connect(_discmenu, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));
            _timer->start(500);
        }
    }

	_library->psl_selected_albums_changed(lst);
}

void GUI_Library_windowed::track_sel_changed(const QList<int>& lst) {
	_info_dialog->setInfoMode(InfoDialogMode_Tracks);
	_library->psl_selected_tracks_changed(lst);
}


void GUI_Library_windowed::disc_pressed(int disc) {
	_library->psl_disc_pressed(disc);
}


void GUI_Library_windowed::track_info_available(const MetaDataList& v_md) {

	tb_title->set_mimedata(v_md, "tracks", false);

}


void GUI_Library_windowed::album_dbl_clicked(const QModelIndex & idx) {
	_library->psl_prepare_album_for_playlist(idx.row());
}

void GUI_Library_windowed::artist_dbl_clicked(const QModelIndex & idx) {
	_library->psl_prepare_artist_for_playlist(idx.row());
}

void GUI_Library_windowed::track_dbl_clicked(const QModelIndex& idx) {

    QList<int> lst = _track_model->get_selected();

    if(lst.size() ==0 ) {
        lst << idx.row();
    }

	_library->psl_prepare_tracks_for_playlist(lst);
}


void GUI_Library_windowed::sortorder_artist_changed(Sort::SortOrder s) {

    LibSortOrder so = _settings->get(Set::Lib_Sorting);
    so.so_artists = s;

     _settings->set(Set::Lib_Sorting, so);
}


void GUI_Library_windowed::sortorder_album_changed(Sort::SortOrder s) {

   LibSortOrder so = _settings->get(Set::Lib_Sorting);
    so.so_albums = s;

    _settings->set(Set::Lib_Sorting, so);
}


void GUI_Library_windowed::sortorder_title_changed(Sort::SortOrder s) {

    LibSortOrder so = _settings->get(Set::Lib_Sorting);
    so.so_tracks = s;

     _settings->set(Set::Lib_Sorting, so);
}


void GUI_Library_windowed::clear_button_pressed() {

	disconnect(le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));

	le_search->setText("");
	_library->refetch();

	connect(le_search, SIGNAL( textEdited(const QString&)), this, SLOT(text_line_edited(const QString&)));
}

void GUI_Library_windowed::return_pressed() {

	text_line_edited(le_search->text(), true);
}

void GUI_Library_windowed::text_line_edited(const QString& search, bool force_emit) {

	bool live_search = _settings->get(Set::Lib_LiveSearch);

	if(!force_emit && !live_search ) return;

    if(search.startsWith("f:", Qt::CaseInsensitive)) {
		combo_searchfilter->setCurrentIndex(0);
		le_search->setText("");
    }

    else if(search.startsWith("g:", Qt::CaseInsensitive)) {
		combo_searchfilter->setCurrentIndex(1);
		le_search->setText("");
    }

    else if(search.startsWith("p:", Qt::CaseInsensitive)) {
		combo_searchfilter->setCurrentIndex(2);
		le_search->setText("");
    }

	Filter filter;
	int idx = combo_searchfilter->currentIndex();
	switch(idx) {
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

	_library->psl_filter_changed(filter);
}

void GUI_Library_windowed::searchfilter_changed(int idx) {
	Q_UNUSED(idx);
	text_line_edited(_cur_searchfilter.filtertext, true);
}



void GUI_Library_windowed::id3_tags_changed() {
	refresh();
}

void GUI_Library_windowed::lib_reload(const QString& str) {

	QString final_str = QString("<b>") + str + "</b>";
	lab_status->setText(final_str);
}


void GUI_Library_windowed::lib_reload_finished() {
	lab_status->setText("");
	refresh();
}



void GUI_Library_windowed::edit_album() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_EDIT);
}

void GUI_Library_windowed::edit_artist() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_EDIT);
}

void GUI_Library_windowed::edit_tracks() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_EDIT);
}


void GUI_Library_windowed::info_album() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_INFO);
}

void GUI_Library_windowed::info_artist() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_INFO);
}

void GUI_Library_windowed::info_tracks() {
	if(!_info_dialog) return;

	_info_dialog->show(TAB_INFO);
}

void GUI_Library_windowed::play_next() {

	_library->psl_play_next_all_tracks();

}

void GUI_Library_windowed::play_next_tracks() {
	QModelIndexList idx_list = tb_title->selectionModel()->selectedRows(0);
	QList<int> lst;
	for(const QModelIndex& idx : idx_list) {
		lst.push_back(idx.row());
	}

	_library->psl_play_next_tracks(lst);
}

void GUI_Library_windowed::append() {

	_library->psl_append_all_tracks();

}

void GUI_Library_windowed::append_tracks() {
	QModelIndexList idx_list = tb_title->selectionModel()->selectedRows(0);
    QList<int> lst;
	for(const QModelIndex&  idx : idx_list) {
        lst.push_back(idx.row());
    }

	_library->psl_append_tracks(lst);
}


void GUI_Library_windowed::lib_delete_answer(QString answer) {
	QMessageBox answerbox(this);

	answerbox.setText(tr("Info"));
	answerbox.setIcon(QMessageBox::Information);
	answerbox.setInformativeText(answer);

	answerbox.exec();
	answerbox.close();
}

void GUI_Library_windowed::lib_no_lib_path(){

	QMessageBox::warning(this, tr("Warning"), tr("Please select your library path first and reload again."));

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),	QDir::homePath(), QFileDialog::ShowDirsOnly);

	if(dir.size() > 3){
		_settings->set(Set::Lib_Path, dir);
	}
}


void GUI_Library_windowed::delete_album() {

	int n_tracks = _track_model->rowCount();
	int answer = show_delete_dialog(n_tracks);

	if(answer) {
		_library->psl_delete_tracks(answer);
	}
}

void GUI_Library_windowed::delete_artist() {

	int n_tracks = _track_model->rowCount();
	int answer = show_delete_dialog(n_tracks);

	if(answer) {
		_library->psl_delete_tracks(answer);
	}
}

void GUI_Library_windowed::delete_tracks() {

	QModelIndexList idx_list = tb_title->selectionModel()->selectedRows(0);
	QList<int> lst;
	for(const QModelIndex& idx : idx_list) {
		lst.push_back(idx.row());
	}

	int answer = show_delete_dialog(lst.size());

    if(answer){
		_library->psl_delete_certain_tracks(lst, answer);
    }
}


int GUI_Library_windowed::show_delete_dialog(int n_tracks) {

		QMessageBox dialog(this);
		QAbstractButton* clicked_button;
        QPushButton* only_library_button;

		dialog.setFocus();
		dialog.setIcon(QMessageBox::Warning);
		dialog.setText("<b>" + tr("Warning") + "!</b>");
		dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		only_library_button = dialog.addButton(tr("Only from library"), QMessageBox::AcceptRole);
		dialog.setDefaultButton(QMessageBox::No);
		QString info_text = tr("You are about to delete %1 files").arg(n_tracks);

		dialog.setInformativeText(info_text + "\n" + tr("Continue?") );

		int answer = dialog.exec();
		clicked_button = dialog.clickedButton();
		dialog.close();


		if(answer == QMessageBox::No)
			return 0;

		if(answer == QMessageBox::Yes)
			return 1;

		if(clicked_button->text() == only_library_button->text()) {
			return 2;
		}

		return 0;
}

void GUI_Library_windowed::artist_middle_clicked(const QPoint& pt) {
    append();
}


void GUI_Library_windowed::album_middle_clicked(const QPoint& pt) {
    append();
}

void GUI_Library_windowed::tracks_middle_clicked(const QPoint& pt) {
    append_tracks();
}


void GUI_Library_windowed::library_changed() {
	refresh();
}

void GUI_Library_windowed::title_rating_changed(int rating) {

    QList<int> idxs = _track_model->get_selected();
    if(idxs.size() == 0) return;

	_library->psl_track_rating_changed(idxs[0], rating);
}

void GUI_Library_windowed::album_rating_changed(int rating) {
    QList<int> idxs = _album_model->get_selected();
    if(idxs.size() == 0) return;

	_library->psl_album_rating_changed(idxs[0], rating);
}


void GUI_Library_windowed::import_result(bool success) {

	QString success_string;
	if(success) {
		success_string = tr("Importing was successful");
	}

	else success_string = tr("Importing failed");

	library_changed();
}

void GUI_Library_windowed::delete_menu() {
	if(!_discmenu)return;
	
	_discmenu->hide();
	_discmenu->close();
		
    	disconnect(_discmenu, SIGNAL(sig_disc_pressed(int)), this, SLOT(disc_pressed(int)));
	delete _discmenu;
	_discmenu = 0;
	
}

void GUI_Library_windowed::timer_timed_out() {
	_timer->stop();
	if(!_discmenu) return;

	QPoint p = QCursor::pos();
	_discmenu->popup(p);
}

void GUI_Library_windowed::import_files(const QStringList & lst) {
    emit sig_import_files(lst);
}
