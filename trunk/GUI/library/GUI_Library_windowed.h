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
#include <GUI/library/LibraryItemModelTracks.h>
#include "GUI/library/LibraryItemDelegateTracks.h"
#include <GUI/library/LibraryItemModelAlbums.h>
#include <GUI/library/LibraryItemDelegateAlbums.h>
#include <GUI/library/LibraryItemDelegateArtists.h>
#include <GUI/library/LibraryItemModelArtists.h>
#include <GUI/MyTableView.h>
#include <HelperStructs/MetaData.h>
#include <DatabaseAccess/CDatabaseConnector.h>

#include <QMenu>
#include <QMessageBox>

#include <vector>

using namespace std;

class GUI_Library_windowed: public QWidget, private Ui::Library_windowed {

Q_OBJECT

public:
	GUI_Library_windowed(QWidget* parent = 0);
	virtual ~GUI_Library_windowed();

private:
	Ui::Library_windowed* 		ui;

	LibraryItemModelTracks* 	_track_model;
	LibraryItemDelegateTracks* 	_track_delegate;
	LibraryItemModelAlbums* 	_album_model;
	LibraryItemDelegateAlbums* 	_album_delegate;
	LibraryItemModelArtists* 	_artist_model;
	LibraryItemDelegateArtists* _artist_delegate;

	vector<MetaData> 	_v_metadata;
	vector<Album> 		_v_albums;
	vector<Artist> 		_v_artists;

	QMenu* 		_right_click_menu;
	QAction* 	_info_action;
	QAction* 	_edit_action;
	QAction* 	_delete_action;

	QMessageBox* _album_msg_box;

	QString		_cur_searchstring;

signals:

	/* those three are intendenden for a nicer sw-design, but are not
	 * used yet
	 */
	/*void sig_artist_changed(int);
	void sig_album_changed(int);
	void sig_clear();*/

	/* emitted when dbl clicked */
	void sig_album_chosen(vector<MetaData>&);

	/* emitted when dbl clicked */
	void sig_artist_chosen(vector<MetaData>&);

	/* emitted when dbl clicked */
	void sig_track_chosen(vector<MetaData>&);
	void sig_reload_library();
	void sig_data_for_id3_change(const vector<MetaData>&);
	void sig_search_cover(const MetaData&);
	void sig_search_artist_image(const QString&);

public slots:
	void fill_library_tracks(vector<MetaData>&);
	void fill_library_albums(vector<Album>&);
	void fill_library_artists(vector<Artist>&);
	void id3_tags_changed();
	void reloading_library(int);
	void reloading_library_finished();
	void library_should_be_reloaded();
	void cover_changed(bool, QString);
	void library_changed();
	void import_result(bool);

private slots:
	void track_pressed(const QModelIndex&);
	void album_pressed(const QModelIndex &);
	void artist_pressed(const QModelIndex &);

	void clear_button_pressed();
	void album_dbl_clicked(const QModelIndex &);
	void artist_dbl_clicked(const QModelIndex &);
	void track_dbl_clicked(const QModelIndex &);

	void text_line_edited(const QString&);
	void sort_albums_by_column(int);
	void sort_artists_by_column(int);
	void sort_tracks_by_column(int);

	void reload_library_slot();

	void show_artist_context_menu(const QPoint& p);
	void show_album_context_menu(const QPoint& p);
	void show_track_context_menu(const QPoint& p);

	void edit_album();
	void info_album();
	void delete_album();
	void edit_artist();
	void info_artist();
	void delete_artist();
	void edit_tracks();
	void info_tracks();
	void delete_tracks();

	void apply_cover_to_entire_album();

protected:
	void resizeEvent(QResizeEvent* e);

public:
	void change_skin(bool dark);

private:
	QString getTotalTimeString(Album& album);
	void init_menues();


	QString _sort_albums;  /* [name | year] [asc | desc] */
	QString _sort_artists; /* [name | tracks] [asc | desc] */
	QString _sort_tracks;  /* [title | album | artist | tracknum] [asc | desc] */

	QList<int> 	_selected_artists;
	QList<int> 	_selected_albums;

	/* this parameter tells if a filter is applied or everything is shown
	 * and is only used in the text_line_edited function
	 */
	bool _everything_loaded;

	CDatabaseConnector* _db;

	void deleteSomeTracks(vector<MetaData>&);
	void refresh();

};

#endif /* GUI_LIBRARY_WINDOWED_H_ */
