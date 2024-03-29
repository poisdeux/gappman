#!/bin/sh
### BEGIN INIT INFO
# Provides:          gappman
# Required-Start:    $local_fs $remote_fs
# Required-Stop:     $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: GAppMan
# Description:      Debian script to start the Graphical APPlication MANager 
### END INIT INFO

# Author: Martijn Brekhof <m.brekhof@gmail.com>

# PATH should only include /usr/* if it runs after the mountnfs.sh script
NAME=gappman
PATH=/sbin:/usr/sbin:/bin:/usr/bin
DAEMON=/usr/bin/gappman
DAEMON_ARGS=""
PIDFILE=/var/run/$NAME.pid
export DISPLAY=:0

# Exit if the package is not installed
[ -x $DAEMON ] || exit 0

# Read configuration variable file if it is present
[ -r /etc/default/$NAME ] && . /etc/default/$NAME

[ "$ENABLE" = "yes" ] || exit 0

export GTK2_RC_FILES
DM_PIDFILE=/var/run/$NAME-DM.pid
WM_PIDFILE=/var/run/$NAME-WM.pid

# Load the VERBOSE setting and other rcS variables
. /lib/init/vars.sh

# Define LSB log_* functions.
# Depend on lsb-base (>= 3.0-6) to ensure that this file is present.
. /lib/lsb/init-functions

#
# Function that starts the daemon/service
#
do_start()
{

	if [ "$START_DM" = "1" ]
	then
		if [ -z $DM_DAEMON ]
		then
			log_daemon_msg "START_X defined but no DM_DAEMON specified in /etc/default/$NAME" "$NAME"
			return 2
		fi
		start-stop-daemon --start --quiet --make-pidfile --pidfile $DM_PIDFILE --exec $DM_DAEMON --test > /dev/null
		if [ $? -eq 0 ]
		then
			start-stop-daemon --background --start --quiet --make-pidfile --pidfile $DM_PIDFILE --exec $DM_DAEMON -- $DISPLAY
		fi
	fi

	if [ -n $WINDOW_MANAGER ]
	then
		start-stop-daemon --start --quiet --make-pidfile --pidfile $WM_PIDFILE --exec $WM_DAEMON --test > /dev/null
    if [ $? -eq 0 ]
    then
      start-stop-daemon --background --start --quiet --make-pidfile --pidfile $WM_PIDFILE --exec $WM_DAEMON
    fi
	fi

	# Return
	#   0 if daemon has been started
	#   1 if daemon was already running
	#   2 if daemon could not be started
	start-stop-daemon --start --quiet --make-pidfile --pidfile $PIDFILE --exec $DAEMON --test > /dev/null \
		|| return 1
	start-stop-daemon --background --start --quiet --make-pidfile --pidfile $PIDFILE --exec $DAEMON -- \
		$DAEMON_ARGS \
		|| return 2
}

#
# Function that stops the daemon/service
#
do_stop()
{
	# Return
	#   0 if daemon has been stopped
	#   1 if daemon was already stopped
	#   2 if daemon could not be stopped
	#   other if a failure occurred
	start-stop-daemon --stop --quiet --retry=TERM/5/KILL/5 --pidfile $PIDFILE --name $NAME
	RETVAL="$?"
	[ "$RETVAL" = 2 ] && return 2
	
	if [ -n $WINDOW_MANAGER ]
	then
    start-stop-daemon --stop --quiet --retry=TERM/5/KILL/5 --pidfile $WM_PIDFILE --exec $WM_DAEMON
	fi

	if [ "$START_DM" = "1" ]
	then
		if [ -z $DM_DAEMON ]
		then
			log_daemon_msg "START_X defined but no DM_DAEMON specified in /etc/default/$NAME" "$NAME"
			return 2
		fi
		start-stop-daemon --stop --quiet --retry=TERM/5/KILL/5 --pidfile $DM_PIDFILE --exec $DM_DAEMON
	fi

	# Wait for children to finish too if this is a daemon that forks
	# and if the daemon is only ever run from this initscript.
	# If the above conditions are not satisfied then add some other code
	# that waits for the process to drop all resources that could be
	# needed by services started subsequently.  A last resort is to
	# sleep for some time.
	start-stop-daemon --stop --quiet --oknodo --retry=0/2/KILL/2 --exec $DAEMON
	[ "$?" = 2 ] && return 2
	# Many daemons don't delete their pidfiles when they exit.
	rm -f $PIDFILE $WM_PIDFILE $DM_PIDFILE
	return "$RETVAL"
}

case "$1" in
  start)
    [ "$VERBOSE" != no ] && log_daemon_msg "Starting $DESC " "$NAME"
    do_start
    case "$?" in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0 ;;
		2) [ "$VERBOSE" != no ] && log_end_msg 1 ;;
	esac
  ;;
  stop)
	[ "$VERBOSE" != no ] && log_daemon_msg "Stopping $DESC" "$NAME"
	do_stop
	case "$?" in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0 ;;
		2) [ "$VERBOSE" != no ] && log_end_msg 1 ;;
	esac
	;;
  status)
       status_of_proc "$DAEMON" "$NAME" && exit 0 || exit $?
       ;;
  restart|force-reload)
	log_daemon_msg "Restarting $DESC" "$NAME"
	do_stop
	case "$?" in
	  0|1)
		do_start
		case "$?" in
			0) log_end_msg 0 ;;
			1) log_end_msg 1 ;; # Old process is still running
			*) log_end_msg 1 ;; # Failed to start
		esac
		;;
	  *)
	  	# Failed to stop
		log_end_msg 1
		;;
	esac
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|status|restart|force-reload}" >&2
	exit 3
	;;
esac

:
