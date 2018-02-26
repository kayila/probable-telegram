#!/bin/bash

if [ -z "${WHALEDO_UID}" ]; then
  WHALEDO_UID=$UID
fi

if [ -z "$@" ]; then
  ARGS="clang++ -std=c++14 -Wall -Wpedantic -static Server.cpp -o server"
elif [ "$@" == "clean" ]; then
  ARGS="rm server"
fi

docker run -v "$(pwd):/tmp/work" -w /tmp/work -u $WHALEDO_UID whaledo/c-development $ARGS
