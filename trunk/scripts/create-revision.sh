#!/bin/bash

if [ $# -eq 0 ] ; then
	echo "Usage: $0 <target revision file>"
	exit
fi

if [ -f $1 ] ; then
	echo "Create revision: Nothing to do... "
	exit
fi

SAYONARA_REVISION=`date +%y%m%d`
echo "#${SAYONARA_REVISION}" > $1
echo "SET(VAR_SAYONARA_REVISION ${SAYONARA_REVISION})" >> $1
