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
#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include <GUI/MyTableView.h>

#include <HelperStructs/MetaData.h>
#include <HelperStructs/Filter.h>

#include <DatabaseAccess/CDatabaseConnector.h>

#include <QMenu>
#include <QMessageBox>

#include <vector>

using namespace std;

class GUI_Library_windowed: public QWidget, private Ui::Library_windowed {

Q_OBJECT

public:
	GUI_Library_windowed(QWidget* parent, GUI_InfoDialog* dialog);
	virtual ~GUI_Library_windowed();

private:
	Ui::Library_windowed* 		ui;

	LibraryItemModelTracks* 	_track_model;
	LibraryItemDelegateTracks* 	_track_delegate;
	LibraryItemModelAlbums* 	_album_model;
	LibraryItemDelegateAlbums* 	_album_delegate;
	LibraryItemModelArtists* 	_artist_model;
	LibraryItemDelegateArtists* _artist_delegate;

	QMenu* 		_right_click_menu;

	QAction* 	_info_action;
	QAction* 	_edit_action;
	QAction* 	_delete_action;

	Filter		_cur_searchfilter;



signals:

	void sig_album_dbl_clicked();
	void sig_artist_dbl_clicked();
	void sig_track_dbl_clicked(int);

	void sig_artist_pressed(const QList<int>&);
	void sig_album_pressed(const QList<int>&);
	void sig_track_pressed(const QList<int>&);

	void sig_delete_tracks();
	void sig_delete_certain_tracks(const QList<int>&);

	void sig_filter_changed(const Filter&);
	void sig_sortorder_changed(QString,QString,QString);

	void sig_show_id3_editor(const QList<int>&);

public slots:
	void fill_library_tracks(vector<MetaData>&);
	void fill_library_albums(vector<Album>&);
	void fill_library_artists(vector<Artist>&);
	void id3_tags_changed();
	void reloading_library(int);
	void reloading_library_finished();
	void library_changed();
	void import_result(bool);
	void psl_delete_answer(QString);


private slots:

	void artist_pressed(const QModelIndex &);
	void album_pressed(const QModelIndex &);
	void track_pressed(const QModelIndex&);
	void track_info_available(const vector<MetaData>& v_md);

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



	void info_artist();
	void info_album();
	void info_tracks();

	void edit_artist();
	void edit_album();
	void edit_tracks();

	void delete_artist();
	void delete_album();
	void delete_tracks();


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

	GUI_InfoDialog* _info_dialog;

	bool show_delete_dialog(int n_tracks);
	void refresh();

};

#endif /* GUI_LIBRARY_WINDOWED_H_ */
