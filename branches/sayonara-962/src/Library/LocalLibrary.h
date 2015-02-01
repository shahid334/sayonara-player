/* LocalLibrary.h */

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


#ifndef LocalLibrary_H
#define LocalLibrary_H

#include "Library/threads/ReloadThread.h"
#include "HelperStructs/CDirectoryReader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Filter.h"
#include "HelperStructs/SayonaraClass.h"

#include <QThread>
#include <QStringList>
#include <QFileSystemWatcher>


class Library : public QObject, protected SayonaraClass {

	Q_OBJECT

public:

	explicit Library(QObject* parent=0) : QObject(parent), SayonaraClass() {

	}

	enum TrackDeletionMode{
		TrackDeletionModeNone=0,
		TrackDeletionModeOnlyLibrary,
		TrackDeletionModeAlsoFiles
	};
};


class LocalLibrary : public Library
{

    Q_OBJECT

public:
	LocalLibrary(QObject *parent = 0);

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
	virtual void psl_reload_library(bool);


	/* Clears all filters and searchstrings and fetches everything again */
	virtual void refetch();

	/* refetches everything from database as it is, keeping selected elements,
	   the user won't recognize anything at all */
	virtual void refresh();

	/* triggered from outside, when playing time or bitrate has been changed*/
	virtual void psl_metadata_changed(const MetaData&);

	/* triggered by tagedit */
	virtual void psl_metadata_changed(const MetaDataList&, const MetaDataList&);

	/* Calls change_[artist|album|track]_selection and emits signal */
	// emits new albums and tracks
	virtual void psl_selected_artists_changed(const QList<int>& idx_lst);

	// emits new tracks
	virtual void psl_selected_albums_changed(const QList<int>& idx_lst);

	// emits new mime data
	virtual void psl_selected_tracks_changed(const QList<int>& idx_lst);

	// emits new tracks, very similar to psl_selected_albums_changed
	virtual void psl_disc_pressed(int);


	// [albums|artist|track[s]] double clicked -> send tracks to playlist
	virtual void psl_prepare_album_for_playlist(int idx=0);
	virtual void psl_prepare_artist_for_playlist(int idx=0);
	// Those two functions are identical (1) calls (2)
	virtual void psl_prepare_track_for_playlist(int idx);
	virtual void psl_prepare_tracks_for_playlist(QList<int> idx_lst);

	// calls fetch_by_filter and emits
	virtual void psl_filter_changed(const Filter&, bool force=false);

	/*delete tracks, if answer is 1, also files are removed */
	virtual void psl_delete_tracks(TrackDeletionMode answer);
	virtual void psl_delete_certain_tracks(const QList<int>& idx_lst, TrackDeletionMode answer);

	/* append tracks after current played track in playlist */
	virtual void psl_play_next_all_tracks();
	virtual void psl_play_next_tracks(const QList<int>& idx_lst);

	/* append tracks after last track in playlist */
	virtual void psl_append_all_tracks();
	virtual void psl_append_tracks(const QList<int>&);

	/* write new rating to database */
	virtual void psl_track_rating_changed(int idx, int rating);
	virtual void psl_album_rating_changed(int idx, int rating);


protected slots:

	virtual void library_reloading_state_slot(QString);
	virtual void library_reloading_state_new_block();
	virtual void reload_thread_finished();

	/* Looks, which sortorder is changed and refetches corresponding fields*/
	virtual void _sl_sortorder_changed();
	virtual void _sl_libpath_changed();

private:
	MetaData			_old_md;

protected:

    CDatabaseConnector*	_db;

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
	LibSortOrder		_sortorder;

	/* Emit 3 signals with shown artists, shown album, shown tracks */
	virtual void 		emit_stuff();

	/* delete tracks by metadata, is called by the deletion slots */
	virtual void		delete_tracks(const MetaDataList& v_md, TrackDeletionMode answer);

	/* delete all tracks from database */
	virtual void		empty_library();


	/* a searchfilter has been entered, nothing is emitted */
	virtual void		fetch_by_filter(const Filter& filter, bool force);


	/* changes artist, album, track selections, nothing is emitted */
	virtual void		change_artist_selection(const QList<int>& idx_list);
	virtual void		change_album_selection(const QList<int>& idx_list);
	MetaDataList		change_track_selection(const QList<int>& idx_list);

	void restore_artist_selection(const QList<int>& old_selected_idx);
	void restore_track_selection(const QList<int>& old_selected_idx);
	void restore_album_selection(const QList<int>& old_selected_idx);


};

#endif // LocalLibrary_H
