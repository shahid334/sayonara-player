/* GUI_PlaylistChooser.h */

/* Copyright (C) 2012  Lucio Carreras
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


#ifndef GUIPLAYLISTCHOOSER_H_
#define GUIPLAYLISTCHOOSER_H_

#include "HelperStructs/MetaData.h"

#include <vector>
#include <QMap>
#include <QWidget>
#include <QDockWidget>
#include <ui_GUI_PlaylistChooser.h>

using namespace std;

class GUI_PlaylistChooser : public QDockWidget, private Ui::PlaylistChooser {

Q_OBJECT


signals:
	void sig_playlist_chosen(int);
	void sig_delete_playlist(int);
	void sig_save_playlist(int);
	void sig_save_playlist(QString);
	void sig_closed();
	void sig_clear_playlist();


private slots:
	void apply_button_pressed();
	void save_button_pressed();
	void save_as_button_pressed();
	void delete_button_pressed();
	void playlist_selected(int);
	void set_radio_active(int);


public slots:
	void all_playlists_fetched(QMap<int, QString>&);

public:
	GUI_PlaylistChooser(QWidget* parent=0);
	virtual ~GUI_PlaylistChooser();

private:
	Ui::PlaylistChooser* ui;
	QMap<int, QString> _playlists;
	int	_cur_idx;
	bool _started;

protected:
	void closeEvent(QCloseEvent* e);
};

#endif /* GUIPLAYLISTCHOOSER_H_ */
