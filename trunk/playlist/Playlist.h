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

#include "HelperStructs/MetaData.h"
#include "HelperStructs/PlaylistMode.h"

#include <vector>
#include <iostream>

#include <QObject>
#include <QList>
#include <QStringList>

using namespace std;


class Playlist  : public QObject {

	Q_OBJECT
public:
	Playlist(QObject * parent);
	virtual ~Playlist();

	void ui_loaded();

	signals:
		void playlist_created(vector<MetaData>&, int);
		void selected_file_changed_md(const MetaData&);
		void selected_file_changed(int row);
		void no_track_to_play();

		void mp3s_loaded_signal(int percent);
		void data_for_id3_change(const vector<MetaData>&);
		void cur_played_info_changed(const MetaData&);
		void goon_playing();
		void search_similar_artists(const QString&);

	public slots:

		void createPlaylist(QStringList&);
		void createPlaylist(vector<MetaData>&);
		void insert_tracks(const vector<MetaData>&, int idx);
		void insert_albums(const vector<Album>&, int);			// not used
		void insert_artists(const vector<Artist>&, int);		// not used
		void change_track(int);
		void next_track();
		void playlist_mode_changed(const Playlist_Mode&);
		void clear_playlist();
		void save_playlist(const QString&);
		void similar_artists_available(QList<int>&);

		void directoryDropped(const QString& dir, int row);

		void play();
		void stop();
		void forward();
		void backward();

		void remove_row(int);
		void save_playlist_to_storage();

		void edit_id3_request();
		void id3_tags_changed(vector<MetaData>&);


	private:



	vector<MetaData>	_v_meta_data;
	QStringList			_pathlist;
	int					_cur_play_idx;

	Playlist_Mode		_playlist_mode;



};

#endif /* PLAYLIST_H_ */
