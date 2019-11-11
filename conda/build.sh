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
sed -i '' '1,1 s/^/#/' $CONDA_PREFIX/lib/python3.7/site-packages/openalea/plantgl/gui/__init__.py
sed -i '' '1,1 s/^/#/' $BUILD_PREFIX/lib/python3.7/site-packages/openalea/plantgl/gui/__init__.py
$PYTHON setup.py install --prefix=${PREFIX}
