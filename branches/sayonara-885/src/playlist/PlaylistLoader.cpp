/* PlaylistLoader.cpp */

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



#include "playlist/PlaylistLoader.h"
#include "HelperStructs/MetaData.h"
#include "HelperStructs/Tagging/id3.h"
#include "DatabaseAccess/CDatabaseConnector.h"
#include <QDir>



PlaylistLoader::PlaylistLoader(QObject *parent) :
    QObject(parent)
{
    _settings = Settings::getInstance();
}


void PlaylistLoader::load_old_playlist() {

        CDatabaseConnector* db = CDatabaseConnector::getInstance();

		bool load_playlist = _settings->get(Set::PL_Load);
		if( !load_playlist ) return;

		bool load_last_track = _settings->get(Set::PL_LoadLastTrack);
		int last_track_idx = _settings->get(Set::PL_LastTrack);
		bool load_last_position = _settings->get(Set::PL_RememberTime);
		bool start_immediatly = _settings->get(Set::PL_StartPlaying);
		QStringList saved_playlist = _settings->get(Set::PL_Playlist);

        if(saved_playlist.size() == 0) return;

        MetaDataList v_md;

        // run over all tracks
        for(int i=0; i<saved_playlist.size(); i++) {

            // convert item into MetaData
            QString item = saved_playlist[i];
            if(item.size() == 0) continue;

            MetaData track;

            // maybe we can get a track id
            bool ok;
            int track_id = item.toInt(&ok);

            // maybe it's an filepath
            QString path_in_list = item;
            QDir d(path_in_list);
            path_in_list = d.absolutePath();


            // we have a track id
            if(track_id >= 0 && ok) {
                track = db->getTrackById(track_id);

                // this track id cannot be found in db
                if(track.id < 0) {
                    if(!ID3::getMetaDataOfFile(track)) continue;
                    track.is_extern = true;
                }

                else{
                    track.is_extern = false;
                }
            }

            // we have an filepath
            else{
                if(!QFile::exists(path_in_list)) continue;

                // maybe it's in the library neverthe less
                track = db->getTrackByPath(path_in_list);
                // we expected that.. try to get metadata
                if(track.id < 0) {
                    if(!ID3::getMetaDataOfFile(track)) continue;
                }

                track.is_extern = true;
            }

            v_md.push_back(track);
        }

        if(v_md.size() == 0) return;

		if(last_track_idx < 0 || last_track_idx > v_md.size()) {
            start_immediatly = false;
            load_last_position = false;
            load_last_track = false;
            emit sig_create_playlist(v_md, false);
            emit sig_stop();
            return;
        }

        emit sig_create_playlist(v_md, start_immediatly);


        int last_pos = 0;
        if(load_last_track && last_track_idx >= 0) {

			if(load_last_position){
				last_pos = _settings->get(Set::Engine_CurTrackPos_s);
            }

			emit sig_change_track(last_track_idx, last_pos, start_immediatly);
        }

}

