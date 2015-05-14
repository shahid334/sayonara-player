
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXCompilerFlag)

PKG_CHECK_MODULES(GLIB glib-2.0)
PKG_CHECK_MODULES(GOBJECT gobject-2.0)
PKG_CHECK_MODULES(TAGLIB REQUIRED taglib>=1.6)
PKG_CHECK_MODULES(CURL REQUIRED libcurl)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

IF( NOT CMAKE_BUILD_TYPE ) 
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX_CAN_11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" CXX_CAN_0X)

IF( ${CXX_CAN_11} )
	MESSAGE("Your Compiler supports C++11")
	SET(COMMON_FLAGS "-std=c++11")
ELSEIF( ${CXX_CAN_0X} )
	MESSAGE("Your Compiler supports C++0x")
	SET(COMMON_FLAGS "-std=c++0x")
ELSE()
	FATAL_ERROR("Your Compiler is not C++11 ready!")
ENDIF()


MESSAGE("Build type = ${CMAKE_BUILD_TYPE}")
SET(COMMON_FLAGS "${COMMON_FLAGS} -Woverloaded-virtual -Wall -Wunreachable-code")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMMON_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMMON_FLAGS} -O2 -s")
SET(CMAKE_CXX_FLAGS_NONE "${CMAKE_CXX_FLAGS_RELEASE}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_NONE}")

SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTSVG TRUE)
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTDBUS TRUE)

SET(LANG ${CMAKE_CURRENT_SOURCE_DIR}/languages/sayonara_lang_de.ts)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXML QtDBus QtSvg REQUIRED)


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
ADD_DEFINITIONS(-DLIB_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/lib/sayonara")

LINK_DIRECTORIES(${TAGLIB_LIBRARY_DIRS})
LINK_DIRECTORIES(${CURL_LIBRARY_DIRS})
LINK_DIRECTORIES(${QJSON_LIBRARY_DIRS})
LINK_DIRECTORIES(${GSTREAMER_LIBRARY_DIRS})


ADD_SUBDIRECTORY(HelperStructs)
ADD_SUBDIRECTORY(DatabaseAccess)
ADD_SUBDIRECTORY(GUI)
ADD_SUBDIRECTORY(Playlist)
ADD_SUBDIRECTORY(Library)
ADD_SUBDIRECTORY(CoverLookup)
ADD_SUBDIRECTORY(Engine/GStreamer)
ADD_SUBDIRECTORY(Notification/libnotify)
#ADD_SUBDIRECTORY(Soundcloud)
ADD_SUBDIRECTORY(languages)
ADD_SUBDIRECTORY(DBus)


SET(HEADERS
    application.h
    PlayManager.h
    Settings/SettingNotifier.h
    Settings/SettingKey.h
    Settings/Setting.h
    Settings/Settings.h
    Settings/SettingConverter.h
    Engine/Engine.h
    Engine/SoundPluginLoader.h
    Notification/Notification.h
    Notification/NotificationPluginLoader.h
    PlayerPlugin/PlayerPlugin.h
    PlayerPlugin/PlayerPluginHandler.h
    PlaylistChooser/PlaylistChooser.h
    RadioStation/StreamWriter.h
    RadioStation/StreamServer.h
    StreamPlugins/LastFM/LastFM.h
    StreamPlugins/LastFM/LFMTrackChangedThread.h
    StreamPlugins/LastFM/LFMLoginThread.h
    TagEdit/TagEdit.h
    TagEdit/TagExpression.h
    TagEdit/MetaDataChangeNotifier.h
)

SET(SOURCES
    Main.cpp
    application.cpp
    PlayManager.cpp
    Settings/Settings.cpp
    Settings/Setting.cpp
    Engine/SoundPluginLoader.cpp
    Notification/NotificationPluginLoader.cpp
    PlayerPlugin/PlayerPlugin.cpp
    PlayerPlugin/PlayerPluginHandler.cpp
    LyricLookup/LyricLookup.cpp
    PlaylistChooser/PlaylistChooser.cpp
    StreamPlugins/LastFM/LastFM.cpp
    StreamPlugins/LastFM/LFMTrackChangedThread.cpp
    StreamPlugins/LastFM/LFMWebAccess.cpp
    StreamPlugins/LastFM/LFMLoginThread.cpp
    RadioStation/StreamWriter.cpp
    RadioStation/StreamServer.cpp
    TagEdit/TagEdit.cpp
    TagEdit/TagExpression.cpp
    TagEdit/MetaDataChangeNotifier.cpp
)

FILE (GLOB TRANSLATIONS_FILES languages/*.ts)


QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(TRANSLATIONS_TARGET DEPENDS ${QM_FILES})

ADD_EXECUTABLE(sayonara ${SOURCES} ${HEADERS_MOC} ${QM_FILES})

TARGET_LINK_LIBRARIES(sayonara ${QT_LIBRARIES} 
    curl
    sayonara_helper
    sayonara_db
    sayonara_gui
    sayonara_playlist
    sayonara_library
    sayonara_covers
#   sayonara_soundcloud
    sayonara_dbus
)

INSTALL(TARGETS sayonara DESTINATION bin)
INSTALL(FILES ${QM_FILES} DESTINATION share/sayonara/translations)

