TEMPLATE	= lib
CONFIG		+= plugin static
INCLUDEPATH	+= . /usr/include/gstreamer-0.10
HEADERS		= MP3_Listen/GSTEngine.h \ 
		MP3_Listen/Engine.h \
		HelperStructs/MetaData.h \
		HelperStructs/id3.h \
		HelperStructs/Equalizer_presets.h

SOURCES		= MP3_Listen/GSTEngine.cpp \
		HelperStructs/Helper.cpp \
		HelperStructs/id3.cpp

TARGET		= $$qtLibraryTarget(sayonara_gstreamer)
DESTDIR		= .
CONFIG 		+= link_pkgconfig
PKGCONFIG 	+= gstreamer-0.10
LIBS 		+= -L/usr/lib `pkg-config --cflags --libs gstreamer-0.10`

