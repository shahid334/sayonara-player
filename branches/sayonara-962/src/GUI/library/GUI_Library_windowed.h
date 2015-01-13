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


#include <QAbstractTableModel>
#include <QPoint>
#include <QTimer>
#include <QMenu>
#include <QMap>
#include <QMessageBox>
#include <QFocusEvent>

#include "GUI/ui_GUI_Library_windowed.h"
#include "GUI/library/DiscPopupMenu.h"
#include "GUI/library/model/LibraryItemModelTracks.h"
#include "GUI/library/model/LibraryItemModelArtists.h"
#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "GUI/library/delegate/LibraryItemDelegateAlbums.h"
#include "GUI/library/delegate/LibraryItemDelegateArtists.h"
#include "GUI/library/delegate/LibraryItemDelegateTracks.h"

#include "GUI/library/InfoBox/GUILibraryInfoBox.h"

#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/library/view/LibraryView.h"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CustomMimeData.h"

#include "HelperStructs/SayonaraClass.h"
#include <QMessageBox>

#include "library/CLibraryBase.h"


using namespace Sort;

class GUI_Library_windowed: public SayonaraWidget, private Ui::Library_windowed {

Q_OBJECT

public:

	GUI_Library_windowed(CLibraryBase* library, GUI_InfoDialog* info_dialog, QWidget* parent);
	virtual ~GUI_Library_windowed();

protected:

	LibraryItemModelTracks* 		_track_model;
	LibraryItemDelegateTracks* 		_track_delegate;
	LibraryItemModelAlbums* 		_album_model;
	LibraryItemDelegateAlbums* 		_album_delegate;
	LibraryItemModelArtists* 		_artist_model;
	LibraryItemDelegateArtists* 	_artist_delegate;
	
    QMenu*      _header_rc_menu_title;
    QMenu*      _header_rc_menu_album;
    QMenu*      _header_rc_menu_artist;


	Filter		_cur_searchfilter;

signals:

    void sig_import_files(const QStringList&);


public slots:
	void id3_tags_changed();

	void library_changed();
	void import_result(bool);


protected slots:

    void artist_sel_changed(const QList<int>&);
    void album_sel_changed(const QList<int>&);
    void album_released();
    void track_sel_changed(const QList<int>&);
	void track_info_available(const MetaDataList& v_md);

	void disc_pressed(int);

    void album_rating_changed(int);
    void title_rating_changed(int);

	void clear_button_pressed();

	void artist_dbl_clicked(const QModelIndex &);
	void album_dbl_clicked(const QModelIndex &);
	void track_dbl_clicked(const QModelIndex &);

	void searchfilter_changed(int);
	void text_line_edited(const QString&, bool force_emit=false);
    void return_pressed();

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
    void append();
    void append_tracks();

    void sortorder_title_changed(Sort::SortOrder);
    void sortorder_album_changed(Sort::SortOrder);
    void sortorder_artist_changed(Sort::SortOrder);

	void columns_title_changed(QList<int>&);
	void columns_album_changed(QList<int>&);
	void columns_artist_changed(QList<int>&);

    void timer_timed_out();
    void delete_menu();
    void import_files(const QStringList&);

	void _sl_show_only_tracks_changed();

	void lib_reload(const QString&);
	void lib_fill_tracks(const MetaDataList&);
	void lib_fill_albums(const AlbumList&);
	void lib_fill_artists(const ArtistList&);
	void lib_delete_answer(QString);
	void lib_reload_finished();
	void lib_no_lib_path();

	void refresh();

protected:
	void resizeEvent(QResizeEvent* e);

	QList<int> _shown_cols_albums;
	QList<int> _shown_cols_artist;
	QList<int> _shown_cols_tracks;

    QStringList _header_names_albums;
    QStringList _header_names_artists;
    QStringList _header_names_tracks;

	GUI_InfoDialog* _info_dialog;
	GUI_Library_Info_Box* _lib_info_dialog;

	MetaDataList _v_md_tmp;
    QTimer*      _timer;
	DiscPopupMenu* _discmenu;

	int show_delete_dialog(int n_tracks);
    void init_headers();
	void language_changed();

private:
	CLibraryBase* _library;

};

#endif /* GUI_LIBRARY_WINDOWED_H_ */

