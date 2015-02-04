/* GUI_SoundCloudLibrary.h */

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



#ifndef GUI_SOUNDCLOUDLIBRARY_H
#define GUI_SOUNDCLOUDLIBRARY_H

#include "GUI/ui_GUI_Library_windowed.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "Soundcloud/SoundcloudLibrary.h"
#include "GUI/ui_GUI_SoundcloudLibrary.h"

class GUI_SoundCloudLibrary : public GUI_AbstractLibrary, protected Ui::GUI_SoundcloudLibrary
{
	Q_OBJECT

public:
	explicit GUI_SoundCloudLibrary(SoundcloudLibrary* library, GUI_InfoDialog* info_dialog, QWidget *parent = 0);

protected:
	virtual AbstractLibrary::TrackDeletionMode show_delete_dialog(int n_tracks);

protected slots:
	void btn_add_clicked();

};

#endif // GUI_SOUNDCLOUDLIBRARY_H
