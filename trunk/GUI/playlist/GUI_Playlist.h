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


#include <QMainWindow>
#include <QWidget>
#include <QKeyEvent>

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
			void directory_dropped(const QString&);
			void row_removed(int);
			void edit_id3_signal();



		public slots:
			void update_progress_bar(int);
			void fillPlaylist(vector<MetaData>&, int);
			void track_changed(int);
			void change_skin(bool);




		private slots:

			void current_row_changed(int);
			void current_row_changed(const QModelIndex &);
			void selected_row_changed(const QModelIndex&);
			void clear_playlist_slot();
			void save_playlist_slot();
			void playlist_mode_changed_slot();
			void edit_id3_but_pressed();



		protected:
			void keyPressEvent(QKeyEvent* e);

		private:
			Ui::Playlist_Window* 	ui;
			Playlist_Mode			_playlist_mode;


			QAbstractItemModel* 	_pli_model;
			QAbstractItemDelegate* 	_pli_delegate;

			QWidget*				_parent;
			qint64 					_total_secs;
			int						_cur_playing_row;
			int						_cur_selected_row;
			void initGUI();
			void set_total_time_label();
			void remove_cur_selected_row();
			bool inner_drag_drop;

			QPoint	_last_known_drag_pos;

	};



#endif /* GUI_PLAYLIST_H_ */
