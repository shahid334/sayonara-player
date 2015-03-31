PROJECT(sayonara_dbus)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(QT_USE_QTDBUS TRUE)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/DBus)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtDBus REQUIRED)

INCLUDE(${QT_USE_FILE})

SET(HEADERS
	DBusHandler.h
	DBusMPRIS.h
	DBusMediaKeysInterface.h
	DBusMediaKeysInterfaceMate.h
	DBusMediaKeysInterfaceGnome.h
)

SET(SOURCES
	DBusHandler.cpp
	DBusMPRIS.cpp
	DBusMediaKeysInterface.cpp
	DBusMediaKeysInterfaceMate.cpp
	DBusMediaKeysInterfaceGnome.cpp

)

QT4_ADD_DBUS_ADAPTOR(SOURCES
    resources/org.mpris.MediaPlayer2.xml
    DBusMPRIS.h
    DBusMPRIS::MediaPlayer2
    org_mpris_media_player2_adaptor
    OrgMprisMediaPlayer2Adaptor
)

QT4_ADD_DBUS_ADAPTOR(SOURCES
    resources/org.mpris.MediaPlayer2.Player.xml
    DBusMPRIS.h
    DBusMPRIS::MediaPlayer2
    org_mpris_media_player2_player_adaptor
    OrgMprisMediaPlayer2PlayerAdaptor
)


QT4_ADD_DBUS_INTERFACE(SOURCES
	resources/org.mate.SettingsDaemon.MediaKeys.xml
	mate_settings_daemon
)

QT4_ADD_DBUS_INTERFACE(SOURCES
	resources/org.gnome.SettingsDaemon.MediaKeys.xml
	gnome_settings_daemon
)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})


ADD_LIBRARY(sayonara_dbus STATIC ${SOURCES} ${HEADERS_MOC})





