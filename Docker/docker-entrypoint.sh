#!/bin/sh

if [ "$1" = 'local' ]; then
    echo "run infinity"
    sleep infinity
else
    eval "$@"
fi