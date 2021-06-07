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

# ld crashes when it sees a symbol it cannot resolve.
# In this case, we must either mark all incriminated symbols as Undefined (WHICH IS DANGEROUS),
# or tell ld to shut up about this. I chose the 2nd option here out of laz
# More on this: https://stackoverflow.com/questions/36662920/xcode-clang-link-build-dynamic-framework-or-dylib-not-embed-dependencies
# More on this: https://stackoverflow.com/questions/17281901/ignoring-an-undefined-symbol-in-a-dynamic-library-from-xcode
# More on this: https://developer.apple.com/forums/thread/17630
export LDFLAGS="-undefined dynamic_lookup ${LDFLAGS}"

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
