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



//#include <lastfm.h>
#include "GUI/GUI_Simpleplayer.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/LastFM/GUI_LastFM.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/equalizer/GUI_Equalizer.h"
//#include "GUI/radio/GUI_RadioWidget.h"
#include "playlist/Playlist.h"
#include "MP3_Listen/MP3_Listen.h"
#include "CoverLookup/CoverLookup.h"
#include "library/CLibraryBase.h"
#include "LastFM/LastFM.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"


#include <QtGui>
#include <QtCore>
#include <QPointer>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QList>

#include <string>
#include <vector>

#include <iostream>


using namespace std;


int main(int argc, char *argv[]){

		if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")){
			QDir().mkdir(QDir::homePath() + QDir::separator() +  "/.Sayonara");
		}



		CSettingsStorage * set = CSettingsStorage::getInstance();
		set  -> runFirstTime(false);
		CDatabaseConnector::getInstance()->load_settings();

		qDebug() << "Cover";
		CoverLookup* cover = CoverLookup::getInstance();
		qDebug() << "application";
        QApplication app (argc, argv);

        app.setApplicationName("Sayonara");
        app.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

        GUI_SimplePlayer 	player;
        player.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

        GUI_PlaylistChooser		ui_playlist_chooser(player.getParentOfPlaylistChooser());
        Playlists				playlists;

        GUI_Playlist 			ui_playlist(player.getParentOfPlaylist());
        Playlist 				playlist(&app);
        GUI_Library_windowed	ui_library(player.getParentOfLibrary());
        CLibraryBase 			library;
        MP3_Listen 				listen (&app);
        LastFM					lastfm;
        GUI_LastFM				ui_lastfm;
        GUI_Equalizer			ui_eq(player.getParentOfEqualizer());
       // GUI_RadioWidget			ui_radio(player.getParentOfEqualizer());
        GUI_TagEdit				ui_tagedit;

       // GUI_Alternate_Covers ui_alternate_covers;

        qDebug() << "connections";

        app.connect (&player, SIGNAL(pause()),							&listen,	SLOT(pause()));
        app.connect (&player, SIGNAL(search(int)),						&listen,	SLOT(jump(int)));
        app.connect (&player, SIGNAL(volumeChanged(qreal)),				&listen,	SLOT(setVolume(qreal)));
        app.connect (&player, SIGNAL(setupLastFM()), 					&ui_lastfm, SLOT(show_win()));
        app.connect (&player, SIGNAL(baseDirSelected(const QString &)),	&library, 	SLOT(baseDirSelected(const QString & )));
        app.connect (&player, SIGNAL(reloadLibrary()), 					&library, 	SLOT(reloadLibrary()));
        app.connect (&player, SIGNAL(importDirectory(QString)),			&library,	SLOT(importDirectory(QString)));
        app.connect (&player, SIGNAL(libpath_changed(QString)), 		&library, 	SLOT(setLibraryPath(QString)));
        app.connect (&player, SIGNAL(fetch_all_covers()),       		cover, 		SLOT(search_all_covers()));
        app.connect (&player, SIGNAL(fileSelected(QStringList &)),		&playlist, 	SLOT(psl_createPlaylist(QStringList&)));
		app.connect (&player, SIGNAL(play()),							&playlist,	SLOT(psl_play()));
		app.connect (&player, SIGNAL(stop()),							&playlist,	SLOT(psl_stop()));
		app.connect (&player, SIGNAL(forward()),						&playlist,	SLOT(psl_forward()));
		app.connect (&player, SIGNAL(backward()),						&playlist,	SLOT(psl_backward()));
		app.connect (&player, SIGNAL(show_playlists()),					&ui_playlist_chooser, SLOT(show()));
        app.connect (&player, SIGNAL(skinChanged(bool)), 				&ui_playlist, SLOT(change_skin(bool)));

        app.connect (&playlist, SIGNAL(sig_selected_file_changed_md(const MetaData&)), 		cover, 			SLOT(search_cover(const MetaData&)));
        app.connect (&playlist, SIGNAL(sig_search_similar_artists(const QString&)), 		&lastfm,		SLOT(get_similar_artists(const QString&)));
        app.connect (&playlist, SIGNAL(sig_selected_file_changed_md(const MetaData&)),		&lastfm,		SLOT(update_track(const MetaData&)));
        app.connect (&playlist, SIGNAL(sig_selected_file_changed_md(const MetaData&)), 		&listen, 		SLOT(changeTrack(const MetaData & )));
        app.connect (&playlist, SIGNAL(sig_no_track_to_play()),								&listen,		SLOT(stop()));
        app.connect (&playlist, SIGNAL(sig_goon_playing()), 								&listen,		SLOT(play()));
        app.connect (&playlist, SIGNAL(sig_selected_file_changed(int)), 					&ui_playlist, 	SLOT(track_changed(int)));
        app.connect (&playlist, SIGNAL(sig_playlist_created(vector<MetaData>&, int)), 		&ui_playlist, 	SLOT(fillPlaylist(vector<MetaData>&, int)));
        app.connect (&playlist, SIGNAL(sig_mp3s_loaded_signal(int)), 						&ui_playlist, 	SLOT(update_progress_bar(int)));
        app.connect (&playlist, SIGNAL(sig_selected_file_changed_md(const MetaData&)),		&player,		SLOT(fillSimplePlayer(const MetaData&)));
        app.connect (&playlist, SIGNAL(sig_cur_played_info_changed(const MetaData&)),   	&player,  		SLOT(update_info(const MetaData&)));
        app.connect (&playlist, SIGNAL(sig_playlist_prepared(int, vector<MetaData>&)), 		&playlists, 	SLOT(save_playlist_as_custom(int, vector<MetaData>&)));
    	app.connect (&playlist, SIGNAL(sig_playlist_prepared(QString, vector<MetaData>&)), 	&playlists, 	SLOT(save_playlist_as_custom(QString, vector<MetaData>&)));
    	app.connect (&playlist, SIGNAL(sig_library_changed()), 								&ui_library, 	SLOT(library_changed()));
    	app.connect (&playlist, SIGNAL(sig_import_files(const vector<MetaData>&)), 			&library, 		SLOT(importFiles(const vector<MetaData>&)));
        app.connect (&playlist, SIGNAL(sig_data_for_id3_change(const vector<MetaData>&)), 	&ui_tagedit,SLOT(change_meta_data(const vector<MetaData>&)));

        app.connect (&ui_playlist, SIGNAL(selected_row_changed(int)), 					&playlist, 	SLOT(psl_change_track(int)));
        app.connect (&ui_playlist, SIGNAL(clear_playlist()), 							&playlist, 	SLOT(psl_clear_playlist()));
        app.connect (&ui_playlist, SIGNAL(playlist_mode_changed(const Playlist_Mode&)), &playlist, 	SLOT(psl_playlist_mode_changed(const Playlist_Mode&)));
        app.connect (&ui_playlist, SIGNAL(dropped_tracks(const vector<MetaData>&, int)),&playlist, 	SLOT(psl_insert_tracks(const vector<MetaData>&, int)));
        app.connect (&ui_playlist, SIGNAL(sound_files_dropped(QStringList&)), 			&playlist, 	SLOT(psl_createPlaylist(QStringList&)));
        app.connect (&ui_playlist, SIGNAL(directory_dropped(const QString&, int)),		&playlist, 	SLOT(psl_directoryDropped(const QString &, int )));
        app.connect (&ui_playlist, SIGNAL(row_removed(int)), 							&playlist, 	SLOT(psl_remove_row(int)));
        app.connect (&ui_playlist, SIGNAL(sig_import_to_library(bool)),					&playlist,	SLOT(psl_import_new_tracks_to_library(bool)));

        app.connect (&listen, 	SIGNAL(track_finished()),							&playlist,	SLOT(psl_next_track() ));
        app.connect (&listen,   SIGNAL(scrobble_track(const MetaData&)), 			&lastfm, 	SLOT(scrobble(const MetaData&)));
        app.connect (&listen,	SIGNAL(eq_presets_loaded(const vector<EQ_Setting>&)), &ui_eq,	SLOT(fill_eq_presets(const vector<EQ_Setting>&)));
        app.connect (&listen, 	SIGNAL(eq_found(const QStringList&)), 				&ui_eq, 	SLOT(fill_available_equalizers(const QStringList&)));
        app.connect (&listen, 	SIGNAL(total_time_changed_signal(qint64)),			&player,	SLOT(total_time_changed(qint64)));
        app.connect (&listen, 	SIGNAL(timeChangedSignal(quint32)),					&player,	SLOT(setCurrentPosition(quint32) ));


        app.connect (cover, 	SIGNAL(cover_found(bool, QString)), 				&player, 		SLOT(cover_changed(bool, QString)));
        app.connect (cover, 	SIGNAL(cover_found(bool, QString)), 				&ui_library,	SLOT(cover_changed(bool, QString)));

        app.connect(&library, SIGNAL(playlistCreated(QStringList&)), 				&playlist, 		SLOT(psl_createPlaylist(QStringList&)));
        app.connect(&library, SIGNAL(sig_import_result(bool)),						&playlist,		SLOT(psl_import_result(bool)));
        app.connect(&library, SIGNAL(sig_import_result(bool)),						&ui_playlist,	SLOT(import_result(bool)));
        app.connect(&library, SIGNAL(mp3s_loaded_signal(int)), 						&ui_playlist, 	SLOT(update_progress_bar(int)));
        app.connect(&library, SIGNAL(signalMetaDataLoaded(vector<MetaData>&)), 		&ui_library, 	SLOT(fill_library_tracks(vector<MetaData>&)));
        app.connect(&library, SIGNAL(allAlbumsLoaded(vector<Album>&)), 				&ui_library, 	SLOT(fill_library_albums(vector<Album>&)));
        app.connect(&library, SIGNAL(allArtistsLoaded(vector<Artist>&)), 			&ui_library, 	SLOT(fill_library_artists(vector<Artist>&)));
        app.connect(&library, SIGNAL(reloading_library_finished()), 				&ui_library, 	SLOT(reloading_library_finished()));
        app.connect(&library, SIGNAL(reloading_library(int)),						&ui_library, 	SLOT(reloading_library(int)));
        app.connect(&library, SIGNAL(library_should_be_reloaded()), 				&ui_library, 	SLOT(library_should_be_reloaded()));
        app.connect(&library, SIGNAL(sig_import_result(bool)),						&ui_library,	SLOT(import_result(bool)));
        app.connect(&library, SIGNAL(sig_import_result(bool)),						&playlists,		SLOT(import_result(bool)));


        app.connect(&ui_library, SIGNAL(search_cover(const MetaData&)), 			cover, 		SLOT(search_cover(const MetaData&)));
        app.connect(&ui_library, SIGNAL(artist_changed_signal(int)), 				&library, 	SLOT(getAlbumsByArtist(int)));
        app.connect(&ui_library, SIGNAL(reload_library()), 							&library, 	SLOT(reloadLibrary()));
        app.connect(&ui_library, SIGNAL(album_changed_signal(int)), 				&library, 	SLOT(getTracksByAlbum(int)));
        app.connect(&ui_library, SIGNAL(clear_signal()), 							&library, 	SLOT(getAllArtistsAlbumsTracks()));
        app.connect(&ui_library, SIGNAL(album_chosen_signal(vector<MetaData>&)), 	&playlist, 	SLOT(psl_createPlaylist(vector<MetaData>&)));
        app.connect(&ui_library, SIGNAL(artist_chosen_signal(vector<MetaData>&)), 	&playlist, 	SLOT(psl_createPlaylist(vector<MetaData>&)));
        app.connect(&ui_library, SIGNAL(track_chosen_signal(vector<MetaData>&)), 	&playlist, 	SLOT(psl_createPlaylist(vector<MetaData>&)));

        app.connect(&ui_lastfm, SIGNAL(new_lfm_credentials(QString, QString)), 		&lastfm, 		SLOT(login_slot(QString, QString)));

        app.connect(&ui_eq, SIGNAL(eq_changed_signal(int, int)), 	&listen, 	SLOT(eq_changed(int, int)));
        app.connect(&ui_eq, SIGNAL(eq_enabled_signal(bool)), 		&listen, 	SLOT(eq_enable(bool)));
        app.connect(&ui_eq, SIGNAL(close_event()), 					&player, 	SLOT(close_eq()));


        app.connect(&ui_playlist, 	SIGNAL(edit_id3_signal()), 								&playlist, 	SLOT(psl_edit_id3_request()));
        app.connect(&ui_library,	SIGNAL(data_for_id3_change(const vector<MetaData>&)), 	&ui_tagedit,SLOT(change_meta_data(const vector<MetaData>&)));
		app.connect(&ui_tagedit, 	SIGNAL(id3_tags_changed()), 							&ui_library,SLOT(id3_tags_changed()));
		app.connect(&ui_tagedit, 	SIGNAL(id3_tags_changed(vector<MetaData>&)), 			&playlist, 	SLOT(psl_id3_tags_changed(vector<MetaData>&)));

		app.connect(&lastfm,		SIGNAL(similar_artists_available(QList<int>&)),			&playlist,		SLOT(psl_similar_artists_available(QList<int>&)));
		app.connect(&lastfm,		SIGNAL(last_fm_logged_in(bool)),						&ui_playlist,	SLOT(last_fm_logged_in(bool)));

		app.connect(&ui_playlist_chooser, SIGNAL(sig_playlist_chosen(int)),		&playlists, SLOT(load_single_playlist(int)));
		app.connect(&ui_playlist_chooser, SIGNAL(sig_delete_playlist(int)), 	&playlists, SLOT(delete_playlist(int)));
		app.connect(&ui_playlist_chooser, SIGNAL(sig_save_playlist(int)), 		&playlist, 	SLOT(psl_prepare_playlist_for_save(int)));
		app.connect(&ui_playlist_chooser, SIGNAL(sig_save_playlist(QString)), 	&playlist, 	SLOT(psl_prepare_playlist_for_save(QString)));
		app.connect(&ui_playlist_chooser, SIGNAL(sig_clear_playlist()), 		&playlist, 	SLOT(psl_clear_playlist()));
		app.connect(&ui_playlist_chooser, SIGNAL(sig_closed()), 				&player, 	SLOT(close_playlist_chooser()));

		app.connect(&playlists, SIGNAL(sig_single_playlist_loaded(CustomPlaylist&)), 	&playlist, 				SLOT(psl_createPlaylist(CustomPlaylist&)));
		app.connect(&playlists, SIGNAL(sig_all_playlists_loaded(QMap<int, QString>&)), 	&ui_playlist_chooser, 	SLOT(all_playlists_fetched(QMap<int, QString>&)));
		app.connect(&playlists, SIGNAL(sig_import_tracks(const vector<MetaData>&)), 	&library, 				SLOT(importFiles(const vector<MetaData>&)));


		//app.connect(&ui_radio,		SIGNAL(listen_clicked(const QString&, bool)),	&lastfm,		SLOT(get_radio(const QString&, bool)));

		qDebug() << "Playlist loaded";
		playlist.ui_loaded();


		qDebug() << "setup player";
		player.setEqualizer(&ui_eq);
		player.setPlaylistChooser(&ui_playlist_chooser);
		player.setPlaylist(&ui_playlist);
		player.setLibrary(&ui_library);

		//player.setRadio(&ui_radio);
		player.setStyle( CSettingsStorage::getInstance()->getPlayerStyle() );

		qDebug() << "volume";
		int vol = set->getVolume();
        player.setVolume(vol);
        listen.setVolume(vol);

       /*player.showEqualizer(false);
   		player.showPlaylistChooser(false);*/
     	playlists.ui_loaded();

    	player.setWindowTitle("Sayonara (0.1)");
    		player.show();



		listen.load_equalizer();

		ui_eq.resize(player.getParentOfEqualizer()->size());
		ui_playlist_chooser.resize(player.getParentOfPlaylistChooser()->size());
		ui_library.resize(player.getParentOfLibrary()->size());
		ui_playlist.resize(player.getParentOfPlaylist()->size());


        library.loadDataFromDb();
        QString user, password;
        set->getLastFMNameAndPW(user, password);
        lastfm.login_slot (user,password);

        vector<EQ_Setting> eq_settings;
        set->getEqualizerSettings(eq_settings);

        app.exec();
        qDebug() << "Store settings";
        CDatabaseConnector::getInstance()->store_settings();

        return 0;
}


