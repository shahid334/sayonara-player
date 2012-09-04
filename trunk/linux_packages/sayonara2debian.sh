#!/bin/bash

DEBIAN_DIR=sayonara.debian
CUR_DIR=$PWD

if [ "$1" = "clean" ] ; then
	rm -rf $DEBIAN_DIR
	exit
fi

mkdir -p $DEBIAN_DIR/DEBIAN
mkdir -p $DEBIAN_DIR/usr/share/sayonara
mkdir -p $DEBIAN_DIR/usr/share/applications
mkdir -p $DEBIAN_DIR/usr/bin
mkdir -p $DEBIAN_DIR/usr/lib/sayonara

cd ..
make all

if [ $? -ne 0 ] ; then
exit
fi

cd $CUR_DIR

cp ../bin/sayonara $DEBIAN_DIR/usr/bin
cp ../GUI/icons/* $DEBIAN_DIR/usr/share/sayonara
cp ../empty.db $DEBIAN_DIR/usr/share/sayonara/player.db
cp ../empty.db $DEBIAN_DIR/usr/share/sayonara/empty.db
cp ./resources/sayonara.postinst $DEBIAN_DIR
cp ./resources/sayonara.prerm $DEBIAN_DIR
cp ./resources/sayonara.control $DEBIAN_DIR/DEBIAN/control
cp ./resources/sayonara.desktop $DEBIAN_DIR/usr/share/applications
cp ../Engine/GStreamer/libsayonara_gstreamer.so $DEBIAN_DIR/usr/lib/sayonara

dpkg-deb -b $DEBIAN_DIR $DEBIAN_DIR/sayonara-0.3.deb
echo Written to $DEBIAN_DIR/sayonara-0.3.deb

