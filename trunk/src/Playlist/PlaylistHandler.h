/* Playlist.h */

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
 * Playlist.h
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#ifndef PLAYLISTHANDLER_H_
#define PLAYLISTHANDLER_H_

#include "GUI/playlist/model/PlaylistItemModel.h"
#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/SayonaraClass.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "Playlist/PlaylistLoader.h"
#include "Playlist/Playlist.h"
#include "PlayManager.h"
#include <QTimer>
#include "HelperStructs/Shutdown/Shutdown.h"




class Playlist;
class PlaylistHandler : public QObject, protected SayonaraClass {

	Q_OBJECT



	SINGLETON_QOBJECT(PlaylistHandler)

public:

	enum PlaylistIndex {
		PlaylistIndex_Current=0,
		PlaylistIndex_Active
	};

	int		add_new_playlist(const QString& name, bool editable, PlaylistType type=PlaylistTypeStd);
	bool	change_playlist_index(int idx);
	void	close_playlist(int idx);
	int		load_old_playlists();
	QString request_new_playlist_name();
	const MetaDataList& get_tracks(PlaylistIndex which);



signals:
	void sig_playlist_created(const Playlist* pl);
	void sig_cur_track_idx_changed(int track_idx, int playlist_idx);

	void sig_playlist_mode_changed(const PlaylistMode&);
	void sig_selection_changed(const MetaDataList&);

	void sig_new_playlist_added(int idx, const QString& name);
	void sig_playlist_index_changed(int idx, bool temporary);
	void sig_playlist_closed(int);
	void sig_tab_name_changed(int, const QString&);
	void sig_playlists_changed();


public slots:

	void change_track(int idx, int playlist_idx=-1);

	void selection_changed(const QList<int>&);
	void playlist_mode_changed();

	void clear_playlist();
	void insert_tracks(const MetaDataList&, int idx);
	void play_next(const MetaDataList&);
	void append_tracks(const MetaDataList&);
	void move_rows(const QList<int>&, int);
	void remove_rows(const QList<int> &);

	void md_changed(const MetaDataList& old_md, const MetaDataList& new_md);
	void md_changed(const MetaData&);

	void similar_artists_available(const QList<int>&);

	void save_playlist(QString filename, bool relative);

	PlaylistDBInterface::SaveAsAnswer save_playlist(int idx);
	PlaylistDBInterface::SaveAsAnswer save_cur_playlist();

	PlaylistDBInterface::SaveAsAnswer save_playlist_as(int idx, const QString& name, bool force_override);
	PlaylistDBInterface::SaveAsAnswer save_cur_playlist_as(const QString& name, bool force_override);

	void delete_playlist(int idx);


	int create_playlist(const QStringList&, QString name, bool temporary=true, PlaylistType type=PlaylistTypeStd);
	int create_playlist(const MetaDataList&, QString name, bool temporary=true, PlaylistType type=PlaylistTypeStd);
	int create_playlist(const QString& dir, QString name, bool temporary=true, PlaylistType type=PlaylistTypeStd);
	int create_playlist(const CustomPlaylist& pl);
	int create_empty_playlist(const QString name, bool editable=true);


private slots:
	void played();
	void paused();
	void stopped();
	void previous();
	void next();
	void playstate_changed(PlayManager::PlayState);

private:

	QStringList				_extern_playlists;
	CDatabaseConnector*		_db;
	PlaylistDBConnector*	_playlist_db_connector;
	PlayManager*			_play_manager;

	QList<Playlist*>		_playlists;
	Playlist*				_dummy_playlist;

	int						_cur_playlist_idx;
	int						_active_playlist_idx;


	Playlist* new_playlist(PlaylistType type, int idx, QString name="");

	Playlist* get_active();
	Playlist* get_current();

	void emit_playlist_created(Playlist* pl=NULL);
	void emit_cur_track_changed(Playlist* pl=NULL);

	void set_active_idx(int idx);


	int exists(QString name);
};

#endif /* PLAYLISTHANDLER_H_ */
