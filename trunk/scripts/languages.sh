#!/bin/bash

if [ $# -eq 0 ] ; then
	echo "$0 update|release"
	exit
fi  


if [ "$1" = "update" ] ; then

	for f in ./languages/*.ts ; do

		lupdate . -ts ./$f

	done	

elif [ "$1" = "release" ] ; then
	for f in ./languages/*.ts ; do
		BASE=$(basename $f)
		FILENAME="${BASE%.*}"
		lrelease -removeidentical $f -qm languages/${FILENAME}.qm
	done

else
	echo bla


fi
		
