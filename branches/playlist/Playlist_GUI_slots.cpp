#include "playlist/Playlist.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/CSettingsStorage.h"

#include <QString>

// GUI -->
void Playlist::psl_clear_playlist(){

    _v_stream_playlist.clear();
    _v_meta_data.clear();
    _cur_play_idx = -1;


    if(_radio_active == RADIO_OFF)
        psl_save_playlist_to_storage();

    _radio_active = RADIO_OFF;

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);

}



// play a track
void Playlist::psl_play(){

    if(_v_meta_data.size() <= 0) return;

    // state was stop until now
    if(_cur_play_idx <= -1){

        int track_num = 0;
        MetaData md = _v_meta_data[track_num];

        if( checkTrack(md) ){
            _cur_play_idx = track_num;

            emit sig_selected_file_changed(track_num);
            emit sig_selected_file_changed_md(md);
        }
    }

    else{
        emit sig_goon_playing();
    }

}

void Playlist::psl_stop(){

    /*if(_radio_active == RADIO_STATION){
        save_stream_playlist();
    }*/

    // track no longer valid
    if(_radio_active == RADIO_LFM){
        psl_clear_playlist();
    }

    _radio_active = RADIO_OFF;

    _cur_play_idx = -1;

    emit sig_no_track_to_play();
    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);
}

// fwd was pressed -> next track
void Playlist::psl_forward(){

    psl_next_track();
    return;

    MetaData md;
    int track_num = -1;

    if(_radio_active == RADIO_LFM){
        if(_v_meta_data.size() == 0){
            emit sig_no_track_to_play();
            emit sig_need_more_radio();
            return;
        }

        int track_num = _cur_play_idx + 1;

        // track too high
        if(track_num > (int) _v_meta_data.size() -1){
            track_num = -1;
            emit sig_no_track_to_play();
            emit sig_need_more_radio();
            return;
        }

        // last track
        else if(track_num == (int) _v_meta_data.size() -1) {
            emit sig_need_more_radio();
        }

        emit sig_playlist_created(_v_meta_data, track_num, _radio_active);
        emit sig_selected_file_changed(track_num);
        emit sig_selected_file_changed_md(_v_meta_data[track_num]);

        _cur_play_idx = track_num;
        return;
    }


    // this shouldn't happen, because forward is disabled
    else if(_radio_active == RADIO_STATION){
        return;
    }

    // SHUFFLE
    if(_playlist_mode.shuffle){
        track_num = rand() % _v_meta_data.size();
    }

    // new track within normal playlist
    else if(_cur_play_idx < (int) _v_meta_data.size() - 1 && _cur_play_idx >= 0){

        if( _playlist_mode.repAll )
            track_num = (_cur_play_idx + 1) % _v_meta_data.size();
        else
            track_num = (_cur_play_idx + 1);
    }

    md = _v_meta_data[track_num];
    md.radio_mode = _radio_active;

    if( checkTrack(md) ){

        _cur_play_idx = track_num;
        _v_meta_data.setCurPlayTrack(track_num);

        emit sig_selected_file_changed(track_num);
        emit sig_selected_file_changed_md(md);
    }
}

// GUI -->
void Playlist::psl_backward(){

    // this shouldn't happen, because backward is disabled
    if(_radio_active != RADIO_OFF) return;

    if(this->_cur_play_idx <= 0) return;

    int track_num = _cur_play_idx - 1;
    MetaData md = _v_meta_data[track_num];
    md.radio_mode = _radio_active;

    if( checkTrack(md) ){
        _cur_play_idx = track_num;
        _v_meta_data.setCurPlayTrack(track_num);
        emit sig_selected_file_changed(track_num);
        emit sig_selected_file_changed_md(md);

    }
}



// GUI -->
void Playlist::psl_change_track(int new_row){

    if( (uint) new_row >= _v_meta_data.size()) return;
    if( _radio_active == RADIO_LFM) return;

    MetaData md = _v_meta_data[new_row];
    md.pl_playing = true;

    if( checkTrack(md) ){

        _cur_play_idx = new_row;
        _v_meta_data.setCurPlayTrack(_cur_play_idx);

        emit sig_selected_file_changed_md(md);
        emit sig_selected_file_changed(_cur_play_idx);
    }

    else{
        _cur_play_idx = -1;
        _db->deleteTrack(md);
        _v_meta_data.setCurPlayTrack(_cur_play_idx);

        remove_row(new_row);
        emit sig_no_track_to_play();
    }
}


// insert tracks (also drag & drop)
void Playlist::psl_insert_tracks(const MetaDataList& v_metadata, int row){

    // turn off radio
    bool switched_from_radio = false;
    if(_radio_active != RADIO_OFF){
        switched_from_radio = true;
        psl_stop();
        row = 0;
    }

    _radio_active = RADIO_OFF;

    // possibly the current playing index has to be updated
    if(row <= _cur_play_idx && _cur_play_idx != -1)
        _cur_play_idx += v_metadata.size();


    // insert new tracks
    for(uint i=0; i<v_metadata.size(); i++){

        MetaData md = v_metadata[i];
        MetaData md_tmp = _db->getTrackByPath(md.filepath);

        if( md_tmp.id >= 0 ){
            md.is_extern = false;
        }

        else {
            md.is_extern = true;
            md.radio_mode = RADIO_OFF;
        }

        _v_meta_data.insert_mid(md, i + row);
    }


    psl_save_playlist_to_storage();
    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);

    // radio was turned off, so we start at beginning of playlist
    if(switched_from_radio && _v_meta_data.size() > 0){
        _cur_play_idx = 0;
        emit sig_selected_file_changed(0);
        emit sig_selected_file_changed_md(_v_meta_data[0]);
    }
}



void Playlist::remove_row(int row){
    QList<int> remove_list;
    remove_list << row;
    psl_remove_rows(remove_list);
}

// remove one row
void Playlist::psl_remove_rows(const QList<int> & rows){

    MetaDataList v_tmp_md;

    int n_tracks = (int) _v_meta_data.size();
    int n_tracks_before_cur_idx = 0;

    bool* to_delete = new bool[n_tracks];

    if(rows.contains(_cur_play_idx)) _cur_play_idx = -1;

    for(int i=0; i<n_tracks; i++){

        if(rows.contains(i)) {
            if(i < _cur_play_idx)
                n_tracks_before_cur_idx++;

            continue;
        }

        MetaData md = _v_meta_data[i];

        md.pl_dragged = false;
        md.pl_selected = false;
        md.pl_playing = false;

        v_tmp_md.push_back(md);
    }

    _cur_play_idx -= n_tracks_before_cur_idx;

    if(_cur_play_idx < 0 )
        _cur_play_idx = -1;

    else
        v_tmp_md[_cur_play_idx].pl_playing = true;

    _v_meta_data = v_tmp_md;

    psl_save_playlist_to_storage();

    emit sig_playlist_created(_v_meta_data, _cur_play_idx, _radio_active);

    delete to_delete;
}



// GUI -->
void Playlist::psl_playlist_mode_changed(const Playlist_Mode& playlist_mode){

    CSettingsStorage::getInstance()->setPlaylistMode(playlist_mode);
    _playlist_mode = playlist_mode;
    _playlist_mode.print();

}


