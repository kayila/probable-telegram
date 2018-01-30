#!/usr/bin/env bash
set -e

rm -rf build
docker build . -t kayila/fdispatcher
docker create --name asdf kayila/fdispatcher
docker cp asdf:/usr/local/bin/ ./build
docker rm asdf
