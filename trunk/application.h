/* application.h */

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



#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>


#define CONNECT(a,b,c,d) app->connect(a, SIGNAL(b), c, SLOT(d))

#include <QApplication>
#include <QStringList>
#include <QMainWindow>

#include "GUI/player/GUI_Simpleplayer.h"
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
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "PlayerPlugin/PlayerPluginHandler.h"
#include "Socket/Socket.h"



class CLibraryBase;
class Application : public QObject
{
    Q_OBJECT

public:
    Application( QApplication* qapp, int n_files, QObject *parent = 0);
    virtual ~Application();
    
signals:
    
public slots:

private:
    GUI_SimplePlayer* 		player;
    GUI_PlaylistChooser*	ui_playlist_chooser;
    Playlists*			playlists;
    Playlist* 			playlist;
    CLibraryBase* 		library;
    LastFM*			lastfm;

    GUI_LastFM*			ui_lastfm;
    GUI_Stream	*		ui_stream;
    GUI_Podcasts*               ui_podcasts;
    GUI_Equalizer*		ui_eq;
    GUI_LFMRadioWidget*		ui_lfm_radio;
    PlayerPluginHandler*	_pph;

    GUI_StreamRecorder*		ui_stream_rec;
    GUI_TagEdit*		ui_id3_editor;
    GUI_InfoDialog*		ui_info_dialog;
    GUI_Library_windowed*	ui_library;
    //GUI_Library_Info_Box*	ui_library_info_box;
    GUI_Playlist* 			ui_playlist;
    GUI_SocketSetup*		ui_socket_setup;
    Socket*					remote_socket;


    SoundPluginLoader*          engine_plugin_loader;
    Engine*                     listen;

    CSettingsStorage*       set;
    SettingsThread*           _setting_thread;
    QApplication*           app;

    bool					_initialized;



    void init_connections();

public:
    void setFiles2Play(QStringList filelist);
    QMainWindow* getMainWindow();
    bool is_initialized();


private:
    QString getVersion();

};


#endif // APPLICATION_H

