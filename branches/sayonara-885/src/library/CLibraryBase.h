/* CLibraryBase.h */

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


#ifndef CLIBRARYBASE_H
#define CLIBRARYBASE_H

#include "library/threads/ReloadThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/SayonaraClass.h"

#include <QThread>
#include <QStringList>
#include <QFileSystemWatcher>


class CLibraryBase : public QObject, protected SayonaraClass
{
    Q_OBJECT
public:
	CLibraryBase(QObject *parent = 0);

	virtual void loadDataFromDb ();


signals:
	void sig_playlist_created(const QStringList&);
	void sig_track_mime_data_available(const MetaDataList&);
	void sig_all_tracks_loaded (const MetaDataList&);
	void sig_all_albums_loaded(const AlbumList&);
	void sig_all_artists_loaded(const ArtistList&);
	void sig_tracks_for_playlist_available(const MetaDataList&);
	void sig_append_tracks_to_playlist(const MetaDataList&);

	void sig_should_reload_library();
	void sig_reload_library_finished();
	void sig_reload_library_allowed(bool);
	void sig_reloading_library(const QString&);

	void sig_change_id3_tags(const MetaDataList&);

    void sig_delete_answer(QString);
	void sig_play_next_tracks(const MetaDataList&);
	void sig_no_library_path();


public slots:
	virtual void baseDirSelected (const QString & baseDir);
	virtual void insertMetaDataIntoDB(MetaDataList& in);

	virtual void reloadLibrary(bool);
	virtual void clearLibrary();

	virtual void refetch();
	virtual void refresh();

	virtual void psl_metadata_changed(const MetaDataList&, const MetaDataList&);


/* New way */
	virtual void psl_selected_artists_changed(const QList<int>&);
	virtual void psl_selected_albums_changed(const QList<int>&);
	virtual void psl_disc_pressed(int);
	virtual void psl_selected_tracks_changed(const QList<int>&);

	virtual void psl_prepare_album_for_playlist(int idx=0);
	virtual void psl_prepare_artist_for_playlist(int idx=0);
	virtual void psl_prepare_track_for_playlist(int idx);
	virtual void psl_prepare_tracks_for_playlist(QList<int> lst);

	virtual void psl_filter_changed(const Filter&, bool force=false);
	virtual void psl_change_id3_tags(const QList<int>& lst);
	virtual void psl_dur_changed(const MetaData&);

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
	virtual void _sl_sortorder_changed();
	virtual void _sl_libpath_changed();



protected:

    CDatabaseConnector*	_db;

    CDirectoryReader    _reader;

	QString				_library_path;

    ReloadThread* 		_reload_thread;
	int					_reload_progress;

    MetaDataList        _vec_md;
    AlbumList			_vec_albums;
    ArtistList			_vec_artists;

    QList<int>			_selected_artists;
    QList<int>			_selected_albums;
    QList<int>          _selected_tracks;

    Filter				_filter;

    void 				emit_stuff();
    void				delete_tracks(MetaDataList& v_md, int answer);

    LibSortOrder        _sortorder;

	void				fetch_by_filter(const Filter& filter, bool force);
	void				change_artist_selection(const QList<int>& idx_list);
	void				change_album_selection(const QList<int>& idx_list);
	MetaDataList		change_track_selection(const QList<int>& idx_list);

	void restore_artist_selection(const QList<int>& old_selected_idx);
	void restore_track_selection(const QList<int>& old_selected_idx);
	void restore_album_selection(const QList<int>& old_selected_idx);


};

#endif // CLIBRARYBASE_H
