######################################################################
# Automatically generated by qmake (2.01a) Wed Mar 9 22:54:06 2011
######################################################################

QT += network
QT += xml
QT += sql
TEMPLATE = app
TARGET = sayonara

DEPENDPATH += . GUI MP3_Listen HelperStructs CoverLookup
INCLUDEPATH += . GUI MP3_Listen HelperStructs CoverLookup
LIBS += -L/usr/lib -lphonon -lcurl -ltag

# Input
HEADERS += MP3_Listen/MP3_Listen.h \
	GUI/GUI_Simpleplayer.h \ 
    GUI/playlist/GUI_Playlist.h \
    GUI/playlist/PlaylistItemModel.h \
    GUI/playlist/PlaylistItemDelegate.h \
    GUI/playlist/GUI_PlaylistEntry.h \
    GUI/LastFM/GUI_LastFM.h \
    GUI/library/GUI_Library_windowed.h \
    GUI/library/LibraryItemModelTracks.h \
    GUI/library/LibraryItemModelAlbums.h \
    GUI/library/LibraryItemDelegateAlbums.h \
    GUI/library/LibraryItemDelegateArtists.h \
    GUI/library/LibraryItemModelArtists.h \
    GUI/tagedit/GUI_TagEdit.h \
    GUI/equalizer/GUI_Equalizer.h \
    HelperStructs/MetaData.h \
    HelperStructs/Helper.h \
    CoverLookup/CoverLookup.h \
    #LyricLookup/LyricLookup.h \
    DatabaseAccess/CDatabaseConnector.h \
    library/CDirectoryReader.h \
    library/CLibraryBase.h \
    library/ReloadThread.h \
    playlist/Playlist.h \
    HelperStructs/id3.h \
    HelperStructs/CSettingsStorage.h \
    HelperStructs/Equalizer_presets.h \
    LastFM/LastFM.h
    

SOURCES += Main.cpp \
	MP3_Listen/MP3_Listen.cpp \
    GUI/GUI_Simpleplayer.cpp \
    GUI/playlist/GUI_Playlist.cpp \
    GUI/playlist/PlaylistItemModel.cpp \
    GUI/playlist/PlaylistItemDelegate.cpp \
    GUI/playlist/GUI_PlaylistEntry.cpp \
    GUI/LastFM/GUI_LastFM.cpp \
    GUI/library/GUI_Library_windowed.cpp \
    GUI/library/LibraryItemModelTracks.cpp \
    GUI/library/LibraryItemModelAlbums.cpp \
    GUI/library/LibraryItemDelegateAlbums.cpp \
    GUI/library/LibraryItemDelegateArtists.cpp \
    GUI/library/LibraryItemModelArtists.cpp \
    GUI/tagedit/GUI_TagEdit.cpp \
    GUI/equalizer/GUI_Equalizer.cpp \
    HelperStructs/Helper.cpp \
    CoverLookup/CoverLookup.cpp \
   # LyricLookup/LyricLookup.cpp \
    library/CDirectoryReader.cpp \
    library/CLibraryBase.cpp \
    library/ReloadThread.cpp \
    DatabaseAccess/CDatabaseConnector.cpp \
    DatabaseAccess/CDatabaseLibrary.cpp \
    DatabaseAccess/CDatabaseSettings.cpp \
    DatabaseAccess/CDatabaseAlbums.cpp \
    DatabaseAccess/CDatabaseArtists.cpp \
    DatabaseAccess/CDatabaseTracks.cpp \
    playlist/Playlist.cpp \
    HelperStructs/id3.cpp \
    LastFM/LastFM.cpp \
    HelperStructs/CSettingsStorage.cpp
   

FORMS += \
    GUI/GUI_Simpleplayer.ui \
    GUI/playlist/GUI_Playlist.ui \
    GUI/playlist/GUI_Playlist_Entry.ui \
    GUI/LastFM/GUI_LastFM_Widget.ui \
    GUI/library/GUI_Library_windowed.ui \
    GUI/equalizer/GUI_Equalizer.ui \
    GUI/tagedit/GUI_TagEdit.ui



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
additional_files.files += empty.db
INSTALLS += additional_files


target.path = /usr/bin
target.files += ./sayonara
INSTALLS += target



