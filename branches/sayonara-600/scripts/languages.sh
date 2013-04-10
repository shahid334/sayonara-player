#!/bin/bash

if [ $# -eq 0 ] ; then
	echo "$0 update | release | new <iso-639-1>"
	exit
fi  


if [ "$1" = "update" ] ; then

	for f in ./languages/*.ts ; do

		lupdate . -ts ./$f

	done	

elif [ "$1" = "new" ] ; then
	lupdate . -ts ./languages/sayonara_lang_$2.ts

elif [ "$1" = "release" ] ; then
	for f in ./languages/*.ts ; do
		BASE=$(basename $f)
		FILENAME="${BASE%.*}"
		lrelease -removeidentical $f -qm languages/${FILENAME}.qm
	done

else
	echo bla


fi
		
