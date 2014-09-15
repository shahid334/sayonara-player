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
#include "DatabaseAccess/CDatabaseConnector.h"

#include <QThread>
#include <QStringList>
#include <QFileSystemWatcher>


class CLibraryBase : public QObject
{
    Q_OBJECT
public:
    CLibraryBase(QWidget* main_window, QObject *parent = 0);

	virtual void loadDataFromDb ();


signals:
	void sig_playlist_created(QStringList&);
	void sig_track_mime_data_available(const MetaDataList&);
	void sig_all_tracks_loaded (MetaDataList&);
	void sig_all_albums_loaded(AlbumList&);
	void sig_all_artists_loaded(ArtistList&);
	void sig_tracks_for_playlist_available(MetaDataList&);
	void sig_append_tracks_to_playlist(MetaDataList&);

	void sig_should_reload_library();
	void sig_reload_library_finished();
	void sig_reload_library_allowed(bool);
	void sig_reloading_library(QString &);
	void sig_libpath_set(QString&);

	void sig_change_id3_tags(const MetaDataList&);

    void sig_delete_answer(QString);
	void sig_play_next_tracks(const MetaDataList&);


public slots:
	virtual void baseDirSelected (const QString & baseDir);
	virtual void insertMetaDataIntoDB(MetaDataList& in);

	virtual void reloadLibrary(bool);
	virtual void clearLibrary();
	virtual void refresh(bool b=true);

	virtual void setLibraryPath(QString);


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
	virtual void psl_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder);
	virtual void psl_change_id3_tags(const QList<int>& lst);
	virtual void psl_track_time_changed(MetaData&);

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

    CDirectoryReader    _reader;

    QString				m_library_path;

    ReloadThread* 		_reload_thread;
	int					_reload_progress;

    MetaDataList        _vec_md;
    AlbumList			_vec_albums;
    ArtistList			_vec_artists;

    Sort::SortOrder		_track_sortorder;
    Sort::SortOrder		_album_sortorder;
    Sort::SortOrder		_artist_sortorder;

    QList<int>			_selected_artists;
    QList<int>			_selected_albums;
    QList<int>          _selected_tracks;

    Filter				_filter;

    void 				emit_stuff();
    void				delete_tracks(MetaDataList& v_md, int answer);

};

#endif // CLIBRARYBASE_H
