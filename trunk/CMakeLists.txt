#VERSION 0.6.2

CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

project("Sayonara Player")
SET(VAR_SAYONARA_VERSION "0.6.4")
ADD_DEFINITIONS( -DSAYONARA_VERSION=\"${VAR_SAYONARA_VERSION}\" )
ADD_SUBDIRECTORY(src)

IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX "/usr")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)

MESSAGE("Create Sayonara v${VAR_SAYONARA_VERSION}")
MESSAGE("Will install to ${CMAKE_INSTALL_PREFIX}")
MESSAGE("To change this, use the -DCMAKE_INSTALL_PREFIX parameter")

SET(CMAKE_SKIP_RPATH:BOOL ON) 
SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")

FILE(WRITE linux_packages/resources/sayonara "?package(sayonara): \\
  needs=\"X11\" \\
  section=\"Applications/Sound\" \\
  title=\"Sayonara\" \\
  command=\"${CMAKE_INSTALL_PREFIX}/bin/sayonara\" \\
  icon=\"${CMAKE_INSTALL_PREFIX}/share/sayonara/sayonara.xpm\""
)

INSTALL(FILES "empty.db" DESTINATION share/sayonara)
INSTALL(FILES "linux_packages/resources/sayonara.desktop" DESTINATION share/applications)
INSTALL(FILES "linux_packages/resources/sayonara" DESTINATION share/menu)
INSTALL(DIRECTORY "linux_packages/resources/logos/" DESTINATION share/icons/hicolor)

IF(${UNIX})
	execute_process(COMMAND update-menus)
ENDIF(${UNIX})



