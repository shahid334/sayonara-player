
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

set (CMAKE_MODULE_PATH ".")

set (CPACK_PACKAGE_VERSION_MAJOR 0)
set (CPACK_PACKAGE_VERSION_MINOR 3)
set (CPACK_PACKAGE_VERSION_PATCH 2)
set (CPACK_PACKAGE_VERSION 0.3.2)
set (CPACK_PACKAGE_FILE_NAME sayonara)
set (CPACK_PACKAGE_NAME sayonara)


set (CPACK_RPM_PACKAGE_SUMMARY "Lightweight music player and library manager")
set (CPACK_RPM_PACKAGE_NAME "sayonara")
set (CPACK_RPM_PACKAGE_VERSION "0.3.2")
set (CPACK_RPM_PACKAGE_RELEASE "4")
set (CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set (CPACK_RPM_PACKAGE_GROUP "audio/video")
set (CPACK_RPM_PACKAGE_VENDOR "luciocarreras@gmail.com")
set (CPACK_RPM_PACKAGE_DESCRIPTION Sayonara player is a lightweight music player and library manager. 
It's main focus is on managing libraries and playing sound out of it.
  Features are:
    Music library,
    Custom playlists,
    Dynamic playlists,
    ID3 Tag Editor,
    LastFM scrobbling,
    LastFM radio,
    Web streaming,
    Equalizer,
    Lyrics,
    Streams,
    Streamrecorder)

set (CPACK_RPM_PACKAGE_REQUIRES "libtaglib2.0-cil, libqt4-gui, libqt4-sql-sqlite, libqt4-xml, libqt4-core, libcurl3, gstreamer0.10-ffmpeg, gstreamer0.10-plugins-base, gstreamer0.10-plugins-good, libgstreamer0.10-0, libgstreamer-plugins-base0.10-0")



INCLUDE(CPack)
INCLUDE(UseRPMTools)
IF(RPMTools_FOUND)
	RPMTools_ADD_RPM_TARGETS(sayonara)
ENDIF(RPMTools_FOUND)

