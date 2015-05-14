PROJECT(sayonara_playlist)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/playlist)

INCLUDE(${QT_USE_FILE})

SET(HEADERS
    StreamPlaylist.h
    PlaylistHandler.h
    PlaylistLoader.h
)

SET(SOURCES
    Playlist.cpp
    StdPlaylist.cpp
    StreamPlaylist.cpp
    PlaylistHandler.cpp
    PlaylistLoader.cpp
    PlaylistDBInterface.cpp
    PlaylistDBConnector.cpp
)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_playlist STATIC ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(sayonara_playlist

    sayonara_helper
    sayonara_db
)



