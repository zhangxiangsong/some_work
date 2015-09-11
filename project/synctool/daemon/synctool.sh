#!/bin/bash
#
# Description:  This shell script takes care of starting and stopping dennis
# Hu Dennis created on Sep. 24th, 2010
#
# Source function library
. /etc/init.d/functions

#the fact synctools project root path
WORKDIR=/mnt/hgfs/sync-last

#the service name  for example: dennis
SNAME=synctool

#the full path and name of the daemon program
#Warning: The name of executable file must be identical with service name
PROG=$WORKDIR/bin/$SNAME


# start function
start() {
    #check the daemon status first
    if [ -f /var/run/$SNAME.pid ]
    then
        echo "$SNAME is already started!"
        exit 0;
    else
        action "Starting $SNAME ..." $PROG
        [ $? -eq 0 ] && touch /var/run/$SNAME.pid
        exit 0;
    fi
}

#stop function
stop() {
    echo "Stopping $SNAME ...\n"
    killproc $SNAME
    rm -rf /var/run/$SNAME.pid
}

case "$1" in
start)
  start
  ;;
stop)
  stop
  ;;
reload|restart)
  stop
  start
  ;;
status)
  status $SNAME
  ;;
*)
  echo $"Usage: $0 {start|stop|restart|status}"
  exit 1
esac
