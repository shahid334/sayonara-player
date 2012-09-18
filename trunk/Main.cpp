/* Main.cpp */

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
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */
#include <iostream>


#define CONNECT(a,b,c,d) app.connect(a, SIGNAL(b), c, SLOT(d))

#include "GUI/player/GUI_Simpleplayer.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/LastFM/GUI_LastFM.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/library/GUILibraryInfoBox.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/LFMRadio/GUI_LFMRadioWidget.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/StreamRecorder/GUI_StreamRecorder.h"
#include "GUI/SocketConfiguration/GUISocketSetup.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"
#include "playlist/Playlist.h"
#include "Engine/Engine.h"
#include "Engine/SoundPluginLoader.h"
#include "StreamPlugins/LastFM/LastFM.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "Socket/Socket.h"


#include <QtGui>
#include <QtCore>
#include <QPointer>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QList>
#include <QString>

#include <string>
#include <vector>
#include <iostream>


using namespace std;


void printHelp(){
	qDebug() << "sayonara [-g]";
	qDebug() << "-g\tuse gstreamer instead of phonon";
}



int main(int argc, char *argv[]){

		if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")){
			QDir().mkdir(QDir::homePath() + QDir::separator() +  "/.Sayonara");
		}

		CSettingsStorage * set = CSettingsStorage::getInstance();

		set  -> runFirstTime(false);
		CDatabaseConnector::getInstance()->load_settings();

		QApplication app (argc, argv);
			app.setApplicationName("Sayonara");
			app.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));



        GUI_SimplePlayer 		player;
        GUI_PlaylistChooser		ui_playlist_chooser(player.getParentOfPlugin());
        Playlists				playlists;

        Playlist 				playlist(&app);

        CLibraryBase 			library;
        LastFM*					lastfm = LastFM::getInstance();
        GUI_LastFM				ui_lastfm(player.centralWidget());
        GUI_Stream				ui_stream(player.getParentOfPlugin());
        GUI_Equalizer			ui_eq(player.getParentOfPlugin());
        GUI_LFMRadioWidget		ui_lfm_radio(player.getParentOfPlugin());
        GUI_StreamRecorder		ui_stream_rec(player.centralWidget());
        GUI_TagEdit*			ui_id3_editor = new GUI_TagEdit();
        GUI_InfoDialog			ui_info_dialog(player.centralWidget(), ui_id3_editor);
        GUI_Library_windowed*	ui_library = new GUI_Library_windowed(player.getParentOfLibrary(), &ui_info_dialog);
        GUI_Library_Info_Box	ui_library_info_box(player.centralWidget());
        GUI_Playlist 			ui_playlist(player.getParentOfPlaylist(), &ui_info_dialog);
        GUI_SocketSetup			ui_socket_setup(player.centralWidget());

        Socket					remote_socket;



        QString dir;

#ifdef Q_OS_UNIX
        dir = "/usr/lib/sayonara";
#else
        dir = app.applicationDirPath();
#endif

        SoundPluginLoader 		plugin_loader(dir);

        Engine* listen = plugin_loader.get_cur_engine();
        if(!listen){
        	qDebug() << "No Sound Engine found! You fucked up the installation. Aborting...";
        	return -1;
        }
        listen->init();
        listen->psl_strrip_set_active(set->getStreamRipper());
        listen->psl_strrip_complete_tracks(set->getStreamRipperCompleteTracks());
        listen->psl_strrip_set_create_playlist(set->getStreamRipperPlaylist());
        listen->psl_strrip_set_path(set->getStreamRipperPath());


        CONNECT (&player, pause(), 								listen,				pause());
        CONNECT (&player, search(int),							listen,				jump(int));
        CONNECT (&player, sig_volume_changed(int),				listen,				setVolume(int));
		CONNECT (&player, sig_rec_button_toggled(bool),			listen,				record_button_toggled(bool));
		CONNECT (&player, setupLastFM(), 						&ui_lastfm, 		show_win());
        CONNECT (&player, baseDirSelected(const QString &),		&library, 			baseDirSelected(const QString & ));
        CONNECT (&player, reloadLibrary(), 						&library, 			reloadLibrary());
        CONNECT (&player, importDirectory(QString),				&library,			importDirectory(QString));
        CONNECT (&player, libpath_changed(QString), 			&library, 			setLibraryPath(QString));
        CONNECT (&player, fileSelected(QStringList &),			&playlist, 			psl_createPlaylist(QStringList&));
		CONNECT (&player, play(),								&playlist,			psl_play());
		CONNECT (&player, stop(),								&playlist,			psl_stop());
		CONNECT (&player, forward(),							&playlist,			psl_forward());
		CONNECT (&player, backward(),							&playlist,			psl_backward());
		CONNECT (&player, sig_stream_selected(const QString&, const QString&), 		&playlist, psl_play_stream(const QString&, const QString&));
		CONNECT (&player, show_playlists(),						&ui_playlist_chooser, 	show());
        CONNECT (&player, skinChanged(bool), 					&ui_playlist, 		change_skin(bool));
        CONNECT (&player, show_small_playlist_items(bool),		&ui_playlist,		psl_show_small_playlist_items(bool));
        CONNECT (&player, sig_sound_engine_changed(QString&), 	&plugin_loader, 	psl_switch_engine(QString&));
        CONNECT (&player, sig_show_stream_rec(bool),			&ui_stream_rec,		psl_show(bool));
        CONNECT (&player, sig_show_socket(),					&ui_socket_setup, 	show());
        if(ui_id3_editor)
        	CONNECT (&player, sig_correct_id3(const MetaData&), 	ui_id3_editor,		change_meta_data(const MetaData&));


        CONNECT (&playlist, sig_selected_file_changed_md(const MetaData&),		&player,		update_track(const MetaData&));
        CONNECT (&playlist, sig_selected_file_changed_md(const MetaData&), 		listen, 		changeTrack(const MetaData & ));
        CONNECT (&playlist, sig_selected_file_changed_md(const MetaData&),		lastfm,			psl_track_changed(const MetaData&));
        CONNECT (&playlist, sig_no_track_to_play(),								listen,			stop());
        CONNECT (&playlist, sig_goon_playing(), 								listen,			play());
        CONNECT (&playlist, sig_selected_file_changed(int), 					&ui_playlist, 	track_changed(int));
        CONNECT (&playlist, sig_playlist_created(vector<MetaData>&, int), 		&ui_playlist, 	fillPlaylist(vector<MetaData>&, int));
        //CONNECT (&playlist, sig_cur_played_info_changed(const MetaData&),   	&player,  		update_info(const MetaData&));
        CONNECT (&playlist, sig_playlist_prepared(int, vector<MetaData>&), 		&playlists, 	save_playlist_as_custom(int, vector<MetaData>&));
    	CONNECT (&playlist, sig_playlist_prepared(QString, vector<MetaData>&), 	&playlists, 	save_playlist_as_custom(QString, vector<MetaData>&));
    	CONNECT (&playlist, sig_library_changed(), 								ui_library, 	library_changed());
    	CONNECT (&playlist, sig_import_files(const vector<MetaData>&), 			&library, 		importFiles(const vector<MetaData>&));
        CONNECT (&playlist, sig_need_more_radio(),								lastfm, 		psl_radio_playlist_request());
        CONNECT (&playlist, sig_radio_active(int),								&player,		set_radio_active(int));
        CONNECT (&playlist, sig_radio_active(int),								&ui_playlist,	set_radio_active(int));
        CONNECT (&playlist, sig_radio_active(int),								&ui_playlist_chooser,	set_radio_active(int));
        if(ui_id3_editor)
        	CONNECT (&playlist, sig_data_for_id3_change(const vector<MetaData>&), 	ui_id3_editor,	change_meta_data(const vector<MetaData>&));

        CONNECT (&ui_playlist, selected_row_changed(int), 					&playlist, 	psl_change_track(int));
        CONNECT (&ui_playlist, clear_playlist(), 							&playlist, 	psl_clear_playlist());
        CONNECT (&ui_playlist, playlist_mode_changed(const Playlist_Mode&), &playlist, 	psl_playlist_mode_changed(const Playlist_Mode&));
        CONNECT (&ui_playlist, dropped_tracks(const vector<MetaData>&, int),&playlist, 	psl_insert_tracks(const vector<MetaData>&, int));
        CONNECT (&ui_playlist, sound_files_dropped(QStringList&), 			&playlist, 	psl_createPlaylist(QStringList&));
        CONNECT (&ui_playlist, directory_dropped(const QString&, int),		&playlist, 	psl_directoryDropped(const QString &, int ));
        CONNECT (&ui_playlist, rows_removed(const QList<int>&), 			&playlist, 	psl_remove_rows(const QList<int>&));
        CONNECT (&ui_playlist, sig_import_to_library(bool),					&playlist,	psl_import_new_tracks_to_library(bool));

        CONNECT (listen, track_finished(),								&playlist,	psl_next_track() );
        CONNECT (listen, sig_valid_strrec_track(const MetaData&),		&playlist,  psl_valid_strrec_track(const MetaData&));
        CONNECT (listen, scrobble_track(const MetaData&), 				lastfm, 	psl_scrobble(const MetaData&));
        CONNECT (listen, eq_presets_loaded(const vector<EQ_Setting>&), 	&ui_eq,	fill_eq_presets(const vector<EQ_Setting>&));
        CONNECT (listen, eq_found(const QStringList&), 					&ui_eq, 	fill_available_equalizers(const QStringList&));
        CONNECT (listen, total_time_changed_signal(qint64),				&player,	total_time_changed(qint64));
        CONNECT (listen, timeChangedSignal(quint32),					&player,	setCurrentPosition(quint32) );

        CONNECT(&library, sig_playlist_created(QStringList&), 			&playlist, 		psl_createPlaylist(QStringList&));
        CONNECT(&library, sig_import_result(bool),						&playlist,		psl_import_result(bool));
        CONNECT(&library, sig_import_result(bool),						&ui_playlist,	import_result(bool));
        CONNECT(&library, sig_reload_library_finished(), 				ui_library, 	reloading_library_finished());
        CONNECT(&library, sig_reloading_library(QString&),				ui_library, 	reloading_library(QString&));
        CONNECT(&library, sig_import_result(bool),						ui_library,	import_result(bool));
        CONNECT(&library, sig_metadata_loaded(vector<MetaData>&), 		ui_library, 	fill_library_tracks(vector<MetaData>&));
        CONNECT(&library, sig_all_albums_loaded(vector<Album>&), 		ui_library, 	fill_library_albums(vector<Album>&));
        CONNECT(&library, sig_all_artists_loaded(vector<Artist>&), 		ui_library, 	fill_library_artists(vector<Artist>&));
        CONNECT(&library, sig_track_mime_data_available(const vector<MetaData>&), 		ui_library, 	track_info_available(const vector<MetaData>&));
        CONNECT(&library, sig_tracks_for_playlist_available(vector<MetaData>&), 		&playlist, psl_createPlaylist(vector<MetaData>&));
        CONNECT(&library, sig_import_result(bool),						&playlists,		import_result(bool));
        CONNECT(&library, sig_delete_answer(QString), 					ui_library, 	psl_delete_answer(QString));
        CONNECT(&library, sig_play_next_tracks(const vector<MetaData>&),&playlist,		psl_play_next_tracks(const vector<MetaData>&));
        if(ui_id3_editor)
        	CONNECT(&library, sig_change_id3_tags(const vector<MetaData>&),	ui_id3_editor,	change_meta_data(const vector<MetaData>&));

        CONNECT(ui_library, sig_album_dbl_clicked(), 						&library, 		psl_prepare_album_for_playlist());
        CONNECT(ui_library, sig_artist_dbl_clicked(), 						&library, 		psl_prepare_artist_for_playlist());
        CONNECT(ui_library, sig_track_dbl_clicked(int), 					&library, 		psl_prepare_track_for_playlist(int));
        CONNECT(ui_library, sig_artist_pressed(const QList<int>&), 		&library, 		psl_selected_artists_changed(const QList<int>&));
        CONNECT(ui_library, sig_album_pressed(const QList<int>&), 		&library, 		psl_selected_albums_changed(const QList<int>&));
        CONNECT(ui_library, sig_track_pressed(const QList<int>&), 		&library, 		psl_selected_tracks_changed(const QList<int>&));
        CONNECT(ui_library, sig_filter_changed(const Filter&), 			&library, 		psl_filter_changed(const Filter&));

        CONNECT(ui_library, sig_sortorder_changed(Sort::ArtistSort, Sort::AlbumSort, Sort::TrackSort),
        		&library, 	 psl_sortorder_changed(Sort::ArtistSort, Sort::AlbumSort, Sort::TrackSort));

        CONNECT(ui_library, sig_show_id3_editor(const QList<int>&),		&library, 		psl_change_id3_tags(const QList<int>&));
        CONNECT(ui_library, sig_delete_tracks(int),						&library,		psl_delete_tracks(int));
        CONNECT(ui_library, sig_delete_certain_tracks(const QList<int>&, int),	&library,		psl_delete_certain_tracks(const QList<int>&, int));
    	CONNECT(ui_library, sig_play_next_tracks(const QList<int>&),		&library,		psl_play_next_tracks(const QList<int>&));
    	CONNECT(ui_library, sig_play_next_all_tracks(),					&library,		psl_play_next_all_tracks());
    	CONNECT(&ui_lastfm, sig_activated(bool), &player, psl_lfm_activated(bool));
    	CONNECT(&ui_lastfm, sig_activated(bool), &ui_playlist, psl_lfm_activated(bool));
        CONNECT(&ui_lastfm, new_lfm_credentials(QString, QString), 		lastfm, 		psl_login(QString, QString));

        CONNECT(&ui_eq, eq_changed_signal(int, int), 	listen, 	eq_changed(int, int));
        CONNECT(&ui_eq, eq_enabled_signal(bool), 		listen, 	eq_enable(bool));
        CONNECT(&ui_eq, close_event(), 					&player, 	close_eq());

        CONNECT(&ui_playlist, edit_id3_signal(), 								&playlist, 		psl_edit_id3_request());

        if(ui_id3_editor){
			CONNECT(ui_id3_editor, id3_tags_changed(), 								ui_library,	id3_tags_changed());
			CONNECT(ui_id3_editor, id3_tags_changed(vector<MetaData>&), 			&playlist, 		psl_id3_tags_changed(vector<MetaData>&));
			CONNECT(ui_id3_editor, id3_tags_changed(vector<MetaData>&), 			&player, 		psl_id3_tags_changed(vector<MetaData>&));
        }

		CONNECT(lastfm,	sig_similar_artists_available(const QList<int>&),		&playlist,		psl_similar_artists_available(const QList<int>&));
		CONNECT(lastfm,	sig_last_fm_logged_in(bool),							&ui_playlist,	last_fm_logged_in(bool));
		CONNECT(lastfm,	sig_last_fm_logged_in(bool),							&player,		last_fm_logged_in(bool));
		CONNECT(lastfm,	sig_new_radio_playlist(const vector<MetaData>&),		&playlist,		psl_new_radio_playlist_available(const vector<MetaData>&));
		CONNECT(lastfm, sig_track_info_fetched(const MetaData&, bool, bool),	&player,		lfm_info_fetched(const MetaData&, bool, bool));

		CONNECT(&ui_playlist_chooser, sig_playlist_chosen(int),		&playlists, load_single_playlist(int));
		CONNECT(&ui_playlist_chooser, sig_delete_playlist(int), 	&playlists, delete_playlist(int));
		CONNECT(&ui_playlist_chooser, sig_save_playlist(int), 		&playlist, 	psl_prepare_playlist_for_save(int));
		CONNECT(&ui_playlist_chooser, sig_save_playlist(QString), 	&playlist, 	psl_prepare_playlist_for_save(QString));
		CONNECT(&ui_playlist_chooser, sig_clear_playlist(), 		&playlist, 	psl_clear_playlist());
		CONNECT(&ui_playlist_chooser, sig_closed(), 				&player, 	close_playlist_chooser());

		CONNECT(&playlists, sig_single_playlist_loaded(CustomPlaylist&), 	&playlist, 				psl_createPlaylist(CustomPlaylist&));
		CONNECT(&playlists, sig_all_playlists_loaded(QMap<int, QString>&), 	&ui_playlist_chooser, 	all_playlists_fetched(QMap<int, QString>&));
		CONNECT(&playlists, sig_import_tracks(const vector<MetaData>&), 	&library, 				importFiles(const vector<MetaData>&));

		CONNECT(&ui_lfm_radio, listen_clicked(const QString&, int),		lastfm,					psl_radio_init(const QString&, int));
		CONNECT(&ui_lfm_radio, close_event(), 								&player, 				close_lfm_radio());

		CONNECT(&ui_stream, sig_play_stream(const QString&, const QString&), 	&playlist, 	psl_play_stream(const QString&, const QString&));
		CONNECT(&ui_stream, sig_close_event(), 									&player, 	close_stream());


		CONNECT (&ui_stream_rec, sig_stream_recorder_active(bool),	listen,		psl_strrip_set_active(bool));
		CONNECT (&ui_stream_rec, sig_stream_recorder_active(bool),	&player,	psl_strrip_set_active(bool));
		CONNECT (&ui_stream_rec, sig_path_changed(const QString&), 	listen,		psl_strrip_set_path(const QString& ));
		CONNECT (&ui_stream_rec, sig_complete_tracks(bool), 		listen,		psl_strrip_complete_tracks(bool));
		CONNECT (&ui_stream_rec, sig_create_playlist(bool), 		listen,		psl_strrip_set_create_playlist(bool ));

		bool is_socket_active = set->getSocketActivated();
		if(is_socket_active){
			CONNECT (&remote_socket, sig_play(),		&playlist,			psl_play());
			CONNECT (&remote_socket, sig_next(),		&playlist,			psl_forward());
			CONNECT (&remote_socket, sig_prev(),		&playlist,			psl_backward());
			CONNECT (&remote_socket, sig_stop(),		&playlist,			psl_stop());
			CONNECT (&remote_socket, sig_pause(),		listen,				pause());
			CONNECT (&remote_socket, sig_setVolume(int),&player,			setVolume(int));

			remote_socket.start();
		}

		playlist.ui_loaded();

		qDebug() << "setup player";
		player.setWindowTitle("Sayonara (0.3)");
        player.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

		player.setPlaylist(&ui_playlist);
		player.setLibrary(ui_library);
		player.setStream(&ui_stream);
		player.setLFMRadio(&ui_lfm_radio);
		player.setEqualizer(&ui_eq);
		player.setPlaylistChooser(&ui_playlist_chooser);
		player.setStyle( CSettingsStorage::getInstance()->getPlayerStyle() );
		player.show();
		player.hideAllPlugins();

		ui_library->resize(player.getParentOfLibrary()->size());
		ui_playlist.resize(player.getParentOfPlaylist()->size());

		player.check_show_plugins();

		vector<EQ_Setting> vec_eq_setting;
		set->getEqualizerSettings(vec_eq_setting);

		int vol = set->getVolume();
		player.setVolume(vol);
		listen->setVolume(vol);
		listen->load_equalizer(vec_eq_setting);


		playlists.ui_loaded();
		library.loadDataFromDb();

		QString user, password;
		if(set->getLastFMActive()){
			set->getLastFMNameAndPW(user, password);
			player.suppress_warning(true);
			bool success = LastFM::getInstance()->lfm_login( user,password, true );
			if(!success){
				QMessageBox::warning(player.centralWidget(), "LastFM warning", "You are not logged in to LastFM.<br />Deactivate LastFM to disable this warning");
			}

			player.suppress_warning(false);

		}

        app.exec();

        CDatabaseConnector::getInstance()->store_settings();

        if(is_socket_active){
        	if(remote_socket.isRunning())
        		remote_socket.exit(0);
        }


        delete ui_library;
        delete ui_id3_editor;



        return 0;
}


