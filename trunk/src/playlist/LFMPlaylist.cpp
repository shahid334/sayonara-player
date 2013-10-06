#include "playlist/LFMPlaylist.h"
#include "StreamPlugins/LastFM/LastFM.h"


LFMPlaylist::LFMPlaylist(QObject* parent) : Playlist(parent) {
    _lfm = LastFM::getInstance();
    _playlist_type = PlaylistTypeLFM;


    connect(this, SIGNAL(sig_need_more_radio()), _lfm, SLOT(psl_radio_playlist_request()));
    connect(_lfm, SIGNAL(sig_new_radio_playlist(const MetaDataList&)), this, SLOT(new_radio_playlist(const MetaDataList&)));

}


void LFMPlaylist::play(){
    return;
}

void LFMPlaylist::pause(){
    return;
}

void LFMPlaylist::stop(){
    _cur_play_idx = -1;
    for(int i=0; i<(int)_v_md.size(); i++){
        _v_md[i].pl_playing = false;
        _v_md[i].is_disabled = true;
    }

    report_changes(false, true);
}

void LFMPlaylist::next(){
    this->fwd();
}

void LFMPlaylist::fwd(){
    // Look if there are enough tracks in the playlist buffer
    // and fetch if not, else add next track to v_md
    if(_v_md_hidden.size() < 2){
        emit sig_need_more_radio();
    }

    if(_v_md_hidden.size() > 0){
        _v_md[_cur_play_idx].is_disabled = true;

        _v_md.push_back(_v_md_hidden[0]);
        _v_md_hidden.erase(_v_md_hidden.begin());
        _cur_play_idx++;
        _v_md[_cur_play_idx].pl_playing = true;
    }
    else{
        stop();
        return;
    }

    report_changes(true, true);
}

void LFMPlaylist::bwd(){
    return;
}

void LFMPlaylist::change_track(int idx){
    Q_UNUSED(idx);
    return;
}


void LFMPlaylist::insert_tracks(const MetaDataList& lst, int tgt){
    return;
}

void LFMPlaylist::append_tracks(const MetaDataList& lst){
    if(lst.size() == 0) return;

    bool start_playing = false;
    if(_cur_play_idx == -1){
        start_playing = true;
        _cur_play_idx = 0;
        _v_md.push_back(lst[0]);

    }
    else
        _v_md_hidden.push_back(lst[0]);

    for(int i=1; i<(int) lst.size(); i++)
        _v_md_hidden.push_back(lst[i]);

    report_changes(true, start_playing);
}


void LFMPlaylist::create_playlist(const MetaDataList& lst, bool start_playing){
    Q_UNUSED(lst);
    Q_UNUSED(start_playing);

    _v_md.clear();
    _cur_play_idx = -1;
    report_changes(true, true);


    emit sig_need_more_radio();
    return;
}

void LFMPlaylist::create_playlist(const QStringList& lst, bool start_playing){
    Q_UNUSED(lst);
    Q_UNUSED(start_playing);


    return;
}

void LFMPlaylist::metadata_changed(const MetaDataList &md_list){
    Q_UNUSED(md_list);
    return;
}


void LFMPlaylist::clear(){
    disable_reports();
    Playlist::clear();
    _v_md_hidden.clear();
    enable_reports();

    report_changes(true, false);
}


void LFMPlaylist::save_for_reload(){
    return;
}

void LFMPlaylist::save_to_m3u_file(QString filepath, bool relative){
    return;
}

bool LFMPlaylist::request_playlist_for_collection(MetaDataList& lst){
    return false;
}

void LFMPlaylist::new_radio_playlist(const MetaDataList& lst){

    append_tracks(lst);
}

