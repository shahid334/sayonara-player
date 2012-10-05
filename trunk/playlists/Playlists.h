/* Playlists.h */

/* Copyright (C) 2012  Lucio Carreras
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



#ifndef PLAYLISTS_H_
#define PLAYLISTS_H_

#define STATE_WAIT 0
#define STATE_IMPORT_SUCCESS 1
#define STATE_IMPORT_FAIL 2

#include "HelperStructs/MetaData.h"
#include <QObject>
#include <QString>
#include <QMap>
#include <vector>



using namespace std;

class Playlists : public QObject {
	Q_OBJECT

public:
	Playlists();
	virtual ~Playlists();

signals:
	void sig_single_playlist_loaded(CustomPlaylist&);
	void sig_all_playlists_loaded(QMap<int, QString>&);
	void sig_import_tracks(const MetaDataList& );


public slots:


	void save_playlist_as_custom(QString name, MetaDataList& vec_md);
	void save_playlist_as_custom(int id, MetaDataList& vec_md);
	void delete_playlist(int id);
	void load_all_playlists();
	void load_single_playlist(int id);
	void import_result(bool);


public:
	void ui_loaded();
private:
	QMap<int, QString> 	_mapping;
	int					_import_state;
	bool				check_for_extern_track(const MetaDataList& src, MetaDataList& tgt);


};

#endif /* PLAYLISTS_H_ */
