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

#include "Library/AbstractLibrary.h"
#include "SoundcloudData.h"
#include "HelperStructs/MetaData.h"



class SoundcloudLibrary : public AbstractLibrary
{
	Q_OBJECT

public:
	SoundcloudLibrary(QObject *parent = 0);

protected:

	SoundcloudData*		_scd;

	virtual void		get_all_artists(ArtistList& artists, LibSortOrder so);
	virtual void		get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so);

	virtual void		get_all_albums(AlbumList& albums, LibSortOrder so);
	virtual void		get_all_albums_by_artist(QList<int> artist_ids, AlbumList& albums, Filter filter, LibSortOrder so);
	virtual void		get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so);

	virtual void		get_all_tracks(MetaDataList& v_md, LibSortOrder so);
	virtual void		get_all_tracks_by_artist(QList<int> artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so);
	virtual	void		get_all_tracks_by_album(QList<int> album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so);
	virtual void		get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so);

	virtual void		update_track(const MetaData& md);
	virtual void		update_album(const Album& album);

};

#endif // LocalLibrary_H
