PROJECT(sayonara_gstreamer)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

include(FindPkgConfig)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

FIND_PACKAGE(Qt4 REQUIRED)

PKG_CHECK_MODULES(GSTREAMER REQUIRED gstreamer-1.0)
PKG_CHECK_MODULES(GSTREAMER_BASE REQUIRED gstreamer-base-1.0)
PKG_CHECK_MODULES(GSTREAMER_BASE REQUIRED gstreamer-app-1.0)


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

        ../Engine.h

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

        ../../HelperStructs/Helper.cpp
        ../../HelperStructs/Tagging/id3.cpp
)

	
QT4_WRAP_CPP(HEADERS_MOC
	${HEADERS}
)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY(sayonara_gstreamer SHARED ${SOURCES} ${HEADERS_MOC})
#TARGET_LINK_LIBRARIES(sayonara_gstreamer ${QT_LIBRARIES} gstreamer-1.0 gstbase-1.0 gstapp-1.0)

TARGET_LINK_LIBRARIES(sayonara_gstreamer ${QT_LIBRARIES} gstreamer-1.0 gstbase-1.0)

INSTALL(TARGETS sayonara_gstreamer DESTINATION lib/sayonara)
