/* StreamPlaylist.cpp */

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




#include "playlist/StreamPlaylist.h"

StreamPlaylist::StreamPlaylist(QObject *parent) :
    StdPlaylist(parent)
{
    _playlist_type = PlaylistTypeStream;
}

/*

void StreamPlaylist:: play() {

}

void StreamPlaylist::pause() {

}

void StreamPlaylist::stop() {

}

void StreamPlaylist::fwd() {

}

void StreamPlaylist::bwd() {

}

void StreamPlaylist::next() {

}

void StreamPlaylist::change_track(int idx) {

}

void StreamPlaylist::create_playlist(const MetaDataList& lst, bool start_playing) {

}

void StreamPlaylist::create_playlist(const QStringList& lst, bool start_playing) {

}

void StreamPlaylist::clear() {

}

void StreamPlaylist::metadata_changed(const MetaDataList& md_list) {

}
void StreamPlaylist::save_for_reload() {

}
void StreamPlaylist::save_to_m3u_file(QString filepath, bool relative) {

}

bool StreamPlaylist::request_playlist_for_collection(MetaDataList& lst) {
    return true;
}
*/
