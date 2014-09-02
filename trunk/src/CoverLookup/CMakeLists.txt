PROJECT(sayonara_covers)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(QT_USE_QTXML TRUE)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/CoverLookup)

FIND_PACKAGE(Qt4 REQUIRED)

SET(HEADERS
    CoverLookup.h
    CoverLookupAlternative.h
    CoverLookupAll.h
    CoverFetchThread.h
)

SET(SOURCES
    CoverDownloader.cpp
    CoverLookup.cpp
    CoverLookupAlternative.cpp
    CoverLookupAll.cpp
    CoverFetchThread.cpp
    CoverLocation.cpp

)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_covers ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(sayonara_covers
	sayonara_helper
	sayonara_db
)

