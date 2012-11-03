#!/bin/bash

MAJOR=0
MINOR=3
SUBMINOR=1
REV=`svn info | grep Revision | tail -c 4`
REV=`expr $REV + 1`

echo "MAJOR ${MAJOR}" > VERSION
echo "MINOR ${MINOR}" >> VERSION
echo "SUBMINOR ${SUBMINOR}" >> VERSION
echo "BUILD ${REV}" >> VERSION

echo "Update to Version ${MAJOR}.${MINOR}.${SUBMINOR}-r${REV}"
