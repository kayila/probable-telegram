#!/usr/bin/env bash
set -e

rm -rf build
docker build . -t kayila/watchdog
docker create --name asdf kayila/watchdog
docker cp asdf:/usr/local/bin/ ./build
docker rm asdf
