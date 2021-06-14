#!/usr/local/bin/bash
echo $1
if [ -d build ] && [ "$1" == "clean" ]; then
    echo "cleaning build directory"
    rm -rf build
fi

if ! [[ -d build ]]; then
    echo "creating build directory"
    mkdir build
fi

export PREFIX=${CONDA_PREFIX}
export PYTHON=${CONDA_PREFIX}/bin/python


cd build
cmake \
      -DPLANTGL_ROOT=../plantgl/build-cmake/ \
      -DCMAKE_INSTALL_PREFIX=${PREFIX} \
      -DCMAKE_PREFIX_PATH=${PREFIX} \
      -DCMAKE_BUILD_TYPE=Release  \
      -DPython3_EXECUTABLE=${PYTHON} \
      .. 

#     depending on the version of Boost, it might not be detected (I don't know why) in that case add:
#     -DBoost_INCLUDE_DIR=${CONDA_PREFIX}/include \

NBPROC=$(sysctl -a | grep machdep.cpu.thread_count | sed s/"machdep.cpu.thread_count: "//g)
make -j $NBPROC
make install
cd ..
python setup.py install
