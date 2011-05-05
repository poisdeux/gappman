#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]
then
	echo "usage: $0 <WIDTH> <HEIGHT>"
	exit 1
fi

[ ! -x ./tests/rungappman.sh ] && echo "Error: script must be executed from package toplevel directory" && exit 1

./appmanager/gappman --width $1 --height $2 --conffile xml-config-files/conf.xml \
    --gtkrc gtk-config/gtkrc --windowed
[ $? -ne 0 ] && echo "Error: gappman failed to start" && exit 1
