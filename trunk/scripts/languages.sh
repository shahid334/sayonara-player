#!/bin/bash

for f in ./languages/* ; do

	lupdate . -ts ./$f

done
