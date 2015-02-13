#!/bin/bash

set -e

if [ $# -lt 1 ] ; then
	echo "$0 <Version>"
	exit
fi

SVNSTATUS=`svn status -q`
#if [ ${#SVNSTATUS} -ne 0 ] ; then
#	echo "SVN not up to date. Exit"
#	exit
#fi

export LC_ALL=en_US
FILEPATH=$PWD
DATETIME=`date +%y%m%d`
VERSION=$1
REVISION="1.${DATETIME}"


RPM_PATH="${HOME}/rpmbuild"
SPEC_PATH="${RPM_PATH}/SPEC"
SOURCE_PATH="${RPM_PATH}/SOURCES"

CHANGELOG_DATE=`date +"%a %b %d %Y"`
CHANGELOG_ENTRY="\%changelog\n\* ${CHANGELOG_DATE} Lucio Carreras <luciocarreras@gmail.com> - ${VERSION}-${REVISION}\n- write some changelog here\n"

mkdir -p ${RPM_PATH}
mkdir -p ${SPEC_PATH}
mkdir -p ${SOURCE_PATH}

cp linux_packages/resources/sayonara.spec ${SPEC_PATH}
sed -i "s/<<VERSION>>/${VERSION}/g" ${SPEC_PATH}/sayonara.spec
sed -i "s/<<REVISION>>/${REVISION}/g" ${SPEC_PATH}/sayonara.spec
sed -i "s/<<CHANGELOG>>/${CHANGELOG_ENTRY}/g" ${SPEC_PATH}/sayonara.spec


vi ${SPEC_PATH}/sayonara.spec

sh ${FILEPATH}/scripts/create-source.sh
mv sayonara-player.tar.gz ${SOURCE_PATH}/sayonara-player-${VERSION}-${REVISION}.tar.gz


