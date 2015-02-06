/* GUI_Library_windowed.h */

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
 * GUI_Library_windowed.h
 *
 *  Created on: Apr 24, 2011
 *      Author: luke
 */

#ifndef GUI_LIBRARY_WINDOWED_H_
#define GUI_LIBRARY_WINDOWED_H_


#include "GUI/library/InfoBox/GUILibraryInfoBox.h"
#include "GUI/library/GUI_AbstractLibrary.h"
#include "GUI/ui_GUI_Library_windowed.h"

#include "Library/LocalLibrary.h"


using namespace Sort;

class GUI_Library_windowed: public GUI_AbstractLibrary, private Ui::Library_windowed {

	friend class GUI_AbstractLibrary;

Q_OBJECT

public:

	GUI_Library_windowed(LocalLibrary* library, GUI_InfoDialog* info_dialog, QWidget* parent);
	virtual ~GUI_Library_windowed();


signals:
    void sig_import_files(const QStringList&);

public slots:
	void import_result(bool);

protected slots:

	virtual void disc_pressed(int);
	virtual void import_files(const QStringList&);
	virtual void lib_no_lib_path();

	virtual AbstractLibrary::TrackDeletionMode show_delete_dialog(int n_tracks);

	virtual void language_changed();

private:
	GUI_Library_Info_Box* _lib_info_dialog;



};

#endif /* GUI_LIBRARY_WINDOWED_H_ */

