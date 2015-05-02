#!/bin/sh

if [ X$1 != X ]
then
	convert $1 -resize 720x480 -gravity center -extent 720x480 -background white rgba:in.rgba
	./greenconv in.rgba
	convert -size 720x480+0+0 -depth 8 rgba:out.rgba -size 720x480 -depth 8 -type truecolor -compress none out.bmp
	gzip -f out.bmp
	rm -f in.rgba out.rgba
	echo
	echo Result is in out.bmp.gz\; Use
	echo
	echo \# dd if=out.bmp.gz of=/dev/\$dev bs=512k seek=1 count=1
	echo
	echo to install into SD card.
	echo
	ls -l out.bmp.gz
else
	echo Need image file as argument
	exit 1
fi
