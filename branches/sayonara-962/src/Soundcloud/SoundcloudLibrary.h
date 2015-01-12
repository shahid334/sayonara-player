/* SoundcloudLibrary.h */

/* Copyright (C) 2014  Lucio Carreras
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


#ifndef SOUNDCLOUD_H
#define SOUNDCLOUD_H

#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QThread>
#include <QStringList>
#include <QtXml>


class SoundcloudLibrary : public QObject
{
	Q_OBJECT
public:
	SoundcloudLibrary(QWidget* main_window, QObject *parent = 0);

	void loadData();


signals:
	void sig_playlist_created(QStringList&);
	void sig_track_mime_data_available(const MetaDataList&);
	void sig_all_tracks_loaded (MetaDataList&);
	void sig_all_albums_loaded(AlbumList&);
	void sig_all_artists_loaded(ArtistList&);
	void sig_tracks_for_playlist_available(MetaDataList&);
	void sig_append_tracks_to_playlist(MetaDataList&);

	void sig_play_next_tracks(const MetaDataList&);


public slots:

	virtual void reloadLibrary(bool);
	virtual void clearLibrary();
	virtual void refresh(bool b=true);

	virtual void psl_selected_artists_changed(const QList<int>&);
	virtual void psl_selected_albums_changed(const QList<int>&);
	virtual void psl_selected_tracks_changed(const QList<int>&);

	virtual void psl_prepare_album_for_playlist(int idx=0);
	virtual void psl_prepare_artist_for_playlist(int idx=0);
	virtual void psl_prepare_track_for_playlist(int idx);
	virtual void psl_prepare_tracks_for_playlist(QList<int> lst);

	virtual void psl_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder);

	virtual void psl_delete_tracks(int);
	virtual void psl_delete_certain_tracks(const QList<int>&,int);

	virtual void psl_play_next_all_tracks();
	virtual void psl_play_next_tracks(const QList<int>&);

	virtual void psl_append_all_tracks();
	virtual void psl_append_tracks(const QList<int>&);

	virtual void psl_track_rating_changed(int idx, int rating);
	virtual void psl_album_rating_changed(int idx, int rating);


protected slots:

   virtual void library_reloading_state_slot(QString);
   virtual void library_reloading_state_new_block();
   virtual void reload_thread_finished();


protected:
	QWidget*            _main_window;
	CDatabaseConnector*	_db;
	CSettingsStorage*   _settings;

	MetaDataList        _vec_md;
	AlbumList			_vec_albums;
	ArtistList			_vec_artists;

	Sort::SortOrder		_track_sortorder;
	Sort::SortOrder		_album_sortorder;
	Sort::SortOrder		_artist_sortorder;

	QList<int>			_selected_artists;
	QList<int>			_selected_albums;
	QList<int>          _selected_tracks;

	void 				emit_stuff();

	bool				dl_all_playlists_by_artist(qint64 id, QString& content);
	bool				dl_all_tracks_by_artist(qint64 id, QString& content);
	bool				dl_all_artist_info(QString name, Artist& artist);






};

#endif // CLIBRARYBASE_H
