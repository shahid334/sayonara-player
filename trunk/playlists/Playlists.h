/*
 * Playlists.h
 *
 *  Created on: Jan 2, 2012
 *      Author: luke
 */

#ifndef PLAYLISTS_H_
#define PLAYLISTS_H_
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


public slots:


	void save_playlist_as_custom(QString name, vector<MetaData>& vec_md);
	void save_playlist_as_custom(int id, vector<MetaData>& vec_md);
	void delete_playlist(int id);
	void load_all_playlists();
	void load_single_playlist(int id);


public:
	void ui_loaded();
private:
	QMap<int, QString> _mapping;
};

#endif /* PLAYLISTS_H_ */
