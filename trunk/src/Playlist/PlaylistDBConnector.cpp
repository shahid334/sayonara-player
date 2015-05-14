#include "Playlist/PlaylistDBConnector.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Parser/PlaylistParser.h"

PlaylistDBConnector::PlaylistDBConnector()
{
	_db = CDatabaseConnector::getInstance();
}


PlaylistDBConnector::~PlaylistDBConnector(){

}


void PlaylistDBConnector::apply_tags(MetaDataList& v_md){
	for(MetaData& md : v_md){
		if(md.is_extern){

			if(Helper::is_file(md.filepath())){
				ID3::getMetaDataOfFile(md);
			}
		}
	}
}


bool PlaylistDBConnector::get_skeletons(QList<CustomPlaylistSkeleton>& skeletons, CDatabaseConnector::PlaylistChooserType type, SortOrderPlaylists so){
	return _db->getAllPlaylistSkeletons(skeletons, type, so);
}

bool PlaylistDBConnector::get_all_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
					   SortOrderPlaylists so)
{
	return get_skeletons(skeletons, CDatabaseConnector::TemporaryAndNoTemporary, so);
}

bool PlaylistDBConnector::get_temporary_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
					   SortOrderPlaylists so)
{
	return get_skeletons(skeletons, CDatabaseConnector::OnlyTemporary, so);
}

bool PlaylistDBConnector:: get_non_temporary_skeletons(QList<CustomPlaylistSkeleton>& skeletons,
					   SortOrderPlaylists so)
{
	return get_skeletons(skeletons, CDatabaseConnector::OnlyNoTemporary, so);
}


bool PlaylistDBConnector::extract_stream(CustomPlaylist& pl, QString name, QString url){

	pl.is_temporary = false;

	if(Helper::is_playlistfile(url)) {
		if(PlaylistParser::parse_playlist(url, pl.tracks) > 0) {

			for(MetaData& md : pl.tracks) {

				md.album = name;
				if(md.title.isEmpty()){
					md.title = name;
				}

				if(md.artist.isEmpty()){
					md.artist = url;
				}

				md.is_extern = true;
				md.id = -1;
			}
		}
	}

	// real stream
	else{

		MetaData md;

		md.title = name;
		md.album = name;
		md.artist = url;
		md.set_filepath(url);

		md.is_extern = true;
		md.id = -1;

		pl.tracks.push_back(md);

	}

	pl.is_valid = (pl.tracks.size() > 0);


	return pl.is_valid;
}


bool PlaylistDBConnector::get_playlists(QList<CustomPlaylist>& playlists, CDatabaseConnector::PlaylistChooserType type, SortOrderPlaylists so){

	bool success;
	QList<CustomPlaylistSkeleton> skeletons;

	success = get_all_skeletons(skeletons, so);
	if(!success){
		return false;
	}

	QMap<QString, QString> streams;
	success = _db->getAllStreams(streams);

	for(const CustomPlaylistSkeleton& skeleton : skeletons){

		CustomPlaylist pl(skeleton);

		bool is_stream = false;

		if(success){
			for(const QString& key : streams.keys()){
				if(skeleton.name.compare(key) == 0){
					is_stream = true;
					extract_stream(pl, key, streams[key]);
					break;
				}
			}
		}

		if(!is_stream){
			success = _db->getPlaylistById(pl);

			if(!success){
				continue;
			}

			apply_tags(pl.tracks);
		}

		playlists.push_back(pl);
	}

	return true;
}


bool PlaylistDBConnector::get_all_playlists(QList<CustomPlaylist>& playlists, SortOrderPlaylists so){
	return get_playlists(playlists, CDatabaseConnector::TemporaryAndNoTemporary, so);
}


bool PlaylistDBConnector::get_temporary_playlists(QList<CustomPlaylist>& playlists, SortOrderPlaylists so){
	return get_playlists(playlists, CDatabaseConnector::OnlyTemporary, so);
}


bool PlaylistDBConnector::get_non_temporary_playlists(QList<CustomPlaylist>& playlists, SortOrderPlaylists so){
	return get_playlists(playlists, CDatabaseConnector::OnlyNoTemporary, so);
}


CustomPlaylist PlaylistDBConnector::get_playlist_by_id(int id){

	bool success;
	CustomPlaylist pl;
	pl.id = id;

	success = _db->getPlaylistById(pl);
	pl.is_valid = success;

	if(!success){
		return pl;
	}

	return pl;
}


CustomPlaylist PlaylistDBConnector::get_playlist_by_name(const QString& name){


	int id = _db->getPlaylistIdByName(name);

	if(id < 0){
		CustomPlaylist pl;
		pl.id = -1;
		pl.is_valid = false;
		return pl;
	}

	return get_playlist_by_id(id);
}


bool PlaylistDBConnector::save_playlist_as(const MetaDataList& v_md, const QString& name){
	return _db->storePlaylist(v_md, name, false);
}

bool PlaylistDBConnector::save_playlist_temporary(const MetaDataList& v_md, const QString& name){
	return _db->storePlaylist(v_md, name, true);
}


bool PlaylistDBConnector::save_playlist(const CustomPlaylist& pl){
	return _db->storePlaylist(pl.tracks, pl.id, pl.is_temporary);
}


bool PlaylistDBConnector::save_playlist(const MetaDataList& v_md, int id){

	bool success;
	CustomPlaylistSkeleton skeleton;
	skeleton.id = id;
	success = _db->getPlaylistSkeletonById(skeleton);

	if(!success){
		return false;
	}

	return _db->storePlaylist(v_md, id, skeleton.is_temporary);
}


bool PlaylistDBConnector::save_playlist(const MetaDataList& v_md, const QString name){

	CustomPlaylistSkeleton skeleton;
	int id = _db->getPlaylistIdByName(name);
	if(id < 0){
		return false;
	}

	skeleton.id = id;
	_db->getPlaylistSkeletonById(skeleton);

	return _db->storePlaylist(v_md, id, skeleton.is_temporary );
}


bool PlaylistDBConnector::delete_playlist(int id){
	return _db->deletePlaylist(id);
}


bool PlaylistDBConnector::delete_playlist(const QString& name){
	int id = _db->getPlaylistIdByName(name);
	return _db->deletePlaylist(id);
}


bool PlaylistDBConnector::exists(const QString& name){
	int id = _db->getPlaylistIdByName(name);
	return (id >= 0);
}
