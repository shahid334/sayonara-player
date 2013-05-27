/* GUI_PlayerConnections.cpp */

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



#include "GUI/player/GUI_Player.h"
#include <QDebug>
#include <QObject>

void GUI_Player::setupConnections(){

	connect(ui->btn_play, SIGNAL(clicked(bool)), this,
			SLOT(playClicked(bool)));
	connect(ui->btn_fw, SIGNAL(clicked(bool)), this,
			SLOT(forwardClicked(bool)));
	connect(ui->btn_bw, SIGNAL(clicked(bool)), this,
			SLOT(backwardClicked(bool)));
    connect(ui->btn_stop, SIGNAL(clicked()), this,
            SLOT(stopClicked()));
	connect(ui->btn_mute, SIGNAL(released()), this,
			SLOT(muteButtonPressed()));
	connect(ui->btn_rec, SIGNAL(toggled(bool)), this,
				SLOT(sl_rec_button_toggled(bool)));
	connect(ui->btn_correct, SIGNAL(clicked(bool)), this,
			SLOT(correct_btn_clicked(bool)));
	connect(ui->albumCover, SIGNAL(clicked()), this, SLOT(coverClicked()));

	// file
	connect(ui->action_OpenFile, SIGNAL(triggered(bool)), this,
			SLOT(fileSelectedClicked(bool)));

	connect(ui->action_OpenFolder, SIGNAL(triggered(bool)), this,
			SLOT(folderSelectedClicked(bool)));
	connect(ui->action_ImportFolder, SIGNAL(triggered(bool)), this,
				SLOT(importFolderClicked()));
        connect(ui->action_ImportFiles, SIGNAL(triggered(bool)), this,
                        SLOT(importFilesClicked()));
	connect(ui->action_reloadLibrary, SIGNAL(triggered(bool)), this,
				SLOT(reloadLibraryClicked(bool)));
        connect(ui->action_clearLibrary, SIGNAL(triggered(bool)), this,
				SLOT(clearLibraryClicked(bool)));

	connect(ui->action_Close, SIGNAL(triggered(bool)), this,
				SLOT(really_close(bool)));


	// view
	connect(ui->action_viewLibrary, SIGNAL(toggled(bool)), this,
			SLOT(showLibrary(bool)));
	connect(ui->action_Dark, SIGNAL(toggled(bool)), this,
			SLOT(changeSkin(bool)));

	connect(ui->action_smallPlaylistItems, SIGNAL(toggled(bool)), this,
			SLOT(small_playlist_items_toggled(bool)));
	connect(ui->action_showOnlyTracks, SIGNAL(toggled(bool)), this,
			SLOT(sl_show_only_tracks(bool)));
	connect(ui->action_Fullscreen, SIGNAL(toggled(bool)), this,
			SLOT(show_fullscreen_toggled(bool)));


	// preferencesF
    connect(ui->action_Language, SIGNAL(triggered(bool)), this,
            SLOT(sl_action_language_toggled(bool)));
	connect(ui->action_lastFM, SIGNAL(triggered(bool)), this,
			SLOT(lastFMClicked(bool)));
	connect(ui->action_setLibPath, SIGNAL(triggered(bool)), this,
			SLOT(setLibraryPathClicked(bool)));
	connect(ui->action_fetch_all_covers, SIGNAL(triggered(bool)), this,
			SLOT(fetch_all_covers_clicked(bool)));
    connect(ui->action_startup, SIGNAL(triggered(bool)), ui_startup_dialog,
            SLOT(show()));
	connect(ui->action_min2tray, SIGNAL(toggled(bool)), this,
			SLOT(min2tray_toggled(bool)));
	connect(ui->action_only_one_instance, SIGNAL(toggled(bool)), this,
				SLOT(only_one_instance_toggled(bool)));

	connect(ui->action_streamrecorder, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_streamripper_toggled(bool)));
    connect(ui->action_notifications, SIGNAL(triggered(bool)), ui_notifications,
            SLOT(show()));
	connect(ui->action_SocketConnection, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_socket_connection_triggered(bool)));

    connect(ui->action_livesearch, SIGNAL(triggered(bool)), this,
            SLOT(sl_live_search(bool)));
    connect(ui->action_notifyNewVersion, SIGNAL(triggered(bool)), this,
            SLOT(sl_notify_new_version(bool)));


	// about
	connect(ui->action_about, SIGNAL(triggered(bool)), this, SLOT(about(bool)));

	connect(ui->action_help, SIGNAL(triggered(bool)), this, SLOT(help(bool)));
    connect(m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));


    connect(ui->volumeSlider, SIGNAL(searchSliderMoved(int)), this,
			SLOT(volumeChanged(int)));
    connect(ui->volumeSlider, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(volumeChanged(int)));
    connect(ui->volumeSlider, SIGNAL(searchSliderPressed(int)), this,
    		SLOT(volumeChanged(int)));


    connect(ui->songProgress, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(setProgressJump(int)));
	connect(ui->songProgress, SIGNAL(searchSliderPressed(int)), this,
			SLOT(setProgressJump(int)));
	connect(ui->songProgress, SIGNAL(searchSliderMoved(int)), this,
			SLOT(setProgressJump(int)));



	// cover lookup
    connect(m_cov_lookup, SIGNAL(sig_covers_found(const QStringList&, QString)),
            this, SLOT(covers_found(const QStringList&, QString)));

    connect(m_alternate_covers, SIGNAL(sig_covers_changed(QString, QString)),
            this,				SLOT(sl_alternate_cover_available(QString, QString)));

    connect(m_alternate_covers, SIGNAL(sig_no_cover()),
            this,				SLOT(sl_no_cover_available()));


    // notifications
    connect(ui_notifications, SIGNAL(sig_settings_changed(bool,int)),
            this, SLOT(notification_changed(bool,int)));

    // language chooser
    connect(ui_language_chooser, SIGNAL(sig_language_changed(QString)),
            this, SLOT(language_changed(QString)));


    connect(m_async_wa, SIGNAL(finished(QString)), this, SLOT(async_wa_finished(QString)));
    
    if(ui_libpath)
        connect(ui_libpath, SIGNAL(sig_library_path_set()), this, SLOT(setLibraryPathClicked()));



    qDebug() << "connections done";
}

