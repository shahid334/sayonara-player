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
#include "DatabaseAccess/CDatabaseConnector.h"
#include "playlist/Playlist.h"

#include <iostream>

#include <QObject>
#include <QList>
#include <QMap>
#include <QStringList>

using namespace std;

struct BackupPlaylist{
    int radio_mode;
    bool is_valid;
    int cur_play_idx;

    MetaDataList v_md;

};



class Playlist;
class PlaylistHandler  : public QObject {

	Q_OBJECT
public:
    PlaylistHandler(QObject * parent=0);
    virtual ~PlaylistHandler();

	uint get_num_tracks();

	signals:
        void sig_playlist_created(const MetaDataList&, int, PlaylistType);
		void sig_auto_next_file(const MetaData&, int pos=0, bool play=true);
        void sig_selected_file_changed_md(const MetaData&, int pos=0, bool play=true);
		void sig_selected_file_changed(int row);
		void sig_no_track_to_play();
        void sig_goon_playing();

		void sig_mp3s_loaded_signal(int percent);
		void sig_data_for_id3_change(const MetaDataList&);
		void sig_cur_played_info_changed(const MetaData&);

		void sig_search_similar_artists(const QString&);
		void sig_playlist_prepared(int, MetaDataList&);
		void sig_playlist_prepared(QString, MetaDataList&);
		void sig_library_changed();
		void sig_import_files(const MetaDataList&);
		void sig_need_more_radio();
        void sig_new_stream_session();
		void sig_playlist_mode_changed(const PlaylistMode&);


	public slots:

        void psl_createPlaylist(QStringList&, bool start_playing=true);
        void psl_createPlaylist(MetaDataList&, bool start_playing=true);
        void psl_createPlaylist(CustomPlaylist&, bool start_playing=false);


        void psl_play();
        void psl_pause();
        void psl_stop();
        void psl_forward();
        void psl_backward();
        void psl_change_track(int, qint32 pos=0, bool start_playing=true);
        void psl_next();

        void psl_selection_changed(const QList<int>&);
        void psl_playlist_mode_changed(const PlaylistMode&);

        void psl_clear_playlist();
        void psl_insert_tracks(const MetaDataList&, int idx);
        void psl_play_next(const MetaDataList&);
        void psl_append_tracks(MetaDataList&);
        void psl_move_rows(const QList<int>&, int);
        void psl_remove_rows(const QList<int> &, bool select_next_row=true);


		void psl_id3_tags_changed(MetaDataList&);

		void psl_track_time_changed(const MetaData&);


        void psl_similar_artists_available(const QList<int>&);

        void psl_save_playlist_to_storage();
        void psl_save_playlist(QString filename, bool relative);
        void psl_prepare_playlist_for_save(int id);
        void psl_prepare_playlist_for_save(QString name);

		void psl_audioconvert_on();
		void psl_audioconvert_off();


	private:

        CDatabaseConnector* _db;
        CSettingsStorage*   _settings;
        Playlist*           _playlist;
        PlaylistState       _state;
        qint32              _last_pos;
        BackupPlaylist      _ba_playlist;

        PlaylistType determine_playlist_type(const MetaDataList& v_md);
        bool new_playlist(PlaylistType type);

private slots:
        void playlist_changed(const MetaDataList&, int);
        void track_changed(const MetaData&, int);
        void no_track_to_play();


};

#endif /* PLAYLISTHANDLER_H_ */
