/* SoundcloudHelper.h */

/* Copyright (C) 2011-2014  Lucio Carreras
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



#ifndef SHOUTCASTHELPER_H
#define SHOUTCASTHELPER_H

#include "HelperStructs/Helper.h"
#include <QtXml>

namespace SoundcloudHelper
{

	QString				create_dl_get_artist(QString name);
	QString				create_dl_get_playlists(qint64 artist_id);
	QString				create_dl_get_tracks(qint64 artist_id);

	bool				parse_artist_xml(const QString& content, Artist& artist);
	bool				parse_playlist_xml(const QString& content,
									   MetaDataList& v_md,
									   ArtistList& v_artists,
									   AlbumList& v_albums);
	bool				parse_tracks_xml(const QString& content,
										 MetaDataList& v_md);

	bool				parse_artist_dom(QDomNode node, Artist& artist);
	bool				parse_track_dom(QDomNode node, MetaData& md, Artist& artist);



};

#endif // SHOUTCASTHELPER_H
