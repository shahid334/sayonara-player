/*
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */
#include <iostream>


//#include <lastfm.h>
#include "GUI/GUI_Simpleplayer.h"
#include "GUI/GUI_Playlist.h"
#include "GUI/LastFM/GUI_LastFM.h"
#include "Playlist/Playlist.h"
#include "MP3_Listen/MP3_Listen.h"
#include "CoverLookup/CoverLookup.h"
#include "library/CLibraryBase.h"
#include "LastFM/LastFM.h"
#include "HelperStructs/Helper.h"


#include <QtGui>
#include <QtCore>
#include <QPointer>
#include <QApplication>

#include <string>
#include <vector>

#include <iostream>





using namespace std;

int main(int argc, char *argv[]){

		CoverLookup cover;

		QApplication app (argc, argv);
		app.setApplicationName("Sayonara");
		app.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

        GUI_SimplePlayer 	player;
        GUI_Playlist 		ui_playlist(player.getParentOfPlaylist());

        Playlist 			playlist(&app);
        MP3_Listen 			listen (&app);
        CLibraryBase 		library;
        LastFM				lastfm;
        GUI_LastFM			ui_lastfm;



        app.connect (&player, SIGNAL(baseDirSelected(const QString &)),	&library, 	SLOT(baseDirSelected(const QString & )));
        app.connect (&player, SIGNAL(fileSelected(QStringList &)),		&playlist, 	SLOT(createPlaylist(QStringList&)));
        app.connect (&player, SIGNAL(play()),							&listen,	SLOT(play()));
        app.connect (&player, SIGNAL(stop()),							&listen,	SLOT(stop()));
        app.connect (&player, SIGNAL(forward()),						&playlist,	SLOT(forward()));
        app.connect (&player, SIGNAL(backward()),						&playlist,	SLOT(backward()));
        app.connect (&player, SIGNAL(pause()),							&listen,	SLOT(pause()));
        app.connect (&player, SIGNAL(search(int)),						&listen,	SLOT(jump(int)));
        app.connect (&player, SIGNAL(volumeChanged(qreal)),				&listen,	SLOT(setVolume(qreal)));
        app.connect (&player, SIGNAL(skinChanged(bool)), 				&ui_playlist, SLOT(change_skin(bool)));
        app.connect (&player, SIGNAL(wantCover(const MetaData&)), 		&cover, 	SLOT(search_cover(const MetaData&)));
        app.connect(&player, 	SIGNAL(setupLastFM()), 								&ui_lastfm, 	SLOT(show_win()));

        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)),	&player,		SLOT(fillSimplePlayer(const MetaData&)));
        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)), 	&listen, 		SLOT(changeTrack(const MetaData & )));

        app.connect (&playlist, SIGNAL(selected_file_changed(int)), 				&ui_playlist, 	SLOT(track_changed(int)));
        app.connect (&playlist, SIGNAL(no_track_to_play()),							&listen,		SLOT(stop()));
        app.connect (&playlist, SIGNAL(playlist_created(vector<MetaData>&)), 		&ui_playlist, 	SLOT(fillPlaylist(vector<MetaData>&)));
        app.connect (&playlist, SIGNAL(mp3s_loaded_signal(int)), 					&ui_playlist, 	SLOT(update_progress_bar(int)));
        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)),	&lastfm,		SLOT(update_track(const MetaData&)));

        app.connect (&ui_playlist, SIGNAL(selected_row_changed(int)), 				&playlist, 		SLOT(change_track(int)));
        app.connect (&ui_playlist, SIGNAL(clear_playlist()), 						&playlist, 		SLOT(clear_playlist()));
        app.connect (&ui_playlist, SIGNAL(save_playlist(const QString&)), 			&playlist, 		SLOT(save_playlist(const QString&)));
        app.connect (&ui_playlist, SIGNAL(playlist_mode_changed(const Playlist_Mode&)), 			&playlist, 	SLOT(playlist_mode_changed(const Playlist_Mode&)));

        app.connect (&listen, 	SIGNAL(timeChangedSignal(quint32)),					&player,		SLOT(setCurrentPosition(quint32) ));
        app.connect (&listen, 	SIGNAL(track_finished()),							&playlist,		SLOT(next_track() ));
        app.connect (&listen,   SIGNAL(scrobble_track(const MetaData&)), 			&lastfm, 		SLOT(scrobble(const MetaData&)));
        app.connect (&cover, 	SIGNAL(cover_found(QPixmap&)), 						&player, 		SLOT(cover_changed(QPixmap&)));

        app.connect(&library, 	SIGNAL(playlistCreated(QStringList&)), 				&playlist, 		SLOT(createPlaylist(QStringList&)));

        app.connect(&ui_lastfm, SIGNAL(new_lfm_credentials(QString, QString)), 		&lastfm, 		SLOT(login_slot(QString, QString)));



        player.setVolume(50);
        player.setPlaylist(&ui_playlist);
        player.setWindowTitle("Sayonara");
        player.show();


        ui_playlist.show();




        return app.exec();
}


