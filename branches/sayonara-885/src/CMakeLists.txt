
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(GLIB glib-2.0)
PKG_CHECK_MODULES(GOBJECT gobject-2.0)
PKG_CHECK_MODULES(TAGLIB REQUIRED taglib>=1.6)
PKG_CHECK_MODULES(CURL REQUIRED libcurl)


SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
#SET(CMAKE_BUILD_TYPE "Release")
#SET(CMAKE_C_FLAGS "-Wall -O2 -s")
#SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -O2 -s -std=c++11")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -std=c++11 -Wunreachable-code")
SET(CMAKE_C_FLAGS "-Wall")

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
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

ADD_DEFINITIONS(${QT_DEFINITIONS})

LINK_DIRECTORIES(${TAGLIB_LIBRARY_DIRS})
LINK_DIRECTORIES(${CURL_LIBRARY_DIRS})
LINK_DIRECTORIES(${QJSON_LIBRARY_DIRS})
LINK_DIRECTORIES(${GSTREAMER_LIBRARY_DIRS})


ADD_SUBDIRECTORY(HelperStructs)
ADD_SUBDIRECTORY(DatabaseAccess)
ADD_SUBDIRECTORY(GUI)
ADD_SUBDIRECTORY(playlist)
ADD_SUBDIRECTORY(library)
ADD_SUBDIRECTORY(CoverLookup)
ADD_SUBDIRECTORY(Engine/GStreamer)
ADD_SUBDIRECTORY(Notification/libnotify)
ADD_SUBDIRECTORY(languages)

SET(HEADERS
    application.h
    Engine/Engine.h
    Engine/SoundPluginLoader.h
    Notification/Notification.h
    Notification/NotificationPluginLoader.h
    PlayerPlugin/PlayerPlugin.h
    PlayerPlugin/PlayerPluginHandler.h
    playlists/Playlists.h
    StreamPlugins/LastFM/LastFM.h
    StreamPlugins/LastFM/LFMTrackChangedThread.h
    StreamPlugins/LastFM/LFMLoginThread.h
    Socket/Socket.h
    #Soundcloud/SoundcloudLibrary.h
#    Soundcloud/SoundcloudHelper.h
)

SET(SOURCES
    Main.cpp
    application.cpp
    Engine/SoundPluginLoader.cpp
    Notification/NotificationPluginLoader.cpp
    PlayerPlugin/PlayerPlugin.cpp
    PlayerPlugin/PlayerPluginHandler.cpp
    LyricLookup/LyricLookup.cpp
    playlists/Playlists.cpp
    StreamPlugins/LastFM/LastFM.cpp
    StreamPlugins/LastFM/LFMTrackChangedThread.cpp
    StreamPlugins/LastFM/LFMWebAccess.cpp
    StreamPlugins/LastFM/LFMLoginThread.cpp
    Socket/Socket.cpp
    HelperStructs/SmartComparison.cpp
#    Soundcloud/SoundcloudLibrary.cpp
#    Soundcloud/SoundcloudHelper.cpp
)


FILE (GLOB TRANSLATIONS_FILES languages/*.ts)
QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(TRANSLATIONS_TARGET DEPENDS ${QM_FILES})

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_EXECUTABLE(sayonara ${SOURCES} ${HEADERS_MOC} ${QM_FILES})

TARGET_LINK_LIBRARIES(sayonara ${QT_LIBRARIES} 
#	tag 
	curl 
	sayonara_helper 
	sayonara_db 
	sayonara_gui 
	sayonara_playlist 
	sayonara_library 
	sayonara_covers 
#	sayonara_gstreamer
)



INSTALL(TARGETS sayonara DESTINATION bin)
INSTALL(FILES ${QM_FILES} DESTINATION share/sayonara/translations)

