/* StdPlaylist.h */

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



#ifndef STDPLAYLIST_H
#define STDPLAYLIST_H

#include "Playlist/Playlist.h"

class StdPlaylist : public Playlist
{

public:
	explicit StdPlaylist(int idx, QString name);
    
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void fwd();
    virtual void bwd();
    virtual void next();
	virtual bool change_track(int idx);

	virtual int create_playlist(const MetaDataList& v_md);

	virtual void metadata_changed(const MetaDataList& old_md, const MetaDataList& new_md);
};

#endif // STDPLAYLISTR_H
