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
#include <QMenu>
#include <QMap>
#include <QMessageBox>
#include <QFocusEvent>

#include "GUI/ui_GUI_Library_windowed.h"
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

#include "Library/LocalLibrary.h"

using namespace Sort;

class GUI_Library_windowed: public SayonaraWidget, private Ui::Library_windowed {

Q_OBJECT

public:

	GUI_Library_windowed(LocalLibrary* library, GUI_InfoDialog* info_dialog, QWidget* parent);
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

	virtual void artist_sel_changed(const QList<int>&);
	virtual void album_sel_changed(const QList<int>&);
	virtual void track_sel_changed(const QList<int>&);
	virtual void track_info_available(const MetaDataList& v_md);

	virtual void disc_pressed(int);

	virtual void album_rating_changed(int);
	virtual void title_rating_changed(int);

	virtual void clear_button_pressed();

	virtual void artist_dbl_clicked(const QModelIndex &);
	virtual void album_dbl_clicked(const QModelIndex &);
	virtual void track_dbl_clicked(const QModelIndex &);

	virtual void searchfilter_changed(int);
	virtual void text_line_edited(const QString&, bool force_emit=false);
	virtual void return_pressed();

	virtual void artist_middle_clicked(const QPoint& p);
	virtual void album_middle_clicked(const QPoint& p);
	virtual void tracks_middle_clicked(const QPoint& p);

	virtual void info_artist();
	virtual void info_album();
	virtual void info_tracks();

	virtual void edit_artist();
	virtual void edit_album();
	virtual void edit_tracks();

	virtual void delete_artist();
	virtual void delete_album();
	virtual void delete_tracks();

	virtual void play_next();
	virtual void play_next_tracks();
	virtual void append();
	virtual void append_tracks();

	virtual void sortorder_title_changed(Sort::SortOrder);
	virtual void sortorder_album_changed(Sort::SortOrder);
	virtual void sortorder_artist_changed(Sort::SortOrder);

	virtual void columns_title_changed(QList<int>&);
	virtual void columns_album_changed(QList<int>&);
	virtual void columns_artist_changed(QList<int>&);


	virtual void import_files(const QStringList&);

	virtual void _sl_show_only_tracks_changed();

	virtual void lib_reload(const QString&);
	virtual void lib_fill_tracks(const MetaDataList&);
	virtual void lib_fill_albums(const AlbumList&);
	virtual void lib_fill_artists(const ArtistList&);
	virtual void lib_delete_answer(QString);
	virtual void lib_reload_finished();
	virtual void lib_no_lib_path();

	virtual void refresh();

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

	LocalLibrary::TrackDeletionMode show_delete_dialog(int n_tracks);
    void init_headers();
	void language_changed();

private:
	LocalLibrary* _library;

};

#endif /* GUI_LIBRARY_WINDOWED_H_ */

