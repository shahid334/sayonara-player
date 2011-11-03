/* Win_Main.cpp */

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
#include "GUI/GUI_Playlist.h"

#include "GUI/Library/GUI_Library_windowed.h"
#include "GUI/Library/LibrarySetupWidget.h"
#include "Playlist/Playlist.h"
#include "MP3_Listen/MP3_Listen.h"
#include "CoverLookup/CoverLookup.h"
#include "library/CLibraryBase.h"

#include "HelperStructs/Helper.h"


#include <QtGui>
#include <QStyle>
#include <QtCore>
#include <QPointer>
#include <QApplication>

#include <string>
#include <vector>

#include <iostream>
#include <HelperStructs/CSettingsStorage.h>





using namespace std;

int main(int argc, char *argv[]){

	
    CSettingsStorage * set = CSettingsStorage::getInstance();
    set  -> runFirstTime(false);
        CoverLookup cover;


		QStyle* style = QStyleFactory::create("Oxygen");

QApplication app (argc, argv);
      

		if(style){
			qDebug() << "Oxygen created"; 
			app.setStyle(style);
		}
		  
		  //app.setStyle(new QCommonStyle);
		  
        app.setApplicationName("Sayonara");
        app.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));


        GUI_SimplePlayer 	player;
        player.setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));
        GUI_Playlist 		ui_playlist(player.getParentOfPlaylist());
        GUI_Library_windowed	ui_library(player.getParentOfLibrary());

        Playlist 			playlist(&app);
        MP3_Listen 			listen (&app);
        CLibraryBase 		library;

        LibrarySetupWidget	ui_librarySetup;




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
        app.connect (&player,SIGNAL(reloadLibrary()), 					&library, 	SLOT(reloadLibrary()));

        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)),	&player,		SLOT(fillSimplePlayer(const MetaData&)));
        app.connect (&playlist, SIGNAL(selected_file_changed_md(const MetaData&)), 	&listen, 		SLOT(changeTrack(const MetaData & )));
        app.connect (&playlist, SIGNAL(selected_file_changed(int)), 				&ui_playlist, 	SLOT(track_changed(int)));
        app.connect (&playlist, SIGNAL(no_track_to_play()),							&listen,		SLOT(stop()));
        app.connect (&playlist, SIGNAL(playlist_created(vector<MetaData>&)), 		&ui_playlist, 	SLOT(fillPlaylist(vector<MetaData>&)));
        app.connect (&playlist, SIGNAL(mp3s_loaded_signal(int)), 					&ui_playlist, 	SLOT(update_progress_bar(int)));
        
        app.connect (&ui_playlist, SIGNAL(selected_row_changed(int)), 				&playlist, 		SLOT(change_track(int)));
        app.connect (&ui_playlist, SIGNAL(clear_playlist()), 						&playlist, 		SLOT(clear_playlist()));
        app.connect (&ui_playlist, SIGNAL(save_playlist(const QString&)), 			&playlist, 		SLOT(save_playlist(const QString&)));
        app.connect (&ui_playlist, SIGNAL(playlist_mode_changed(const Playlist_Mode&)), 			&playlist, 	SLOT(playlist_mode_changed(const Playlist_Mode&)));
        app.connect (&ui_playlist, SIGNAL(dropped_tracks(const vector<MetaData>&, int)), 			&playlist, 	SLOT(insert_tracks(const vector<MetaData>&, int)));
        app.connect (&ui_playlist, SIGNAL(sound_files_dropped(QStringList&)), 						&playlist, 	SLOT(createPlaylist(QStringList&)));
        app.connect (&ui_playlist, SIGNAL(directory_dropped(const QString&)), 						&library, 	SLOT(baseDirSelected(const QString & )));
		app.connect (&ui_playlist, SIGNAL(row_removed(int)),						&playlist,		SLOT(remove_row(int)));

        app.connect (&listen, 	SIGNAL(timeChangedSignal(quint32)),					&player,		SLOT(setCurrentPosition(quint32) ));
        app.connect (&listen, 	SIGNAL(track_finished()),							&playlist,		SLOT(next_track() ));
    
        app.connect (&cover, 	SIGNAL(cover_found(QPixmap&)), 						&player, 		SLOT(cover_changed(QPixmap&)));

        app.connect(&library, 	SIGNAL(playlistCreated(QStringList&)), 				&playlist, 		SLOT(createPlaylist(QStringList&)));
        app.connect(&library, 	SIGNAL(signalMetaDataLoaded(vector<MetaData>&)), 	&ui_library, 	SLOT(fill_library_tracks(vector<MetaData>&)));
        app.connect (&library,  SIGNAL(mp3s_loaded_signal(int)), 					&ui_playlist, 	SLOT(update_progress_bar(int)));

        app.connect(&ui_library, 	SIGNAL(artist_changed_signal(int)), 			&library, 		SLOT(getAlbumsByArtist(int)));
        app.connect(&ui_library, 	SIGNAL(album_changed_signal(int)), 				&library, 		SLOT(getTracksByAlbum(int)));
        app.connect(&ui_library, 	SIGNAL(clear_signal()), 						&library, 		SLOT(getAllArtistsAlbumsTracks()));
        app.connect(&ui_library, SIGNAL(album_chosen_signal(vector<MetaData>&)),	&playlist, SLOT(createPlaylist(vector<MetaData>&)));
        app.connect(&ui_library, SIGNAL(artist_chosen_signal(vector<MetaData>&)),	&playlist, SLOT(createPlaylist(vector<MetaData>&)));
		app.connect(&ui_library, SIGNAL(track_chosen_signal(vector<MetaData>&)),	&playlist, SLOT(createPlaylist(vector<MetaData>&)));


        app.connect(&library,   SIGNAL(allAlbumsLoaded(vector<Album>&)), 			&ui_library, 	SLOT(fill_library_albums(vector<Album>&)));
        app.connect(&library,   SIGNAL(allArtistsLoaded(vector<Artist>&)), 			&ui_library, 	SLOT(fill_library_artists(vector<Artist>&)));


    
        app.connect(&ui_librarySetup, SIGNAL(libpath_changed(QString)), 			&library, 		SLOT(setLibraryPath(QString)));
        app.connect(&player, SIGNAL(setupLibraryPath()),    						&ui_librarySetup, SLOT(show()));

        library.loadDataFromDb();

        
        player.setVolume(50);
        player.setPlaylist(&ui_playlist);
        player.setLibrary(&ui_library);
        player.setWindowTitle("Sayonara");
        player.show();


        ui_playlist.show();




        return app.exec();
}


