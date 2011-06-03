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
#include "playlist/Playlist.h"
#include "MP3_Listen/MP3_Listen.h"
#include "CoverLookup/CoverLookup.h"
#include "library/CLibraryBase.h"
#include "LastFM/LastFM.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Equalizer_presets.h"
#include "LyricLookup/LyricLookup.h"


#include <QtGui>
#include <QtCore>
#include <QPointer>
#include <QApplication>
#include <QFile>
#include <QDir>

#include <string>
#include <vector>

#include <iostream>
#include <CSettingsStorage.h>

using namespace std;


int main(int argc, char *argv[]){

		if(!QFile::exists(QDir::homePath() + ".Sayonara")){
			QDir().mkdir(QDir::homePath() + ".Sayonara");
		}

		CSettingsStorage * set = CSettingsStorage::getInstance();
		set  -> runFirstTime(false);
		CDatabaseConnector::getInstance()->load_settings();


		CoverLookup cover;
	qDebug() << "Load GUI";
        QApplication app (argc, argv);
        app.setApplicationName("Sayonara");
        app.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

        GUI_SimplePlayer 	player;
        player.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));


        GUI_Playlist 		ui_playlist(player.getParentOfPlaylist());

        Playlist 			playlist(&app);

        GUI_Library_windowed	ui_library(player.getParentOfLibrary());


        CLibraryBase 		library;


        MP3_Listen 			listen (&app);

        LastFM				lastfm;
        GUI_LastFM			ui_lastfm;

        GUI_Equalizer		ui_eq(player.getParentOfEqualizer());
        GUI_TagEdit			ui_tagedit;

        qDebug() << "Setup connections";
        app.connect (&player, SIGNAL(baseDirSelected(const QString &)),	&library, 	SLOT(baseDirSelected(const QString & )));
        app.connect (&player, SIGNAL(fileSelected(QStringList &)),		&playlist, 	SLOT(createPlaylist(QStringList&)));
        app.connect (&player, SIGNAL(play()),							&playlist,	SLOT(play()));
        app.connect (&player, SIGNAL(stop()),							&playlist,	SLOT(stop()));
        app.connect (&player, SIGNAL(forward()),						&playlist,	SLOT(forward()));
        app.connect (&player, SIGNAL(backward()),						&playlist,	SLOT(backward()));
        app.connect (&player, SIGNAL(pause()),							&listen,	SLOT(pause()));


        app.connect (&player, SIGNAL(search(int)),						&listen,	SLOT(jump(int)));
        app.connect (&player, SIGNAL(volumeChanged(qreal)),				&listen,	SLOT(setVolume(qreal)));
        app.connect (&player, SIGNAL(skinChanged(bool)), 				&ui_playlist, SLOT(change_skin(bool)));
        app.connect (&player, SIGNAL(setupLastFM()), 					&ui_lastfm, SLOT(show_win()));
        app.connect (&player, SIGNAL(reloadLibrary()), 					&library, 	SLOT(reloadLibrary()));
        app.connect (&player, SIGNAL(fetch_all_covers()),       		&cover, 	SLOT(search_all_covers()));
        app.connect (&player, SIGNAL(libpath_changed(QString)), 		&library, 	SLOT(setLibraryPath(QString)));


        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)),	&player,		SLOT(fillSimplePlayer(const MetaData&)));
        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)), 	&listen, 		SLOT(changeTrack(const MetaData & )));
        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)), 	&cover, 	SLOT(search_cover(const MetaData&)));
        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)),	&lastfm,		SLOT(update_track(const MetaData&)));
        app.connect (&playlist, SIGNAL(selected_file_changed(int)), 				&ui_playlist, 	SLOT(track_changed(int)));
        app.connect (&playlist, SIGNAL(no_track_to_play()),							&listen,		SLOT(stop()));
        app.connect (&playlist, SIGNAL(goon_playing()), 							&listen,		SLOT(play()));
        app.connect (&playlist, SIGNAL(playlist_created(vector<MetaData>&, int)), 	&ui_playlist, 	SLOT(fillPlaylist(vector<MetaData>&, int)));
        app.connect (&playlist, SIGNAL(mp3s_loaded_signal(int)), 					&ui_playlist, 	SLOT(update_progress_bar(int)));
        app.connect( &playlist, SIGNAL(cur_played_info_changed(const MetaData&)),   &player,  		SLOT(update_info(const MetaData&)));

        app.connect (&ui_playlist, SIGNAL(selected_row_changed(int)), 				&playlist, 		SLOT(change_track(int)));
        app.connect (&ui_playlist, SIGNAL(clear_playlist()), 						&playlist, 		SLOT(clear_playlist()));
        app.connect (&ui_playlist, SIGNAL(save_playlist(const QString&)), 			&playlist, 		SLOT(save_playlist(const QString&)));
        app.connect (&ui_playlist, SIGNAL(playlist_mode_changed(const Playlist_Mode&)), 			&playlist, 	SLOT(playlist_mode_changed(const Playlist_Mode&)));
        app.connect (&ui_playlist, SIGNAL(dropped_tracks(const vector<MetaData>&, int)), 			&playlist, 	SLOT(insert_tracks(const vector<MetaData>&, int)));
        app.connect (&ui_playlist, SIGNAL(sound_files_dropped(QStringList&)), 						&playlist, 	SLOT(createPlaylist(QStringList&)));
        app.connect (&ui_playlist, SIGNAL(directory_dropped(const QString&)), 						&library, 	SLOT(baseDirSelected(const QString & )));
        app.connect (&ui_playlist, SIGNAL(row_removed(int)), 						&playlist, 		SLOT(remove_row(int)));


        app.connect (&listen, 	SIGNAL(timeChangedSignal(quint32)),					&player,		SLOT(setCurrentPosition(quint32) ));
        app.connect (&listen, 	SIGNAL(track_finished()),							&playlist,		SLOT(next_track() ));
        app.connect (&listen,   SIGNAL(scrobble_track(const MetaData&)), 			&lastfm, 		SLOT(scrobble(const MetaData&)));
        app.connect (&listen,	SIGNAL(eq_presets_loaded(const vector<EQ_Setting>&)), &ui_eq,		SLOT(fill_eq_presets(const vector<EQ_Setting>&)));
        app.connect( &listen, 	SIGNAL(eq_found(const QStringList&)), 				&ui_eq, 		SLOT(fill_available_equalizers(const QStringList&)));
        app.connect( &listen, 	SIGNAL(total_time_changed_signal(qint64)),			&player,		SLOT(total_time_changed(qint64)));

        app.connect (&cover, 	SIGNAL(cover_found(QPixmap&)), 						&player, 		SLOT(cover_changed(QPixmap&)));

        app.connect(&library, SIGNAL(playlistCreated(QStringList&)), 				&playlist, 		SLOT(createPlaylist(QStringList&)));
        app.connect(&library, SIGNAL(signalMetaDataLoaded(vector<MetaData>&)), 		&ui_library, 	SLOT(fill_library_tracks(vector<MetaData>&)));
        app.connect(&library, SIGNAL(mp3s_loaded_signal(int)), 						&ui_playlist, 	SLOT(update_progress_bar(int)));
        app.connect(&library, SIGNAL(allAlbumsLoaded(vector<Album>&)), 				&ui_library, 	SLOT(fill_library_albums(vector<Album>&)));
        app.connect(&library, SIGNAL(allArtistsLoaded(vector<Artist>&)), 			&ui_library, 	SLOT(fill_library_artists(vector<Artist>&)));

        app.connect(&ui_library, SIGNAL(artist_changed_signal(int)), 				&library, 		SLOT(getAlbumsByArtist(int)));
        app.connect(&ui_library, SIGNAL(album_changed_signal(int)), 				&library, 		SLOT(getTracksByAlbum(int)));
        app.connect(&ui_library, SIGNAL(clear_signal()), 							&library, 		SLOT(getAllArtistsAlbumsTracks()));
        app.connect(&ui_library, SIGNAL(album_chosen_signal(vector<MetaData>&)), 	&playlist, 		SLOT(createPlaylist(vector<MetaData>&)));
        app.connect(&ui_library, SIGNAL(artist_chosen_signal(vector<MetaData>&)), 	&playlist, 		SLOT(createPlaylist(vector<MetaData>&)));
        app.connect(&ui_library, SIGNAL(track_chosen_signal(vector<MetaData>&)), 	&playlist, 		SLOT(createPlaylist(vector<MetaData>&)));

        app.connect(&ui_lastfm, SIGNAL(new_lfm_credentials(QString, QString)), 		&lastfm, 		SLOT(login_slot(QString, QString)));

        app.connect(&ui_eq, SIGNAL(eq_changed_signal(int, int)), &listen, SLOT(eq_changed(int, int)));
        app.connect(&ui_eq, SIGNAL(eq_enabled_signal(bool)), &listen, SLOT(eq_enable(bool)));
        app.connect(&ui_eq, SIGNAL(close_event()), &player, SLOT(close_eq()));

        app.connect(&ui_playlist, SIGNAL(edit_id3_signal()), &playlist, SLOT(edit_id3_request()));
        app.connect(&playlist, SIGNAL(data_for_id3_change(const vector<MetaData>&)), &ui_tagedit, SLOT(change_meta_data(const vector<MetaData>&)));
		app.connect(&ui_tagedit, SIGNAL(id3_tags_changed()), &ui_library, SLOT(id3_tags_changed()));
		app.connect(&ui_tagedit, SIGNAL(id3_tags_changed(vector<MetaData>&)), &playlist, SLOT(id3_tags_changed(vector<MetaData>&)));


		int vol = set->getVolume();
        player.setVolume(vol);
        listen.setVolume(vol);

		player.setPlaylist(&ui_playlist);
		player.setLibrary(&ui_library);
		player.setEqualizer(&ui_eq);

		/* version_number:
		 * A.B.D[u|e|s]
		 * A = Main Version number: only increased if ALL milestones
		 *     fullfilled and there's a stable version of it
		 * B = Sub version number: increased for every milstone current milestone
		 * D = Date (increased for little bugfixes)
		 * u = unstable (automatically if a new feature was implemented)
		 * e = experimental (successor may be unstable again or stable)
		 * s = stable (sucessor of experimental)
		 *
		 */

		player.setWindowTitle("Sayonara (0.6.0603e)");
		player.show();


		listen.load_equalizer();

		QRect rect = ui_eq.geometry();
			rect.setHeight(player.getParentOfEqualizer()->height());
			rect.setWidth(player.getParentOfEqualizer()->width());
			ui_eq.setGeometry(rect);

		rect = ui_playlist.geometry();
			rect.setHeight(player.getParentOfPlaylist()->height());
			rect.setWidth(player.getParentOfPlaylist()->width());
			ui_playlist.setGeometry(rect);

		rect = ui_library.geometry();
			rect.setHeight(player.getParentOfLibrary()->height());
			rect.setWidth(player.getParentOfLibrary()->width());
			ui_library.setGeometry(rect);

		player.showEqualizer(false);

        library.loadDataFromDb();
        QString user, password;
        set->getLastFMNameAndPW(user, password);
        lastfm.login_slot (user,password);

        vector<EQ_Setting> eq_settings;
        set->getEqualizerSettings(eq_settings);

        app.exec();

        CDatabaseConnector::getInstance()->store_settings();

        return 0;
}


