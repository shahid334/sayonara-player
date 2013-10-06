
#include "playlist/StreamPlaylist.h"

StreamPlaylist::StreamPlaylist(QObject *parent) :
    StdPlaylist(parent)
{
    _playlist_type = PlaylistTypeStream;
}

/*

void StreamPlaylist:: play(){

}

void StreamPlaylist::pause(){

}

void StreamPlaylist::stop(){

}

void StreamPlaylist::fwd(){

}

void StreamPlaylist::bwd(){

}

void StreamPlaylist::next(){

}

void StreamPlaylist::change_track(int idx){

}

void StreamPlaylist::create_playlist(const MetaDataList& lst, bool start_playing){

}

void StreamPlaylist::create_playlist(const QStringList& lst, bool start_playing){

}

void StreamPlaylist::clear(){

}

void StreamPlaylist::metadata_changed(const MetaDataList& md_list){

}
void StreamPlaylist::save_for_reload(){

}
void StreamPlaylist::save_to_m3u_file(QString filepath, bool relative){

}

bool StreamPlaylist::request_playlist_for_collection(MetaDataList& lst){
    return true;
}
*/
