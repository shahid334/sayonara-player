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
#include <QMessageBox>
#include "DatabaseAccess/CDatabaseConnector.h"
#include "StreamPlugins/LastFM/LastFM.h"

#include <QMap>
#include <QSharedMemory>

#include <fstream>
#include <string>

//#include "Soundcloud/SoundcloudHelper.h"

bool Application::is_initialized() {
	return _initialized;
}


Application::Application(int & argc, char ** argv) :
	QApplication(argc, argv),
	SayonaraClass()
{
	QFont font = this->font();
	font.setPointSize(8);
	app->setFont(font);
	_initialized = false;
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

	QString version    = getVersion();
	_settings->set(Set::Player_Version, version);

	_dbus				= new DBusHandler(this);


	player              = new GUI_Player(translator);

	playlist_handler    = PlaylistHandler::getInstance();
	library_importer    = new LibraryImporter(getMainWindow(), this);

	lastfm              = LastFM::getInstance();

	ui_stream_rec       = new GUI_StreamRecorder(player->centralWidget());
	tag_edit			= new TagEdit();

	ui_info_dialog      = new GUI_InfoDialog(tag_edit, player->centralWidget());
	ui_socket_setup     = new GUI_SocketSetup(player->centralWidget());

	library             = new LocalLibrary();
	ui_library          = new GUI_Library_windowed(library, ui_info_dialog, player->getParentOfLibrary());
	stream_server       = new StreamServer(this);

	/*sc_library			= new SoundcloudLibrary();
	ui_sc_library		= new GUI_SoundCloudLibrary(sc_library, ui_info_dialog, player->getParentOfLibrary());*/

	ui_playlist         = new GUI_Playlist(ui_info_dialog, player->getParentOfPlaylist());

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
	ui_broadcast		= new GUI_Broadcast(tr("&Broadcast"), stream_server, player->getParentOfPlugin());

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

	PlayManager::getInstance()->pause();

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

	QStringList libraries;
	libraries << tr("Music Library");
	//libraries << tr("Soundcloud Library");
	ui_library->set_lib_chooser(libraries);

	player->setWindowTitle("Sayonara " + version);
	player->setWindowIcon(Helper::getIcon("logo.png"));
	player->setPlaylist(ui_playlist);
	player->setLibrary(ui_library);
	//player->setLibrary(ui_sc_library);
	player->setInfoDialog(ui_info_dialog);
	player->setPlayerPluginHandler(_pph);
	player->show();
	player->ui_loaded();

	if(_settings->get(Set::PL_StartPlaying)){
		PlayManager::getInstance()->play();
	}

	else{
		PlayManager::getInstance()->pause();
	}

	_initialized = true;
}

Application::~Application() {

	CDatabaseConnector::getInstance()->store_settings();

	if(!_initialized) return;

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
	delete ui_playlist_chooser;
	delete player;
	delete _dbus;

	CDatabaseConnector::getInstance()->closeDatabase();
}


void Application::init_connections() {

	CONNECT (player, sig_rec_button_toggled(bool),				listen,				record_button_toggled(bool));
	CONNECT (player, sig_rec_button_toggled(bool),				ui_stream_rec,		record_button_toggled(bool));
	CONNECT (player, sig_reload_library(bool),					library,            psl_reload_library(bool));
	CONNECT (player, sig_import_dir(const QString&),			library_importer,   psl_import_dir(const QString&));
	CONNECT (player, sig_import_files(const QStringList&),		library_importer,   psl_import_files(const QStringList&));
	CONNECT (player, sig_file_selected(const QStringList &),	playlist_handler, 	create_playlist(const QStringList&));
	CONNECT (player, sig_basedir_selected(const QString &),		playlist_handler,   create_playlist(const QString&));
	CONNECT (player, sig_setup_LastFM(),						ui_lastfm,			show_win()); // IND
	CONNECT (player, sig_show_socket(),							ui_socket_setup,	show_win()); // IND


	CONNECT (playlist_handler,		sig_selection_changed(const MetaDataList&),
			 tag_edit,				set_metadata(const MetaDataList&));
	CONNECT (playlist_handler,		sig_selection_changed(const MetaDataList&),
			 ui_info_dialog,		set_metadata(const MetaDataList&));

	CONNECT (listen, sig_scrobble(const MetaData&),                      lastfm,			scrobble(const MetaData&));
	CONNECT (listen, sig_level(float, float),                            ui_level,			set_level(float,float));
	CONNECT (listen, sig_spectrum(QList<float>&),                        ui_spectrum,		set_spectrum(QList<float>&));
	CONNECT (listen, sig_md_changed(const MetaData&),	player,				psl_md_changed(const MetaData&));
	CONNECT (listen, sig_md_changed(const MetaData&),	playlist_handler,	md_changed(const MetaData&));
	CONNECT (listen, sig_md_changed(const MetaData&),	library,			psl_metadata_changed(const MetaData&));

	CONNECT(ui_speed, sig_speed_changed(float),		listen,					psl_set_speed(float) );

	CONNECT(library, sig_all_tracks_loaded(const MetaDataList&),			ui_info_dialog, set_metadata(const MetaDataList&));
	CONNECT(library, sig_track_mime_data_available(const MetaDataList&),	ui_info_dialog, set_metadata(const MetaDataList&));
	CONNECT(library, sig_all_tracks_loaded(const MetaDataList&),			tag_edit, set_metadata(const MetaDataList&));
	CONNECT(library, sig_track_mime_data_available(const MetaDataList&),	tag_edit, set_metadata(const MetaDataList&));

	CONNECT(library_importer, sig_lib_changes_allowed(bool),	player,         psl_reload_library_allowed(bool));
	CONNECT(library_importer, sig_imported(),					library,		refresh());

	CONNECT(ui_library, sig_import_files(const QStringList&),	library_importer, psl_import_files(const QStringList&));

	CONNECT(tag_edit, sig_metadata_changed(const MetaDataList&, const MetaDataList&),		library,			psl_metadata_changed(const MetaDataList&, const MetaDataList&));
	CONNECT(tag_edit, sig_metadata_changed(const MetaDataList&, const MetaDataList&),		playlist_handler,	md_changed(const MetaDataList&, const MetaDataList&));
	CONNECT(tag_edit, sig_metadata_changed(const MetaDataList&, const MetaDataList&),		player,				psl_id3_tags_changed(const MetaDataList&, const MetaDataList&));

	CONNECT(ui_audioconverter, sig_active(),		listen, start_convert());
	CONNECT(ui_audioconverter, sig_inactive(),		listen, end_convert());

	CONNECT(ui_eq, sig_eq_changed(int, int),		listen, eq_changed(int, int));

	CONNECT(lastfm,	sig_similar_artists_available(const QList<int>&),		playlist_handler,	similar_artists_available(const QList<int>&));
	CONNECT(lastfm,	sig_last_fm_logged_in(bool),							player,				last_fm_logged_in(bool));
	CONNECT(lastfm, sig_track_info_fetched(const MetaData&, bool, bool),	player,				lfm_info_fetched(const MetaData&, bool, bool));

	CONNECT (listen, sig_data(uchar*, quint64),		stream_server,	new_data(uchar*, quint64));
	CONNECT (listen, destroyed(),					stream_server,	stop());
}


void Application::setFiles2Play(const QStringList& filelist) {

	if(filelist.size() > 0) {
		playlist_handler->create_playlist(filelist, tr("Library"));
	}
}


QString Application::getVersion() {

	return QString(SAYONARA_VERSION);
}

QMainWindow* Application::getMainWindow() {
	return this->player;
}
