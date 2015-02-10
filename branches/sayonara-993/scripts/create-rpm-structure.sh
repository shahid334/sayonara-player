#!/bin/bash

if [ $# -lt 1 ] ; then
	echo "$0 <Version>"
	exit
fi

SVNSTATUS=`svn status -q`
if [ ${#SVNSTATUS} -ne 0 ] ; then
	echo "SVN not up to date. Exit"
	exit
fi

FILEPATH=$PWD
DATETIME=`date +%y%m%d`
VERSION=$1

RPM_PATH="rpmbuild"
SPEC_PATH="${RPM_PATH}/SPEC"
SOURCE_PATH="${RPM_PATH}/SOURCE"

rm -rf linux_packages/${RPM_PATH}
cd linux_packages

mkdir ${RPM_PATH}
mkdir ${SPEC_PATH}
mkdir ${SOURCE_PATH}

cp resources/sayonara.spec ${SPEC_PATH}
sed -i "s/<<VERSION>>/${VERSION}/g" ${SPEC_PATH}/sayonara.spec
sed -i "s/<<REVISION>>/${DATETIME}/g" ${SPEC_PATH}/sayonara.spec

vi ${SPEC_PATH}/sayonara.spec

sh ${FILEPATH}/scripts/create-source.sh
mv sayonara-player.tar.gz ${SOURCE_PATH}/sayonara-player-${VERSION}-${DATETIME}


