/* application.h */

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

#ifndef APPLICATION_H
#define APPLICATION_H

#define CONNECT(a,b,c,d) app->connect(a, SIGNAL(b), c, SLOT(d))

#include <QApplication>
#include <QMainWindow>
#include <QTranslator>
#include <QFont>

#include "DBus/DBusHandler.h"

#include "GUI/player/GUI_Player.h"
#include "Playlist/PlaylistHandler.h"
#include "GUI/playlist/GUI_Playlist.h"
#include "Library/LocalLibrary.h"
#include "Library/LibraryImporter.h"
#include "PlayerPlugin/PlayerPluginHandler.h"
#include "GUI/LastFM/GUI_LastFM.h"
#include "GUI/playlist_chooser/GUI_PlaylistChooser.h"
#include "GUI/AudioConverter/GUI_AudioConverter.h"
#include "GUI/engine/GUI_LevelPainter.h"
#include "GUI/engine/GUI_Spectrum.h"
#include "GUI/stream/GUI_Stream.h"
#include "GUI/stream/GUI_Podcasts.h"
#include "GUI/equalizer/GUI_Equalizer.h"
#include "GUI/bookmarks/GUI_Bookmarks.h"
#include "GUI/speed/GUI_Speed.h"
#include "GUI/broadcasting/GUI_Broadcast.h"

#include "Soundcloud/SoundcloudLibrary.h"
#include "GUI/soundcloud/GUI_SoundcloudLibrary.h"
#include "StreamPlugins/LastFM/LastFM.h"
#include "GUI/StreamRecorder/GUI_StreamRecorder.h"
#include "GUI/tagedit/GUI_TagEdit.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/library/GUI_Library_windowed.h"

#include "Engine/SoundPluginLoader.h"
#include "Engine/GStreamer/GSTEngineHandler.h"


#include "GUI/SocketConfiguration/GUISocketSetup.h"
#include "GUI/AlternativeCovers/GUI_AlternativeCovers.h"
#include "RadioStation/StreamServer.h"






class Application : public QApplication, private SayonaraClass
{
    Q_OBJECT

public:
	Application(int & argc, char ** argv);
    virtual ~Application();

signals:
	void doConnections();
	void connectionsDone();

public slots:

private:
	GUI_Player*             player;
	PlaylistHandler*        playlist_handler;
	LocalLibrary*           library;
	LibraryImporter*        library_importer;

	PlayerPluginHandler*    _pph;

	LastFM*                 lastfm;
	GUI_LastFM*             ui_lastfm;
	GUI_PlaylistChooser*    ui_playlist_chooser;
	GUI_AudioConverter*     ui_audioconverter;
	GUI_LevelPainter*       ui_level;
	GUI_Spectrum*           ui_spectrum;
	GUI_Stream*				ui_stream;
	GUI_Podcasts*           ui_podcasts;
	GUI_Equalizer*          ui_eq;
	GUI_Bookmarks*          ui_bookmarks;
	GUI_Speed*				ui_speed;
	GUI_Broadcast*			ui_broadcast;


	SoundcloudLibrary*		sc_library;
	GUI_SoundCloudLibrary*  ui_sc_library;

	GUI_StreamRecorder*     ui_stream_rec;
	TagEdit*				tag_edit;
	GUI_InfoDialog*         ui_info_dialog;
	GUI_Library_windowed*   ui_library;

	GUI_Playlist*           ui_playlist;
	GUI_SocketSetup*        ui_socket_setup;
	StreamServer*           stream_server;


	SoundPluginLoader*      engine_plugin_loader;
	Engine*                 listen;

	DBusHandler*			_dbus;



	QApplication*           app;

	bool                    _initialized;
	QTranslator*            _translator;

    void init_connections();

public:
	void setFiles2Play(const QStringList& filelist);
    QMainWindow* getMainWindow();
    bool is_initialized();
	void init(int n_files, QTranslator* translator);

private:
    QString getVersion();
	void check_for_crash();
};

#endif // APPLICATION_H


