/* GUILibraryInfoBox.h

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 2, 2012 
 *
 */

#ifndef GUILIBRARYINFOBOX_H_
#define GUILIBRARYINFOBOX_H_

#include <QObject>
#include <QDialog>
#include <QString>
#include "ui_GUI_Library_Info_Box.h"

#include "DatabaseAccess/CDatabaseConnector.h"
#include "StreamPlugins/LastFM/LastFM.h"

class GUI_Library_Info_Box : public QDialog, private Ui::Library_Info_Box {

	Q_OBJECT

public:
	GUI_Library_Info_Box(QWidget* parent=0);
	virtual ~GUI_Library_Info_Box();


signals:
	void sig_lfm_usercount_request();

public slots:
	void psl_refresh();
	void lfm_data_available();

private:
	Ui::Library_Info_Box* 	ui;
	CDatabaseConnector*   	_db;
	LastFM*					_lfm;

	uint 		_n_tracks;
	uint		_n_albums;
	uint		_n_artists;
	uint 		_n_lfm_playcount;
	uint		_n_lfm_days_registered;
	qint64 		_duration_ms;
	QString		_duration_string;
};

#endif /* GUILIBRARYINFOBOX_H_ */
