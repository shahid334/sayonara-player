CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
project("Sayonara Player")
ADD_SUBDIRECTORY(src)
#ADD_SUBDIRECTORY(linux_packages/resources)


IF(${UNIX})
        execute_process(COMMAND sh ${CMAKE_BINARY_DIR}/scripts/setversion.sh ${CMAKE_BINARY_DIR})
ENDIF(${UNIX})

IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX "/usr")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)

MESSAGE("Will install to ${CMAKE_INSTALL_PREFIX}")
MESSAGE("To change this, use the -DCMAKE_INSTALL_PREFIX parameter")

#SET(CMAKE_BUILD_TYPE "Release")
#SET(CMAKE_C_FLAGS "-Wall -O2 -s")
#SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -O2 -s -std=c++0x")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -std=c++11 -Wunreachable-code")
SET(CMAKE_C_FLAGS "-Wall")

ADD_DEFINITIONS(-std=c++11)

SET(CMAKE_SKIP_RPATH:BOOL ON) 
SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")

SET(CPACK_GENERATOR "RPM")
SET(CPACK_PACKAGE_SUMMARY "Audio player and library organizer")
SET(CPACK_PACKAGE_NAME "sayonara")
SET(CPACK_PACKAGE_VERSION "0.6")
SET(CPACK_PACKAGE_RELEASE "1")
SET(CPACK_PACKAGE_VENDOR "Lucio Carreras")
SET(CPACK_PACKAGE_CONTACT "LucioCarreras@gmail.com")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")


SET(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_FILE_NAME})
SET(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
SET(CPACK_RPM_PACKAGE_ARCHITECTURE ${CPACK_SYSTEM_PROCESSOR})
SET(CPACK_RPM_PACKAGE_GROUP "Applications/Multimedia")
SET(CPACK_RPM_PACKAGE_SUMMARY "Audio player and library organizer")
SET(CPACK_RPM_PACKAGE_DESCRIPTION "Description: A small and lightweight music player.
  Its main focus is on managing libraries and playing sound out of it.
  It contains a music library, custom playlists, dynamic playlists,
  ID3 Tag Editor, LastFM scrobbling, LastFM radio, web streaming, equalizer,
  lyrics, stream recorder and podcasts"
)

#SET(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST 
#	/usr/bin
#	/usr/lib
#	/usr/share
#)



SET(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
INCLUDE(CPack)

INSTALL(FILES "empty.db" DESTINATION share/sayonara)
INSTALL(FILES "VERSION" DESTINATION share/sayonara)
INSTALL(FILES "linux_packages/resources/sayonara.desktop" DESTINATION share/applications)
INSTALL(DIRECTORY "linux_packages/resources/logos/" DESTINATION share/icons/hicolor)

IF(${UNIX})
	execute_process(COMMAND update-menus)

ENDIF(${UNIX})



