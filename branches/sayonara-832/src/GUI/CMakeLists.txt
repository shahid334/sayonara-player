PROJECT(sayonara_gui)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

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

    SearchableWidget.h
    MiniSearcher.h
    RatingLabel.h
    AudioConverter/GUI_AudioConverter.h
    engine/GUI_LevelPainter.h
    engine/GUI_Spectrum.h
    engine/EngineColorStyleChooser.h
    engine/GUI_StyleSettings.h
    player/GUI_Player.h
    player/GUI_TrayIcon.h
    player/SearchSlider.h
    playlist/GUI_Playlist.h
    playlist/view/PlaylistView.h
    playlist/model/PlaylistItemModel.h
    playlist/delegate/PlaylistItemDelegate.h
    playlist/entry/GUI_PlaylistEntry.h
    playlist/entry/GUI_PlaylistEntryBig.h
    playlist/entry/GUI_PlaylistEntrySmall.h
    playlist_chooser/GUI_PlaylistChooser.h
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h
    LastFM/GUI_LastFM.h
    LastFM/GUI_LFMRadioWidget.h
    library/GUI_Library_windowed.h
    library/view/LibraryView.h    
    library/model/LibraryItemModel.h
    library/model/LibraryItemModelTracks.h
    library/model/LibraryItemModelAlbums.h
    library/model/LibraryItemModelArtists.h
    library/delegate/LibraryRatingDelegate.h
    library/delegate/LibraryItemDelegateAlbums.h
    library/delegate/LibraryItemDelegateArtists.h
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

    SearchableWidget.cpp
    MiniSearcher.cpp
    RatingLabel.cpp
    AudioConverter/GUI_AudioConverter.cpp
    engine/GUI_LevelPainter.cpp
    engine/GUI_Spectrum.cpp
    engine/EngineColorStyleChooser.cpp
    engine/GUI_StyleSettings.cpp
    player/GUI_Player.cpp
    player/GUI_PlayerPlugins.cpp
    player/GUI_PlayerMenubar.cpp
    player/GUI_PlayerControls.cpp
    player/GUI_PlayerCover.cpp
    player/GUI_PlayerConnections.cpp
    player/GUI_TrayIcon.cpp
    player/SearchSlider.cpp
    playlist/GUI_Playlist.cpp
    playlist/view/PlaylistView.cpp
    playlist/model/PlaylistItemModel.cpp
    playlist/delegate/PlaylistItemDelegate.cpp
    playlist/entry/GUI_PlaylistEntryBig.cpp
    playlist/entry/GUI_PlaylistEntrySmall.cpp
    playlist_chooser/GUI_PlaylistChooser.cpp
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.cpp
    LastFM/GUI_LastFM.cpp
    LastFM/GUI_LFMRadioWidget.cpp
    library/GUI_Library_windowed.cpp
    library/view/LibraryView.cpp
    library/view/LibraryViewHeader.cpp
    library/model/LibraryItemModel.cpp
    library/model/LibraryItemModelTracks.cpp
    library/model/LibraryItemModelAlbums.cpp
    library/model/LibraryItemModelArtists.cpp
    library/delegate/LibraryRatingDelegate.cpp
    library/delegate/LibraryItemDelegateAlbums.cpp
    library/delegate/LibraryItemDelegateArtists.cpp
    library/delegate/LibraryItemDelegateTracks.cpp
    library/InfoBox/GUILibraryInfoBox.cpp
    library/ImportFolderDialog/GUIImportFolder.cpp
    library/GUI_LibraryPath.cpp
    alternate_covers/AlternateCoverItemDelegate.cpp
    alternate_covers/AlternateCoverItemModel.cpp
    alternate_covers/GUI_Alternate_Covers.cpp
    tagedit/GUI_TagEdit.cpp
    equalizer/GUI_Equalizer.cpp
    Podcasts/GUI_Podcasts.cpp
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
    AudioConverter/GUI_AudioConvert.ui
    engine/GUI_LevelPainter.ui
    engine/GUI_Spectrum.ui
    engine/GUI_Style.ui
    player/GUI_Player.ui
    playlist/GUI_Playlist.ui
    playlist/entry/GUI_PlaylistEntryBig.ui
    playlist/entry/GUI_PlaylistEntrySmall.ui
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
INSTALL(FILES "standard.css" DESTINATION share/sayonara)
INSTALL(FILES "dark.css" DESTINATION share/sayonara)

