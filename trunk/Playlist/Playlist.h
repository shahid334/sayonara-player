/*
 * Playlist.h
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include "HelperStructs/MetaData.h"

#include <vector>
#include <iostream>

#include <QObject>
#include <QStringList>


using namespace std;


#include <phonon/mediaobject.h>


struct Playlist_Mode{

	bool				rep1;
	bool				repAll;
	bool				repNone;
	bool				append;
	bool				shuffle;

	Playlist_Mode(){
		rep1 = false;
		repAll = false;
		repNone = true;
		append = false;
		shuffle = false;

	}

	void print(){
		cout << "rep1 = " << rep1 << ", "
			<< "repAll = " << repAll << ", "
			<< "repNone = " << repNone << ", "
			<< "append = " << append <<", " << endl;
	}

};



class Playlist  : public QObject {

	Q_OBJECT
public:
	Playlist(QObject * parent);
	virtual ~Playlist();

	signals:
		void playlist_created(vector<MetaData>&);
		void selected_file_changed_md(const MetaData&);
		void selected_file_changed(int row);
		void no_track_to_play();

		void mp3s_loaded_signal(int percent);

	public slots:

		void createPlaylist(QStringList&);
		void createPlaylist(vector<MetaData>&);
		void change_track(int);
		void next_track();
		void playlist_mode_changed(const Playlist_Mode&);
		void clear_playlist();
		void save_playlist(const QString&);

		void forward();
		void backward();


	private:



	vector<MetaData>	_v_meta_data;
	QStringList			_pathlist;
	int					_cur_play_idx;

	Playlist_Mode		_playlist_mode;



};

#endif /* PLAYLIST_H_ */
