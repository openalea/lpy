#!/bin/bash

export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig

cp options_conda_build.py options.py

scons --implicit-deps-changed -h

$PYTHON setup.py scons --scons-ext-param=" --implicit-deps-changed " install --prefix=$PREFIX 
