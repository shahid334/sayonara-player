/* GUI_PlayerConnections.cpp */

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

#include "GUI/player/GUI_Player.h"

void GUI_Player::setupConnections() {

	qRegisterMetaType<CoverLocation>("CoverLocation");

	connect(btn_play, SIGNAL(clicked(bool)), this,
			SLOT(playClicked(bool)));
	connect(btn_fw, SIGNAL(clicked(bool)), this,
			SLOT(forwardClicked(bool)));
	connect(btn_bw, SIGNAL(clicked(bool)), this,
			SLOT(backwardClicked(bool)));
	connect(btn_stop, SIGNAL(clicked()), this,
            SLOT(stopClicked()));
	connect(btn_mute, SIGNAL(released()), this,
			SLOT(muteButtonPressed()));
	connect(btn_rec, SIGNAL(toggled(bool)), this,
				SLOT(sl_rec_button_toggled(bool)));
	connect(btn_correct, SIGNAL(clicked(bool)), this,
			SLOT(correct_btn_clicked(bool)));
	connect(albumCover, SIGNAL(clicked()), this, SLOT(coverClicked()));

	// file
	connect(action_OpenFile, SIGNAL(triggered(bool)), this,
			SLOT(fileSelectedClicked(bool)));

	connect(action_OpenFolder, SIGNAL(triggered(bool)), this,
			SLOT(folderSelectedClicked(bool)));
	connect(action_ImportFolder, SIGNAL(triggered(bool)), this,
				SLOT(importFolderClicked()));
		connect(action_ImportFiles, SIGNAL(triggered(bool)), this,
                        SLOT(importFilesClicked()));
	connect(action_reloadLibrary, SIGNAL(triggered(bool)), this,
				SLOT(reloadLibraryClicked(bool)));

	connect(action_Close, SIGNAL(triggered(bool)), this,
				SLOT(really_close(bool)));


	// view
	connect(action_viewLibrary, SIGNAL(toggled(bool)), this,
			SLOT(showLibrary(bool)));
	connect(action_Dark, SIGNAL(toggled(bool)), this,
			SLOT(changeSkin(bool)));

	connect(action_smallPlaylistItems, SIGNAL(toggled(bool)), this,
			SLOT(small_playlist_items_toggled(bool)));
	connect(action_showOnlyTracks, SIGNAL(toggled(bool)), this,
			SLOT(sl_show_only_tracks(bool)));
	connect(action_Fullscreen, SIGNAL(toggled(bool)), this,
			SLOT(show_fullscreen_toggled(bool)));


	// preferencesF
	connect(action_Language, SIGNAL(triggered(bool)), this,
            SLOT(sl_action_language_toggled(bool)));
	connect(action_lastFM, SIGNAL(triggered(bool)), this,
			SLOT(lastFMClicked(bool)));
	connect(action_setLibPath, SIGNAL(triggered(bool)), this,
			SLOT(setLibraryPathClicked(bool)));
	connect(action_fetch_all_covers, SIGNAL(triggered(bool)), this,
			SLOT(fetch_all_covers_clicked(bool)));
	connect(action_startup, SIGNAL(triggered(bool)), ui_startup_dialog,
            SLOT(show()));
	connect(action_min2tray, SIGNAL(toggled(bool)), this,
			SLOT(min2tray_toggled(bool)));
	connect(action_only_one_instance, SIGNAL(toggled(bool)), this,
				SLOT(only_one_instance_toggled(bool)));

	connect(action_streamrecorder, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_streamripper_toggled(bool)));
	connect(action_notifications, SIGNAL(triggered(bool)), ui_notifications,
            SLOT(show()));
	connect(action_SocketConnection, SIGNAL(triggered(bool)), this,
			SLOT(sl_action_socket_connection_triggered(bool)));

	connect(action_livesearch, SIGNAL(triggered(bool)), this,
            SLOT(sl_live_search(bool)));
	connect(action_notifyNewVersion, SIGNAL(triggered(bool)), this,
            SLOT(sl_notify_new_version(bool)));


	// about
	connect(action_about, SIGNAL(triggered(bool)), this, SLOT(about(bool)));

	connect(action_help, SIGNAL(triggered(bool)), this, SLOT(help(bool)));
    connect(m_trayIcon, SIGNAL(onVolumeChangedByWheel(int)), this, SLOT(volumeChangedByTick(int)));


	connect(volumeSlider, SIGNAL(searchSliderMoved(int)), this,
			SLOT(volumeChanged(int)));
	connect(volumeSlider, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(volumeChanged(int)));
	connect(volumeSlider, SIGNAL(searchSliderPressed(int)), this,
    		SLOT(volumeChanged(int)));

	connect(songProgress, SIGNAL(searchSliderReleased(int)), this,
    		SLOT(setProgressJump(int)));
	connect(songProgress, SIGNAL(searchSliderPressed(int)), this,
			SLOT(setProgressJump(int)));
	connect(songProgress, SIGNAL(searchSliderMoved(int)), this,
			SLOT(setProgressJump(int)));



	// cover lookup
	connect(m_cov_lookup, SIGNAL(sig_cover_found(const CoverLocation&)),
			this, SLOT(cover_found(const CoverLocation&)));

	connect(m_alternate_covers, SIGNAL(sig_cover_changed(const CoverLocation&)),
			this,				SLOT(sl_alternate_cover_available(const CoverLocation&)));

    connect(m_alternate_covers, SIGNAL(sig_no_cover()),
            this,				SLOT(sl_no_cover_available()));


    // notifications
    connect(ui_notifications, SIGNAL(sig_settings_changed(bool,int)),
            this, SLOT(notification_changed(bool,int)));

    // language chooser
    connect(ui_language_chooser, SIGNAL(sig_language_changed(QString)),
            this, SLOT(language_changed(QString)));


    connect(m_awa_version, SIGNAL(finished()), this, SLOT(awa_version_finished()));
    connect(m_awa_translators, SIGNAL(finished()), this, SLOT(awa_translators_finished()));
    
    if(ui_libpath)
        connect(ui_libpath, SIGNAL(sig_library_path_set()), this, SLOT(setLibraryPathClicked()));



    QList<QKeySequence> lst;
    lst << QKeySequence(Qt::Key_MediaTogglePlayPause) << QKeySequence(Qt::Key_MediaPlay) << QKeySequence(Qt::Key_MediaPause) << QKeySequence(Qt::Key_Space);
    QAction* play_pause_action = createAction(lst);
	connect(play_pause_action, SIGNAL(triggered()), btn_play, SLOT(click()));

    QList<QKeySequence> lst_fwd;
    lst_fwd << QKeySequence(Qt::Key_MediaNext) << QKeySequence(Qt::ControlModifier | Qt::Key_Right);
    QAction* fwd_action = createAction(lst_fwd);
	connect(fwd_action, SIGNAL(triggered()), btn_fw, SLOT(click()));

    QList<QKeySequence> lst_bwd;
    lst_bwd << QKeySequence(Qt::Key_MediaPrevious) << QKeySequence(Qt::ControlModifier | Qt::Key_Left);
    QAction* bwd_action = createAction(lst_bwd);
	connect(bwd_action, SIGNAL(triggered()), btn_bw, SLOT(click()));

    QAction* stop_action = createAction(QKeySequence(Qt::ControlModifier | Qt::Key_Space));
	connect(stop_action, SIGNAL(triggered()), btn_stop, SLOT(click()));

    QAction* louder_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Up));
    connect(louder_action, SIGNAL(triggered()), this, SLOT(volumeHigher()));

    QAction* leiser_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Down));
    connect(leiser_action, SIGNAL(triggered()), this, SLOT(volumeLower()));

    QAction* two_perc_plus_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Right));
    connect(two_perc_plus_action, SIGNAL(triggered()), this, SLOT(jump_forward()));

    QAction* two_perc_minus_action = createAction(QKeySequence(Qt::AltModifier | Qt::Key_Left));
    connect(two_perc_minus_action, SIGNAL(triggered()), this, SLOT(jump_backward()));

    qDebug() << "connections done";
}

