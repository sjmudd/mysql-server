#!/bin/bash

set -ex

cd /workspaces/mysql-server

if [ -d bld ]; then
	cd bld && rm -rf *
else
	mkdir bld
fi

cd bld
cmake \
    -DDOWNLOAD_BOOST=1 \
    -DWITH_BOOST=../boost \
    ../
make
