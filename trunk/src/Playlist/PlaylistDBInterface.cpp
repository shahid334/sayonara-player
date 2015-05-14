#include "Playlist/PlaylistDBInterface.h"

PlaylistDBInterface::PlaylistDBInterface(const QString& name)
{
	_playlist_db_connector = PlaylistDBConnector::getInstance();

	_name = name;
	_db_id = _playlist_db_connector->get_playlist_by_name(name).id;

	_is_temporary = true;
}

int PlaylistDBInterface::get_db_id() const {
	return _db_id;
}

void PlaylistDBInterface::set_db_id(int db_id){
	_db_id = db_id;
}


QString PlaylistDBInterface::get_name() const {
	return _name;
}

void PlaylistDBInterface::set_name(const QString& name){
	_name = name;
}

bool PlaylistDBInterface::is_temporary() const{
	return _is_temporary;
}

void PlaylistDBInterface::set_temporary(bool b){
	_is_temporary = b;
}


void PlaylistDBInterface::set_skeleton(const CustomPlaylistSkeleton& skeleton){
	_name = skeleton.name;
	_db_id = skeleton.id;
	_is_temporary = skeleton.is_temporary;
}



PlaylistDBInterface::SaveAsAnswer PlaylistDBInterface::save(){

	MetaDataList v_md = this->get_playlist();
	bool success = false;

	if(_db_id >= 0){
		success = _playlist_db_connector->save_playlist(v_md, _db_id);
	}

	else {
		return save_as(_name, true);
	}

	if(success){
		return SaveAs_Success;
	}

	return SaveAs_Error;
}


bool PlaylistDBInterface::insert_temporary_into_db(){

	if(!_is_temporary) return false;
	if(_db_id != -1) return false;
	MetaDataList v_md = get_playlist();

	bool success = _playlist_db_connector->save_playlist_temporary(v_md, _name);
	if(!success){
		return false;
	}

	_db_id = _playlist_db_connector->get_playlist_by_name(_name).id;

	return true;
}


PlaylistDBInterface::SaveAsAnswer PlaylistDBInterface::save_as(const QString& name, bool force_override){

	MetaDataList v_md = this->get_playlist();

	int tgt_id = -1;
	bool success;
	QList<CustomPlaylistSkeleton> skeletons;
	_playlist_db_connector->get_all_skeletons(skeletons);

	for(const CustomPlaylistSkeleton& skeleton : skeletons){

		QString tmp_name = skeleton.name;

		if( tmp_name.compare(name, Qt::CaseInsensitive) == 0 ){

			tgt_id = skeleton.id;

			if(force_override){
				return SaveAs_AlreadyThere;
			}

			else {
				break;
			}
		}
	}

	// Name already exists, override
	if(tgt_id >= 0){
		success = _playlist_db_connector->save_playlist(v_md, tgt_id);
	}

	// New playlist
	else{
		success = _playlist_db_connector->save_playlist_as( v_md, name);
	}

	if(success){
		return SaveAs_Success;
	}

	return SaveAs_Error;
}


bool PlaylistDBInterface::remove_from_db(){

	bool success;
	if(_db_id >= 0){
		success = _playlist_db_connector->delete_playlist(_db_id);
	}

	else{
		success = _playlist_db_connector->delete_playlist(_name);
	}

	_is_temporary = true;
	return success;
}


QString PlaylistDBInterface::request_new_db_name(){

	QList<CustomPlaylistSkeleton> skeletons;
	PlaylistDBConnector::getInstance()->get_all_skeletons(skeletons);

	QString target_name;

	for(int idx = 1; idx < 1000; idx++){

		bool found = false;
		target_name = tr("New %1").arg(idx);
		for(const CustomPlaylistSkeleton& skeleton : skeletons){

			QString name = skeleton.name;

			if(name.compare(target_name, Qt::CaseInsensitive) == 0){
				found = true;
				break;
			}
		}

		if(!found){
			break;
		}
	}

	return target_name;
}
