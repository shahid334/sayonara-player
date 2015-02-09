PROJECT(sayonara_library)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/library)
FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(HEADERS
	AbstractLibrary.h
    LocalLibrary.h
    LibraryImporter.h
    threads/ReloadThread.h
    threads/ImportCachingThread.h
    threads/ImportCopyThread.h
)

SET(SOURCES
	AbstractLibrary.cpp
    LocalLibrary.cpp
    LibraryImporter.cpp
    threads/ReloadThread.cpp
    threads/ImportCachingThread.cpp
	threads/ImportCopyThread.cpp
)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

ADD_LIBRARY(sayonara_library STATIC ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(sayonara_library
	sayonara_helper
	sayonara_gui
)


