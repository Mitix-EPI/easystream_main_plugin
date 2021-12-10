#!/bin/sh
set -ex

rm -rf build && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_UBUNTU=yes ..
make -j4
