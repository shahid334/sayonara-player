/* GUI_PlaylistChooser.h */

/* Copyright (C) 2011 - 2014  Lucio Carreras
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

#include "HelperStructs/MetaData/MetaData.h"

#include "PlayerPlugin/PlayerPlugin.h"

#include "GUI/TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h"
#include "GUI/ui_GUI_PlaylistChooser.h"
#include "PlaylistChooser/PlaylistChooser.h"

#include <QMap>
#include <QDockWidget>


class GUI_PlaylistChooser : public PlayerPlugin, private Ui::GUI_PlaylistChooser {

Q_OBJECT

signals:
	void sig_playlist_chosen(int);
	void sig_delete_playlist(int);
	void sig_save_playlist(int);
	void sig_save_playlist(QString);
	void sig_save_playlist_file(QString, bool);
	void sig_closed();
	void sig_clear_playlist();
	void sig_files_selected(const QStringList&);


private slots:
	void save_button_pressed();
	void save_as_button_pressed();
	void delete_button_pressed();
    void load_button_pressed();
	void playlist_selected(int);
    void text_changed(const QString&);
    void got_save_params(const QString&, bool);


public slots:
	void all_playlists_fetched(const QList<CustomPlaylistSkeleton>&);


public:
	GUI_PlaylistChooser(QString name, QWidget *parent = 0);
	virtual ~GUI_PlaylistChooser();

private:

	GUI_Target_Playlist_Dialog*		_target_playlist_dialog;
	PlaylistChooser*				_playlist_chooser;

	QMap<int, QString>				_playlists;

	int			_cur_idx;
	bool		_started;

	QString		_text_before_save;
	QString		_last_dir;



    int show_warning(QString title_text);

protected:
	virtual void  language_changed();
};

#endif /* GUIPLAYLISTCHOOSER_H_ */
