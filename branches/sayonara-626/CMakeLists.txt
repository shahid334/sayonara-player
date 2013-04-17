CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(linux_packages/resources)


IF(${UNIX})
        execute_process(COMMAND sh ./scripts/setversion.sh)
ENDIF(${UNIX})

SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
SET(CMAKE_INSTALL_PREFIX "/usr")

INSTALL(FILES "empty.db" DESTINATION share/sayonara)
INSTALL(FILES "VERSION" DESTINATION share/sayonara)

IF(${UNIX})
	execute_process(COMMAND update-menus)
ENDIF(${UNIX})



