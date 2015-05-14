/* GUI_PlaylistEntry.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef GUI_PLAYLISTENTRY_H
#define GUI_PLAYLISTENTRY_H


#include <QFrame>
#include <QWidget>
#include "HelperStructs/MetaData/MetaData.h"
#include "HelperStructs/SayonaraClass.h"

class GUI_PlaylistEntry : public QFrame, protected SayonaraClass {

    Q_OBJECT

public:
	GUI_PlaylistEntry(QWidget* parent=0) :
		QFrame(parent),
		SayonaraClass()
	{

	}

    virtual ~GUI_PlaylistEntry(){}

    virtual  void setContent(const MetaData& md, int idx)=0;
    void setWidth(int w){
        this->resize(w, this->height());
    }
};


#endif // GUI_PLAYLISTENTRY_H
