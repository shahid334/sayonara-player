#VERSION 0.6.2

CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

PROJECT("Sayonara Player")

IF(${UNIX})
	EXECUTE_PROCESS(COMMAND sh ${CMAKE_SOURCE_DIR}/scripts/create-revision.sh ${CMAKE_SOURCE_DIR}/revision)
	INCLUDE(${CMAKE_SOURCE_DIR}/revision)
ELSE()
	SET(VAR_SAYONARA_REVISION 0)
ENDIF(${UNIX})

SET(VAR_SAYONARA_VERSION "0.6.2-${VAR_SAYONARA_REVISION}")
ADD_DEFINITIONS( -DSAYONARA_VERSION=\"${VAR_SAYONARA_VERSION}\" )
ADD_SUBDIRECTORY(src)

MESSAGE("Create Sayonara v${VAR_SAYONARA_VERSION}")
MESSAGE("Will install to ${CMAKE_INSTALL_PREFIX}")
MESSAGE("To change this, use the -DCMAKE_INSTALL_PREFIX parameter")

SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")

FILE(WRITE linux_packages/resources/sayonara "?package(sayonara): \\
  needs=\"X11\" \\
  section=\"Applications/Sound\" \\
  title=\"Sayonara\" \\
  command=\"${CMAKE_INSTALL_PREFIX}/bin/sayonara\" \\
  icon=\"${CMAKE_INSTALL_PREFIX}/share/sayonara/sayonara.xpm\""
)

INSTALL(FILES "empty.db" DESTINATION share/sayonara)
SET(CMAKE_SKIP_RPATH:BOOL ON) 
INSTALL(FILES "linux_packages/resources/sayonara.desktop" DESTINATION share/applications)
INSTALL(FILES "license.txt" DESTINATION share/doc/sayonara)
INSTALL(FILES "linux_packages/resources/sayonara" DESTINATION share/menu)
INSTALL(DIRECTORY "linux_packages/resources/logos/" DESTINATION share/icons/hicolor)

IF(${UNIX})
	execute_process(COMMAND update-menus)
ENDIF(${UNIX})



