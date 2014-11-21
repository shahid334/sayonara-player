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

#include <QMetaType>
#include "DatabaseAccess/CDatabaseConnector.h"
#include "StreamPlugins/LastFM/LastFM.h"

#include <QMap>
#include <QSharedMemory>

#include <fstream>
#include <string>

using namespace std;

bool Application::is_initialized() {
    return _initialized;
}


Application::Application(int & argc, char ** argv) : QApplication(argc, argv)
{

}

void Application::check_for_crash(){
	QString error_file = Helper::getErrorFile();
	if(!QFile::exists(error_file)) return;


	QString info_text;
	QString mail;
	if(!Helper::read_file_into_str(error_file, &mail)){
		mail = "";
		mail.prepend("mailto:luciocarreras@gmail.com?subject=Sayonara Crash&amp;body=Hi Lucio,\n\nhere is the trace for a Sayonara crash\n\n");

	}

	else{
		mail.prepend("mailto:luciocarreras@gmail.com?subject=Sayonara Crash&amp;body=Hi Lucio,\n\nhere is the trace for a Sayonara crash\n\n");
		mail.append("\n\nI hope this will not happen again...");
	}



	info_text = QString("Sayonara seems to have crashed the last time<br />") +
				"Please send " +
			Helper::createLink(error_file, error_file) +
			" in " + Helper::createLink(Helper::getSayonaraPath(), Helper::getSayonaraPath()) +
			" to " + Helper::createLink("luciocarreras@gmail.com", mail);

	QMessageBox::information(0, "Error detected", info_text);

	QFile f(error_file);
	f.open(QIODevice::ReadOnly);
	if(!f.isOpen()){
		qDebug() << "Cannot oopen " << error_file;
		return;
	}

	f.remove();
	f.close();

	return;

}

void Application::init(int n_files, QTranslator *translator) {


	check_for_crash();
	_translator        = translator;

	set                = Settings::getInstance();

	QString version    = getVersion();
	set->set(Set::Player_Version, version);

	player              = new GUI_Player(translator);

	playlist_handler    = new PlaylistHandler();
	playlist_loader     = new PlaylistLoader(this);
	library             = new CLibraryBase(this->getMainWindow());
	library_importer    = new LibraryImporter(this->getMainWindow());
	playlists           = new Playlists();

	lastfm              = LastFM::getInstance();


	ui_stream_rec       = new GUI_StreamRecorder(player->centralWidget());
	ui_id3_editor       = new GUI_TagEdit();

	ui_info_dialog      = new GUI_InfoDialog(player->centralWidget(), ui_id3_editor);
	ui_socket_setup     = new GUI_SocketSetup(player->centralWidget());

	ui_library          = new GUI_Library_windowed(player->getParentOfLibrary());
	ui_library->set_info_dialog(ui_info_dialog);
	ui_playlist         = new GUI_Playlist(player->getParentOfPlaylist(), ui_info_dialog);

	ui_lastfm           = new GUI_LastFM(player->centralWidget());
	ui_level            = new GUI_LevelPainter(tr("Le&vel"), player->getParentOfPlugin());
	ui_spectrum         = new GUI_Spectrum(tr("&Spectrum"), player->getParentOfPlugin());
	ui_stream           = new GUI_Stream(tr("&Webstreams"), player->getParentOfPlugin());
	ui_podcasts         = new GUI_Podcasts(tr("P&odcasts"), player->getParentOfPlugin());
	ui_eq               = new GUI_Equalizer(tr("&Equalizer"), player->getParentOfPlugin());
	ui_playlist_chooser = new GUI_PlaylistChooser(tr("Pla&ylists"), player->getParentOfPlugin());
	ui_audioconverter   = new GUI_AudioConverter(tr("&mp3 Converter"), player->getParentOfPlugin());
	ui_bookmarks        = new GUI_Bookmarks(tr("&Bookmarks"), player->getParentOfPlugin());
	ui_speed			= new GUI_Speed(tr("Spee&d"), player->getParentOfPlugin());
	ui_broadcast		= new GUI_Broadcast(tr("&Broadcast"), player->getParentOfPlugin());


	ui_style_settings = new GUI_StyleSettings(player);

	stream_server       = new StreamServer(this);

	_pph = new PlayerPluginHandler(NULL);

	_pph->addPlugin(ui_level);
	_pph->addPlugin(ui_spectrum);
	_pph->addPlugin(ui_eq);
	_pph->addPlugin(ui_stream);
	_pph->addPlugin(ui_podcasts);
	_pph->addPlugin(ui_playlist_chooser);
	_pph->addPlugin(ui_audioconverter);
	_pph->addPlugin(ui_bookmarks);
	_pph->addPlugin(ui_speed);
	_pph->addPlugin(ui_broadcast);

	qDebug() << "Plugin " << ui_level->getVisName();
	qDebug() << "Plugin " << ui_stream->getVisName();
	qDebug() << "Plugin " << ui_eq->getVisName();
	qDebug() << "Plugin " << ui_playlist_chooser->getVisName();
	qDebug() << "Plugin " << ui_podcasts->getVisName();
	qDebug() << "Plugin " << ui_audioconverter->getVisName();
	qDebug() << "Plugin " << ui_bookmarks->getVisName();
	qDebug() << "Plugin " << ui_speed->getVisName();
	qDebug() << "Plugin " << ui_broadcast->getVisName();

	QString dir;

#ifndef Q_OS_WIN
	  dir = Helper::getLibPath();
	  qDebug() << "Lib path = " << dir;
#else
	  dir = this->applicationDirPath();
#endif

	SoundPluginLoader loader(dir);
	listen = loader.get_first_engine();
	if(!listen) {
		qDebug() << "No Sound Engine found! You fucked up the installation. Aborting...";
		exit(1);
	}

	init_connections();


	// emit do connections
	// emit connections done

	qDebug() << "setting up player";
	bool is_maximized = set->get(Set::Player_Maximized);

	player->setWindowTitle("Sayonara " + version);
    player->setWindowIcon(Helper::getIcon("logo.png"));

	/* Into Player */
	player->setPlaylist(ui_playlist);
	player->setLibrary(ui_library);
	player->setInfoDialog(ui_info_dialog);
	player->setPlayerPluginHandler(_pph);

	player->setStyle( set->get(Set::Player_Style) );

	/* --> INTO Player*/
	if(is_maximized) player->showMaximized();
	else player->show();

	ui_library->resize(player->getParentOfLibrary()->size());
	ui_playlist->resize(player->getParentOfPlaylist()->size());

	qDebug() << "Set up library...";
	/* Into Library */
	library->loadDataFromDb();

	qDebug() << "Set up Last.fm...";

	/* Into LastFM */

	bool last_fm_active = set->get(Set::LFM_Active);
	if(last_fm_active) {
		LastFM::getInstance()->psl_login();
	}

	bool load_old_playlist = (n_files == 0);
	if(load_old_playlist){
		playlist_loader->load_old_playlist();
	}

	playlists->ui_loaded();
	player->ui_loaded();

	/* Into Player */

	QString shown_plugin = set->get(Set::Player_ShownPlugin);
	PlayerPlugin* p  = _pph->find_plugin(shown_plugin);

	player->showPlugin(p);

	_initialized = true;
}

Application::~Application() {

    CDatabaseConnector::getInstance()->store_settings();

    delete listen;
    delete ui_socket_setup;
    delete ui_playlist;
    delete ui_library;
    delete ui_stream_rec;
    delete ui_eq;
    delete ui_stream;
    delete ui_podcasts;
    delete ui_lastfm;
    delete library_importer;
    delete library;
    delete playlist_handler;
    delete playlists;
    delete ui_playlist_chooser;
    delete player;
    
    CDatabaseConnector::getInstance()->closeDatabase();
}



void Application::init_connections() {

	CONNECT (player, sig_seek_rel(quint32),					listen,			jump_rel(quint32));
    CONNECT (player, sig_seek_rel_ms(qint64),				listen,			jump_rel_ms(qint64));

    CONNECT (player, sig_rec_button_toggled(bool),			listen,			record_button_toggled(bool));
    CONNECT (player, sig_rec_button_toggled(bool),			ui_stream_rec,	record_button_toggled(bool));

	CONNECT (player, sig_basedir_selected(const QString &),	library,            baseDirSelected(const QString & ));
	CONNECT (player, sig_reload_library(bool),				library,            reloadLibrary(bool));
	CONNECT (player, sig_import_dir(const QString&),		library_importer,        psl_import_dir(const QString&));
	CONNECT (player, sig_import_files(const QStringList&),	library_importer,        psl_import_files(const QStringList&));

	CONNECT (player, sig_file_selected(const QStringList &),playlist_handler, 			psl_createPlaylist(const QStringList&));
	CONNECT (player, sig_play(),		playlist_handler,			psl_play());
	CONNECT (player, sig_pause(),		playlist_handler,			psl_pause());
	CONNECT (player, sig_pause(),		listen,						pause());
	CONNECT (player, sig_stop(),		playlist_handler,			psl_stop());
	CONNECT (player, sig_stop(),		ui_spectrum,				psl_stop());
	CONNECT (player, sig_stop(),		ui_level,					psl_stop());
	CONNECT (player, sig_stop(),		ui_audioconverter,			stopped());
	CONNECT (player, sig_pause(),		ui_spectrum,				psl_stop());
	CONNECT (player, sig_pause(),		ui_level,					psl_stop());
	CONNECT (player, sig_forward(),		playlist_handler,			psl_forward());
	CONNECT (player, sig_backward(),	playlist_handler,			psl_backward());

	CONNECT (player, sig_show_playlists(),						 ui_playlist_chooser, 	show()); // IND
    CONNECT (player, sig_setup_LastFM(),                         ui_lastfm,              show_win()); // IND
    CONNECT (player, sig_show_stream_rec(),                      ui_stream_rec,          show_win()); // IND
    CONNECT (player, sig_show_socket(),                      ui_socket_setup,        show_win()); // IND


    CONNECT (player, sig_correct_id3(const MetaData&), 	ui_id3_editor,		change_meta_data(const MetaData&)); // IND


    CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&, int, bool),	player,		psl_update_track(const MetaData&, int, bool));
    CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&, int, bool),	listen, 	change_track(const MetaData &, int, bool ));
    CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&),               lastfm,		psl_track_changed(const MetaData&));
    CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&),               ui_bookmarks,	track_changed(const MetaData&));
    CONNECT (playlist_handler, sig_selected_file_changed(int),                              ui_playlist, 	track_changed(int));


    CONNECT (playlist_handler, sig_no_track_to_play(),								listen,			stop());
    CONNECT (playlist_handler, sig_no_track_to_play(),								player,			stopped());
    CONNECT (playlist_handler, sig_goon_playing(),                                  listen,			play());

    CONNECT (playlist_handler, sig_playlist_created(const MetaDataList&, int, PlaylistType), 		ui_playlist, 	fillPlaylist(const MetaDataList&, int, PlaylistType));
    CONNECT (playlist_handler, sig_playlist_created(const MetaDataList&, int, PlaylistType), 		ui_playlist_chooser, 	playlist_changed(const MetaDataList&, int, PlaylistType));
	CONNECT (playlist_handler, sig_playlist_prepared(int, const MetaDataList&),            playlists,     save_playlist_as_custom(int, const MetaDataList&));
	CONNECT (playlist_handler, sig_playlist_prepared(QString, const MetaDataList&),        playlists,     save_playlist_as_custom(QString, const MetaDataList&));

	CONNECT (ui_playlist, sig_cur_idx_changed(int),                      playlist_handler, 	psl_change_track(int));
	CONNECT (ui_playlist, sig_selection_changed(const QList<int>&),      playlist_handler, 	psl_selection_changed(const QList<int>&));
	CONNECT (ui_playlist, sig_cleared(),                                 playlist_handler, 	psl_clear_playlist());

	CONNECT (ui_playlist, sig_tracks_dropped(const MetaDataList&, int),  playlist_handler, 	psl_insert_tracks(const MetaDataList&, int));
	CONNECT (ui_playlist, sig_rows_removed(const QList<int>&, bool),     playlist_handler, 	psl_remove_rows(const QList<int>&, bool));
	CONNECT (ui_playlist, sig_rows_moved(const QList<int>&, int),        playlist_handler, 	psl_move_rows(const QList<int>&, int));
	CONNECT (ui_playlist, sig_no_focus(),                                ui_library,		setFocus());

	CONNECT( playlist_loader, sig_stop(),                                playlist_handler,       psl_stop());
	CONNECT( playlist_loader, sig_create_playlist(const MetaDataList&, bool),  playlist_handler,       psl_createPlaylist(const MetaDataList&, bool));
	CONNECT( playlist_loader, sig_change_track(int, qint32, bool),       playlist_handler,       psl_change_track(int, qint32, bool));

	CONNECT (listen, sig_track_finished(),                               playlist_handler,	psl_next() );
	CONNECT (listen, sig_scrobble(const MetaData&),                      lastfm, 	psl_scrobble(const MetaData&));
	CONNECT (listen, sig_level(float, float),                            ui_level,  set_level(float,float));
	CONNECT (listen, sig_spectrum(QList<float>&),                        ui_spectrum, set_spectrum(QList<float>&));


    // should be sent to player
	CONNECT (listen, sig_pos_changed_s(quint32),                      player,              psl_set_cur_pos(quint32) );
	CONNECT (listen, sig_pos_changed_s(quint32),                      ui_bookmarks,        pos_changed_s(quint32) );
	CONNECT (listen, sig_bitrate_changed(qint32),					  player,              psl_bitrate_changed(qint32));
	CONNECT (listen, sig_dur_changed(const MetaData&),                player,              psl_dur_changed(const MetaData&));
	CONNECT (listen, sig_dur_changed(const MetaData&),                playlist_handler,    psl_dur_changed(const MetaData&));
	CONNECT (listen, sig_dur_changed(const MetaData&),                library,             psl_dur_changed(const MetaData&));

	CONNECT (ui_speed, sig_speed_changed(float),                      listen,              psl_set_speed(float) );

	CONNECT(library, sig_playlist_created(const QStringList&),            playlist_handler, 		psl_createPlaylist(const QStringList&));
	CONNECT(library, sig_reloading_library(const QString&),				ui_library, 	reloading_library(const QString&));
	CONNECT(library, sig_all_tracks_loaded(const MetaDataList&), 			ui_library, 	fill_library_tracks(const MetaDataList&));
	CONNECT(library, sig_all_albums_loaded(const AlbumList&),				ui_library, 	fill_library_albums(const AlbumList&));
	CONNECT(library, sig_all_artists_loaded(const ArtistList&),			ui_library, 	fill_library_artists(const ArtistList&));
    CONNECT(library, sig_track_mime_data_available(const MetaDataList&), 			ui_library, 	track_info_available(const MetaDataList&));
	CONNECT(library, sig_tracks_for_playlist_available(const MetaDataList&),           	playlist_handler, psl_createPlaylist(const MetaDataList&));
	CONNECT(library, sig_append_tracks_to_playlist(const MetaDataList&),                  playlist_handler, psl_append_tracks(const MetaDataList&));
    CONNECT(library, sig_delete_answer(QString), 					ui_library, 	psl_delete_answer(QString));
	CONNECT(library, sig_play_next_tracks(const MetaDataList&),		playlist_handler,          psl_play_next(const MetaDataList&));
	CONNECT(library, sig_change_id3_tags(const MetaDataList&),		ui_id3_editor,	change_meta_data(const MetaDataList&)); // IND
	CONNECT(library, sig_reload_library_finished(),                 ui_library, 	reloading_library_finished());

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
    CONNECT(ui_library, sig_no_focus(),                                      ui_playlist,   setFocus());
    CONNECT(ui_library, sig_import_files(const QStringList&),                library_importer,       psl_import_files(const QStringList&));

	CONNECT(ui_id3_editor, id3_tags_changed(),							library,			refresh());
	CONNECT(ui_id3_editor, id3_tags_changed(const MetaDataList&),		playlist_handler,	psl_id3_tags_changed(const MetaDataList&));
	CONNECT(ui_id3_editor, id3_tags_changed(const MetaDataList&),		player,				psl_id3_tags_changed(const MetaDataList&));

	CONNECT(ui_audioconverter, sig_active(),							playlist_handler, psl_audioconvert_on());
	CONNECT(ui_audioconverter, sig_inactive(),							playlist_handler, psl_audioconvert_off());
	CONNECT(ui_audioconverter, sig_active(),							listen, start_convert());
	CONNECT(ui_audioconverter, sig_inactive(),							listen, end_convert());
	CONNECT(ui_bookmarks, sig_bookmark(quint32),						listen, jump_abs_s(quint32) );

	CONNECT(ui_eq, eq_changed_signal(int, int),                         listen, 	eq_changed(int, int));

    CONNECT(ui_level, sig_right_clicked(int), ui_style_settings, show(int));
    CONNECT(ui_spectrum, sig_right_clicked(int), ui_style_settings, show(int));

	CONNECT(lastfm,	sig_similar_artists_available(const QList<int>&),			playlist_handler,	psl_similar_artists_available(const QList<int>&));
	CONNECT(lastfm,	sig_last_fm_logged_in(bool),								player,				last_fm_logged_in(bool));
	CONNECT(lastfm, sig_track_info_fetched(const MetaData&, bool, bool),		player,				lfm_info_fetched(const MetaData&, bool, bool));

	CONNECT(ui_playlist_chooser, sig_playlist_chosen(int),						playlists,			load_single_playlist(int));
	CONNECT(ui_playlist_chooser, sig_delete_playlist(int),						playlists,			delete_playlist(int));
	CONNECT(ui_playlist_chooser, sig_save_playlist(int),						playlist_handler, 	psl_prepare_playlist_for_save(int));
	CONNECT(ui_playlist_chooser, sig_save_playlist(QString),					playlist_handler, 	psl_prepare_playlist_for_save(QString));
	CONNECT(ui_playlist_chooser, sig_save_playlist_file(QString, bool),			playlist_handler, 	psl_save_playlist(QString, bool));
	CONNECT(ui_playlist_chooser, sig_clear_playlist(),							playlist_handler, 	psl_clear_playlist());
	CONNECT(ui_playlist_chooser, sig_files_selected(const QStringList &),		playlist_handler,	psl_createPlaylist(const QStringList&));

	CONNECT(playlists, sig_single_playlist_loaded(const CustomPlaylist&),       playlist_handler, 	psl_createPlaylist(const CustomPlaylist&));
	CONNECT(playlists, sig_all_playlists_loaded(const QMap<int, QString>&),     ui_playlist_chooser,all_playlists_fetched(const QMap<int, QString>&));
	CONNECT(ui_stream, sig_create_playlist(const MetaDataList&, bool),			playlist_handler, 	psl_createPlaylist(const MetaDataList&, bool));
	CONNECT(ui_stream, sig_play_track(int, qint32, bool),                       playlist_handler,   psl_change_track(int, qint32, bool));

	CONNECT(ui_podcasts, sig_create_playlist(const MetaDataList&, bool),        playlist_handler, 	psl_createPlaylist(const MetaDataList&, bool));
	CONNECT(ui_podcasts, sig_play_track(int, qint32, bool),                     playlist_handler,   psl_change_track(int, qint32, bool));

    CONNECT (ui_style_settings, sig_style_update(),             ui_spectrum, psl_style_update());
    CONNECT (ui_style_settings, sig_style_update(),             ui_level, psl_style_update());

	CONNECT (stream_server, sig_new_connection_request(const QString&),	ui_broadcast,	new_connection_request(const QString&));
	CONNECT (stream_server, sig_new_connection(const QString&),			ui_broadcast,	new_connection(const QString&));
	CONNECT (stream_server, sig_connection_closed(const QString&),		ui_broadcast,	connection_closed(const QString&));
	CONNECT (listen, destroyed(),										stream_server,	stop());

	CONNECT (playlist_handler, sig_selected_file_changed_md(const MetaData&),	stream_server,		update_track(const MetaData&));
	CONNECT (ui_broadcast, sig_dismiss(int),									stream_server,	dismiss(int));
	CONNECT (ui_broadcast, sig_accepted(),										stream_server,	accept_client());
	CONNECT (ui_broadcast, sig_rejected(),										stream_server,	reject_client());
	CONNECT (listen, sig_data(uchar*, quint64),									stream_server,	new_data(uchar*, quint64));

    qDebug() << "connections done";
}


void Application::setFiles2Play(const QStringList& filelist) {

    if(filelist.size() > 0) {
        playlist_handler->psl_createPlaylist(filelist);
    }
}


QString Application::getVersion() {

    ifstream istr;
    QString version_file = Helper::getSharePath() + "VERSION";

    istr.open(version_file.toUtf8()  );
    if(!istr || !istr.is_open() ) return "0.3.1";

    QMap<QString, int> map;

    while(istr.good()) {
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

QMainWindow* Application::getMainWindow() {
    return this->player;
}
