#!/bin/bash

if [[ -d build ]]; then
    rm -rf build
fi
mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_PREFIX_PATH=${PREFIX} -DCMAKE_BUILD_TYPE=Release ..
make -j${CPU_COUNT}
make install

cd ..
$PYTHON setup.py install --prefix=${PREFIX}
