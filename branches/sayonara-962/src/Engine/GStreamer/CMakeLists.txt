PROJECT(sayonara_gstreamer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

include(FindPkgConfig)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

FIND_PACKAGE(Qt4 REQUIRED)

PKG_CHECK_MODULES(GSTREAMER gstreamer-1.0)
PKG_CHECK_MODULES(GSTREAMER_BASE gstreamer-base-1.0)
PKG_CHECK_MODULES(GSTREAMER_BASE_APP gstreamer-app-1.0)

IF(GSTREAMER_FOUND AND GSTREAMER_BASE_FOUND AND GSTREAMER_BASE_APP_FOUND)
    MESSAGE("Will use Gstreamer ${GSTREAMER_VERSION}")
ELSE()
    MESSAGE("*******************************************")
    MESSAGE("*** Will use Gstreamer 0.10 as fallback ***")
    MESSAGE("*******************************************")

    SET(GSTREAMER_VERSION "")
    SET(GSTREAMER "")
    SET(GSTREAMER_BASE "")
    SET(GSTREAMER_BASE_APP "")
    SET(GSTREAMER_FOUND 0)
    SET(GSTREAMER_BASE_FOUND 0)
    SET(GSTREAMER_BASE_APP_FOUND 0)
    PKG_CHECK_MODULES(GSTREAMER REQUIRED gstreamer-0.10)
    PKG_CHECK_MODULES(GSTREAMER_BASE REQUIRED gstreamer-plugins-base-0.10)
    PKG_CHECK_MODULES(GSTREAMER_BASE_APP REQUIRED gstreamer-app-0.10)

    IF(GSTREAMER_FOUND AND GSTREAMER_BASE_FOUND AND GSTREAMER_BASE_APP_FOUND)
        MESSAGE("Will use Gstreamer ${GSTREAMER_VERSION}")
    ELSE()
        MESSAGE("Please install the Gstreamer 1.0 dev packages (base, base app, and core)")
        MESSAGE("... or at least the 0.10")
        MESSAGE(FATAL_ERROR "")
    ENDIF()
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${GSTREAMER_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GOBJECT_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GSTREAMER_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GSTREAMER_BASE_INCLUDE_DIRS})

SET(HEADERS
        GSTPipeline.h
        GSTConvertPipeline.h
        GSTConvertEngine.h
        GSTConvertPipeline.h
        GSTConvertEngine.h

        GSTPlaybackPipeline.h
        GSTPlaybackEngine.h
        GSTEngineHandler.h
        GSTEngineHelper.h

        StreamRipperBufferThread.h
        StreamRecorder.h
)

SET(SOURCES
        GSTPipeline.cpp

        GSTConvertPipeline.cpp
        GSTConvertEngine.cpp

        GSTPlaybackPipeline.cpp
        GSTPlaybackEngine.cpp
        GSTEngineHandler.cpp

        GSTEngineCallbacks.cpp

        StreamRipperBufferThread.cpp
        StreamRecorder.cpp
)

	
QT4_WRAP_CPP(HEADERS_MOC
	${HEADERS}
)

INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS} -std=c++11)

ADD_LIBRARY(sayonara_gstreamer SHARED ${SOURCES} ${HEADERS_MOC})
#TARGET_LINK_LIBRARIES(sayonara_gstreamer ${QT_LIBRARIES} gstreamer-1.0 gstbase-1.0 gstapp-1.0)

TARGET_LINK_LIBRARIES(sayonara_gstreamer 
	${GSTREAMER_LIBRARIES}
	${GSTREAMER_BASE_LIBRARIES}
    ${GSTREAMER_BASE_APP_LIBRARIES}
)

INSTALL(TARGETS sayonara_gstreamer DESTINATION lib/sayonara)
