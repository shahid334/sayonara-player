#!/bin/bash

DEBIAN_DIR=phonon.debian
CUR_DIR=$PWD

if [ "$1" = "clean" ] ; then
	rm -rf $DEBIAN_DIR
	exit
fi

mkdir -p $DEBIAN_DIR/DEBIAN
mkdir -p $DEBIAN_DIR/usr/lib/sayonara

cd ..
make phonon

if [ $? -ne 0 ] ; then
exit
fi

cd $CUR_DIR

cp ../libsayonara_phonon.so $DEBIAN_DIR/usr/lib
cp ./resources/libsayonara-phonon.prerm $DEBIAN_DIR
cp ./resources/libsayonara-phonon.control $DEBIAN_DIR/DEBIAN/control

dpkg-deb -b $DEBIAN_DIR $DEBIAN_DIR/libsayonara-phonon.deb
echo Written to $DEBIAN_DIR/libsayonara-phonon.deb

