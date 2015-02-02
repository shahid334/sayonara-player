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
#include "Playlist/PlaylistHandler.h"

#include "HelperStructs/MetaData.h"
#include "HelperStructs/SayonaraClass.h"

#include <QTextEdit>
#include <QFocusEvent>
#include <QKeyEvent>



class GUI_Playlist : public SayonaraWidget, private Ui::Playlist_Window
{
	Q_OBJECT

public:
	GUI_Playlist(PlaylistHandler* playlist, GUI_InfoDialog* dialog, QWidget *parent);
	~GUI_Playlist();

	void dragEnterEvent(QDragEnterEvent* event);
	void dragLeaveEvent(QDragLeaveEvent* event);
	void dropEvent(QDropEvent* event);
	void dragMoveEvent(QDragMoveEvent* event);


public slots:
	void fill_playlist(const MetaDataList&, int, PlaylistType, int pl_idx);


	void library_path_changed(QString);
	void set_playlist_type(PlaylistType playlist_type);


private slots:

	void track_changed(int row, int playlist_idx);

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

	void playlist_added(int idx, QString name);
	void playlist_closed(int idx);
	void playlist_idx_changed(int idx);

	void add_playlist_clicked();
	void tab_playlist_clicked(int);
	void close_playlist_clicked(int);

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
	PlaylistHandler*	_playlist;

	int _tab_idx;



protected:
	void changeEvent(QEvent* e);
	void resizeEvent(QResizeEvent *e);
	void language_changed();


};



#endif /* GUI_PLAYLIST_H_ */
