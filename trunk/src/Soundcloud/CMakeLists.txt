PROJECT(sayonara_helper)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(QT_USE_QTXML TRUE)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CAMKE_SOURCE_DIR}/src/Soundcloud)
FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})

SET(HEADERS
	SoundcloudData.h
	SoundcloudHelper.h
	SoundcloudLibrary.h
)

SET(SOURCES
	SoundcloudData.cpp
	SoundcloudHelper.cpp
	SoundcloudLibrary.cpp
)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_soundcloud STATIC ${SOURCES} ${HEADERS_MOC})

TARGET_LINK_LIBRARIES(sayonara_soundcloud
	sayonara_library
)



