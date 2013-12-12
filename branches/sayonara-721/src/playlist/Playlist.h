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

#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include "GUI/playlist/model/PlaylistItemModel.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"
#include "HelperStructs/globals.h"
#include "DatabaseAccess/CDatabaseConnector.h"

#include <iostream>

#include <QObject>
#include <QList>
#include <QMap>
#include <QStringList>

using namespace std;

struct BackupPlaylist{
    RadioMode radio_mode;
    bool is_valid;
    int cur_play_idx;

    MetaDataList v_md;

};


class Playlist  : public QObject {

	Q_OBJECT
public:
    Playlist(QObject * parent=0);
	virtual ~Playlist();


    void load_old_playlist();
	uint get_num_tracks();

	signals:
        void sig_playlist_created(MetaDataList&, int, int);
        void sig_selected_file_changed_md(const MetaData&, int pos=0, bool play=true);
		void sig_selected_file_changed(int row);
		void sig_no_track_to_play();
		void sig_mp3s_loaded_signal(int percent);
		void sig_data_for_id3_change(const MetaDataList&);
		void sig_cur_played_info_changed(const MetaData&);
		void sig_goon_playing();
		void sig_search_similar_artists(const QString&);
		void sig_playlist_prepared(int, MetaDataList&);
		void sig_playlist_prepared(QString, MetaDataList&);
		void sig_library_changed();
		void sig_import_files(const MetaDataList&);
		void sig_need_more_radio();
        void sig_gapless_track(const MetaData&);
        void sig_new_stream_session();




	public slots:

        void psl_createPlaylist(QStringList&);
        void psl_createPlaylist(MetaDataList&);
        void psl_createPlaylist(CustomPlaylist&);

		void psl_change_track(int);
		void psl_next_track();
		void psl_playlist_mode_changed(const Playlist_Mode&);
        void psl_save_playlist(const QString&, const MetaDataList& v_md, bool relative);
        void psl_prepare_playlist_for_save(int id);
		void psl_prepare_playlist_for_save(QString name);
        void psl_prepare_playlist_for_save_file(QString, bool);
		void psl_play();
        void psl_pause();
		void psl_stop();
		void psl_forward();
		void psl_backward();

		void psl_save_playlist_to_storage();
		void psl_edit_id3_request();
		void psl_id3_tags_changed(MetaDataList&);
        void psl_track_time_changed(MetaData&);
		void psl_import_new_tracks_to_library(bool);
		void psl_import_result(bool);
        void psl_lfm_radio_init(bool);
        void psl_new_lfm_playlist_available(const MetaDataList&);
		void psl_play_stream(const QString&, const QString&);
        void psl_play_podcast(const QString&, const QString&);
		void psl_valid_strrec_track(const MetaData&);
		void psl_play_next_tracks(const MetaDataList&);
        void psl_gapless_track();

        void psl_insert_tracks(const MetaDataList&, int idx);
        void psl_append_tracks(MetaDataList&);
        void psl_remove_rows(const QList<int> &, bool select_next_row=true);
        void psl_clear_playlist();
        void psl_similar_artists_available(const QList<int>&);




	private:

        MetaDataList        _v_meta_data;
        QStringList			_pathlist;

        int					_cur_play_idx;
        bool                _is_playing;
        RadioMode   		_radio_active;
        bool                _pause;


        Playlist_Mode		_playlist_mode;

        CDatabaseConnector* _db;
        CSettingsStorage*   _settings;
        MetaDataList        _v_stream_playlist;
        BackupPlaylist      _backup_playlist;

        void                send_cur_playing_signal(int);


	/* wrapper for Helper::checkTrack */
	bool 				checkTrack(const MetaData& md);
	void				save_stream_playlist();

	void				remove_row(int row);

};

#endif /* PLAYLIST_H_ */
