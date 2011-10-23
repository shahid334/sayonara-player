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
#include <QStringList>


using namespace std;


#include <phonon/mediaobject.h>



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
		void similar_artists_available(const int&);

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
