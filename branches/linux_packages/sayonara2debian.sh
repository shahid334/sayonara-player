#!/bin/bash

cd ..

sh setversion.sh

STR_MAJOR=`grep -e "^MAJOR" ./VERSION`
STR_MINOR=`grep -e "^MINOR" ./VERSION`
STR_SUBMINOR=`grep -e "^SUBMINOR" ./VERSION`
STR_BUILD=`grep -e "^BUILD" ./VERSION`

VER_MAJOR=${STR_MAJOR#[A-Z]*[" "]}
VER_MINOR=${STR_MINOR#[A-Z]*[" "]}
VER_SUBMINOR=${STR_SUBMINOR#[A-Z]*[" "]}
VER_BUILD=${STR_BUILD#[A-Z]*[" "]}

ARCH=`uname -m`


cd linux_packages

OUTPUT_FILE="sayonara-${VER_MAJOR}.${VER_MINOR}.${VER_SUBMINOR}-r${VER_BUILD}-${ARCH}.deb"

CONTROL_FILE=./resources/sayonara32.control
DEBIAN_DIR=sayonara.debian
CUR_DIR=$PWD

if [ "$1" = "clean" ] ; then
	rm -rf $DEBIAN_DIR
	exit
fi

if [ "$1" = "-ax64" ] ; then
	CONTROL_FILE=./resources/sayonara64.control
fi
	

mkdir -p $DEBIAN_DIR/DEBIAN
mkdir -p $DEBIAN_DIR/usr/share/sayonara
mkdir -p $DEBIAN_DIR/usr/share/applications
mkdir -p $DEBIAN_DIR/usr/bin
mkdir -p $DEBIAN_DIR/usr/lib/sayonara

cd ..
make all

if [ $? -ne 0 ] ; then
	echo "BUILD WAS NOT SUCCESSFUL!"
	exit
fi

cd $CUR_DIR

cp ../bin/sayonara $DEBIAN_DIR/usr/bin
cp ../GUI/icons/* $DEBIAN_DIR/usr/share/sayonara
cp ../VERSION ${DEBIAN_DIR}/usr/share/sayonara
cp ../empty.db $DEBIAN_DIR/usr/share/sayonara/player.db
cp ../empty.db $DEBIAN_DIR/usr/share/sayonara/empty.db
cp ./resources/sayonara.postinst $DEBIAN_DIR
cp ./resources/sayonara.prerm $DEBIAN_DIR
cp $CONTROL_FILE $DEBIAN_DIR/DEBIAN/control
cp ./resources/sayonara.desktop $DEBIAN_DIR/usr/share/applications
cp ../Engine/GStreamer/libsayonara_gstreamer.so $DEBIAN_DIR/usr/lib/sayonara

dpkg-deb -b $DEBIAN_DIR $DEBIAN_DIR/$OUTPUT_FILE
echo Written to $DEBIAN_DIR/$OUTPUT_FILE

