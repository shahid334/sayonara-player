#!/bin/bash

svn update

REV=`svn info | grep Revision | tail -c 4`
TARGET_FILENAME="sayonara-player-r${REV}"
TARGET_PATH="/tmp/${TARGET_FILENAME}"


echo "Checking out..."
svn export "https://sayonara-player.googlecode.com/svn/trunk" ${TARGET_PATH} 

echo "Creating archive"

CUR_PATH=$PWD
cd /tmp
tar czvf "${CUR_PATH}/${TARGET_FILENAME}.tar.gz" ${TARGET_FILENAME}
cd $CUR_PATH

echo "Removing tmp files"

rm -rf ${TARGET_PATH}
if [ -f "${TARGET_FILENAME}.tar.gz" ] ; then
	echo "Written to ${TARGET_FILENAME}.tar.gz"
else 
	echo "Could not write ${TARGET_FILENAME}.tar.gz"
fi

