#!/bin/bash

DIR="/home/indra/Pictures/wallpapers"
PIC=$(ls $DIR/* | shuf -n1)
gsettings set org.gnome.desktop.background picture-url "file://$PIC"