PROJECT(sayonara_db)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(QT_USE_QTSQL TRUE)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/DatabaseAccess)
FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})

SET(HEADERS
	CDatabaseConnector.h

)

SET(SOURCES
    CDatabaseConnector.cpp
    CDatabaseLibrary.cpp
    CDatabaseSettings.cpp
    CDatabaseAlbums.cpp
    CDatabaseArtists.cpp
    CDatabaseTracks.cpp
    CDatabasePlaylist.cpp
    CDatabaseStreams.cpp
    CDatabasePodcasts.cpp
    CDatabaseVisStyles.cpp
    CDatabaseBookmarks.cpp
)
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_db STATIC ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(sayonara_db
	${QT_LIBRARIES}
	sayonara_helper
)

