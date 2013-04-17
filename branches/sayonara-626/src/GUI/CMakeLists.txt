PROJECT(sayonara_gui)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)
SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_C_FLAGS "-Wall -O2 -s")
SET(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wall -O2 -s")

SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}) 
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src) 
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/GUI) 
FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_SUBDIRECTORY(icons)

SET(HEADERS
    player/GUI_Player.h
    player/GUI_TrayIcon.h
    player/SearchSlider.h
    playlist/GUI_Playlist.h
    playlist/PlaylistItemModel.h
    playlist/PlaylistItemDelegate.h
    playlist/PlaylistItemDelegateSmall.h
    playlist/GUI_PlaylistEntry.h
    playlist/GUI_PlaylistEntrySmall.h
    playlist_chooser/GUI_PlaylistChooser.h
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h
    LastFM/GUI_LastFM.h
    LastFM/GUI_LFMRadioWidget.h
    library/GUI_Library_windowed.h
    library/models/LibraryItemModel.h
    library/models/LibraryItemModelTracks.h
    library/models/LibraryItemModelAlbums.h
    library/models/LibraryItemModelArtists.h
    library/delegates/LibraryItemDelegateAlbums.h
    library/delegates/LibraryItemDelegateArtists.h
    library/InfoBox/GUILibraryInfoBox.h
    library/ImportFolderDialog/GUIImportFolder.h
    library/DiscPopupMenu.h
    library/GUI_LibraryPath.h
    alternate_covers/AlternateCoverItemDelegate.h
    alternate_covers/AlternateCoverItemModel.h
    alternate_covers/GUI_Alternate_Covers.h
    tagedit/GUI_TagEdit.h
    equalizer/GUI_Equalizer.h
    Podcasts/GUI_Podcasts.h
    MyColumnHeader.h
    MyTableView.h
    MyListView.h
    ContextMenu.h
    InfoDialog/GUI_InfoDialog.h
    SocketConfiguration/GUISocketSetup.h
    stream/GUI_Stream.h
    StreamRecorder/GUI_StreamRecorder.h
    Notifications/GUI_Notifications.h
    startup_dialog/GUI_Startup_Dialog.h
    LanguageChooser/GUI_LanguageChooser.h

)


SET(SOURCES
    player/GUI_Player.cpp
    player/GUI_PlayerPlugins.cpp
    player/GUI_PlayerMenubar.cpp
    player/GUI_PlayerControls.cpp
    player/GUI_PlayerCover.cpp
    player/GUI_PlayerConnections.cpp
    player/GUI_TrayIcon.cpp
    player/SearchSlider.cpp
    playlist/GUI_Playlist.cpp
    playlist/PlaylistItemModel.cpp
    playlist/PlaylistItemDelegate.cpp
    playlist/PlaylistItemDelegateSmall.cpp
    playlist/GUI_PlaylistEntry.cpp
    playlist/GUI_PlaylistEntrySmall.cpp
    playlist_chooser/GUI_PlaylistChooser.cpp
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.cpp
    LastFM/GUI_LastFM.cpp
    LastFM/GUI_LFMRadioWidget.cpp
    library/GUI_Library_windowed.cpp
    library/models/LibraryItemModel.cpp
    library/models/LibraryItemModelTracks.cpp
    library/models/LibraryItemModelAlbums.cpp
    library/models/LibraryItemModelArtists.cpp
    library/delegates/LibraryItemDelegateAlbums.cpp
    library/delegates/LibraryItemDelegateArtists.cpp
    library/delegates/LibraryItemDelegateTracks.cpp
    library/InfoBox/GUILibraryInfoBox.cpp
    library/ImportFolderDialog/GUIImportFolder.cpp
    library/GUI_LibraryPath.cpp
    alternate_covers/AlternateCoverItemDelegate.cpp
    alternate_covers/AlternateCoverItemModel.cpp
    alternate_covers/GUI_Alternate_Covers.cpp
    tagedit/GUI_TagEdit.cpp
    equalizer/GUI_Equalizer.cpp
    Podcasts/GUI_Podcasts.cpp
    MyTableView.cpp
    MyListView.cpp
    ContextMenu.cpp
    stream/GUI_Stream.cpp
    StreamRecorder/GUI_StreamRecorder.cpp
    Notifications/GUI_Notifications.cpp
    startup_dialog/GUI_Startup_Dialog.cpp
    InfoDialog/GUI_InfoDialog.cpp
    SocketConfiguration/GUISocketSetup.cpp
    LanguageChooser/GUI_LanguageChooser.cpp
)


SET(FORMS
    player/GUI_Player.ui
    playlist/GUI_Playlist.ui
    playlist/GUI_Playlist_Entry.ui
    playlist/GUI_Playlist_EntrySmall.ui
    LastFM/GUI_LastFM_Dialog.ui
    LastFM/GUI_LFMRadioWidget.ui
    library/GUI_Library_windowed.ui
    library/ImportFolderDialog/GUI_ImportFolder.ui
    library/InfoBox/GUI_Library_Info_Box.ui
    library/GUI_LibraryPath.ui
    equalizer/GUI_Equalizer.ui
    tagedit/GUI_TagEdit.ui
    alternate_covers/GUI_Alternate_Covers.ui
    playlist_chooser/GUI_PlaylistChooser.ui
    Podcasts/GUI_Podcasts.ui
    stream/GUI_Stream.ui
    StreamRecorder/GUI_StreamRecorder.ui
    InfoDialog/GUI_InfoDialog.ui
    startup_dialog/GUI_Startup_Dialog.ui
    SocketConfiguration/GUI_SocketSetup.ui
    alternate_covers/GUI_Alternate_Covers.ui
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.ui
    Notifications/GUI_Notifications.ui
    LanguageChooser/GUI_LanguageChooser.ui
)



QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
ADD_LIBRARY(sayonara_gui ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS})
#SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
SET(CMAKE_INSTALL_PREFIX "/usr")
INSTALL(FILES "standard.css" DESTINATION share/sayonara)
INSTALL(FILES "dark.css" DESTINATION share/sayonara)

