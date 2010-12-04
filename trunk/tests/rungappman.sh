#!/bin/bash

[ ! -x ./tests/rungappman.sh ] && echo "Error: script must be executed from package toplevel directory" && exit 1

./applets/netman/daemon/gm_netmand &
GM_NETMAND_PID=$?
[ $GM_NETMAND_PID -ne 0 ] && echo "Error: gm_netmand failed to start" && exit 1

./appmanager/gappman --width 400 --height 300 --conffile xml-config-files/conf.xml \
    --gtkrc gtk-config/gtkrc --windowed
[ $? -ne 0 ] && echo "Error: gappman failed to start" && exit 1

kill $GM_NETMAND_PID
