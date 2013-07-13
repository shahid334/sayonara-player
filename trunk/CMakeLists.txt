CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(linux_packages/resources)

IF(${UNIX})
        execute_process(COMMAND sh ./scripts/setversion.sh)
ENDIF(${UNIX})

SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
SET(CMAKE_INSTALL_PREFIX "/usr")


SET(CPACK_GENERATOR "RPM")
SET(CPACK_PACKAGE_SUMMARY "Audio player and library organizer")
SET(CPACK_PACKAGE_NAME "sayonara")
SET(CPACK_PACKAGE_VERSION "0.4.0")
SET(CPACK_PACKAGE_RELEASE "1")
SET(CPACK_PACKAGE_LICENSE "GPLv3")
SET(CPACK_PACKAGE_GROUP "Applications/Multimedia")
SET(CPACK_PACKAGE_VENDOR "Lucio Carreras")
SET(CPACK_PACKAGE_DESCRIPTION "Description: A small and lightweight music player.
  Its main focus is on managing libraries and playing sound out of it.
  It contains a music library, custom playlists, dynamic playlists,
  ID3 Tag Editor, LastFM scrobbling, LastFM radio, web streaming, equalizer,
  lyrics, stream recorder and podcasts"
)
IF(${CMAKE_SYSTEM_PROCESSOR} EQUAL 64)
	SET(CPACK_RPM_PACKAGE_ARCHITECTURE "x64")
ELSEIF(${CMAKE_SYSTEM_PROCESSOR} EQUAL 32)
	SET(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
ENDIF(${CMAKE_SYSTEM_PROCESSOR} EQUAL 64)

SET(CPACK_PACKAGE_REQUIRES "qt taglib libcurl gstreamer gstreamer-plugins-base libnotify")

SET(CPACK_PACKAGE_CONTACT "LucioCarreras@gmail.com")
SET(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

INCLUDE(CPack)

INSTALL(FILES "empty.db" DESTINATION share/sayonara)
INSTALL(FILES "VERSION" DESTINATION share/sayonara)

IF(${UNIX})
	execute_process(COMMAND update-menus)

ENDIF(${UNIX})



