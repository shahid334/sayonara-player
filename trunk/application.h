#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>


#define CONNECT(a,b,c,d) app->connect(a, SIGNAL(b), c, SLOT(d))

#include <QApplication>
#include <QStringList>

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
#include "LyricLookup/LyricLookup.h"
#include "playlists/Playlists.h"
#include "Socket/Socket.h"


class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application( QApplication* qapp, QObject *parent = 0);
    virtual ~Application();
    
signals:
    
public slots:

private:
    GUI_SimplePlayer* 		player;
    GUI_PlaylistChooser*	ui_playlist_chooser;
    Playlists*				playlists;
    Playlist* 				playlist;
    CLibraryBase* 			library;
    LastFM*					lastfm;
    GUI_LastFM*				ui_lastfm;
    GUI_Stream	*			ui_stream;
    GUI_Equalizer*			ui_eq;
    GUI_LFMRadioWidget*		ui_lfm_radio;
    GUI_StreamRecorder*		ui_stream_rec;
    GUI_TagEdit*			ui_id3_editor;
    GUI_InfoDialog*			ui_info_dialog;
    GUI_Library_windowed*	ui_library;
    //GUI_Library_Info_Box*	ui_library_info_box;
    GUI_Playlist* 			ui_playlist;
    GUI_SocketSetup*		ui_socket_setup;
    Socket*					remote_socket;

    SoundPluginLoader*      plugin_loader;
    Engine*                 listen;
    CSettingsStorage*       set;
    QApplication*           app;


    void init_connections();

public:
    void setFiles2Play(QStringList filelist);
};

#endif // APPLICATION_H
