#!/bin/bash

if [ $# -ne 2 ] ; then
	echo "$0 <VERSION> <KEY>"
	exit 1
fi
svn commit
svn update

VERSION=$1
TARGET_FILENAME="sayonara-${VERSION}"
TARGET_PATH="./${TARGET_FILENAME}-ppa"
DEBIAN_DIR=${TARGET_PATH}/debian
APP_DIR=${TARGET_PATH}/applications

echo "Checking out..."
svn export "https://sayonara-player.googlecode.com/svn/trunk" ${TARGET_PATH} 

mkdir $DEBIAN_DIR
mkdir $APP_DIR

cp resources/ppa.control ${DEBIAN_DIR}/control
cp resources/changelog ${DEBIAN_DIR}
cp resources/copyright ${DEBIAN_DIR}
cp resources/rules ${DEBIAN_DIR}
echo 7 > ${DEBIAN_DIR}/compat

cp ../src/GUI/icons/sayonara.xpm ${APP_DIR}

vi ${DEBIAN_DIR}/changelog
grep ${VERSION} ${DEBIAN_DIR}/changelog
RET=$?

if [ $RET -gt 0 ] ; then
	echo "VERSION in changelog is not ${VERSION}"
	exit 1
fi

cd $TARGET_PATH

KEY=$2
debuild -S -k${KEY}
RET=$?

if [ ${RET} -ne 0 ] ; then
        echo "debuild failed"
        exit 1
fi

cd ..

echo "Upload? y/n"
UPLOAD="n"
read UPLOAD

if [ "${UPLOAD}" = "y" ] ; then
	dput ppa:lucioc/sayonara sayonara_${VERSION}_source.changes
else
	echo "Upload command:"
	echo "   dput ppa:lucioc/sayonara sayonara_${VERSION}_source.changes"
fi


