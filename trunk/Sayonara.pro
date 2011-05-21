######################################################################
# Automatically generated by qmake (2.01a) Wed Mar 9 22:54:06 2011
######################################################################

QT += network
QT += xml
QT += sql
TEMPLATE = app
TARGET = sayonara
#DESTDIR = /usr/bin
DEPENDPATH += . GUI MP3_Listen HelperStructs CoverLookup
INCLUDEPATH += . GUI MP3_Listen HelperStructs CoverLookup
LIBS += -L/usr/lib -lphonon -lcurl -ltag -lclastfm 

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
    GUI/library/LibraryItemModelArtists.h \
    GUI/library/LibrarySetupWidget.h \
    GUI/equalizer/GUI_Equalizer.h \
    HelperStructs/MetaData.h \
    HelperStructs/Helper.h \
    CoverLookup/CoverLookup.h \
    library/CDatabaseConnector.h \
    library/CDirectoryReader.h \
    library/CLibraryBase.h \
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
    GUI/library/LibraryItemModelArtists.cpp \
    GUI/library/LibrarySetupWidget.cpp \
    GUI/equalizer/GUI_Equalizer.cpp \
    HelperStructs/Helper.cpp \
    CoverLookup/CoverLookup.cpp \
    library/CDirectoryReader.cpp \
    library/CLibraryBase.cpp \
    library/CDatabaseConnector.cpp \
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
    GUI/library/GUI_LibrarySetup_Widget.ui \
    GUI/equalizer/GUI_Equalizer.ui

RESOURCES +=


target.path = /usr/bin
target.files += ./sayonara
INSTALLS += target

images.path = ~/.Sayonara/images
images.files += GUI/icons/append.png
images.files += GUI/icons/broom.png
images.files += GUI/icons/bwd.png
images.files += GUI/icons/close.png
images.files += GUI/icons/fwd.png
images.files += GUI/icons/pause.png
images.files += GUI/icons/play.png
images.files += GUI/icons/rec.png
images.files += GUI/icons/rep1.png
images.files += GUI/icons/repAll.png
images.files += GUI/icons/save.png
images.files += GUI/icons/shuffle.png
images.files += GUI/icons/stop.png
images.files += GUI/icons/vol_1.png
images.files += GUI/icons/vol_1_dark.png
images.files += GUI/icons/vol_2.png
images.files += GUI/icons/vol_2_dark.png
images.files += GUI/icons/vol_3.png
images.files += GUI/icons/vol_3_dark.png
images.files += GUI/icons/vol_mute.png
images.files += GUI/icons/gui.jpg
images.files += GUI/icons/play_small.png
images.files += GUI/icons/lastfm_logo.jpg
images.files += GUI/icons/power_on.png
images.files += GUI/icons/power_off.png
INSTALLS += images
