#!/bin/bash

export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig

cp options_conda_build.py options.py

$PYTHON setup.py  install --prefix=$PREFIX 
