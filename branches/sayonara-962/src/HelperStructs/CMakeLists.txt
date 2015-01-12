PROJECT(sayonara_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(QT_USE_QTXML TRUE)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/HelperStructs)
FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(HEADERS
    Tagging/ID3_Fileheader.h
    AsyncWebAccess.h
    MetaDataInfo.h
	SayonaraClass.h
)

SET(SOURCES
    MetaDataInfo.cpp
    ArtistInfo.cpp
    AlbumInfo.cpp
    MetaData.cpp
    Artist.cpp
    Album.cpp
    Helper.cpp
    Style.cpp
    CDirectoryReader.cpp
    PlaylistParser.cpp
    WebAccess.cpp
    AsyncWebAccess.cpp
    SmartComparison.cpp
    PodcastParser/PodcastParser.cpp
    Tagging/id3.cpp
    Tagging/id3access.cpp
    Tagging/ID3_Fileheader.cpp

)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_helper STATIC ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(sayonara_helper
	curl
	tag
	sayonara_db
)



