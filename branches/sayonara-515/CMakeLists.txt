
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)


INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(GSTREAMER gstreamer-0.10)
PKG_CHECK_MODULES(GSTREAMER_BASE gstreamer-base-0.10)
PKG_CHECK_MODULES(GLIB glib-2.0)
PKG_CHECK_MODULES(GOBJECT gobject-2.0)

PKG_CHECK_MODULES(TAGLIB REQUIRED taglib>=1.6)
PKG_CHECK_MODULES(CURL REQUIRED libcurl)


SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
SET(CMAKE_C_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall")
SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)
SET(EXECUTABLE_OUTPUT_PATH lib)
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "Engine/GStreamer")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "Notification/libnotify")


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

SET(HEADERS
    application.h
    Engine/Engine.h
    Engine/SoundPluginLoader.h
    Notification/Notification.h
    Notification/NotificationPluginLoader.h
    
    CoverLookup/CoverLookup.h
    DatabaseAccess/CDatabaseConnector.h

    PlayerPlugin/PlayerPlugin.h
    PlayerPlugin/PlayerPluginHandler.h
    library/CLibraryBase.h
    library/ReloadThread.h
    playlist/Playlist.h
    playlists/Playlists.h
    StreamPlugins/LastFM/LastFM.h
    StreamPlugins/LastFM/LFMTrackChangedThread.h
    StreamPlugins/LastFM/LFMLoginThread.h
    Socket/Socket.h
    HelperStructs/CSettingsStorage.h
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
    HelperStructs/PodcastParser/PodcastParser.cpp
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
    DatabaseAccess/CDatabasePodcasts.cpp
    playlist/Playlist.cpp
    playlist/Playlist_GUI_slots.cpp
    playlists/Playlists.cpp
    HelperStructs/Tagging/id3.cpp
    HelperStructs/Tagging/id3access.cpp
    StreamPlugins/LastFM/LastFM.cpp
    StreamPlugins/LastFM/LFMTrackChangedThread.cpp
    StreamPlugins/LastFM/LFMWebAccess.cpp
    StreamPlugins/LastFM/LFMLoginThread.cpp
    Socket/Socket.cpp
    HelperStructs/CSettingsStorage.cpp
)



QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

IF(${UNIX})
	execute_process(COMMAND sh ./setversion.sh)
ENDIF(${UNIX})

ADD_EXECUTABLE(sayonara ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(sayonara ${QT_LIBRARIES} tag curl sayonara_gui)
SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
SET(CMAKE_INSTALL_PREFIX "/usr")
INSTALL(TARGETS sayonara DESTINATION bin)
INSTALL(FILES "empty.db" DESTINATION share/sayonara)
INSTALL(FILES "VERSION" DESTINATION share/sayonara)
INSTALL(FILES "linux_packages/resources/sayonara" DESTINATION share/menu)
INSTALL(FILES "linux_packages/resources/sayonara.desktop" DESTINATION share/applications)

IF(${UNIX})
	execute_process(COMMAND update-menus)
	execute_process(COMMAND sh ./setversion.sh)
ENDIF(${UNIX})



