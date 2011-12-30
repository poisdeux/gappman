#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]
then
	echo "usage: $0 <WIDTH> <HEIGHT>"
	exit 1
fi

[ ! -x ./tests/rungappman.sh ] && echo "Error: script must be executed from package toplevel directory as follows:
./tests/rungappman.sh" && exit 1

GTK2_RC_FILES=./gtk-config/gtkrc ./appmanager/gappman --width $1 --height $2 --conffile xml-config-files/conf.xml
[ $? -ne 0 ] && echo "Error: gappman failed to start" && exit 1
