CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

SET(LOGOS logos/)

INSTALL(DIRECTORY ${LOGOS} DESTINATION /usr/share/icons/hicolor FILES_MATCHING
    PATTERN "*.png"
    PATTERN "*.svn" EXCLUDE)

INSTALL(FILES "sayonara" DESTINATION /usr/share/menu)
INSTALL(FILES "sayonara.desktop" DESTINATION /usr/share/applications)



