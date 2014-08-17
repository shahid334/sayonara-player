/* application.cpp */

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



#include "application.h"

#include <QApplication>
#include "DatabaseAccess/CDatabaseConnector.h"
#include "GUI/player/GUI_Player.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/LastFM/GUI_LastFM.h"
#include "GUI/LastFM/GUI_LFMRadioWidget.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/Podcasts/GUI_Podcasts.h"
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/StreamRecorder/GUI_StreamRecorder.h"
#include "GUI/SocketConfiguration/GUISocketSetup.h"
#include "GUI/alternate_covers/GUI_Alternate_Covers.h"

#include "Engine/GStreamer/GSTEngineHandler.h"

#include "StreamPlugins/LastFM/LastFM.h"
#include "library/CLibraryBase.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "HelperStructs/CDirectoryReader.h"
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "Socket/Socket.h"


#include <QMap>
#include <QSharedMemory>


#include <fstream>
#include <string>

using namespace std;

bool Application::is_initialized(){
    return _initialized;

}



Application::Application(int & argc, char ** argv) : QApplication(argc, argv)
{

}

void Application::init(int n_files, QTranslator *translator){


	_translator        = translator;

	set                = CSettingsStorage::getInstance();

	//connect(this, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focus_changed(QWidget*,QWidget*)));


	QString version    = getVersion();
	set->setVersion( version );

	player              = new GUI_Player(translator);
	_setting_thread    = new SettingsThread(player);

	playlist_handler    = new PlaylistHandler();
	playlist_loader     = new PlaylistLoader(this);
	library             = new CLibraryBase(this->getMainWindow());
    //sc_library          = new SoundcloudLibrary(this->getMainWindow());
	library_importer    = new LibraryImporter(this->getMainWindow());
	playlists           = new Playlists();

	lastfm              = LastFM::getInstance();
	ui_lastfm           = new GUI_LastFM(player->centralWidget());

	ui_level            = new GUI_LevelPainter(tr("Le&vel"), player->getParentOfPlugin());
	ui_spectrum         = new GUI_Spectrum(tr("&Spectrum"), player->getParentOfPlugin());
	ui_stream           = new GUI_Stream(tr("&Webstreams"), player->getParentOfPlugin());
	ui_podcasts         = new GUI_Podcasts(tr("P&odcasts"), player->getParentOfPlugin());
	ui_eq               = new GUI_Equalizer(tr("&Equalizer"), player->getParentOfPlugin());
	ui_lfm_radio        = new GUI_LFMRadioWidget(tr("Last.&fm"), player->getParentOfPlugin());
	ui_playlist_chooser = new GUI_PlaylistChooser(tr("Pla&ylists"), player->getParentOfPlugin());
	ui_audioconverter   = new GUI_AudioConverter(tr("&mp3 Converter"), player->getParentOfPlugin());
	ui_bookmarks        = new GUI_Bookmarks(tr("&Bookmarks"), player->getParentOfPlugin());
	ui_speed			= new GUI_Speed(tr("Spee&d"), player->getParentOfPlugin());

	ui_stream_rec       = new GUI_StreamRecorder(player->centralWidget());
	ui_id3_editor       = new GUI_TagEdit();

	ui_info_dialog      = new GUI_InfoDialog(player->centralWidget(), ui_id3_editor);
	ui_socket_setup     = new GUI_SocketSetup(player->centralWidget());

	ui_library          = new GUI_Library_windowed(player->getParentOfLibrary());
	ui_library->set_info_dialog(ui_info_dialog);

	ui_sc		        = new GUI_SoundCloudLibrary(player->getParentOfLibrary());
	ui_sc->set_info_dialog(ui_info_dialog);

	ui_playlist         = new GUI_Playlist(player->getParentOfPlaylist(), ui_info_dialog);

	ui_style_settings = new GUI_StyleSettings(player);

	remote_socket       = new Socket();

	_pph = new PlayerPluginHandler(NULL);

	_pph->addPlugin(ui_level);
	_pph->addPlugin(ui_spectrum);
	_pph->addPlugin(ui_eq);
	_pph->addPlugin(ui_lfm_radio);
	_pph->addPlugin(ui_stream);
	_pph->addPlugin(ui_podcasts);
	_pph->addPlugin(ui_playlist_chooser);
	_pph->addPlugin(ui_audioconverter);
	_pph->addPlugin(ui_bookmarks);
	_pph->addPlugin(ui_speed);

	qDebug() << "Plugin " << ui_level->getVisName();
	qDebug() << "Plugin " << ui_stream->getVisName();
	qDebug() << "Plugin " << ui_eq->getVisName();
	qDebug() << "Plugin " << ui_playlist_chooser->getVisName();
	qDebug() << "Plugin " << ui_podcasts->getVisName();
	qDebug() << "Plugin " << ui_lfm_radio->getVisName();
	qDebug() << "Plugin " << ui_audioconverter->getVisName();
	qDebug() << "Plugin " << ui_bookmarks->getVisName();
	qDebug() << "Plugin " << ui_speed->getVisName();

	QString dir;

#ifndef Q_OS_WIN
	  dir = Helper::getLibPath();
	  qDebug() << "Lib path = " << dir;
#else
	  dir = this->applicationDirPath();
#endif

	SoundPluginLoader loader(dir);
	listen = loader.get_first_engine();
	if(!listen){
		qDebug() << "No Sound Engine found! You fucked up the installation. Aborting...";
		exit(1);
	}

   else{
		listen->psl_sr_set_active(set->getStreamRipper());
		listen->psl_set_gapless(set->getGapless());
   }

	init_connections();

	// emit do connections
	// emit connections done

	qDebug() << "setting up player";
	bool is_maximized = set->getPlayerMaximized();

	player->setWindowTitle("Sayonara " + version);
	player->setWindowIcon(QIcon(Helper::getIconPath() + "logo.png"));

	/* Into Player */
	player->setPlaylist(ui_playlist);
	player->setLibrary(ui_library);
	player->setInfoDialog(ui_info_dialog);
	player->setPlayerPluginHandler(_pph);
	player->setStyle( set->getPlayerStyle() );

	/* --> INTO Player*/
	if(is_maximized) player->showMaximized();
	else player->show();

	ui_library->resize(player->getParentOfLibrary()->size());
	ui_playlist->resize(player->getParentOfPlaylist()->size());
	int vol = set->getVolume();
	player->setVolume(vol);


	qDebug() << "Set up library...";
	/* Into Library */
	library->loadDataFromDb();

	qDebug() << "Set up Last.fm...";

	/* Into LastFM */
	QString user, password;
	if(set->getLastFMActive()){
		set->getLastFMNameAndPW(user, password);
		LastFM::getInstance()->lfm_login( user,password, true );
	}


	bool load_old_playlist = (n_files == 0);
	if(load_old_playlist)
		playlist_loader->load_old_playlist();

	playlists->ui_loaded();
	player->ui_loaded();

	/* Into Player */
	QString shown_plugin = set->getShownPlugin();
	PlayerPlugin* p = _pph->find_plugin(shown_plugin);
	player->showPlugin(p);

    //sc_library->loadData();

	_initialized = true;

	_setting_thread->start();


}

Application::~Application(){

    CDatabaseConnector::getInstance()->store_settings();

    if( set->getSocketActivated() ){
        if(remote_socket->isRunning()){
            remote_socket->quit();
        }
    }

    _setting_thread->stop();

    delete listen;
    delete ui_socket_setup;
    delete ui_playlist;
    delete ui_library;
    delete ui_id3_editor;
    delete ui_stream_rec;
    delete ui_lfm_radio;
    delete ui_eq;
    delete ui_stream;
    delete ui_podcasts;
    delete ui_lastfm;
    delete library_importer;
	delete ui_sc;
    delete library;
    delete playlist_handler;
    delete playlists;
    delete ui_playlist_chooser;
    delete player;

    
    CDatabaseConnector::getInstance()->closeDatabase();
}



void Application::init_connections(){



	CONNECT (player, sig_seek_rel(quint32),					listen,			jump_rel(quint32));
	CONNECT (player, sig_volume_changed(int),				listen,			set_volume(int));
    CONNECT (player, sig_rec_button_toggled(bool),			listen,			record_button_toggled(bool));
    CONNECT (player, sig_rec_button_toggled(bool),			ui_stream_rec,	record_button_toggled(bool));

	CONNECT (player, baseDirSelected(const QString &),      library,            baseDirSelected(const QString & ));
	CONNECT (player, reloadLibrary(bool),                   library,            reloadLibrary(bool));
	CONNECT (player, clearLibrary(),                        library,	clearLibrary());
	CONNECT (player, sig_import_dir(const QString&),        library_importer,        psl_import_dir(const QString&));
	CONNECT (player, sig_import_files(const QStringList&),  library_importer,        psl_import_files(const QStringList&));
	CONNECT (player, libpath_changed(QString),              library, 			setLibraryPath(QString));
	CONNECT (player, sig_show_only_tracks(bool),            ui_library,			show_only_tracks(bool));

    CONNECT (player, fileSelected(QStringList &),			 playlist_handler, 			psl_createPlaylist(QStringList&));
    CONNECT (player, play(),                                 playlist_handler,			psl_play());
    CONNECT (player, pause(),                                playlist_handler,			psl_pause());
    CONNECT (player, pause(),                                listen,			pause());
    CONNECT (player, stop(),                                 playlist_handler,			psl_stop());
    CONNECT (player, stop(),                                 ui_spectrum,		psl_stop());
    CONNECT (player, stop(),                                 ui_level,          psl_stop());
	CONNECT (player, pause(),                                ui_spectrum,		psl_stop());
	CONNECT (player, pause(),                                ui_level,         psl_stop());
    CONNECT (player, forward(),                              playlist_handler,			psl_forward());
    CONNECT (player, backward(),                             playlist_handler,			psl_backward());
	CONNECT (player, show_small_playlist_items(bool),		 ui_playlist,		psl_show_small_playlist_items(bool));
	CONNECT (player, show_playlists(),						 ui_playlist_chooser, 	show()); // IND
    CONNECT (player, setupLastFM(),                          ui_lastfm,              show_win()); // IND
    CONNECT (player, sig_show_stream_rec(),                  ui_stream_rec,          show_win()); // IND
    CONNECT (player, sig_show_socket(),                      ui_socket_setup,        show_win()); // IND



    CONNECT (player, sig_skin_changed(bool),                      ui_eq,              changeSkin(bool));
    CONNECT (player, sig_skin_changed(bool),                      ui_info_dialog,     changeSkin(bool));
	CONNECT (player, sig_skin_changed(bool),                      ui_library,         change_skin(bool));
	CONNECT (player, sig_skin_changed(bool),                      ui_sc,              change_skin(bool));

    CONNECT (player, sig_correct_id3(const MetaData&), 	ui_id3_editor,		change_meta_data(const MetaData&)); // IND


    CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&, int, bool),	player,		update_track(const MetaData&, int, bool));
	CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&, int, bool),	listen, 	change_track(const MetaData &, int, bool ));
    CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&),               lastfm,		psl_track_changed(const MetaData&));
	CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&),               ui_bookmarks,	track_changed(const MetaData&));
    CONNECT (playlist_handler, sig_selected_file_changed(int),                              ui_playlist, 	track_changed(int));

    CONNECT (playlist_handler, sig_no_track_to_play(),								listen,			stop());
    CONNECT (playlist_handler, sig_no_track_to_play(),								player,			stopped());
    CONNECT (playlist_handler, sig_goon_playing(),                                  listen,			play());

    CONNECT (playlist_handler, sig_playlist_created(const MetaDataList&, int, PlaylistType), 		ui_playlist, 	fillPlaylist(const MetaDataList&, int, PlaylistType));
    CONNECT (playlist_handler, sig_playlist_created(const MetaDataList&, int, PlaylistType), 		ui_playlist_chooser, 	playlist_changed(const MetaDataList&, int, PlaylistType));
    CONNECT (playlist_handler, sig_playlist_prepared(int, MetaDataList&),            playlists,     save_playlist_as_custom(int, MetaDataList&));
    CONNECT (playlist_handler, sig_playlist_prepared(QString, MetaDataList&),        playlists,     save_playlist_as_custom(QString, MetaDataList&));

	CONNECT (ui_playlist, sig_cur_idx_changed(int),                      playlist_handler, 	psl_change_track(int));
	CONNECT (ui_playlist, sig_selection_changed(const QList<int>&),      playlist_handler, 	psl_selection_changed(const QList<int>&));
	CONNECT (ui_playlist, sig_cleared(),                                 playlist_handler, 	psl_clear_playlist());
	CONNECT (ui_playlist, playlist_mode_changed(const Playlist_Mode&),   playlist_handler, 	psl_playlist_mode_changed(const Playlist_Mode&));
	CONNECT (ui_playlist, sig_tracks_dropped(const MetaDataList&, int),  playlist_handler, 	psl_insert_tracks(const MetaDataList&, int));
	CONNECT (ui_playlist, sig_rows_removed(const QList<int>&, bool),     playlist_handler, 	psl_remove_rows(const QList<int>&, bool));
	CONNECT (ui_playlist, sig_rows_moved(const QList<int>&, int),        playlist_handler, 	psl_move_rows(const QList<int>&, int));
	CONNECT (ui_playlist, sig_no_focus(),                                ui_library,		setFocus());
	CONNECT (ui_playlist, sig_gapless(bool),                             listen,            psl_set_gapless(bool));

	CONNECT( playlist_loader, sig_stop(),                                playlist_handler,       psl_stop());
	CONNECT( playlist_loader, sig_create_playlist(MetaDataList&, bool),  playlist_handler,       psl_createPlaylist(MetaDataList&, bool));
	CONNECT( playlist_loader, sig_change_track(int, qint32, bool),       playlist_handler,       psl_change_track(int, qint32, bool));

	CONNECT (listen, sig_track_finished(),                               playlist_handler,	psl_next() );
	CONNECT (listen, sig_scrobble(const MetaData&),                      lastfm, 	psl_scrobble(const MetaData&));
	CONNECT (listen, sig_level(float, float),                            ui_level,  set_level(float,float));
	CONNECT (listen, sig_spectrum(QList<float>&),                        ui_spectrum, set_spectrum(QList<float>&));


    // should be sent to player

	CONNECT (listen, sig_pos_changed_s(quint32),                      player,              setCurrentPosition(quint32) );
	CONNECT (listen, sig_pos_changed_s(quint32),                      ui_bookmarks,        pos_changed_s(quint32) );
	CONNECT (listen, sig_bitrate_changed(qint32),					  player,              psl_bitrate_changed(qint32));
	CONNECT (listen, sig_dur_changed(MetaData&),                      player,              psl_track_time_changed(MetaData&));
	CONNECT (listen, sig_dur_changed(MetaData&),                      playlist_handler,    psl_track_time_changed(MetaData&));
	CONNECT (listen, sig_dur_changed(MetaData&),                      library,             psl_track_time_changed(MetaData&));

	CONNECT (ui_speed, sig_speed_changed(float),                      listen,              psl_set_speed(float) );


    CONNECT(library, sig_playlist_created(QStringList&),            playlist_handler, 		psl_createPlaylist(QStringList&));
    CONNECT(library, sig_reloading_library(QString&),				ui_library, 	reloading_library(QString&));
    CONNECT(library, sig_all_tracks_loaded(MetaDataList&), 			ui_library, 	fill_library_tracks(MetaDataList&));
	CONNECT(library, sig_all_albums_loaded(AlbumList&),				ui_library, 	fill_library_albums(AlbumList&));
	CONNECT(library, sig_all_artists_loaded(ArtistList&),			ui_library, 	fill_library_artists(ArtistList&));
    CONNECT(library, sig_track_mime_data_available(const MetaDataList&), 			ui_library, 	track_info_available(const MetaDataList&));
    CONNECT(library, sig_tracks_for_playlist_available(MetaDataList&),           	playlist_handler, psl_createPlaylist(MetaDataList&));
    CONNECT(library, sig_append_tracks_to_playlist(MetaDataList&),                  playlist_handler, psl_append_tracks(MetaDataList&));
    CONNECT(library, sig_delete_answer(QString), 					ui_library, 	psl_delete_answer(QString));
	CONNECT(library, sig_play_next_tracks(const MetaDataList&),		playlist_handler,          psl_play_next(const MetaDataList&));
	CONNECT(library, sig_libpath_set(QString&),						player,         psl_libpath_changed(QString&));
	CONNECT(library, sig_change_id3_tags(const MetaDataList&),		ui_id3_editor,	change_meta_data(const MetaDataList&)); // IND
	CONNECT(library, sig_reload_library_finished(),                 ui_library, 	reloading_library_finished());

	/**/

	/*CONNECT(sc_library, sig_track_mime_data_available(const MetaDataList&),  ui_sc, 	track_info_available(const MetaDataList&));
	CONNECT(sc_library, sig_delete_answer(QString), 					     ui_sc, 	psl_delete_answer(QString));
	CONNECT(sc_library, sig_playlist_created(QStringList&),                  playlist_handler, psl_createPlaylist(QStringList&));
	CONNECT(sc_library, sig_tracks_for_playlist_available(MetaDataList&),    playlist_handler, psl_createPlaylist(MetaDataList&));
	CONNECT(sc_library, sig_append_tracks_to_playlist(MetaDataList&),        playlist_handler, psl_append_tracks(MetaDataList&));
	CONNECT(sc_library, sig_play_next_tracks(const MetaDataList&),		         playlist_handler,          psl_play_next(const MetaDataList&));*/

	/**/


    CONNECT(ui_library, sig_album_dbl_clicked(int), 					library, 		psl_prepare_album_for_playlist(int));
    CONNECT(ui_library, sig_artist_dbl_clicked(int), 					library, 		psl_prepare_artist_for_playlist(int));
    CONNECT(ui_library, sig_tracks_dbl_clicked(QList<int>),             library, 		psl_prepare_tracks_for_playlist(QList<int>));
    CONNECT(ui_library, sig_artist_sel_changed(const QList<int>&),      library, 		psl_selected_artists_changed(const QList<int>&));
    CONNECT(ui_library, sig_album_sel_changed(const QList<int>&),       library, 		psl_selected_albums_changed(const QList<int>&));
    CONNECT(ui_library, sig_track_sel_changed(const QList<int>&),       library, 		psl_selected_tracks_changed(const QList<int>&));
    CONNECT(ui_library, sig_disc_pressed(int),                          library,		psl_disc_pressed(int));
    CONNECT(ui_library, sig_album_rating_changed(int, int),             library,        psl_album_rating_changed(int, int));
    CONNECT(ui_library, sig_track_rating_changed(int, int),             library,        psl_track_rating_changed(int, int));

    CONNECT(library_importer, sig_lib_changes_allowed(bool),			player,         psl_reload_library_allowed(bool));
    CONNECT(library_importer, sig_import_result(bool),					library,		refresh(bool));

    CONNECT(ui_library, sig_show_id3_editor(const QList<int>&),              library, 		psl_change_id3_tags(const QList<int>&));
    CONNECT(ui_library, sig_delete_tracks(int),                              library,		psl_delete_tracks(int));
    CONNECT(ui_library, sig_delete_certain_tracks(const QList<int>&, int),	 library,		psl_delete_certain_tracks(const QList<int>&, int));
    CONNECT(ui_library, sig_play_next_tracks(const QList<int>&),             library,		psl_play_next_tracks(const QList<int>&));
    CONNECT(ui_library, sig_play_next_all_tracks(),                          library,		psl_play_next_all_tracks());
    CONNECT(ui_library, sig_append_tracks(const QList<int>&),                library,		psl_append_tracks(const QList<int>&));
    CONNECT(ui_library, sig_append_all_tracks(),                             library,		psl_append_all_tracks());

    CONNECT(ui_library, sig_filter_changed(const Filter&),                   library, 		psl_filter_changed(const Filter&));

    CONNECT(ui_library, sig_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder),
            library, 	psl_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder));

    CONNECT(ui_library, sig_no_focus(),                                      ui_playlist,   setFocus());
    CONNECT(ui_library, sig_import_files(const QStringList&),                library_importer,       psl_import_files(const QStringList&));

    CONNECT(ui_lastfm, sig_activated(bool),                                  player,         psl_lfm_activated(bool));
    CONNECT(ui_lastfm, new_lfm_credentials(QString, QString),                lastfm, 		psl_login(QString, QString));

	CONNECT(ui_id3_editor, id3_tags_changed(), 						  library,          refresh());
	CONNECT(ui_id3_editor, id3_tags_changed(MetaDataList&), 	      playlist_handler, psl_id3_tags_changed(MetaDataList&));
	CONNECT(ui_id3_editor, id3_tags_changed(MetaDataList&), 		  player, 		    psl_id3_tags_changed(MetaDataList&));

	CONNECT(ui_audioconverter, sig_active(),                          playlist_handler, psl_stop());
	CONNECT(ui_audioconverter, sig_inactive(),                        playlist_handler, psl_stop());
	CONNECT(ui_audioconverter, sig_active(),						  listen, start_convert());
	CONNECT(ui_audioconverter, sig_inactive(),						  listen, end_convert());
	CONNECT(ui_bookmarks, sig_bookmark(quint32),                      listen, jump_abs_s(quint32) );


	CONNECT(ui_eq, eq_changed_signal(int, int),                         listen, 	eq_changed(int, int));
	CONNECT(ui_eq, eq_enabled_signal(bool),                             listen, 	eq_enable(bool));

    CONNECT(ui_level, sig_show(bool), listen, psl_calc_level(bool));
    CONNECT(ui_spectrum, sig_show(bool), listen, psl_calc_spectrum(bool));
    CONNECT(ui_level, sig_right_clicked(int), ui_style_settings, show(int));
    CONNECT(ui_spectrum, sig_right_clicked(int), ui_style_settings, show(int));



    CONNECT(lastfm,	sig_similar_artists_available(const QList<int>&),		playlist_handler,	psl_similar_artists_available(const QList<int>&));
    CONNECT(lastfm,	sig_last_fm_logged_in(bool),							player,		last_fm_logged_in(bool));
    CONNECT(lastfm,  sig_track_info_fetched(const MetaData&, bool, bool),   player,		lfm_info_fetched(const MetaData&, bool, bool));
    CONNECT(lastfm, sig_create_playlist(MetaDataList&, bool),               playlist_handler,   psl_createPlaylist(MetaDataList&, bool));
    CONNECT(lastfm, sig_new_radio_playlist(MetaDataList&),                  playlist_handler,   psl_append_tracks(MetaDataList&));


    CONNECT(ui_playlist_chooser, sig_playlist_chosen(int),		playlists, load_single_playlist(int));
    CONNECT(ui_playlist_chooser, sig_delete_playlist(int),      playlists, delete_playlist(int));
    CONNECT(ui_playlist_chooser, sig_save_playlist(int), 		playlist_handler, 	psl_prepare_playlist_for_save(int));
    CONNECT(ui_playlist_chooser, sig_save_playlist(QString), 	playlist_handler, 	psl_prepare_playlist_for_save(QString));
    CONNECT(ui_playlist_chooser, sig_save_playlist_file(QString, bool), 	playlist_handler, 	psl_save_playlist(QString, bool));
    CONNECT(ui_playlist_chooser, sig_clear_playlist(),           playlist_handler, 	psl_clear_playlist());
    CONNECT(ui_playlist_chooser, sig_files_selected(QStringList &), playlist_handler, psl_createPlaylist(QStringList&));

    CONNECT(playlists, sig_single_playlist_loaded(CustomPlaylist&),      playlist_handler, 				psl_createPlaylist(CustomPlaylist&));
	CONNECT(playlists, sig_all_playlists_loaded(QMap<int, QString>&), 	 ui_playlist_chooser, 	all_playlists_fetched(QMap<int, QString>&));





    qDebug() << "stream";
    CONNECT(ui_stream, sig_create_playlist(MetaDataList&, bool), 	playlist_handler, 	psl_createPlaylist(MetaDataList&, bool));
    CONNECT(ui_stream, sig_play_track(int, qint32, bool),           playlist_handler,   psl_change_track(int, qint32, bool));

    qDebug() << "podcasts";
    CONNECT(ui_podcasts, sig_create_playlist(MetaDataList&, bool), 	  playlist_handler, 	psl_createPlaylist(MetaDataList&, bool));
    CONNECT(ui_podcasts, sig_play_track(int, qint32, bool),           playlist_handler,   psl_change_track(int, qint32, bool));


    CONNECT (ui_stream_rec, sig_stream_recorder_active(bool),	listen,		psl_sr_set_active(bool));
    CONNECT (ui_stream_rec, sig_stream_recorder_active(bool),	player,     psl_strrip_set_active(bool));

    CONNECT (ui_style_settings, sig_style_update(),             ui_spectrum, psl_style_update());
    CONNECT (ui_style_settings, sig_style_update(),             ui_level, psl_style_update());




    bool is_socket_active = set->getSocketActivated();
    if(is_socket_active){
        CONNECT (remote_socket, sig_play(),		playlist_handler,			psl_play());
        CONNECT (remote_socket, sig_next(),		playlist_handler,			psl_forward());
        CONNECT (remote_socket, sig_prev(),		playlist_handler,			psl_backward());
        CONNECT (remote_socket, sig_stop(),		playlist_handler,			psl_stop());
        CONNECT (remote_socket, sig_pause(),		listen,				pause());
        CONNECT (remote_socket, sig_setVolume(int),player,			setVolume(int));

        remote_socket->start();
    }

    connect_languages();

    qDebug() << "connections done";
}


void Application::connect_languages(){
     CONNECT (player, sig_language_changed(),	ui_playlist_chooser, 	language_changed());
     CONNECT (player, sig_language_changed(),	ui_lastfm,              language_changed());
     CONNECT (player, sig_language_changed(),	ui_stream,              language_changed());
     CONNECT (player, sig_language_changed(),	ui_podcasts,            language_changed());
     CONNECT (player, sig_language_changed(),	ui_eq,                  language_changed());

     CONNECT (player, sig_language_changed(),	ui_lfm_radio,           language_changed());
     CONNECT (player, sig_language_changed(),	ui_stream_rec,          language_changed());
     CONNECT (player, sig_language_changed(),	ui_id3_editor,          language_changed());

     CONNECT (player, sig_language_changed(),	ui_info_dialog,         language_changed());
     CONNECT (player, sig_language_changed(),	ui_library,             language_changed());
     CONNECT (player, sig_language_changed(),	ui_playlist,            language_changed());
     CONNECT (player, sig_language_changed(),	ui_socket_setup,        language_changed());
     CONNECT (player, sig_language_changed(),	ui_style_settings,      language_changed());
}

void Application::setFiles2Play(QStringList filelist){

    if(filelist.size() > 0){
        playlist_handler->psl_createPlaylist(filelist);
    }
}



QString Application::getVersion(){

    ifstream istr;
    QString version_file = Helper::getSharePath() + "VERSION";

    istr.open(version_file.toUtf8()  );
    if(!istr || !istr.is_open() ) return "0.3.1";

    QMap<QString, int> map;

    while(istr.good()){
        string type;
        int version;
        istr >> type >> version;
        if(type.size() > 0)
            qDebug() << type.c_str() << ": " << version;

        map[QString(type.c_str())] = version;
    }

    istr.close();

    QString version_str = QString::number(map["MAJOR"]) + "." +
            QString::number(map["MINOR"]) + "." +
            QString::number(map["SUBMINOR"]) + " r" + QString::number(map["BUILD"]);
    return version_str;
}

QMainWindow* Application::getMainWindow(){
    return this->player;
}



void Application::focus_changed(QWidget *src, QWidget *dst){
	if(src)
		src->setStyleSheet("background: red;");
	if(dst)
		dst->setStyleSheet("background: green;");

}
/*
bool Application::notify(QObject *receiver, QEvent *event){
	if (event && event->type() == QEvent::KeyPress)
	{
		QKeyEvent * keyEvent = dynamic_cast<QKeyEvent*>(event);
		if (keyEvent && keyEvent->key() == Qt::Key_Tab)
			return false;
	}

	return QApplication::notify(receiver, event);
}

*/


