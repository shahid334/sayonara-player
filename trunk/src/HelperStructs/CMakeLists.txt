PROJECT(sayonara_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(QT_USE_QTXML TRUE)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/HelperStructs)

INCLUDE(${QT_USE_FILE})

SET(HEADERS
    Tagging/ID3_Fileheader.h
    MetaData/MetaDataInfo.h
    Shutdown/Shutdown.h
    WebAccess/AsyncWebAccess.h
    SayonaraClass.h
)

SET(SOURCES
    DirectoryReader/DirectoryReader.cpp
    MetaData/MetaDataInfo.cpp
    MetaData/ArtistInfo.cpp
    MetaData/AlbumInfo.cpp
    MetaData/MetaData.cpp
    MetaData/Artist.cpp
    MetaData/Album.cpp
    Parser/PlaylistParser.cpp
    Parser/PodcastParser.cpp
    Shutdown/Shutdown.cpp
    SmartCompare/SmartCompare.cpp
    Style/Style.cpp
    Tagging/id3.cpp
    Tagging/id3access.cpp
    Tagging/ID3_Fileheader.cpp
    WebAccess/WebAccess.cpp
    WebAccess/AsyncWebAccess.cpp

    Helper.cpp
)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_helper STATIC ${SOURCES} ${HEADERS_MOC})

TARGET_LINK_LIBRARIES(sayonara_helper
	curl
	tag
	sayonara_db
)



