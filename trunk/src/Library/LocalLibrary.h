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
#include "HelperStructs/DirectoryReader/DirectoryReader.h"
#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/Filter.h"
#include "Library/AbstractLibrary.h"

#include <QThread>
#include <QStringList>
#include <QFileSystemWatcher>


class LocalLibrary : public AbstractLibrary
{

    Q_OBJECT

public:
	LocalLibrary(QObject *parent = 0);




signals:
	void sig_no_library_path();


public slots:

	virtual void delete_tracks(const MetaDataList& v_md, TrackDeletionMode answer);
	virtual void psl_reload_library(bool);

	// emits new tracks, very similar to psl_selected_albums_changed
	virtual void psl_disc_pressed(int);

protected slots:

	virtual void library_reloading_state_slot(QString);
	virtual void library_reloading_state_new_block();
	virtual void reload_thread_finished();

	/* Looks, which sortorder is changed and refetches corresponding fields*/
	virtual void _sl_libpath_changed();


protected:

    CDatabaseConnector*	_db;

	QString				_library_path;

    ReloadThread* 		_reload_thread;
	int					_reload_progress;

	virtual void		get_all_artists(ArtistList& artists, LibSortOrder so);
	virtual void		get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so);

	virtual void		get_all_albums(AlbumList& albums, LibSortOrder so);
	virtual void		get_all_albums_by_artist(QList<int> artist_ids, AlbumList& albums, Filter filter, LibSortOrder so);
	virtual void		get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so);

	virtual void		get_all_tracks(MetaDataList& v_md, LibSortOrder so);
	virtual void		get_all_tracks_by_artist(QList<int> artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so);
	virtual	void		get_all_tracks_by_album(QList<int> album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so);
	virtual void		get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so);

	virtual void		get_album_by_id(int album_id, Album& album);
	virtual void		get_artist_by_id(int artist_id, Artist& artist);


	virtual void		update_track(const MetaData& md);
	virtual void		update_album(const Album& album);

};

#endif // LocalLibrary_H
