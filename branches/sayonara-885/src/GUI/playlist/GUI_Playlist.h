/* GUI_Playlist.h */

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
 * GUI_Playlist.h
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#ifndef GUI_PLAYLIST_H_
#define GUI_PLAYLIST_H_

#include "GUI/ui_GUI_Playlist.h"
#include "GUI/playlist/PlaylistTabWidget.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/SayonaraClass.h"

#include <QTextEdit>
#include <QFocusEvent>
#include <QKeyEvent>



class GUI_Playlist : public SayonaraWidget, private Ui::Playlist_Window
{
	Q_OBJECT

	public:
		GUI_Playlist(QWidget *parent, GUI_InfoDialog* dialog);
		~GUI_Playlist();

		void dragEnterEvent(QDragEnterEvent* event);
		void dragLeaveEvent(QDragLeaveEvent* event);
		void dropEvent(QDropEvent* event);
		void dragMoveEvent(QDragMoveEvent* event);

	signals:
		void sig_cur_idx_changed(int);
		void sig_selection_changed(const QList<int>&);

		void sig_save_playlist(const QString&);

		void sig_rows_moved(const QList<int>&, int);
		void sig_tracks_dropped(const MetaDataList&, int);
		void sig_rows_removed(const QList<int>&);
		void sig_cleared();
		void sig_gapless(bool);

		void search_similar_artists(const QString&);
		void sig_no_focus();
		void sig_close_connection(int);
		void sig_accept_connection(bool);

		void sig_playlist_idx_changed(int);
		void sig_delete_tab_playlist(int);
		void sig_create_tab_playlist();


	public slots:
		void fillPlaylist(const MetaDataList&, int, PlaylistType, int pl_idx);
		void track_changed(int);

		void library_path_changed(QString);
		void set_playlist_type(PlaylistType playlist_type);

		void language_changed();


	private slots:

		void sel_changed(const MetaDataList&, const QList<int>&);
		void double_clicked(int);
		void clear_playlist_slot();
		void playlist_mode_changed();
		void rows_moved(const QList<int>&, int);

		void psl_info_tracks();
		void psl_edit_tracks();

		void btn_numbers_changed(bool);
		void metadata_dropped(const MetaDataList&, int);
		void rows_removed(const QList<int>&);
		void no_focus();

		void new_playlist_clicked();
		void delete_playlist_clicked(int idx);
		void playlist_idx_changed(int idx);

		void _sl_change_small_playlist_items();
		void _sl_playlist_mode_changed();



	private:

		QWidget*			_parent;
		GUI_InfoDialog*		_info_dialog;
		PlaylistMode		_playlist_mode;
		PlaylistType		_playlist_type;

		QList<PlaylistView*> _playlist_views;
		PlaylistView* _cur_playlist_view;
		int _cur_playlist_idx;

		void initGUI();
		void initPlaylistView(const PlaylistView* pl_view);

		void set_total_time_label(qint64 dur_ms=0);
		void check_dynamic_play_button();
		void prepare_info();

		QStringList _connections;



	protected:
		void changeEvent(QEvent* e);
		void resizeEvent(QResizeEvent *e);
		void focusInEvent(QFocusEvent *e);


};



#endif /* GUI_PLAYLIST_H_ */
