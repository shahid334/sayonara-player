#!/bin/bash

cd ..
sh scripts/setversion.sh


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

CONTROL_FILE=$1
DEBIAN_DIR=sayonara.debian
rm -rf $DEBIAN_DIR


CUR_DIR=$PWD

if [ "$1" = "clean" ] ; then
	rm -rf $DEBIAN_DIR
	exit
fi
	
chmod +x resources/sayonara.postinst
chmod +x resources/sayonara.prerm

mkdir -p $DEBIAN_DIR/DEBIAN
mkdir -p $DEBIAN_DIR/usr/bin
mkdir -p $DEBIAN_DIR/usr/lib/sayonara
mkdir -p $DEBIAN_DIR/usr/share/sayonara
mkdir -p $DEBIAN_DIR/usr/share/menu
mkdir -p $DEBIAN_DIR/usr/share/pixmaps
mkdir -p $DEBIAN_DIR/usr/share/icons
mkdir -p $DEBIAN_DIR/usr/share/applications
mkdir -p $DEBIAN_DIR/usr/share/doc/sayonara
mkdir -p $DEBIAN_DIR/usr/share/icons/hicolor
mkdir -p $DEBIAN_DIR/usr/share/sayonara/translations

cd ..
make all

if [ $? -ne 0 ] ; then
	echo "BUILD WAS NOT SUCCESSFUL!"
	exit
fi

cd $CUR_DIR

# debian stuff
cp $CONTROL_FILE $DEBIAN_DIR/DEBIAN/control
cp ./resources/sayonara.prerm $DEBIAN_DIR/DEBIAN/prerm
cp ./resources/sayonara.postinst $DEBIAN_DIR/DEBIAN/postinst
cp ./resources/copyright ${DEBIAN_DIR}/usr/share/doc/sayonara/copyright

# linux stuff

cp -r ./resources/logos/*  $DEBIAN_DIR/usr/share/icons/hicolor
find $DEBIAN_DIR/usr/share/icons/hicolor -iname "*svn*" | xargs rm -rf
cp ./resources/sayonara $DEBIAN_DIR/usr/share/menu
cp ../GUI/icons/sayonara.png ${DEBIAN_DIR}/usr/share/pixmaps
cp ../GUI/icons/sayonara.png ${DEBIAN_DIR}/usr/share/icons
cp ../GUI/icons/* $DEBIAN_DIR/usr/share/sayonara
chmod -x $DEBIAN_DIR/usr/share/sayonara/logo.png
cp ../GUI/icons/sayonara.xpm ${DEBIAN_DIR}/usr/share/sayonara/sayonara.xpm


# sayonara stuff
cp ../VERSION ${DEBIAN_DIR}/usr/share/sayonara
cp ../empty.db $DEBIAN_DIR/usr/share/sayonara/player.db
cp ../empty.db $DEBIAN_DIR/usr/share/sayonara/empty.db
cp ../GUI/dark.css $DEBIAN_DIR/usr/share/sayonara/dark.css
cp ../GUI/standard.css $DEBIAN_DIR/usr/share/sayonara/standard.css
cp ./resources/sayonara.desktop $DEBIAN_DIR/usr/share/applications
cp ../*.qm $DEBIAN_DIR/usr/share/sayonara/translations
cp ../languages/icons/* $DEBIAN_DIR/usr/share/sayonara/translations
cp ../Engine/GStreamer/libsayonara_gstreamer.so $DEBIAN_DIR/usr/lib/sayonara
chmod -x $DEBIAN_DIR/usr/lib/sayonara/libsayonara_gstreamer.so
cp ../Notification/libnotify/libsayonara_libnotify.so $DEBIAN_DIR/usr/lib/sayonara
chmod -x $DEBIAN_DIR/usr/lib/sayonara/libsayonara_libnotify.so
cp ../bin/sayonara $DEBIAN_DIR/usr/bin


gzip --best -c resources/changelog > $DEBIAN_DIR/usr/share/doc/sayonara/changelog.gz 

fakeroot chmod -R g-w $DEBIAN_DIR

fakeroot dpkg-deb -b $DEBIAN_DIR $OUTPUT_FILE

echo Written to $OUTPUT_FILE

lintian $OUTPUT_FILE

