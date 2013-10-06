#include "playlist/PlaylistLoader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Tagging/id3.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include <QDir>



PlaylistLoader::PlaylistLoader(QObject *parent) :
    QObject(parent)
{
    _settings = CSettingsStorage::getInstance();
}


void PlaylistLoader::load_old_playlist(){

        bool loadPlaylist = _settings->getLoadPlaylist();
        if( !loadPlaylist ) return;

        bool load_last_track = _settings->getLoadLastTrack();
        LastTrack* last_track = _settings->getLastTrack();
        bool load_last_position = _settings->getRememberTime();
        bool start_immediatly = _settings->getStartPlaying();

        QStringList saved_playlist = _settings->getPlaylist();

        if(saved_playlist.size() == 0) return;

        // the path of the last played track
        QString last_track_path = last_track->filepath;
        QDir d2(last_track_path);
        last_track_path = d2.absolutePath();

        int last_track_idx = -1;
        MetaDataList v_md;

        // run over all tracks
        for(int i=0; i<saved_playlist.size(); i++){

            // convert item into MetaData
            QString item = saved_playlist[i];
            if(item.size() == 0) continue;

            // maybe we can get a track id
            bool ok;
            int track_id = item.toInt(&ok);

            // maybe it's an filepath
            QString path_in_list = item;
            QDir d(path_in_list);
            path_in_list = d.absolutePath();

            MetaData track;
            CDatabaseConnector* db = CDatabaseConnector::getInstance();

            // we have a track id
            if(track_id >= 0 && ok){
                track = db->getTrackById(track_id);

                // this track id cannot be found in db
                if(track.id < 0){
                    if(!ID3::getMetaDataOfFile(track)) continue;
                    track.is_extern = true;
                }

                else
                    track.is_extern = false;

                if(track_id == last_track->id)
                    last_track_idx = i;
            }

            // we have an filepath
            else{
                if(!QFile::exists(path_in_list)) continue;

                // maybe it's in the library neverthe less
                track = db->getTrackByPath(path_in_list);
                // we expected that.. try to get metadata
                if(track.id < 0){
                    if(!ID3::getMetaDataOfFile(track)) continue;
                }

                track.is_extern = true;

                if(!path_in_list.compare(last_track_path, Qt::CaseInsensitive)){
                    last_track_idx = i;
                }
            }

            v_md.push_back(track);
        }

        if(v_md.size() == 0) return;

        if(last_track_idx == -1) {
            start_immediatly = false;
            load_last_position = false;
            load_last_track = false;
            emit sig_create_playlist(v_md, false);
            emit sig_stop();
            return;
        }

        emit sig_create_playlist(v_md, start_immediatly);


        int last_pos = 0;
        if(load_last_track && last_track_idx >= 0){
            if(load_last_position)
                last_pos = last_track->pos_sec;

            emit sig_change_track(last_track_idx, last_pos, start_immediatly);
        }

}

