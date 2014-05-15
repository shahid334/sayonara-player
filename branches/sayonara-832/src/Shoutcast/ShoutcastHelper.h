#ifndef SHOUTCASTHELPER_H
#define SHOUTCASTHELPER_H

#include "HelperStructs/Helper.h"
#include <QtXml>

namespace ShoutcastHelper
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
