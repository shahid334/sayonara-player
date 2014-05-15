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

#include <QObject>


#define CONNECT(a,b,c,d) app->connect(a, SIGNAL(b), c, SLOT(d))

#include <QApplication>
#include <QStringList>
#include <QMainWindow>
#include <QTranslator>

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
#include "GUI/engine/GUI_LevelPainter.h"
#include "GUI/engine/GUI_Spectrum.h"
#include "GUI/engine/GUI_StyleSettings.h"
#include "GUI/AudioConverter/GUI_AudioConverter.h"
#include "GUI/bookmarks/GUI_Bookmarks.h"
#include "GUI/speed/GUI_Speed.h"

#include "playlist/PlaylistHandler.h"
#include "playlist/PlaylistLoader.h"
#include "Engine/Engine.h"
#include "Engine/SoundPluginLoader.h"

#include "StreamPlugins/LastFM/LastFM.h"
#include "library/CLibraryBase.h"
#include "library/LibraryImporter.h"
#include "Shoutcast/ShoutcastLibrary.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Equalizer_presets.h"
#include "HelperStructs/CSettingsStorage.h"
#include "HelperStructs/Style.h"
#include "HelperStructs/globals.h"
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "PlayerPlugin/PlayerPluginHandler.h"
#include "Socket/Socket.h"



class Application : public QApplication
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
	GUI_PlaylistChooser*    ui_playlist_chooser;
	GUI_AudioConverter*     ui_audioconverter;
	Playlists*              playlists;
	PlaylistHandler*        playlist_handler;
	PlaylistLoader*         playlist_loader;
	CLibraryBase*           library;
	LibraryImporter*        library_importer;
	LastFM*                 lastfm;
	ShoutcastLibrary*		shoutcast_library;

	GUI_LevelPainter*       ui_level;
	GUI_Spectrum*           ui_spectrum;
	GUI_LastFM*             ui_lastfm;
	GUI_Stream	*           ui_stream;
	GUI_Podcasts*           ui_podcasts;
	GUI_Equalizer*          ui_eq;
	GUI_LFMRadioWidget*     ui_lfm_radio;
	GUI_Bookmarks*          ui_bookmarks;
	GUI_Speed*				ui_speed;
	PlayerPluginHandler*    _pph;

	GUI_StyleSettings*      ui_style_settings;
	GUI_StreamRecorder*     ui_stream_rec;
	GUI_TagEdit*            ui_id3_editor;
	GUI_InfoDialog*         ui_info_dialog;
	GUI_Library_windowed*   ui_library;
	GUI_Playlist*           ui_playlist;
	GUI_SocketSetup*        ui_socket_setup;
	Socket*                 remote_socket;


	SoundPluginLoader*      engine_plugin_loader;
	Engine*                 listen;

	CSettingsStorage*       set;
	SettingsThread*         _setting_thread;
	QApplication*           app;

	bool                    _initialized;
	QTranslator*            _translator;

    void init_connections();
    void connect_languages();

public:
    void setFiles2Play(QStringList filelist);
    QMainWindow* getMainWindow();
    bool is_initialized();
	void init(int n_files, QTranslator* translator);

private:
    QString getVersion();

private slots:
	void focus_changed(QWidget*, QWidget*);

};




class ApplicationClient : public QObject {

	Q_OBJECT

	public:

		explicit ApplicationClient(QObject* parent, const Application* app) :
			QObject(parent),
			sayonara(app)
		{
			connect(app, SIGNAL(doConnections()), this, SIGNAL(initConnections()));
			connect(app, SIGNAL(connectionsDone()), this, SIGNAL(initRemainder()));
		}

		virtual ~ApplicationClient(){}

	private slots:

		virtual void initConnections()=0;
		virtual void initRemainder()=0;

	private:
		const Application* sayonara;

};



#endif // APPLICATION_H


