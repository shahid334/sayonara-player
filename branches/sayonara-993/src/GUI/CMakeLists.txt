PROJECT(sayonara_gui)
CMAKE_MINIMUM_REQUIRED(VERSION 2.7)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}) 
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src) 

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS} -std=c++11)

ADD_SUBDIRECTORY(icons)

SET(HEADERS
    SearchableWidget.h
    MiniSearcher.h
    AlternativeCovers/AlternativeCoverItemDelegate.h
    AlternativeCovers/AlternativeCoverItemModel.h
    AlternativeCovers/GUI_AlternativeCovers.h
    AudioConverter/GUI_AudioConverter.h
    bookmarks/GUI_Bookmarks.h
    broadcasting/GUI_Broadcast.h
    broadcasting/GUI_BroadcastSetup.h
    engine/EnginePlugin.h
    engine/GUI_LevelPainter.h
    engine/GUI_Spectrum.h
    engine/EngineColorStyleChooser.h
    engine/GUI_StyleSettings.h
    equalizer/EqSlider.h
    equalizer/GUI_Equalizer.h
    LastFM/GUI_LastFM.h
    library/GUI_AbstractLibrary.h
    library/GUI_Library_windowed.h
    library/RatingLabel.h
    library/view/LibraryView.h
    library/view/LibraryViewAlbum.h
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
    player/GUI_Player.h
    player/GUI_TrayIcon.h
    player/SearchSlider.h
    playlist/GUI_Playlist.h
    playlist/PlaylistTabWidget.h
    playlist/view/PlaylistView.h
    playlist/model/PlaylistItemModel.h
    playlist/delegate/PlaylistItemDelegate.h
    playlist/entry/GUI_PlaylistEntry.h
    playlist/entry/GUI_PlaylistEntryBig.h
    playlist/entry/GUI_PlaylistEntrySmall.h
    playlist_chooser/GUI_PlaylistChooser.h
    soundcloud/GUI_SoundcloudLibrary.h
    tagedit/GUI_TagEdit.h
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.h

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
    speed/GUI_Speed.h
)

SET(SOURCES

    SearchableWidget.cpp
    MiniSearcher.cpp

    AlternativeCovers/AlternativeCoverItemDelegate.cpp
    AlternativeCovers/AlternativeCoverItemModel.cpp
    AlternativeCovers/GUI_AlternativeCovers.cpp
    AudioConverter/GUI_AudioConverter.cpp
    bookmarks/GUI_Bookmarks.cpp
    broadcasting/GUI_Broadcast.cpp
    broadcasting/GUI_BroadcastSetup.cpp
    engine/EnginePlugin.cpp
    engine/GUI_LevelPainter.cpp
    engine/GUI_Spectrum.cpp
    engine/EngineColorStyleChooser.cpp
    engine/GUI_StyleSettings.cpp
    equalizer/GUI_Equalizer.cpp
    equalizer/EqSlider.cpp
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
    library/GUI_AbstractLibrary.cpp
    library/RatingLabel.cpp
    library/GUI_Library_windowed.cpp
    library/view/LibraryView.cpp
    library/view/LibraryViewHeader.cpp
    library/view/LibraryViewAlbum.cpp
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
    soundcloud/GUI_SoundcloudLibrary.cpp
    tagedit/GUI_TagEdit.cpp
    Podcasts/GUI_Podcasts.cpp
    ContextMenu.cpp
    stream/GUI_Stream.cpp
    StreamRecorder/GUI_StreamRecorder.cpp
    Notifications/GUI_Notifications.cpp
    startup_dialog/GUI_Startup_Dialog.cpp
    InfoDialog/GUI_InfoDialog.cpp
    SocketConfiguration/GUISocketSetup.cpp
    LanguageChooser/GUI_LanguageChooser.cpp
    speed/GUI_Speed.cpp
)


SET(FORMS
    AlternativeCovers/GUI_AlternativeCovers.ui
    AudioConverter/GUI_AudioConvert.ui
    bookmarks/GUI_Bookmarks.ui
    broadcasting/GUI_Broadcast.ui
    broadcasting/GUI_BroadcastSetup.ui
    engine/GUI_LevelPainter.ui
    engine/GUI_Spectrum.ui
    engine/GUI_Style.ui
    equalizer/GUI_Equalizer.ui
    InfoDialog/GUI_InfoDialog.ui
    LanguageChooser/GUI_LanguageChooser.ui
    LastFM/GUI_LastFM_Dialog.ui
	library/GUI_Library_windowed.ui
    library/ImportFolderDialog/GUI_ImportFolder.ui
    library/InfoBox/GUI_Library_Info_Box.ui
    library/GUI_LibraryPath.ui
    Notifications/GUI_Notifications.ui
    player/GUI_Player.ui
    playlist/GUI_Playlist.ui
    playlist/entry/GUI_PlaylistEntryBig.ui
    playlist/entry/GUI_PlaylistEntrySmall.ui
    playlist_chooser/GUI_PlaylistChooser.ui
	playlist/view/GUI_PlaylistView.ui

    Podcasts/GUI_Podcasts.ui
    SocketConfiguration/GUI_SocketSetup.ui
	soundcloud/GUI_SoundcloudLibrary.ui
    speed/GUI_Speed.ui
    startup_dialog/GUI_Startup_Dialog.ui
    stream/GUI_Stream.ui
    StreamRecorder/GUI_StreamRecorder.ui
    tagedit/GUI_TagEdit.ui
    TargetPlaylistDialog/GUI_Target_Playlist_Dialog.ui
)

SET(RESOURCES
	Icons.qrc
)

QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

ADD_LIBRARY(sayonara_gui STATIC ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS} ${RESOURCES_RCC})
TARGET_LINK_LIBRARIES(sayonara_gui
	sayonara_helper
	sayonara_db
	sayonara_covers
)
#SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
INSTALL(FILES "standard.css" DESTINATION share/sayonara)
INSTALL(FILES "dark.css" DESTINATION share/sayonara)

