PROJECT(sayonara_libnotify)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

include(FindPkgConfig)

SET(CMAKE_C_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall")

pkg_check_modules(LIBNOTIFY libnotify)
MESSAGE(${LIBNOTIFY_VERSION})

if (LIBNOTIFY_FOUND)

    INCLUDE_DIRECTORIES("../..")
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
    INCLUDE_DIRECTORIES(${GLIB_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${GOBJECT_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${LIBNOTIFY_INCLUDE_DIRS})

    LINK_DIRECTORIES(${LIBNOTIFY_LIBRARY_DIRS})

    FIND_PACKAGE(Qt4 REQUIRED)

    if( ${LIBNOTIFY_VERSION_MINOR} LESS 7 )
	
	execute_process(COMMAND sh ./Notification/libnotify/write_version.sh LIBNOTIFY_OLD)
	
    endif( ${LIBNOTIFY_VERSION_MINOR} LESS 7 )

    SET(HEADERS
            ../../HelperStructs/CSettingsStorage.h
            ../Notification.h
            LN_Notification.h
            LN_Version.h
    )
    SET(SOURCES
       	    ../../HelperStructs/CSettingsStorage.cpp
            LN_Notification.cpp
            ../../HelperStructs/Helper.cpp
    )


    QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})

    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})

    ADD_LIBRARY(sayonara_libnotify SHARED ${SOURCES} ${HEADERS_MOC})
    TARGET_LINK_LIBRARIES(sayonara_libnotify ${QT_LIBRARIES} notify)

    SET(CMAKE_INSTALL_PREFIX "/usr")
    INSTALL(TARGETS sayonara_libnotify DESTINATION lib/sayonara)

else ()
    MESSAGE("")
    MESSAGE("**********************************************")
    MESSAGE("*         Libnotify was not found!           *")
    MESSAGE("*  Please install dev packages of libnotify  *")
    MESSAGE("**********************************************")
    MESSAGE("")

endif (LIBNOTIFY_FOUND)
