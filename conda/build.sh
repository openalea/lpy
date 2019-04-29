#!/bin/bash

# Working Dir
mkdir build-cmake
cd build-cmake

# Build
cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_PREFIX_PATH=${PREFIX} -DCMAKE_BUILD_TYPE=Release ..
make -j${CPU_COUNT}
make install
