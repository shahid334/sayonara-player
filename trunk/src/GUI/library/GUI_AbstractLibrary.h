/* GUI_AbstractLibrary.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef GUI_ABSTRACTLIBRARY_H
#define GUI_ABSTRACTLIBRARY_H


#include "Library/AbstractLibrary.h"
#include "HelperStructs/SayonaraClass.h"
#include "HelperStructs/CustomMimeData.h"

#include "GUI/library/model/LibraryItemModelAlbums.h"
#include "GUI/library/model/LibraryItemModelArtists.h"
#include "GUI/library/model/LibraryItemModelTracks.h"


#include "GUI/library/delegate/LibraryItemDelegateAlbums.h"
#include "GUI/library/delegate/LibraryItemDelegateArtists.h"
#include "GUI/library/delegate/LibraryItemDelegateTracks.h"

#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/MyColumnHeader.h"

#include "GUI/library/view/LibraryView.h"
#include "GUI/library/view/LibraryViewAlbum.h"

#include <QStringList>
#include <QPoint>
#include <QList>


using namespace Sort;

class GUI_AbstractLibrary : public SayonaraWidget
{
	Q_OBJECT

public:

	explicit GUI_AbstractLibrary(AbstractLibrary* library,
								 QWidget *parent=0);

	virtual ~GUI_AbstractLibrary();

	void set_lib_chooser(const QStringList& lst);


protected:
	AbstractLibrary* _library;
	GUI_InfoDialog*	 _info_dialog;

	Filter		_cur_searchfilter;


	QList<int> _shown_cols_albums;
	QList<int> _shown_cols_artist;
	QList<int> _shown_cols_tracks;


	QStringList _header_names_albums;
	QStringList _header_names_artists;
	QStringList _header_names_tracks;


	LibraryItemModelTracks* 	_track_model;
	LibraryItemModelAlbums* 	_album_model;
	LibraryItemModelArtists*	_artist_model;


	LibraryItemDelegateTracks* 		_track_delegate;
	LibraryItemDelegateAlbums* 		_album_delegate;
	LibraryItemDelegateArtists* 	_artist_delegate;


	QList<ColumnHeader>	_track_columns;
	QList<ColumnHeader>	_album_columns;
	QList<ColumnHeader>	_artist_columns;

	virtual void init_headers();
	virtual void language_changed();
	virtual AbstractLibrary::TrackDeletionMode show_delete_dialog(int n_tracks)=0;

	virtual void resizeEvent(QResizeEvent* e);


private:
	QComboBox*			_combo_libchooser;
	LibraryView*		_lv_artist;
	LibraryViewAlbum*	_lv_album;
	LibraryView*		_lv_tracks;

	QComboBox*			_combo_search;
	QPushButton*		_btn_clear;
	QLineEdit*			_le_search;
	QPushButton*		_btn_info;
	QPushButton*		_btn_refresh;
	QLabel*				_lab_status;

	void init_finished();



private slots:
	virtual void _sl_show_only_tracks_changed();


protected slots:

	virtual void refresh();

	virtual void lib_fill_tracks(const MetaDataList&);
	virtual void lib_fill_albums(const AlbumList&);
	virtual void lib_fill_artists(const ArtistList&);
	virtual void track_info_available(const MetaDataList& v_md);

	virtual void artist_sel_changed(const QList<int>&);
	virtual void album_sel_changed(const QList<int>&);
	virtual void track_sel_changed(const QList<int>&);

	virtual void artist_middle_clicked(const QPoint& p);
	virtual void album_middle_clicked(const QPoint& p);
	virtual void tracks_middle_clicked(const QPoint& p);

	virtual void artist_dbl_clicked(const QModelIndex &);
	virtual void album_dbl_clicked(const QModelIndex &);
	virtual void track_dbl_clicked(const QModelIndex &);

	virtual void columns_title_changed(QList<int>&);
	virtual void columns_album_changed(QList<int>&);
	virtual void columns_artist_changed(QList<int>&);

	virtual void sortorder_title_changed(Sort::SortOrder);
	virtual void sortorder_album_changed(Sort::SortOrder);
	virtual void sortorder_artist_changed(Sort::SortOrder);

	virtual void text_line_edited(const QString&, bool force_emit=false);
	virtual void clear_button_pressed();
	virtual void combo_search_changed(int idx);
	virtual void return_pressed();

	virtual void info_artist();
	virtual void info_album();
	virtual void info_tracks();

	virtual void edit_artist();
	virtual void edit_album();
	virtual void edit_tracks();

	virtual void delete_artist();
	virtual void delete_album();
	virtual void delete_tracks();

	virtual void album_rating_changed(int);
	virtual void title_rating_changed(int);

	virtual void append();
	virtual void append_tracks();
	virtual void play_next();
	virtual void play_next_tracks();

	virtual void show_delete_answer(QString);

	virtual void lib_reload(const QString& str);
	virtual void reload_finished();

public slots:
	virtual void id3_tags_changed();

protected:
	template<typename T>
	void setup_parent(T* subclass){

		subclass->setupUi(subclass);

		_combo_libchooser = subclass->combo_lib_chooser;
		_lv_artist = subclass->lv_artist;
		_lv_album = subclass->lv_album;
		_lv_tracks = subclass->tb_title;
		_btn_clear = subclass->btn_clear;
		_le_search = subclass->le_search;
		_combo_search = subclass->combo_searchfilter;
		_btn_info = subclass->btn_info;
		_lab_status = subclass->lab_status;

		init_finished();

	}
};

#endif // GUI_ABSTRACTLIBRARY_H
