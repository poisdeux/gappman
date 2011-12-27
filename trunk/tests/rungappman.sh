#!/bin/bash

[ ! -x ./tests/rungappman.sh ] && echo "Error: script must be executed from package toplevel directory as follows:
./tests/rungappman.sh" && exit 1

GTK2_RC_FILES=./gtk-config/gtkrc ./appmanager/gappman --width 400 --height 300 --conffile xml-config-files/conf.xml \
    --gtkrc gtk-config/gtkrc --windowed
[ $? -ne 0 ] && echo "Error: gappman failed to start" && exit 1
