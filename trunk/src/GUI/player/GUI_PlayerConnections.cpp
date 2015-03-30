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

	connect(btn_play,	SIGNAL(clicked()),	this, SLOT(play_clicked()));
	connect(btn_fw,		SIGNAL(clicked()),	this, SLOT(next_clicked()));
	connect(btn_bw,		SIGNAL(clicked()),	this, SLOT(prev_clicked()));
	connect(btn_stop,	SIGNAL(clicked()),	this, SLOT(stop_clicked()));
	connect(btn_mute,	SIGNAL(released()),	this, SLOT(muteButtonPressed()));
	connect(btn_rec,	SIGNAL(toggled(bool)), this, SLOT(sl_rec_button_toggled(bool)));
	connect(btn_correct,SIGNAL(clicked()), this, SLOT(correct_btn_clicked()));
	connect(albumCover, SIGNAL(clicked()), this, SLOT(coverClicked()));

	connect(m_play_manager, SIGNAL(sig_playstate_changed(PlayManager::PlayState)),
			this,			SLOT(playstate_changed(PlayManager::PlayState)));

	connect(m_play_manager, SIGNAL(sig_track_changed(const MetaData&)),
			this,			SLOT(track_changed(const MetaData&)));

	connect(m_play_manager, SIGNAL(sig_position_changed_ms(quint64)),
			this,			SLOT(psl_set_cur_pos_ms(quint64)));

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
			SLOT(skin_toggled(bool)));

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
			SLOT(sl_libpath_clicked(bool)));
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


	connect(volumeSlider, SIGNAL(sig_slider_moved(int)), this,
			SLOT(volumeChanged(int)));

	connect(songProgress, SIGNAL(sig_slider_moved(int)), this,
			SLOT(seek(int)));



	// cover lookup
	connect(m_cov_lookup, SIGNAL(sig_cover_found(const CoverLocation&)),
			this, SLOT(sl_cover_found(const CoverLocation&)));

	connect(m_AlternativeCovers, SIGNAL(sig_cover_changed(const CoverLocation&)),
			this,				SLOT(sl_alternate_cover_available(const CoverLocation&)));

    connect(m_AlternativeCovers, SIGNAL(sig_no_cover()),
            this,				SLOT(sl_no_cover_available()));


    connect(m_awa_version, SIGNAL(finished()), this, SLOT(awa_version_finished()));
    connect(m_awa_translators, SIGNAL(finished()), this, SLOT(awa_translators_finished()));

	connect(ui_libpath, SIGNAL(sig_library_path_set()), this, SLOT(sl_libpath_clicked()));

    QList<QKeySequence> lst;
	lst << QKeySequence(Qt::Key_Space);
    QAction* play_pause_action = createAction(lst);
	connect(play_pause_action, SIGNAL(triggered()), btn_play, SLOT(click()));

    qDebug() << "connections done";
}

