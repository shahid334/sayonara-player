#include "Shoutcast/ShoutcastLibrary.h"
#include "Shoutcast/ShoutcastHelper.h"
#include "HelperStructs/WebAccess.h"


ShoutcastLibrary::ShoutcastLibrary(QWidget* main_window, QObject *parent) :
	QObject(parent)
{

	_main_window = main_window;
	_settings = CSettingsStorage::getInstance();
	_db = CDatabaseConnector::getInstance();

	QList<int> sortings = _settings->getLibSorting();


	_artist_sortorder = (SortOrder) sortings[0];
	_album_sortorder = (SortOrder) sortings[1];
	_track_sortorder = (SortOrder) sortings[2];
}




bool ShoutcastLibrary::dl_all_artist_info(QString name, Artist& artist){
	bool success;
	QString content;

	QString dl = ShoutcastHelper::create_dl_get_artist(name);
	if(dl.size() == 0) return false;

	success = WebAccess::read_http_into_str(dl, &content);
	if(!success || content.size() == 0) return false;

	success = ShoutcastHelper::parse_artist_xml(content, artist);
	return success;
}


bool ShoutcastLibrary::dl_all_playlists_by_artist(qint64 artist_id, QString& content){

	bool success;

	QString dl = ShoutcastHelper::create_dl_get_playlists(artist_id);
	if(dl.size() == 0) return false;

	success = WebAccess::read_http_into_str(dl, &content);
	if(!success || content.size() == 0) {
		qDebug() << "Cannot get any data from " << dl;
		return false;
	}

	return success;
}


bool ShoutcastLibrary::dl_all_tracks_by_artist(qint64 artist_id, QString& content){

	bool success;

	QString dl = ShoutcastHelper::create_dl_get_tracks(artist_id);
	if(dl.size() == 0) return false;

	success = WebAccess::read_http_into_str(dl, &content);
	if(!success || content.size() == 0) {
		qDebug() << "Cannot get any data from " << dl;
		return false;
	}

	return success;
}


void ShoutcastLibrary::loadData(){

	bool success;
	Artist artist;
	QString content;

	success = dl_all_artist_info("Alloinyx", artist);
	if(!success){
		qDebug() << "Cannot get info from Shoutcast Artist Alloinyx";
		return;
	}

	success = dl_all_playlists_by_artist(artist.id, content);
	if(!success){
		return;
	}

	success = ShoutcastHelper::parse_playlist_xml(content, _vec_md, _vec_artists, _vec_albums);
	if(!success){
		qDebug() << "Could not parse Playlists xml file";
		return;
	}

	qDebug() << "Got " << _vec_md.size() << " tracks "
			 << ", " << _vec_artists.size() << " artists "
			 << ", " << _vec_albums.size() << " albums";

	content.clear();

	MetaDataList v_md;
	success = dl_all_tracks_by_artist(artist.id, content);
	if(!success){
		return;
	}

	success = ShoutcastHelper::parse_tracks_xml(content, v_md);
	qDebug() << "Got " << v_md.size() << " tracks ";

	if( v_md.size() == _vec_md.size() ) return;

	foreach(MetaData md_wo_pl, v_md){

		bool track_found_in_pl = false;
		foreach(MetaData md_in_pl, _vec_md){

			if(md_wo_pl.id == md_in_pl.id){
				track_found_in_pl = true;
				break;
			}
		}

		if(!track_found_in_pl){
			qDebug() << "Add one track " << md_wo_pl.title;
			md_wo_pl.album_id = 0;
			md_wo_pl.album = tr("Misc");
			_vec_md.push_back(md_wo_pl);
		}
	}
}


void ShoutcastLibrary::reloadLibrary(bool clear){

}

void ShoutcastLibrary::clearLibrary(){

}

void ShoutcastLibrary::refresh(bool emit_changed){

}

void ShoutcastLibrary::psl_selected_artists_changed(const QList<int>& lst){

}

void ShoutcastLibrary::psl_selected_albums_changed(const QList<int>& lst){

}

void ShoutcastLibrary::psl_selected_tracks_changed(const QList<int>& lst){

}

void ShoutcastLibrary::psl_prepare_album_for_playlist(int idx){

}

void ShoutcastLibrary::psl_prepare_artist_for_playlist(int idx){

}

void ShoutcastLibrary::psl_prepare_track_for_playlist(int idx){

}

void ShoutcastLibrary::psl_prepare_tracks_for_playlist(QList<int> lst){

}


void ShoutcastLibrary::
psl_sortorder_changed(Sort::SortOrder artist_sort, Sort::SortOrder album_sort, Sort::SortOrder track_sort){

}

void ShoutcastLibrary::psl_delete_tracks(int idx){

}

void ShoutcastLibrary::psl_delete_certain_tracks(const QList<int>&,int lst){

}

void ShoutcastLibrary::psl_play_next_all_tracks(){

}

void ShoutcastLibrary::psl_play_next_tracks(const QList<int>& lst){

}

void ShoutcastLibrary::psl_append_all_tracks(){

}

void ShoutcastLibrary::psl_append_tracks(const QList<int>& lst){

}

void ShoutcastLibrary::psl_track_rating_changed(int idx, int rating){

}

void ShoutcastLibrary::psl_album_rating_changed(int idx, int rating){

}

void ShoutcastLibrary::library_reloading_state_slot(QString str){

}

void ShoutcastLibrary::library_reloading_state_new_block(){

}

void ShoutcastLibrary::reload_thread_finished(){

}
