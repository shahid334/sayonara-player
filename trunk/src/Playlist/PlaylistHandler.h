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
#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/SayonaraClass.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "Playlist/PlaylistLoader.h"
#include "Playlist/Playlist.h"
#include "PlayManager.h"



class Playlist;
class PlaylistHandler : public QObject, protected SayonaraClass {

	Q_OBJECT

public:

    PlaylistHandler(QObject * parent=0);
    virtual ~PlaylistHandler();

	uint get_num_tracks();

	int add_new_playlist(QString name="");
	bool change_playlist_index(int idx);
	void close_playlist(int idx);
	void load_old_playlist();

signals:

	void sig_playlist_created(const MetaDataList&, int cur_track_idx, PlaylistType type, int playlist_idx=-1);
	void sig_cur_track_idx_changed(int track_idx, int playlist_idx);

	void sig_playlist_prepared(int, const MetaDataList&);
	void sig_playlist_prepared(QString, const MetaDataList&);

	void sig_playlist_mode_changed(const PlaylistMode&);
	void sig_selection_changed(const MetaDataList&);

	void sig_new_playlist_added(int, QString);
	void sig_playlist_index_changed(int);
	void sig_playlist_closed(int);


public slots:

	void psl_change_track(int idx, int playlist_idx=-1);

	void psl_selection_changed(const QList<int>&);
	void psl_playlist_mode_changed();

	void psl_clear_playlist();
	void psl_insert_tracks(const MetaDataList&, int idx);
	void psl_play_next(const MetaDataList&);
	void psl_append_tracks(const MetaDataList&);
	void psl_move_rows(const QList<int>&, int);
	void psl_remove_rows(const QList<int> &);

	void psl_md_changed(const MetaDataList& old_md, const MetaDataList& new_md);
	void psl_md_changed(const MetaData&);

	void psl_similar_artists_available(const QList<int>&);

	void psl_save_playlist(QString filename, bool relative);
	void psl_prepare_playlist_for_save(int id);
	void psl_prepare_playlist_for_save(QString name);

	void create_playlist(const QStringList&, bool new_tab=true);
	void create_playlist(const MetaDataList&, bool new_tab=true);
	void create_playlist(const CustomPlaylist&, bool new_tab=true);
	void create_playlist(const QString& dir, bool new_tab=true);

	void psl_audioconvert_on();
	void psl_audioconvert_off();

private slots:
	void played();
	void paused();
	void stopped();
	void previous();
	void next();
	void playstate_changed(PlayManager::PlayState);

private:

	CDatabaseConnector* _db;
	PlayManager*		_play_manager;
	PlaylistLoader*		_playlist_loader;

	QList<Playlist*>    _playlists;

	int					_cur_playlist_idx;
	int					_active_playlist_idx;


	PlaylistType determine_playlist_type(const MetaDataList& v_md);
	Playlist* new_playlist(PlaylistType type, int idx);

	Playlist* get_active();
	Playlist* get_current();

	void emit_playlist_created(Playlist* pl=NULL);
	void emit_cur_track_changed(Playlist* pl=NULL);
};

#endif /* PLAYLISTHANDLER_H_ */
