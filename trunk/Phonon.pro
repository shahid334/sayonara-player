TEMPLATE	= lib
CONFIG		+= plugin
INCLUDEPATH	+= ./
HEADERS		= MP3_Listen/PhononEngine.h \ 
		MP3_Listen/Engine.h \
		HelperStructs/MetaData.h \
		HelperStructs/id3.h \
		HelperStructs/Equalizer_presets.h

SOURCES		= MP3_Listen/PhononEngine.cpp \
		HelperStructs/Helper.cpp \
		HelperStructs/id3.cpp \

TARGET		= $$qtLibraryTarget(sayonara_phonon)
DESTDIR		= .
LIBS 		+= -lphonon


target.path = /usr/lib/sayonara

INSTALLS += target
