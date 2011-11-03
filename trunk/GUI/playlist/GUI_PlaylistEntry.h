/* GUI_PlaylistEntry.h */

/* Copyright (C) 2011  Lucio Carreras
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


/*
 * GUI_PlaylistEntry.h
 *
 *  Created on: Apr 9, 2011
 *      Author: luke
 */

#ifndef GUI_PLAYLISTENTRY_H_
#define GUI_PLAYLISTENTRY_H_

#include "ui_GUI_Playlist_Entry.h"

#include <QObject>


class GUI_PlaylistEntry : public QFrame, private Ui::PlaylistEntry {

	Q_OBJECT
public:
	GUI_PlaylistEntry(QWidget* parent=0);
	virtual ~GUI_PlaylistEntry();

public:
	void setArtist(QString artist);
	void setAlbum(QString album);
	void setTitle(QString title);
	void setTime(QString time);

	void setBackgroundColorPlaying();
	void setBackgroundColorNotPlaying();

	void setWidth(int w);



private:
	Ui::PlaylistEntry* ui;

};

#endif /* GUI_PLAYLISTENTRY_H_ */
