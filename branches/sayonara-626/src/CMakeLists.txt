
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)


INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(GSTREAMER gstreamer-0.10)
PKG_CHECK_MODULES(GSTREAMER_BASE gstreamer-base-0.10)
PKG_CHECK_MODULES(GLIB glib-2.0)
PKG_CHECK_MODULES(GOBJECT gobject-2.0)
PKG_CHECK_MODULES(TAGLIB REQUIRED taglib>=1.6)
PKG_CHECK_MODULES(CURL REQUIRED libcurl)


SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_C_FLAGS "-Wall -O3 -s")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -O3 -s")
SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

SET(LANG ${CMAKE_CURRENT_SOURCE_DIR}/languages/sayonara_lang_de.ts)

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${GSTREAMER_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${TAGLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})


ADD_DEFINITIONS(${QT_DEFINITIONS})

LINK_DIRECTORIES(${TAGLIB_LIBRARY_DIRS})
LINK_DIRECTORIES(${CURL_LIBRARY_DIRS})
LINK_DIRECTORIES(${QJSON_LIBRARY_DIRS})
LINK_DIRECTORIES(${GSTREAMER_LIBRARY_DIRS})

ADD_SUBDIRECTORY(GUI)
ADD_SUBDIRECTORY(Engine/GStreamer)
ADD_SUBDIRECTORY(Notification/libnotify)
ADD_SUBDIRECTORY(languages)

SET(HEADERS
    application.h
    Engine/Engine.h
    Engine/SoundPluginLoader.h
    Notification/Notification.h
    Notification/NotificationPluginLoader.h
    CoverLookup/CoverLookup.h
    CoverLookup/CoverFetchThread.h
    DatabaseAccess/CDatabaseConnector.h
    PlayerPlugin/PlayerPlugin.h
    PlayerPlugin/PlayerPluginHandler.h
    library/CLibraryBase.h
    library/threads/ReloadThread.h
    library/threads/ImportCachingThread.h
    library/threads/ImportCopyThread.h
    playlist/Playlist.h
    playlists/Playlists.h
    StreamPlugins/LastFM/LastFM.h
    StreamPlugins/LastFM/LFMTrackChangedThread.h
    StreamPlugins/LastFM/LFMLoginThread.h
    Socket/Socket.h
    HelperStructs/CSettingsStorage.h
    HelperStructs/Tagging/ID3_Fileheader.h
    HelperStructs/AsyncWebAccess.h
)

SET(SOURCES
    Main.cpp
    application.cpp
    Engine/SoundPluginLoader.cpp
    Notification/NotificationPluginLoader.cpp
    PlayerPlugin/PlayerPlugin.cpp
    PlayerPlugin/PlayerPluginHandler.cpp
    HelperStructs/Helper.cpp
    HelperStructs/CSettingsStorage.cpp
    HelperStructs/Style.cpp
    HelperStructs/CDirectoryReader.cpp
    HelperStructs/PlaylistParser.cpp
    HelperStructs/WebAccess.cpp
    HelperStructs/AsyncWebAccess.cpp
    HelperStructs/PodcastParser/PodcastParser.cpp
    HelperStructs/Tagging/id3.cpp
    HelperStructs/Tagging/id3access.cpp
    HelperStructs/Tagging/ID3_Fileheader.cpp
    HelperStructs/CSettingsStorage.cpp
    CoverLookup/CoverLookup.cpp
    CoverLookup/CoverFetchThread.cpp
    LyricLookup/LyricLookup.cpp
    library/CLibraryAdmin.cpp
    library/CLibraryBase.cpp
    library/threads/ReloadThread.cpp

    library/threads/ImportCachingThread.cpp
    library/threads/ImportCopyThread.cpp

    DatabaseAccess/CDatabaseConnector.cpp
    DatabaseAccess/CDatabaseLibrary.cpp
    DatabaseAccess/CDatabaseSettings.cpp
    DatabaseAccess/CDatabaseAlbums.cpp
    DatabaseAccess/CDatabaseArtists.cpp
    DatabaseAccess/CDatabaseTracks.cpp
    DatabaseAccess/CDatabasePlaylist.cpp
    DatabaseAccess/CDatabaseStreams.cpp
    DatabaseAccess/CDatabasePodcasts.cpp
    playlist/Playlist.cpp
    playlist/Playlist_GUI_slots.cpp
    playlists/Playlists.cpp
    StreamPlugins/LastFM/LastFM.cpp
    StreamPlugins/LastFM/LFMTrackChangedThread.cpp
    StreamPlugins/LastFM/LFMWebAccess.cpp
    StreamPlugins/LastFM/LFMLoginThread.cpp
    Socket/Socket.cpp
)


FILE (GLOB TRANSLATIONS_FILES languages/*.ts)
QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(TRANSLATIONS_TARGET DEPENDS ${QM_FILES})

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_EXECUTABLE(sayonara ${SOURCES} ${HEADERS_MOC} ${QM_FILES})
TARGET_LINK_LIBRARIES(sayonara ${QT_LIBRARIES} tag curl sayonara_gui)

SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
SET(CMAKE_INSTALL_PREFIX "/usr")
INSTALL(TARGETS sayonara DESTINATION bin)
INSTALL(FILES ${QM_FILES} DESTINATION share/sayonara/translations)

