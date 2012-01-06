/*
 * Playlists.h
 *
 *  Created on: Jan 2, 2012
 *      Author: luke
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
	void sig_import_tracks(const vector<MetaData>& );


public slots:


	void save_playlist_as_custom(QString name, vector<MetaData>& vec_md);
	void save_playlist_as_custom(int id, vector<MetaData>& vec_md);
	void delete_playlist(int id);
	void load_all_playlists();
	void load_single_playlist(int id);
	void import_result(bool);


public:
	void ui_loaded();
private:
	QMap<int, QString> 	_mapping;
	int					_import_state;
	bool				check_for_extern_track(const vector<MetaData>& src, vector<MetaData>& tgt);
};

#endif /* PLAYLISTS_H_ */
