#!/bin/bash

set -x

cd /usr/src/mysql-server
if [ -d bld ]; then
	cd bld && rm -rf .
else
	mkdir bld
fi

cd bld
cmake \
    -DDOWNLOAD_BOOST=1 \
    -DWITH_BOOST=../boost \
    -DBUILD_CONFIG=mysql_release \
    -DINSTALL_LAYOUT=RPM \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    ../
make
