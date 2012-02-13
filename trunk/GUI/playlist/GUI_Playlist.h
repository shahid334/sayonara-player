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

#include "playlist/Playlist.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "LyricLookup/LyricLookup.h"


#include <QMainWindow>
#include <QWidget>
#include <QKeyEvent>
#include <QTextEdit>

#include <string>
#include <vector>

#include "ui_GUI_Playlist.h"



	class GUI_Playlist : public QWidget, private Ui::Playlist_Window
	{
		Q_OBJECT

		public:
			GUI_Playlist(QWidget *parent = 0);
			~GUI_Playlist();

			void dragEnterEvent(QDragEnterEvent* event);
			void dragLeaveEvent(QDragLeaveEvent* event);
			void dropEvent(QDropEvent* event);
			void dragMoveEvent(QDragMoveEvent* event);
			void clear_drag_lines(int row);


		signals:
			void selected_row_changed(int);
			void clear_playlist();
			void save_playlist(const QString&);
			void playlist_mode_changed(const Playlist_Mode&);
			void dropped_tracks(const vector<MetaData>&, int);
			void dropped_albums(const vector<Album>&, int);
			void dropped_artists(const vector<Artist>&, int);
			void playlist_filled(vector<MetaData>&);
			void sound_files_dropped(QStringList&);
			void directory_dropped(const QString&, int);
			void row_removed(int);
			void edit_id3_signal();
			void search_similar_artists(const QString&);
			void sig_import_to_library(bool);

		public slots:
			void update_progress_bar(int);
			void fillPlaylist(vector<MetaData>&, int);
			void track_changed(int);
			void change_skin(bool);
			void library_path_changed(QString);
			void last_fm_logged_in(bool);
			void import_result(bool);
			void set_radio_active(int radio);

		private slots:
			void double_clicked(const QModelIndex &);
			void pressed(const QModelIndex&);
			void released(const QModelIndex&);
			void clear_playlist_slot();
			void save_playlist_slot();
			void playlist_mode_changed_slot();
			void edit_id3_but_pressed();
			void dummy_pressed();
			void import_button_clicked();
			void lyric_button_toggled(bool on);
			void lyric_server_changed(int);

			void lyric_thread_finished();
			void lyric_thread_terminated();



		protected:
			void keyPressEvent(QKeyEvent* e);

		private:


			Ui::Playlist_Window* 	ui;
			QWidget*				_parent;

			Playlist_Mode			_playlist_mode;
			QAbstractItemModel* 	_pli_model;
			QAbstractItemDelegate* 	_pli_delegate;
			QTextEdit* 				_text;
			QAction**				_action_lyric_servers;
			QMenu*					_menu_lyrics;

			LyricLookupThread*		_lyrics_thread;
			int 					_cur_lyric_server;

			qint64 					_total_secs;
			int						_cur_playing_row;
			int						_cur_selected_row;
			bool inner_drag_drop;
			QPoint	_last_known_drag_pos;

			int						_radio_active;


			void initGUI();
			void set_total_time_label();
			void remove_cur_selected_row();
			void check_for_library_path();





	};



#endif /* GUI_PLAYLIST_H_ */
