
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)


INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(GSTREAMER gstreamer-0.10)
PKG_CHECK_MODULES(GSTREAMER_BASE gstreamer-base-0.10)
PKG_CHECK_MODULES(GLIB glib-2.0)
PKG_CHECK_MODULES(TAGLIB REQUIRED taglib>=1.6)
PKG_CHECK_MODULES(GOBJECT gobject-2.0)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
SET(CMAKE_C_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall")
SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(EXECUTABLE_OUTPUT_PATH lib)
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "Engine/GStreamer")


FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${TAGLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GSTREAMER_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})


ADD_DEFINITIONS(${QT_DEFINITIONS})

LINK_DIRECTORIES(${TAGLIB_LIBRARY_DIRS})
LINK_DIRECTORIES(${QJSON_LIBRARY_DIRS})
LINK_DIRECTORIES(${GSTREAMER_LIBRARY_DIRS})


ADD_SUBDIRECTORY(Engine/GStreamer)

SET(HEADERS
	Engine/Engine.h
	Engine/SoundPluginLoader.h
	GUI/player/GUI_Simpleplayer.h
	GUI/player/GUI_TrayIcon.h 
    GUI/playlist/GUI_Playlist.h
    GUI/playlist/PlaylistItemModel.h
    GUI/playlist/PlaylistItemDelegate.h
    GUI/playlist/PlaylistItemDelegateSmall.h
    GUI/playlist/PlaylistItemDelegateInterface.h
    GUI/playlist/GUI_PlaylistEntry.h
    GUI/playlist/GUI_PlaylistEntrySmall.h
    GUI/playlist_chooser/GUI_PlaylistChooser.h
    GUI/LastFM/GUI_LastFM.h
    GUI/library/GUI_Library_windowed.h
    GUI/library/LibraryItemModelTracks.h
    GUI/library/LibraryItemModelAlbums.h
    GUI/library/LibraryItemDelegateAlbums.h
    GUI/library/LibraryItemDelegateArtists.h
    GUI/library/LibraryItemModelArtists.h
    GUI/library/LibraryItemDelegateTracks.h
    GUI/tagedit/GUI_TagEdit.h
    GUI/equalizer/GUI_Equalizer.h
    GUI/MyTableView.h
    GUI/MyListView.h
    GUI/library/GUIImportFolder.h
    GUI/LyricMenuButton/LyricMenuButton.h
    HelperStructs/MetaData.h
    HelperStructs/Helper.h
    HelperStructs/PlaylistMode.h
    HelperStructs/Style.h
    HelperStructs/CDirectoryReader.h
    HelperStructs/PlaylistParser.h
    HelperStructs/WebAccess.h
    HelperStructs/globals.h
    CoverLookup/CoverLookup.h
    CoverLookup/CoverFetchThread.h
    CoverLookup/CoverDownloader.h
    LyricLookup/LyricLookup.h
    LyricLookup/LyricServer.h
    DatabaseAccess/CDatabaseConnector.h
    library/CLibraryBase.h
    library/ReloadThread.h
    playlist/Playlist.h
    playlists/Playlists.h
    HelperStructs/id3.h
    HelperStructs/CSettingsStorage.h
    HelperStructs/Equalizer_presets.h
    StreamPlugins/LFMGlobals.h
    StreamPlugins/LastFM/LastFM.h
    StreamPlugins/LastFM/LFMWebAccess.h
    StreamPlugins/LastFM/LFMTrackChangedThread.h
    GUI/LFMRadio/GUI_LFMRadioWidget.h
    GUI/stream/GUI_Stream.h
    GUI/StreamRecorder/GUI_StreamRecorder.h
    Socket/Socket.h
)

SET(SOURCES
	Engine/SoundPluginLoader.cpp
	GUI/player/GUI_Simpleplayer.cpp
	GUI/player/GUI_TrayIcon.cpp
    GUI/playlist/GUI_Playlist.cpp
    GUI/playlist/PlaylistItemModel.cpp
    GUI/playlist/PlaylistItemDelegate.cpp
    GUI/playlist/PlaylistItemDelegateSmall.cpp
    GUI/playlist/GUI_PlaylistEntry.cpp
    GUI/playlist/GUI_PlaylistEntrySmall.cpp
    GUI/playlist_chooser/GUI_PlaylistChooser.cpp
    GUI/LastFM/GUI_LastFM.cpp
    GUI/library/GUI_Library_windowed.cpp
    GUI/library/LibraryItemModelTracks.cpp
    GUI/library/LibraryItemModelAlbums.cpp
    GUI/library/LibraryItemDelegateAlbums.cpp
    GUI/library/LibraryItemDelegateArtists.cpp
    GUI/library/LibraryItemModelArtists.cpp
    GUI/library/LibraryItemDelegateTracks.cpp
    GUI/tagedit/GUI_TagEdit.cpp
    GUI/equalizer/GUI_Equalizer.cpp
    GUI/MyTableView.cpp
    GUI/MyListView.cpp
    GUI/library/GUIImportFolder.cpp
    GUI/LyricMenuButton/LyricMenuButton.cpp
    HelperStructs/Helper.cpp
    HelperStructs/CSettingsStorage.cpp
    HelperStructs/Style.cpp
    HelperStructs/CDirectoryReader.cpp
    HelperStructs/PlaylistParser.cpp
    HelperStructs/WebAccess.cpp
    CoverLookup/CoverLookup.cpp
    LyricLookup/LyricLookup.cpp
    CoverLookup/CoverFetchThread.cpp
    library/CLibraryBase.cpp
    library/ReloadThread.cpp
    DatabaseAccess/CDatabaseConnector.cpp
    DatabaseAccess/CDatabaseLibrary.cpp
    DatabaseAccess/CDatabaseSettings.cpp
    DatabaseAccess/CDatabaseAlbums.cpp
    DatabaseAccess/CDatabaseArtists.cpp
    DatabaseAccess/CDatabaseTracks.cpp
    DatabaseAccess/CDatabasePlaylist.cpp
    DatabaseAccess/CDatabaseStreams.cpp
    playlist/Playlist.cpp
    playlists/Playlists.cpp
    HelperStructs/id3.cpp
    StreamPlugins/LastFM/LastFM.cpp
    StreamPlugins/LastFM/LFMTrackChangedThread.cpp
    StreamPlugins/LastFM/LFMWebAccess.cpp
    GUI/LFMRadio/GUI_LFMRadioWidget.cpp
    GUI/stream/GUI_Stream.cpp
    GUI/StreamRecorder/GUI_StreamRecorder.cpp
    Socket/Socket.cpp
    Main.cpp
)

SET(FORMS
    GUI/player/GUI_Simpleplayer.ui
    GUI/playlist/GUI_Playlist.ui
    GUI/playlist/GUI_Playlist_Entry.ui
    GUI/playlist/GUI_Playlist_EntrySmall.ui
    GUI/LastFM/GUI_LastFM_Widget.ui
    GUI/library/GUI_Library_windowed.ui
    GUI/library/GUI_ImportFolder.ui
    GUI/equalizer/GUI_Equalizer.ui
    GUI/tagedit/GUI_TagEdit.ui
    GUI/alternate_covers/GUI_Alternate_Covers.ui
    GUI/playlist_chooser/GUI_PlaylistChooser.ui
    GUI/LFMRadio/GUI_LFMRadioWidget.ui
    GUI/stream/GUI_Stream.ui
    GUI/StreamRecorder/GUI_StreamRecorder.ui
)

SET(ICONS 
	GUI/icons/append.png
	GUI/icons/append_small.png
	GUI/icons/broom.png
	GUI/icons/bwd.png
	GUI/icons/bwd_orange.png
	GUI/icons/close.png
	GUI/icons/fwd.png
	GUI/icons/fwd_orange.png
	GUI/icons/pause.png
	GUI/icons/play.png
	GUI/icons/rec.png
	GUI/icons/rep1.png
	GUI/icons/repAll.png
	GUI/icons/save.png
	GUI/icons/save_as.png
	GUI/icons/shuffle.png
	GUI/icons/stop.png
	GUI/icons/vol_1.png
	GUI/icons/vol_1_dark.png
	GUI/icons/vol_2.png
	GUI/icons/vol_2_dark.png
	GUI/icons/vol_3.png
	GUI/icons/vol_3_dark.png
	GUI/icons/vol_mute.png
	GUI/icons/gui.jpg
	GUI/icons/play_small.png
	GUI/icons/lastfm_logo.jpg
	GUI/icons/lastfm_red_small.png
	GUI/icons/power_on.png
	GUI/icons/power_off.png
	GUI/icons/edit.png
	GUI/icons/reload.png
	GUI/icons/dynamic.png
	GUI/icons/arrow_up.png
	GUI/icons/arrow_down.png
	GUI/icons/arrow_right.png
	GUI/icons/arrow_left.png
	GUI/icons/delete.png
	GUI/icons/import.png
	GUI/icons/lyrics.png
)


QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_EXECUTABLE(sayonara ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES(sayonara ${QT_LIBRARIES} tag curl)

SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
SET(CMAKE_INSTALL_PREFIX "/usr")
INSTALL(TARGETS sayonara DESTINATION bin)
INSTALL(FILES "empty.db" DESTINATION share/sayonara)
INSTALL(FILES ${ICONS} DESTINATION share/sayonara)
