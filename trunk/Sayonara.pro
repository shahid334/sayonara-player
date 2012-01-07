######################################################################
# Automatically generated by qmake (2.01a) Wed Mar 9 22:54:06 2011
######################################################################

QT += network
QT += xml
QT += sql
TEMPLATE = app
TARGET = sayonara
#CXX_FLAGS += -O2
CONFIG += debug



DEPENDPATH += . GUI MP3_Listen HelperStructs CoverLookup
INCLUDEPATH += . GUI MP3_Listen HelperStructs CoverLookup
LIBS += -L/usr/lib -lphonon -lcurl -ltag -fopenmp


# Input
HEADERS += MP3_Listen/MP3_Listen.h \
	GUI/GUI_Simpleplayer.h \ 
    GUI/playlist/GUI_Playlist.h \
    GUI/playlist/PlaylistItemModel.h \
    GUI/playlist/PlaylistItemDelegate.h \
    GUI/playlist/GUI_PlaylistEntry.h \
    GUI/playlist_chooser/GUI_PlaylistChooser.h \
    GUI/LastFM/GUI_LastFM.h \
    GUI/library/GUI_Library_windowed.h \
    GUI/library/LibraryItemModelTracks.h \
    GUI/library/LibraryItemModelAlbums.h \
    GUI/library/LibraryItemDelegateAlbums.h \
    GUI/library/LibraryItemDelegateArtists.h \
    GUI/library/LibraryItemModelArtists.h \
    GUI/library/LibraryItemDelegateTracks.h \
    GUI/tagedit/GUI_TagEdit.h \
    GUI/equalizer/GUI_Equalizer.h \
    GUI/MyTableView.h \
    GUI/MyListview.h \
    GUI/library/GUIImportFolder.h \
    HelperStructs/MetaData.h \
    HelperStructs/Helper.h \
    HelperStructs/PlaylistMode.h \
    HelperStructs/Style.h \
    HelperStructs/CDirectoryReader.h \
    CoverLookup/CoverLookup.h \
    CoverLookup/CoverFetchThread.h \
    CoverLookup/CoverDownloader.h \
    LyricLookup/LyricLookup.h \
    DatabaseAccess/CDatabaseConnector.h \
    library/CLibraryBase.h \
    library/ReloadThread.h \
    playlist/Playlist.h \
    playlists/Playlists.h \
    HelperStructs/id3.h \
    HelperStructs/CSettingsStorage.h \
    HelperStructs/Equalizer_presets.h \
    LastFM/LastFM.h \
    LastFM/LFMWebAccess.h \
    LastFM/LFMSimilarArtistsThread.h  \
    GUI/GUI_TrayIcon.h
    

SOURCES += Main.cpp \
	MP3_Listen/MP3_Listen.cpp \
    GUI/GUI_Simpleplayer.cpp \
    GUI/playlist/GUI_Playlist.cpp \
    GUI/playlist/PlaylistItemModel.cpp \
    GUI/playlist/PlaylistItemDelegate.cpp \
    GUI/playlist/GUI_PlaylistEntry.cpp \
    GUI/playlist_chooser/GUI_PlaylistChooser.cpp \
    GUI/LastFM/GUI_LastFM.cpp \
    GUI/library/GUI_Library_windowed.cpp \
    GUI/library/LibraryItemModelTracks.cpp \
    GUI/library/LibraryItemModelAlbums.cpp \
    GUI/library/LibraryItemDelegateAlbums.cpp \
    GUI/library/LibraryItemDelegateArtists.cpp \
    GUI/library/LibraryItemModelArtists.cpp \
    GUI/library/LibraryItemDelegateTracks.cpp \
    GUI/tagedit/GUI_TagEdit.cpp \
    GUI/equalizer/GUI_Equalizer.cpp \
    GUI/MyTableView.cpp \
    GUI/MyListView.cpp \
    GUI/library/GUIImportFolder.cpp \
    HelperStructs/Helper.cpp \
    HelperStructs/CSettingsStorage.cpp \
    HelperStructs/Style.cpp \
    HelperStructs/CDirectoryReader.cpp \
    CoverLookup/CoverLookup.cpp \
    LyricLookup/LyricLookup.cpp \
    CoverLookup/CoverFetchThread.cpp \
    library/CLibraryBase.cpp \
    library/ReloadThread.cpp \
    DatabaseAccess/CDatabaseConnector.cpp \
    DatabaseAccess/CDatabaseLibrary.cpp \
    DatabaseAccess/CDatabaseSettings.cpp \
    DatabaseAccess/CDatabaseAlbums.cpp \
    DatabaseAccess/CDatabaseArtists.cpp \
    DatabaseAccess/CDatabaseTracks.cpp \
    DatabaseAccess/CdatabasePlaylist.cpp \
    playlist/Playlist.cpp \
    playlists/Playlists.cpp \
    HelperStructs/id3.cpp \
    LastFM/LastFM.cpp \
    LastFM/LFMSimilarArtistsThread.cpp \
    LastFM/LFMWebAccess.cpp \
    GUI/GUI_TrayIcon.cpp
    
    
   

FORMS += \
    GUI/GUI_Simpleplayer.ui \
    GUI/playlist/GUI_Playlist.ui \
    GUI/playlist/GUI_Playlist_Entry.ui \
    GUI/LastFM/GUI_LastFM_Widget.ui \
    GUI/library/GUI_Library_windowed.ui \
    GUI/library/GUI_ImportFolder.ui\
    GUI/equalizer/GUI_Equalizer.ui \
    GUI/tagedit/GUI_TagEdit.ui \
    GUI/alternate_covers/GUI_Alternate_Covers.ui \
    GUI/playlist_chooser/GUI_PlaylistChooser.ui \
    
   
additional_files.path = /usr/share/sayonara
additional_files.files += GUI/icons/append.png
additional_files.files += GUI/icons/broom.png
additional_files.files += GUI/icons/bwd.png
additional_files.files += GUI/icons/close.png
additional_files.files += GUI/icons/fwd.png
additional_files.files += GUI/icons/pause.png
additional_files.files += GUI/icons/play.png
additional_files.files += GUI/icons/rec.png
additional_files.files += GUI/icons/rep1.png
additional_files.files += GUI/icons/repAll.png
additional_files.files += GUI/icons/save.png
additional_files.files += GUI/icons/save_as.png
additional_files.files += GUI/icons/shuffle.png
additional_files.files += GUI/icons/stop.png
additional_files.files += GUI/icons/vol_1.png
additional_files.files += GUI/icons/vol_1_dark.png
additional_files.files += GUI/icons/vol_2.png
additional_files.files += GUI/icons/vol_2_dark.png
additional_files.files += GUI/icons/vol_3.png
additional_files.files += GUI/icons/vol_3_dark.png
additional_files.files += GUI/icons/vol_mute.png
additional_files.files += GUI/icons/gui.jpg
additional_files.files += GUI/icons/play_small.png
additional_files.files += GUI/icons/lastfm_logo.jpg
additional_files.files += GUI/icons/power_on.png
additional_files.files += GUI/icons/power_off.png
additional_files.files += GUI/icons/edit.png
additional_files.files += GUI/icons/reload.png
additional_files.files += GUI/icons/dynamic.png
additional_files.files += GUI/icons/arrow_up.png
additional_files.files += GUI/icons/arrow_down.png
additional_files.files += GUI/icons/arrow_right.png
additional_files.files += GUI/icons/arrow_left.png
additional_files.files += GUI/icons/delete.png
additional_files.files += GUI/icons/import.png

additional_files.files += empty.db

INSTALLS += additional_files


target.path = /usr/bin
target.files += ./sayonara
INSTALLS += target



