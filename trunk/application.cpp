/* application.cpp */

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



Application::Application(QApplication* qapp, int n_files, QTranslator* translator, QObject *parent) : QObject(parent)
{

    app                = qapp;
    _translator        = translator;

    set                = CSettingsStorage::getInstance();
    _setting_thread    = new SettingsThread();



    QString version    = getVersion();
    set->setVersion( version );

    player              = new GUI_Player(translator);

    playlist            = new Playlist();
    library             = new CLibraryBase(this);
    playlists           = new Playlists();

    lastfm              = LastFM::getInstance();
    ui_lastfm           = new GUI_LastFM(player->centralWidget());

    ui_stream           = new GUI_Stream("Stream", GUI_Stream::getVisName(), player->getParentOfPlugin());
    ui_podcasts         = new GUI_Podcasts("Podcasts", GUI_Podcasts::getVisName(),  player->getParentOfPlugin());
    ui_eq               = new GUI_Equalizer("Equalizer", GUI_Equalizer::getVisName(),  player->getParentOfPlugin());
    ui_lfm_radio        = new GUI_LFMRadioWidget("LastFM", GUI_LFMRadioWidget::getVisName(), player->getParentOfPlugin());
    ui_playlist_chooser = new GUI_PlaylistChooser("Playlists", GUI_PlaylistChooser::getVisName(), player->getParentOfPlugin());

    ui_stream_rec       = new GUI_StreamRecorder(player->centralWidget());
    ui_id3_editor       = new GUI_TagEdit();

    ui_info_dialog      = new GUI_InfoDialog(player->centralWidget(), ui_id3_editor);
    ui_socket_setup     = new GUI_SocketSetup(player->centralWidget());

    ui_library          = new GUI_Library_windowed(player->getParentOfLibrary());
    ui_library->set_info_dialog(ui_info_dialog);
    ui_playlist         = new GUI_Playlist(player->getParentOfPlaylist(), ui_info_dialog);

    remote_socket       = new Socket();

    _pph = new PlayerPluginHandler(NULL);

    _pph->addPlugin(ui_eq);
    _pph->addPlugin(ui_lfm_radio);
    _pph->addPlugin(ui_stream);
    _pph->addPlugin(ui_podcasts);
    _pph->addPlugin(ui_playlist_chooser);

    qDebug() << "Plugin " << GUI_Stream::getVisName();
    qDebug() << "Plugin " << GUI_Equalizer::getVisName();
    qDebug() << "Plugin " << GUI_PlaylistChooser::getVisName();
    qDebug() << "Plugin " << GUI_Podcasts::getVisName();
    qDebug() << "Plugin " << GUI_LFMRadioWidget::getVisName();



    QString dir;

#ifdef Q_OS_UNIX
    dir = "/usr/lib/sayonara";
#else
    dir = app->applicationDirPath();
#endif

    engine_plugin_loader = new SoundPluginLoader(dir);
    listen = engine_plugin_loader->get_cur_engine();
    if(!listen){
        qDebug() << "No Sound Engine found! You fucked up the installation. Aborting...";
	exit(1);
    }

    else{
        listen->init();
        listen->psl_sr_set_active(set->getStreamRipper());
    }

    init_connections();

    qDebug() << "setting up player";
    player->setWindowTitle("Sayonara " + version);
    player->setWindowIcon(QIcon(Helper::getIconPath() + "logo.png"));

    player->setPlaylist(ui_playlist);
    player->setLibrary(ui_library);
    player->setInfoDialog(ui_info_dialog);

    player->setPlayerPluginHandler(_pph);

    player->setStyle( set->getPlayerStyle() );
    player->show();

    ui_library->resize(player->getParentOfLibrary()->size());
    ui_playlist->resize(player->getParentOfPlaylist()->size());

    qDebug() << "Set up engine...";
    vector<EQ_Setting> vec_eq_setting;
    set->getEqualizerSettings(vec_eq_setting);

    int vol = set->getVolume();
    player->setVolume(vol);
    listen->setVolume(vol);
    listen->load_equalizer(vec_eq_setting);

    qDebug() << "Set up library...";
    library->loadDataFromDb();

    qDebug() << "Set up Last.fm...";
    QString user, password;
    if(set->getLastFMActive()){
        set->getLastFMNameAndPW(user, password);
        LastFM::getInstance()->lfm_login( user,password, true );
    }


    bool load_old_playlist = (n_files == 0);
    if(load_old_playlist)
        playlist->load_old_playlist();

    playlists->ui_loaded();
    player->ui_loaded();

    QString shown_plugin = set->getShownPlugin();
    PlayerPlugin* p = _pph->find_plugin(shown_plugin);
    player->showPlugin(p);

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
    delete library;
    delete playlist;
    delete playlists;
    delete ui_playlist_chooser;
    delete player;

    
    
    CDatabaseConnector::getInstance()->closeDatabase();
}



void Application::init_connections(){

    CONNECT (player, pause(),                                listen,				pause());
    CONNECT (player, search(int),							listen,			jump(int));
    CONNECT (player, sig_volume_changed(int),				listen,			setVolume(int));
    CONNECT (player, sig_rec_button_toggled(bool),			listen,			record_button_toggled(bool));
    CONNECT (player, sig_rec_button_toggled(bool),			ui_stream_rec,	record_button_toggled(bool));

    CONNECT (player, baseDirSelected(const QString &),		library,            baseDirSelected(const QString & ));
    CONNECT (player, reloadLibrary(bool), 					library,            reloadLibrary(bool));
    CONNECT (player, clearLibrary(),					library,	clearLibrary());
    CONNECT (player, importDirectory(QString),				library,            importDirectory(QString));
    CONNECT (player, libpath_changed(QString),               library, 			setLibraryPath(QString));
    CONNECT (player, sig_show_only_tracks(bool),				ui_library,			show_only_tracks(bool));

    CONNECT (player, fileSelected(QStringList &),			playlist, 			psl_createPlaylist(QStringList&));
    CONNECT (player, play(),                                 playlist,			psl_play());
    CONNECT (player, stop(),                                 playlist,			psl_stop());
    CONNECT (player, forward(),                              playlist,			psl_forward());
    CONNECT (player, backward(),                             playlist,			psl_backward());
    CONNECT (player, sig_stream_selected(const QString&, const QString&), 		playlist, psl_play_stream(const QString&, const QString&));

    CONNECT (player, show_small_playlist_items(bool),		ui_playlist,		psl_show_small_playlist_items(bool));

    CONNECT (player, show_playlists(),						ui_playlist_chooser, 	show()); // IND
    CONNECT (player, setupLastFM(),                          ui_lastfm,              show_win()); // IND
    CONNECT (player, sig_show_stream_rec(),                  ui_stream_rec,          show_win()); // IND
    CONNECT (player, sig_show_socket(),                      ui_socket_setup,        show_win()); // IND


    CONNECT (player, sig_skin_changed(bool),                      ui_eq,              changeSkin(bool));
    CONNECT (player, sig_skin_changed(bool),                      ui_info_dialog,     changeSkin(bool));
    CONNECT (player, sig_skin_changed(bool),                      ui_library,     change_skin(bool));

    CONNECT (player, sig_correct_id3(const MetaData&), 	ui_id3_editor,		change_meta_data(const MetaData&)); // IND


    CONNECT (playlist, sig_selected_file_changed_md(const MetaData&, int, bool),	player,		update_track(const MetaData&, int, bool));
    CONNECT (playlist, sig_selected_file_changed_md(const MetaData&, int, bool),	listen, 	changeTrack(const MetaData &, int, bool ));
    //CONNECT (playlist, sig_gapless_track(const MetaData&),                   listen, 		psl_gapless_track(const MetaData & ));
    CONNECT (playlist, sig_new_stream_session(),                             listen,         psl_new_stream_session());
    CONNECT (playlist, sig_selected_file_changed_md(const MetaData&),		lastfm,			psl_track_changed(const MetaData&));
    CONNECT (playlist, sig_no_track_to_play(),								listen,			stop());
    CONNECT (playlist, sig_no_track_to_play(),								player,			stopped());
    CONNECT (playlist, sig_goon_playing(),                                   listen,			play());
    CONNECT (playlist, sig_selected_file_changed(int),                       ui_playlist, 	track_changed(int));
    CONNECT (playlist, sig_playlist_created(MetaDataList&, int, int), 		ui_playlist, 	fillPlaylist(MetaDataList&, int, int));
    CONNECT (playlist, sig_playlist_created(MetaDataList&, int, int), 		ui_playlist_chooser, 	playlist_changed(MetaDataList&, int, int));
    //CONNECT (&playlist, sig_cur_played_info_changed(const MetaData&),   	&player,  		update_info(const MetaData&));
    CONNECT (playlist, sig_playlist_prepared(int, MetaDataList&),            playlists,      save_playlist_as_custom(int, MetaDataList&));
    CONNECT (playlist, sig_playlist_prepared(QString, MetaDataList&),        playlists,      save_playlist_as_custom(QString, MetaDataList&));
    CONNECT (playlist, sig_library_changed(), 								library,        refresh());
    CONNECT (playlist, sig_import_files(const MetaDataList&),                library, 		importFiles(const MetaDataList&));
    CONNECT (playlist, sig_need_more_radio(),								lastfm, 		psl_radio_playlist_request());

    CONNECT (playlist, sig_data_for_id3_change(const MetaDataList&), 	ui_id3_editor,	change_meta_data(const MetaDataList&)); // IND

    CONNECT (ui_playlist, selected_row_changed(int), 					playlist, 	psl_change_track(int));
    CONNECT (ui_playlist, clear_playlist(),                              playlist, 	psl_clear_playlist());
    CONNECT (ui_playlist, playlist_mode_changed(const Playlist_Mode&),   playlist, 	psl_playlist_mode_changed(const Playlist_Mode&));
    CONNECT (ui_playlist, dropped_tracks(const MetaDataList&, int),      playlist, 	psl_insert_tracks(const MetaDataList&, int));
    CONNECT (ui_playlist, sig_rows_removed(const QList<int>&, bool),     playlist, 	psl_remove_rows(const QList<int>&, bool));
    //CONNECT (ui_playlist, sig_import_to_library(bool),					playlist,	psl_import_new_tracks_to_library(bool));

    CONNECT (listen, track_finished(),                                   playlist,	psl_next_track() );
    CONNECT (listen, sig_valid_strrec_track(const MetaData&),            playlist,  psl_valid_strrec_track(const MetaData&));
    CONNECT (listen, scrobble_track(const MetaData&),                    lastfm, 	psl_scrobble(const MetaData&));
    CONNECT (listen, wanna_gapless_track(),                              playlist,   psl_gapless_track() );

    // should be sent to player
    CONNECT (listen, eq_presets_loaded(const vector<EQ_Setting>&),       ui_eq,	fill_eq_presets(const vector<EQ_Setting>&));
    CONNECT (listen, eq_found(const QStringList&),                       ui_eq, 	fill_available_equalizers(const QStringList&));
    CONNECT (listen, timeChangedSignal(quint32),                         player,	setCurrentPosition(quint32) );

    CONNECT(library, sig_playlist_created(QStringList&), 			playlist, 		psl_createPlaylist(QStringList&));
    CONNECT(library, sig_import_result(bool),						playlist,		psl_import_result(bool));
    CONNECT(library, sig_reload_library_allowed(bool),			player, psl_reload_library_allowed(bool));
    // CONNECT(library, sig_import_result(bool),						ui_playlist,	import_result(bool));
    CONNECT(library, sig_reload_library_finished(),                  ui_library, 	reloading_library_finished());
    CONNECT(library, sig_reloading_library(QString&),				ui_library, 	reloading_library(QString&));
    CONNECT(library, sig_import_result(bool),						ui_library,		import_result(bool));
    CONNECT(library, sig_all_tracks_loaded(MetaDataList&), 			ui_library, 	fill_library_tracks(MetaDataList&));
    CONNECT(library, sig_all_albums_loaded(AlbumList&),          ui_library, 	fill_library_albums(AlbumList&));
    CONNECT(library, sig_all_artists_loaded(ArtistList&), 		ui_library, 	fill_library_artists(ArtistList&));
    CONNECT(library, sig_track_mime_data_available(const MetaDataList&), 			ui_library, 	track_info_available(const MetaDataList&));
    CONNECT(library, sig_tracks_for_playlist_available(MetaDataList&),           	playlist, psl_createPlaylist(MetaDataList&));
    CONNECT(library, sig_delete_answer(QString), 					ui_library, 	psl_delete_answer(QString));
    CONNECT(library, sig_play_next_tracks(const MetaDataList&),  playlist,		psl_play_next_tracks(const MetaDataList&));

    CONNECT(library, sig_change_id3_tags(const MetaDataList&),	ui_id3_editor,	change_meta_data(const MetaDataList&)); // IND

    CONNECT(ui_library, sig_album_dbl_clicked(), 						library, 		psl_prepare_album_for_playlist());
    CONNECT(ui_library, sig_artist_dbl_clicked(), 						library, 		psl_prepare_artist_for_playlist());
    CONNECT(ui_library, sig_track_dbl_clicked(int),                      library, 		psl_prepare_track_for_playlist(int));
    CONNECT(ui_library, sig_artist_pressed(const QList<int>&),           library, 		psl_selected_artists_changed(const QList<int>&));
    CONNECT(ui_library, sig_album_pressed(const QList<int>&),            library, 		psl_selected_albums_changed(const QList<int>&));
    CONNECT(ui_library, sig_disc_pressed(int),                          library,		psl_disc_pressed(int));
    CONNECT(ui_library, sig_track_pressed(const QList<int>&),            library, 		psl_selected_tracks_changed(const QList<int>&));
    CONNECT(ui_library, sig_filter_changed(const Filter&),               library, 		psl_filter_changed(const Filter&));

    CONNECT(ui_library, sig_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder),
            library, 	 psl_sortorder_changed(Sort::SortOrder, Sort::SortOrder, Sort::SortOrder));

    CONNECT(ui_library, sig_show_id3_editor(const QList<int>&),              library, 		psl_change_id3_tags(const QList<int>&));
    CONNECT(ui_library, sig_delete_tracks(int),                              library,		psl_delete_tracks(int));
    CONNECT(ui_library, sig_delete_certain_tracks(const QList<int>&, int),	library,		psl_delete_certain_tracks(const QList<int>&, int));
    CONNECT(ui_library, sig_play_next_tracks(const QList<int>&),             library,		psl_play_next_tracks(const QList<int>&));
    CONNECT(ui_library, sig_play_next_all_tracks(),                          library,		psl_play_next_all_tracks());

    CONNECT(ui_lastfm, sig_activated(bool),                                  player,         psl_lfm_activated(bool));
    CONNECT(ui_lastfm, new_lfm_credentials(QString, QString),                lastfm, 		psl_login(QString, QString));

    CONNECT(ui_id3_editor, id3_tags_changed(), 						library,        refresh());
    CONNECT(ui_id3_editor, id3_tags_changed(MetaDataList&), 			playlist, 		psl_id3_tags_changed(MetaDataList&));
    CONNECT(ui_id3_editor, id3_tags_changed(MetaDataList&), 			player, 		psl_id3_tags_changed(MetaDataList&));

    qDebug() << "Equalizer";
    CONNECT(ui_eq, eq_changed_signal(int, int),                          listen, 	eq_changed(int, int));
    CONNECT(ui_eq, eq_enabled_signal(bool),                              listen, 	eq_enable(bool));

    qDebug() << "last fm";
    CONNECT(lastfm,	sig_similar_artists_available(const QList<int>&),		playlist,	psl_similar_artists_available(const QList<int>&));
    CONNECT(lastfm,  sig_radio_initialized(bool),                            playlist,	psl_lfm_radio_init(bool));
    CONNECT(lastfm,	sig_last_fm_logged_in(bool),							player,		last_fm_logged_in(bool));
    CONNECT(lastfm,	sig_new_radio_playlist(const MetaDataList&),            playlist,	psl_new_lfm_playlist_available(const MetaDataList&));
    CONNECT(lastfm,  sig_track_info_fetched(const MetaData&, bool, bool),    player,		lfm_info_fetched(const MetaData&, bool, bool));

    qDebug() << "playlist chooser";
    CONNECT(ui_playlist_chooser, sig_playlist_chosen(int),		playlists, load_single_playlist(int));
    CONNECT(ui_playlist_chooser, sig_delete_playlist(int),       playlists, delete_playlist(int));
    CONNECT(ui_playlist_chooser, sig_save_playlist(int), 		playlist, 	psl_prepare_playlist_for_save(int));
    CONNECT(ui_playlist_chooser, sig_save_playlist(QString), 	playlist, 	psl_prepare_playlist_for_save(QString));
    CONNECT(ui_playlist_chooser, sig_save_playlist_file(QString, bool), 	playlist, 	psl_prepare_playlist_for_save_file(QString, bool));
    CONNECT(ui_playlist_chooser, sig_clear_playlist(),           playlist, 	psl_clear_playlist());
    CONNECT(ui_playlist_chooser, sig_files_selected(QStringList &), playlist, psl_createPlaylist(QStringList&));

    CONNECT(playlists, sig_single_playlist_loaded(CustomPlaylist&),      playlist, 				psl_createPlaylist(CustomPlaylist&));
    CONNECT(playlists, sig_all_playlists_loaded(QMap<int, QString>&), 	ui_playlist_chooser, 	all_playlists_fetched(QMap<int, QString>&));
    qDebug() << "last fm radio";
    CONNECT(ui_lfm_radio, listen_clicked(const QString&, int),          lastfm,		psl_radio_init(const QString&, int));

    qDebug() << "stream";
    CONNECT(ui_stream, sig_play_stream(const QString&, const QString&), 	playlist, 	psl_play_stream(const QString&, const QString&));

    qDebug() << "podcasts";
    CONNECT(ui_podcasts, sig_play_podcast(const QString&, const QString&), 	playlist, 	psl_play_podcast(const QString&, const QString&));


    CONNECT (ui_stream_rec, sig_stream_recorder_active(bool),	listen,		psl_sr_set_active(bool));
    CONNECT (ui_stream_rec, sig_stream_recorder_active(bool),	player,     psl_strrip_set_active(bool));




    bool is_socket_active = set->getSocketActivated();
    if(is_socket_active){
        CONNECT (remote_socket, sig_play(),		playlist,			psl_play());
        CONNECT (remote_socket, sig_next(),		playlist,			psl_forward());
        CONNECT (remote_socket, sig_prev(),		playlist,			psl_backward());
        CONNECT (remote_socket, sig_stop(),		playlist,			psl_stop());
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
}

void Application::setFiles2Play(QStringList filelist){

    if(filelist.size() > 0){
        playlist->psl_createPlaylist(filelist);

        if(playlist->get_num_tracks() > 0)
            playlist->psl_play();
    }
}



QString Application::getVersion(){

    ifstream istr;
    QString version_file = Helper::getSharePath() + "/VERSION";

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








