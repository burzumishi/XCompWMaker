#!/bin/sh

autoreconf -vfi -I m4

exit 0

if [ -x config.status -a -z "$*" ]; then
  ./config.status --recheck
else
  if test -z "$*"; then
    echo "I am going to run ./configure with no arguments - if you wish "
    echo "to pass any to it, please specify them on the $0 command line."
    echo "If you do not wish to run ./configure, press Ctrl-C now."
    trap 'echo "configure aborted" ; exit 0' 1 2 15
    sleep 1
  fi
  ./configure "$@"
fi
