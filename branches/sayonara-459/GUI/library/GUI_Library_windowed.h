/* GUI_Library_windowed.h */

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
 * GUI_Library_windowed.h
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#ifndef GUI_LIBRARY_WINDOWED_H_
#define GUI_LIBRARY_WINDOWED_H_

#include <QObject>
#include <QWidget>
#include <QAbstractTableModel>
#include <QPoint>

#include <ui_GUI_Library_windowed.h>
#include "GUI/library/LibraryItemModelTracks.h"
#include "GUI/library/LibraryItemDelegateTracks.h"
#include "GUI/library/LibraryItemModelAlbums.h"
#include "GUI/library/LibraryItemDelegateAlbums.h"
#include "GUI/library/LibraryItemDelegateArtists.h"
#include "GUI/library/LibraryItemModelArtists.h"
#include "GUI/library/GUILibraryInfoBox.h"


#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/MyTableView.h"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CustomMimeData.h"

#include "DatabaseAccess/CDatabaseConnector.h"

#include <QMenu>
#include <QMap>
#include <QMessageBox>

using namespace Sort;

class GUI_Library_windowed: public QWidget, private Ui::Library_windowed {

Q_OBJECT

public:
	GUI_Library_windowed(QWidget* parent, GUI_InfoDialog* dialog);
	virtual ~GUI_Library_windowed();



private:
	Ui::Library_windowed* 			ui;

	LibraryItemModelTracks* 		_track_model;
	LibraryItemDelegateTracks* 		_track_delegate;
	LibraryItemModelAlbums* 		_album_model;
	LibraryItemDelegateAlbums* 		_album_delegate;
	LibraryItemModelArtists* 		_artist_model;
	LibraryItemDelegateArtists* 	_artist_delegate;

	QMenu* 		_right_click_menu;
    QMenu*      _header_rc_menu_title;
    QMenu*      _header_rc_menu_album;
    QMenu*      _header_rc_menu_artist;

    QMap<QString, int> _header_map_name_col_title;
    QMap<QString, int> _header_map_name_col_album;
    QMap<QString, int> _header_map_name_col_artist;

    QList<QAction*> _header_rc_actions_title;
    QList<QAction*> _header_rc_actions_album;
    QList<QAction*> _header_rc_actions_artist;



	QAction* 	_info_action;
	QAction* 	_edit_action;
    QAction* 	_delete_action;
	QAction*	_play_next_action;

	Filter		_cur_searchfilter;



signals:

	void sig_album_dbl_clicked();
	void sig_artist_dbl_clicked();
	void sig_track_dbl_clicked(int);

	void sig_artist_pressed(const QList<int>&);
	void sig_album_pressed(const QList<int>&);
	void sig_track_pressed(const QList<int>&);

	void sig_delete_tracks(int);
	void sig_delete_certain_tracks(const QList<int>&, int);

	void sig_filter_changed(const Filter&);
	void sig_sortorder_changed(Sort::ArtistSort, Sort::AlbumSort, Sort::TrackSort);

	void sig_show_id3_editor(const QList<int>&);
	void sig_play_next_tracks(const QList<int>& lst);
	void sig_play_next_all_tracks();

	void sig_info_btn_clicked();

public slots:
	void fill_library_tracks(MetaDataList&);
	void fill_library_albums(AlbumList&);
	void fill_library_artists(ArtistList&);
	void id3_tags_changed();
	void reloading_library(QString&);
	void reloading_library_finished();
	void library_changed();
	void import_result(bool);
	void psl_delete_answer(QString);
    void change_skin(bool dark);


private slots:

    void artist_pressed(const QModelIndex & = QModelIndex());
    void artist_released(const QModelIndex & = QModelIndex());
    void album_pressed(const QModelIndex & = QModelIndex());
    void album_released(const QModelIndex & = QModelIndex());
    void track_pressed(const QModelIndex& = QModelIndex());
    void track_released(const QModelIndex & = QModelIndex());
	void track_info_available(const MetaDataList& v_md);

	void clear_button_pressed();

	void artist_dbl_clicked(const QModelIndex &);
	void album_dbl_clicked(const QModelIndex &);
	void track_dbl_clicked(const QModelIndex &);

	void searchfilter_changed(int);
	void text_line_edited(const QString&, bool force_emit=false);

	void sort_artists_by_column(int);
	void sort_albums_by_column(int);
	void sort_tracks_by_column(int);

	void show_artist_context_menu(const QPoint& p);
	void show_album_context_menu(const QPoint& p);
	void show_track_context_menu(const QPoint& p);

    void header_rc_menu_title_changed(bool b=false);


	void artist_middle_clicked(const QPoint& p);
	void album_middle_clicked(const QPoint& p);
	void tracks_middle_clicked(const QPoint& p);


	void info_artist();
	void info_album();
	void info_tracks();

	void edit_artist();
	void edit_album();
	void edit_tracks();

	void delete_artist();
	void delete_album();
	void delete_tracks();

	void play_next();
	void play_next_tracks();




protected:
	void resizeEvent(QResizeEvent* e);


private:
	void init_menues();


	AlbumSort _sort_albums;  /* [name | year] [asc | desc] */
	ArtistSort _sort_artists; /* [name | tracks] [asc | desc] */
	TrackSort _sort_tracks;  /* [title | album | artist | tracknum] [asc | desc] */

	GUI_InfoDialog* _info_dialog;
	GUI_Library_Info_Box* _lib_info_dialog;

	MetaDataList _v_md_tmp;

	CustomMimeData* _mime_data_album;
	CustomMimeData* _mime_data_artist;
	CustomMimeData* _mime_data;


	int show_delete_dialog(int n_tracks);
	void refresh();

    QList<int> calc_selections(int table);
    int set_title_sizes();

};

#endif /* GUI_LIBRARY_WINDOWED_H_ */
