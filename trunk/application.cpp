#include "application.h"

#include <QApplication>
#include "GUI/player/GUI_Simpleplayer.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "GUI/LastFM/GUI_LastFM.h"
#include "GUI/library/GUI_Library_windowed.h"
#include "GUI/library/GUILibraryInfoBox.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"
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
#include "HelperStructs/CDirectoryReader.h"
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "Socket/Socket.h"

#include <QMap>

#include <fstream>
#include <string>

using namespace std;

Application::Application(QApplication* qapp, QObject *parent) : QObject(parent)
{
    app                = qapp;

    set                 = CSettingsStorage::getInstance();
    set  -> runFirstTime(false);
    CDatabaseConnector::getInstance()->load_settings();

    player              = new GUI_SimplePlayer();

    ui_playlist_chooser = new GUI_PlaylistChooser(player->getParentOfPlugin());

    playlist            = new Playlist();
    library             = new CLibraryBase(this);
    playlists           = new Playlists();

    lastfm              = LastFM::getInstance();
    ui_lastfm           = new GUI_LastFM(player->centralWidget());
    ui_stream           = new GUI_Stream(player->getParentOfPlugin());
    ui_eq               = new GUI_Equalizer(player->getParentOfPlugin());
    ui_lfm_radio        = new GUI_LFMRadioWidget(player->getParentOfPlugin());

    ui_stream_rec       = new GUI_StreamRecorder(player->centralWidget());
    ui_id3_editor       = new GUI_TagEdit();
    ui_info_dialog      = new GUI_InfoDialog(player->centralWidget(), ui_id3_editor);
    //ui_library_info_box = new GUI_Library_Info_Box(player->centralWidget());
    ui_socket_setup     = new GUI_SocketSetup(player->centralWidget());

    ui_library          = new GUI_Library_windowed(player->getParentOfLibrary(), ui_info_dialog);
    ui_playlist         = new GUI_Playlist(player->getParentOfPlaylist(), ui_info_dialog);

    remote_socket       = new Socket();

    QString dir;

#ifdef Q_OS_UNIX
    dir = "/usr/lib/sayonara";
#else
    dir = app->applicationDirPath();
#endif

    plugin_loader = new SoundPluginLoader(dir);
    listen = plugin_loader->get_cur_engine();
    if(!listen){
        qDebug() << "No Sound Engine found! You fucked up the installation. Aborting...";
    }

    else{
        listen->init();
        listen->psl_sr_set_active(set->getStreamRipper());
    }

    qDebug() << "Init connections";
    init_connections();

    qDebug() << "setup player";
    player->setWindowTitle("Sayonara (0.3)");
    player->setWindowIcon(QIcon(Helper::getIconPath() + "play.png"));

    player->setPlaylist(ui_playlist);
    player->setLibrary(ui_library);
    player->setStream(ui_stream);
    player->setLFMRadio(ui_lfm_radio);
    player->setEqualizer(ui_eq);
    player->setPlaylistChooser(ui_playlist_chooser);
    player->setStyle( set->getPlayerStyle() );
    player->show();



   qDebug() << "player is set up";
    ui_library->resize(player->getParentOfLibrary()->size());
    ui_playlist->resize(player->getParentOfPlaylist()->size());





    vector<EQ_Setting> vec_eq_setting;
    set->getEqualizerSettings(vec_eq_setting);

    int vol = set->getVolume();
    player->setVolume(vol);
    listen->setVolume(vol);
    listen->load_equalizer(vec_eq_setting);

    library->loadDataFromDb();

    QString user, password;
    if(set->getLastFMActive()){
        set->getLastFMNameAndPW(user, password);
        player->suppress_warning(true);
        LastFM::getInstance()->lfm_login( user,password, true );
    }

    playlist->ui_loaded();
    playlists->ui_loaded();
    player->ui_loaded();

    int shown_plugin = set->getShownPlugin();
    player->hideAllPlugins();
    set->setShownPlugin(shown_plugin);
    player->check_show_plugins();

    getVersion();
}

Application::~Application(){

    CDatabaseConnector::getInstance()->store_settings();

    if( set->getSocketActivated() ){
        if(remote_socket->isRunning())
            remote_socket->exit(0);
    }

    delete remote_socket;
    delete ui_socket_setup;
    delete ui_playlist;
    delete ui_library;
    delete ui_id3_editor;
    delete ui_stream_rec;
    delete ui_lfm_radio;
    delete ui_eq;
    delete ui_stream;
    delete ui_lastfm;
    delete library;
    delete playlist;
    delete playlists;
    delete ui_playlist_chooser;
    delete player;
}



void Application::init_connections(){

   CONNECT (player, pause(),                                listen,				pause());
   CONNECT (player, search(int),							listen,			jump(int));
   CONNECT (player, sig_volume_changed(int),				listen,			setVolume(int));
   CONNECT (player, sig_rec_button_toggled(bool),			listen,			record_button_toggled(bool));

   CONNECT (player, baseDirSelected(const QString &),		library,            baseDirSelected(const QString & ));
   CONNECT (player, reloadLibrary(), 						library,            reloadLibrary());
   CONNECT (player, importDirectory(QString),				library,            importDirectory(QString));
   CONNECT (player, libpath_changed(QString),               library, 			setLibraryPath(QString));
   CONNECT (player, fileSelected(QStringList &),			playlist, 			psl_createPlaylist(QStringList&));
   CONNECT (player, play(),                                 playlist,			psl_play());
   CONNECT (player, stop(),                                 playlist,			psl_stop());
   CONNECT (player, forward(),                              playlist,			psl_forward());
   CONNECT (player, backward(),                             playlist,			psl_backward());
   CONNECT (player, sig_stream_selected(const QString&, const QString&), 		playlist, psl_play_stream(const QString&, const QString&));

   CONNECT (player, skinChanged(bool),                      ui_playlist, 		change_skin(bool));
   CONNECT (player, skinChanged(bool),                      ui_library, 		change_skin(bool));
   CONNECT (player, skinChanged(bool),                      ui_eq,              changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_stream,          changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_lfm_radio, 		changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_playlist_chooser, changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_info_dialog,     changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_stream_rec,      changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_id3_editor,      changeSkin(bool));
   CONNECT (player, skinChanged(bool),                      ui_lastfm,          changeSkin(bool));

   CONNECT (player, show_small_playlist_items(bool),		ui_playlist,		psl_show_small_playlist_items(bool));
   CONNECT (player, sig_sound_engine_changed(QString&), 	plugin_loader,      psl_switch_engine(QString&));

   CONNECT (player, show_playlists(),						ui_playlist_chooser, 	show()); // IND
   CONNECT (player, setupLastFM(),                          ui_lastfm,              show_win()); // IND
   CONNECT (player, sig_show_stream_rec(),                  ui_stream_rec,          show_win()); // IND
   CONNECT (player, sig_show_socket(),                      ui_socket_setup,        show_win()); // IND


	   CONNECT (player, sig_correct_id3(const MetaData&), 	ui_id3_editor,		change_meta_data(const MetaData&)); // IND


	   CONNECT (playlist, sig_selected_file_changed_md(const MetaData&),		player,			update_track(const MetaData&));
	   CONNECT (playlist, sig_selected_file_changed_md(const MetaData&), 		listen, 		changeTrack(const MetaData & ));
       CONNECT (playlist, sig_gapless_track(const MetaData&),                   listen, 		psl_gapless_track(const MetaData & ));
       CONNECT (playlist, sig_selected_file_changed_md(const MetaData&),		lastfm,			psl_track_changed(const MetaData&));
	   CONNECT (playlist, sig_no_track_to_play(),								listen,			stop());
	   CONNECT (playlist, sig_goon_playing(),                                   listen,			play());
	   CONNECT (playlist, sig_selected_file_changed(int),                       ui_playlist, 	track_changed(int));
	   CONNECT (playlist, sig_playlist_created(MetaDataList&, int), 		ui_playlist, 	fillPlaylist(MetaDataList&, int));
	   //CONNECT (&playlist, sig_cur_played_info_changed(const MetaData&),   	&player,  		update_info(const MetaData&));
	   CONNECT (playlist, sig_playlist_prepared(int, MetaDataList&), 		playlists,      save_playlist_as_custom(int, MetaDataList&));
	   CONNECT (playlist, sig_playlist_prepared(QString, MetaDataList&), 	playlists,      save_playlist_as_custom(QString, MetaDataList&));
	   CONNECT (playlist, sig_library_changed(), 								ui_library, 	library_changed());
	   CONNECT (playlist, sig_import_files(const MetaDataList&), 			library, 		importFiles(const MetaDataList&));
	   CONNECT (playlist, sig_need_more_radio(),								lastfm, 		psl_radio_playlist_request());

	   CONNECT (playlist, sig_data_for_id3_change(const MetaDataList&), 	ui_id3_editor,	change_meta_data(const MetaDataList&)); // IND

	   CONNECT (ui_playlist, selected_row_changed(int), 					playlist, 	psl_change_track(int));
	   CONNECT (ui_playlist, clear_playlist(),                              playlist, 	psl_clear_playlist());
	   CONNECT (ui_playlist, playlist_mode_changed(const Playlist_Mode&),   playlist, 	psl_playlist_mode_changed(const Playlist_Mode&));
	   CONNECT (ui_playlist, dropped_tracks(const MetaDataList&, int),      playlist, 	psl_insert_tracks(const MetaDataList&, int));
	   CONNECT (ui_playlist, rows_removed(const QList<int>&),               playlist, 	psl_remove_rows(const QList<int>&));
	   CONNECT (ui_playlist, sig_import_to_library(bool),					playlist,	psl_import_new_tracks_to_library(bool));
	   CONNECT(ui_playlist, edit_id3_signal(),								playlist, 	psl_edit_id3_request());

	   CONNECT (listen, track_finished(),                                   playlist,	psl_next_track() );
	   CONNECT (listen, sig_valid_strrec_track(const MetaData&),            playlist,  psl_valid_strrec_track(const MetaData&));

	   CONNECT (listen, scrobble_track(const MetaData&),                    lastfm, 	psl_scrobble(const MetaData&));
       CONNECT (listen, wanna_gapless_track(),                              playlist,   psl_gapless_track() );

	   // should be sent to player
	   CONNECT (listen, eq_presets_loaded(const vector<EQ_Setting>&),       ui_eq,	fill_eq_presets(const vector<EQ_Setting>&));
	   CONNECT (listen, eq_found(const QStringList&),                       ui_eq, 	fill_available_equalizers(const QStringList&));
	   CONNECT (listen, total_time_changed_signal(qint64),                  player,	total_time_changed(qint64));
	   CONNECT (listen, timeChangedSignal(quint32),                         player,	setCurrentPosition(quint32) );


	   CONNECT(library, sig_playlist_created(QStringList&), 			playlist, 		psl_createPlaylist(QStringList&));
	   CONNECT(library, sig_import_result(bool),						playlist,		psl_import_result(bool));
	   CONNECT(library, sig_import_result(bool),						ui_playlist,	import_result(bool));
	   CONNECT(library, sig_reload_library_finished(),                  ui_library, 	reloading_library_finished());
	   CONNECT(library, sig_reloading_library(QString&),				ui_library, 	reloading_library(QString&));
	   CONNECT(library, sig_import_result(bool),						ui_library,		import_result(bool));
	   CONNECT(library, sig_metadata_loaded(MetaDataList&), 			ui_library, 	fill_library_tracks(MetaDataList&));
	   CONNECT(library, sig_all_albums_loaded(AlbumList&),          ui_library, 	fill_library_albums(AlbumList&));
	   CONNECT(library, sig_all_artists_loaded(ArtistList&), 		ui_library, 	fill_library_artists(ArtistList&));
	   CONNECT(library, sig_track_mime_data_available(const MetaDataList&), 			ui_library, 	track_info_available(const MetaDataList&));
	   CONNECT(library, sig_tracks_for_playlist_available(MetaDataList&),           	playlist, psl_createPlaylist(MetaDataList&));
	   CONNECT(library, sig_import_result(bool),						playlists,		import_result(bool));
	   CONNECT(library, sig_delete_answer(QString), 					ui_library, 	psl_delete_answer(QString));
	   CONNECT(library, sig_play_next_tracks(const MetaDataList&),  playlist,		psl_play_next_tracks(const MetaDataList&));

	   CONNECT(library, sig_change_id3_tags(const MetaDataList&),	ui_id3_editor,	change_meta_data(const MetaDataList&)); // IND

	   CONNECT(ui_library, sig_album_dbl_clicked(), 						library, 		psl_prepare_album_for_playlist());
	   CONNECT(ui_library, sig_artist_dbl_clicked(), 						library, 		psl_prepare_artist_for_playlist());
	   CONNECT(ui_library, sig_track_dbl_clicked(int),                      library, 		psl_prepare_track_for_playlist(int));
	   CONNECT(ui_library, sig_artist_pressed(const QList<int>&),           library, 		psl_selected_artists_changed(const QList<int>&));
	   CONNECT(ui_library, sig_album_pressed(const QList<int>&),            library, 		psl_selected_albums_changed(const QList<int>&));
	   CONNECT(ui_library, sig_track_pressed(const QList<int>&),            library, 		psl_selected_tracks_changed(const QList<int>&));
	   CONNECT(ui_library, sig_filter_changed(const Filter&),               library, 		psl_filter_changed(const Filter&));

	   CONNECT(ui_library, sig_sortorder_changed(Sort::ArtistSort, Sort::AlbumSort, Sort::TrackSort),
			   library, 	 psl_sortorder_changed(Sort::ArtistSort, Sort::AlbumSort, Sort::TrackSort));

	   CONNECT(ui_library, sig_show_id3_editor(const QList<int>&),              library, 		psl_change_id3_tags(const QList<int>&));
	   CONNECT(ui_library, sig_delete_tracks(int),                              library,		psl_delete_tracks(int));
	   CONNECT(ui_library, sig_delete_certain_tracks(const QList<int>&, int),	library,		psl_delete_certain_tracks(const QList<int>&, int));
	   CONNECT(ui_library, sig_play_next_tracks(const QList<int>&),             library,		psl_play_next_tracks(const QList<int>&));
	   CONNECT(ui_library, sig_play_next_all_tracks(),                          library,		psl_play_next_all_tracks());

       CONNECT(ui_lastfm, sig_activated(bool),                                  player,         psl_lfm_activated(bool));
       CONNECT(ui_lastfm, new_lfm_credentials(QString, QString),                lastfm, 		psl_login(QString, QString));
       CONNECT(ui_lfm_radio, close_event(),                                        player, 		close_lfm_radio());

	   CONNECT(ui_eq, eq_changed_signal(int, int),                          listen, 	eq_changed(int, int));
	   CONNECT(ui_eq, eq_enabled_signal(bool),                              listen, 	eq_enable(bool));
	   CONNECT(ui_eq, close_event(),                                        player, 	close_eq());


	   CONNECT(ui_id3_editor, id3_tags_changed(), 						ui_library, 	id3_tags_changed());
	   CONNECT(ui_id3_editor, id3_tags_changed(MetaDataList&), 			playlist, 		psl_id3_tags_changed(MetaDataList&));
	   CONNECT(ui_id3_editor, id3_tags_changed(MetaDataList&), 			player, 		psl_id3_tags_changed(MetaDataList&));


       CONNECT(lastfm,	sig_similar_artists_available(const QList<int>&),		playlist,	psl_similar_artists_available(const QList<int>&));
       CONNECT(lastfm,	sig_last_fm_logged_in(bool),							player,		last_fm_logged_in(bool));
       CONNECT(lastfm,	sig_new_radio_playlist(const MetaDataList&),            playlist,		psl_new_radio_playlist_available(const MetaDataList&));
       CONNECT(lastfm,  sig_track_info_fetched(const MetaData&, bool, bool),     player,		lfm_info_fetched(const MetaData&, bool, bool));

	   CONNECT(ui_playlist_chooser, sig_playlist_chosen(int),		playlists, load_single_playlist(int));
	   CONNECT(ui_playlist_chooser, sig_delete_playlist(int),       playlists, delete_playlist(int));
	   CONNECT(ui_playlist_chooser, sig_save_playlist(int), 		playlist, 	psl_prepare_playlist_for_save(int));
	   CONNECT(ui_playlist_chooser, sig_save_playlist(QString), 	playlist, 	psl_prepare_playlist_for_save(QString));
	   CONNECT(ui_playlist_chooser, sig_clear_playlist(),           playlist, 	psl_clear_playlist());
	   CONNECT(ui_playlist_chooser, sig_closed(),                   player, 	close_playlist_chooser());

	   CONNECT(playlists, sig_single_playlist_loaded(CustomPlaylist&),      playlist, 				psl_createPlaylist(CustomPlaylist&));
	   CONNECT(playlists, sig_all_playlists_loaded(QMap<int, QString>&), 	ui_playlist_chooser, 	all_playlists_fetched(QMap<int, QString>&));
	   CONNECT(playlists, sig_import_tracks(const MetaDataList&),       library, 				importFiles(const MetaDataList&));


		CONNECT(ui_lfm_radio, listen_clicked(const QString&, int),          lastfm,		psl_radio_init(const QString&, int));
		CONNECT(ui_lfm_radio, close_event(), 								player, 	close_lfm_radio());

		CONNECT(ui_stream, sig_play_stream(const QString&, const QString&), 	playlist, 	psl_play_stream(const QString&, const QString&));
		CONNECT(ui_stream, sig_close_event(), 									player, 	close_stream());


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
}


void Application::setFiles2Play(QStringList filelist){

    if(filelist.size() > 0){
        playlist->psl_createPlaylist(filelist);

        if(playlist->get_num_tracks() > 0)
            playlist->psl_play();
    }
}



void Application::getVersion(){



    ifstream istr;
    QString version_file = Helper::getSharePath() + "/VERSION";

    istr.open(version_file.toUtf8()  );
    if(!istr || !istr.is_open()) return;

    QMap<QString, int> map;

    while(istr.good()){
        string type;
        int version;
        istr >> type >> version;
        map[QString(type.c_str())] = version;
    }

    istr.close();
}

QMainWindow* Application::getMainWindow(){
    return this->player;
}
