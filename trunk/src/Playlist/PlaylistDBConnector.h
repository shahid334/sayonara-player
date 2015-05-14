#ifndef PLAYLISTDBCONNECTOR_H
#define PLAYLISTDBCONNECTOR_H

#include "HelperStructs/MetaData/MetaData.h"
#include "DatabaseAccess/CDatabaseConnector.h"

class PlaylistDBConnector
{

	SINGLETON(PlaylistDBConnector)

private:
	CDatabaseConnector* _db;

	void apply_tags(MetaDataList& v_md);
	bool get_playlists(QList<CustomPlaylist>& playlists,
					   CDatabaseConnector::PlaylistChooserType type,
					   SortOrderPlaylists sortorder);

	bool extract_stream(CustomPlaylist& pl, QString name, QString url);


public:



	bool get_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
						   CDatabaseConnector::PlaylistChooserType type,
						   SortOrderPlaylists so=Sort::IDAsc);

	bool get_all_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
						   SortOrderPlaylists so=Sort::IDAsc);

	bool get_temporary_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
						   SortOrderPlaylists so=Sort::IDAsc);

	bool get_non_temporary_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
						   SortOrderPlaylists so=Sort::IDAsc);

	bool get_all_playlists(QList<CustomPlaylist>& playlists,
						   SortOrderPlaylists so=Sort::IDAsc);

	bool get_temporary_playlists(QList<CustomPlaylist>& playlists,
								 SortOrderPlaylists so=Sort::IDAsc);

	bool get_non_temporary_playlists(QList<CustomPlaylist>& playlists,
									 SortOrderPlaylists so=Sort::IDAsc);

	CustomPlaylist get_playlist_by_id(int id);
	CustomPlaylist get_playlist_by_name(const QString& name);

	bool save_playlist_as(const MetaDataList& v_md, const QString& name);
	bool save_playlist_temporary(const MetaDataList& v_md, const QString& name);
	bool save_playlist(const CustomPlaylist& pl);
	bool save_playlist(const MetaDataList& v_md, int id);
	bool save_playlist(const MetaDataList& v_md, const QString name);


	bool delete_playlist(int id);
	bool delete_playlist(const QString& name);
	bool exists(const QString& name);
};

#endif // PLAYLISTDBCONNECTOR_H
