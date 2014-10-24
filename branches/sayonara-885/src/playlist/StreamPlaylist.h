/* StreamPlaylist.h */

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



#ifndef STREAMPLAYLIST_H
#define STREAMPLAYLIST_H

#include "playlist/StdPlaylist.h"

//class StreamPlaylist : public Playlist
class StreamPlaylist : public StdPlaylist
{
    Q_OBJECT
public:
    explicit StreamPlaylist(QObject *parent);

   /* virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void fwd();
    virtual void bwd();
    virtual void next();
    virtual void change_track(int idx);
    virtual void create_playlist(const MetaDataList& lst, bool start_playing);
    virtual void create_playlist(const QStringList& lst, bool start_playing);
    virtual void clear();
    virtual void metadata_changed(const MetaDataList& md_list);

    virtual void save_for_reload();
    virtual void save_to_m3u_file(QString filepath, bool relative);
    
signals:
    
public slots:*/
    
};

#endif // STREAMPLAYLIST_H
