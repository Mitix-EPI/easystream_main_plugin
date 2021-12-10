#!/bin/sh
set -ex

sudo rm -rf build && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_UBUNTU=yes ../plugin
make -j4
