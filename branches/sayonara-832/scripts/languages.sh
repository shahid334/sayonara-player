#!/bin/bash

if [ $# -eq 0 ] ; then
	echo "$0 update | new <iso-639-1>"
	exit
fi



if [ "$1" = "update" ] ; then
	STR=""
	for f in ./src/languages/*.ts ; do

		lupdate . -ts ./$f
		STR="$STR ./$f"
	done
	
#	linguist $STR	

elif [ "$1" = "new" ] ; then
	lupdate . -ts ./src/languages/sayonara_lang_$2.ts

fi
		
