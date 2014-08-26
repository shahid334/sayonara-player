PROJECT(sayonara_libnotify)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

include(FindPkgConfig)

SET(CMAKE_C_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -O2 -s")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

PKG_CHECK_MODULES(LIBNOTIFY libnotify)
if (LIBNOTIFY_FOUND)

    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)
    INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${GOBJECT_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${LIBNOTIFY_INCLUDE_DIRS})

    LINK_DIRECTORIES(${LIBNOTIFY_LIBRARY_DIRS})

    FIND_PACKAGE(Qt4 REQUIRED)


    SET(HEADERS
            ${CMAKE_SOURCE_DIR}/src/HelperStructs/CSettingsStorage.h
            ${CMAKE_SOURCE_DIR}/src/Notification/Notification.h
            ${CMAKE_SOURCE_DIR}/src/Notification/libnotify/LN_Notification.h
    )

    SET(SOURCES
            ${CMAKE_SOURCE_DIR}/src/HelperStructs/Helper.cpp
            ${CMAKE_SOURCE_DIR}/src/HelperStructs/CSettingsStorage.cpp
            ${CMAKE_SOURCE_DIR}/src/Notification/libnotify/LN_Notification.cpp
    )

    QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})


    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})

    ADD_LIBRARY(sayonara_libnotify SHARED ${SOURCES} ${HEADERS_MOC})
    TARGET_LINK_LIBRARIES(sayonara_libnotify 
		sayonara_helper
		notify
	)

    INSTALL(TARGETS sayonara_libnotify DESTINATION lib/sayonara)

else ()

    MESSAGE("")
    MESSAGE("**********************************************")
    MESSAGE("*         Libnotify was not found!           *")
    MESSAGE("*  Please install dev packages of libnotify  *")
    MESSAGE("**********************************************")
    MESSAGE("")

endif (LIBNOTIFY_FOUND)
