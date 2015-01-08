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
#include "playlist/PlaylistLoader.h"
#include "playlist/Playlist.h"



struct BackupPlaylist{

	int radio_mode;
    bool is_valid;
    int cur_play_idx;

    MetaDataList v_md;
};


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
	void sig_play();

	void sig_playlist_created(const MetaDataList&, int cur_track_idx, PlaylistType type, int playlist_idx=-1);
	void sig_auto_next_file(const MetaData&);
	void sig_cur_track_changed(const MetaData&);
	void sig_cur_track_idx_changed(int track_idx);

	void sig_no_track_to_play();
	void sig_goon_playing();

	void sig_mp3s_loaded_signal(int percent);
	void sig_data_for_id3_change(const MetaDataList&);
	void sig_cur_played_info_changed(const MetaData&);

	void sig_search_similar_artists(const QString&);
	void sig_playlist_prepared(int, const MetaDataList&);
	void sig_playlist_prepared(QString, const MetaDataList&);
	void sig_library_changed();
	void sig_import_files(const MetaDataList&);
	void sig_need_more_radio();
	void sig_new_stream_session();
	void sig_playlist_mode_changed(const PlaylistMode&);
	void sig_selection_changed(const MetaDataList&);

	void sig_new_playlist_added(int, QString);
	void sig_playlist_index_changed(int);
	void sig_playlist_closed(int);


public slots:

	void psl_play();
	void psl_pause();
	void psl_stop();
	void psl_forward();
	void psl_backward();
	void psl_change_track(int);
	void psl_next();

	void psl_selection_changed(const QList<int>&);
	void psl_playlist_mode_changed();

	void psl_clear_playlist();
	void psl_insert_tracks(const MetaDataList&, int idx);
	void psl_play_next(const MetaDataList&);
	void psl_append_tracks(const MetaDataList&);
	void psl_move_rows(const QList<int>&, int);
	void psl_remove_rows(const QList<int> &);

	void psl_id3_tags_changed(const MetaDataList& old_md, const MetaDataList& new_md);

	void psl_md_changed(const MetaData&);

	void psl_similar_artists_available(const QList<int>&);

	void psl_save_playlist(QString filename, bool relative);
	void psl_prepare_playlist_for_save(int id);
	void psl_prepare_playlist_for_save(QString name);

	void create_playlist(const QStringList&);
	void create_playlist(const MetaDataList&);
	void create_playlist(const CustomPlaylist&);

	void psl_audioconvert_on();
	void psl_audioconvert_off();


private:

	CDatabaseConnector* _db;
	PlaylistLoader*		_playlist_loader;

	QList<Playlist*>    _playlists;
	Playlist*			_cur_playlist;
	Playlist*			_active_playlist;
	int					_cur_playlist_idx;
	int					_active_playlist_idx;

	PlaylistState       _state;
	BackupPlaylist      _ba_playlist;
	bool				_start_play;

	PlaylistType determine_playlist_type(const MetaDataList& v_md);
	Playlist* new_playlist(PlaylistType type, int idx);



private slots:

	void playlist_changed(const Playlist* pl);
	void track_changed(const MetaData&, int cur_track_idx);
	void no_track_to_play();




};

#endif /* PLAYLISTHANDLER_H_ */
