#!/usr/local/bin/bash
## Note: this script can also be run in ZSH if, for whatever reason, you want to run it in ZSH (the default shell for macOS 10.15+)
#!/bin/zsh

echo $1
if [[ -d build ]] && [[ "$1" == "clean" ]]; then
    echo "cleaning build directory"
    rm -rf build
fi

if ! [[ -d build ]]; then
    echo "creating build directory"
    mkdir build
fi

export ENV_NAME=lpydev
if [[ ${CONDA_PREFIX} != ${ENV_NAME} ]] ; then
    echo "initialize CONDA"
    if [[ -d $HOME/miniconda3/ ]] ; then
        source ~/miniconda3/etc/profile.d/conda.sh
    fi
    if [[ -d $HOME/anaconda3/ ]] ; then
        source ~/anaconda3/etc/profile.d/conda.sh
    fi
    if [[ ${CONDA_EXE} == "" ]] ; then
        echo "error activating conda. Did you install it in the default directory? Exit."
        exit
    fi
    conda activate ${ENV_NAME}
fi
echo "Conda prefix: ${CONDA_PREFIX}"


export PREFIX=${CONDA_PREFIX}
export PYTHON=${CONDA_PREFIX}/bin/python


cd build
cmake \
      -DPLANTGL_ROOT=../plantgl/build-cmake/ \
      -DCMAKE_INSTALL_PREFIX=${PREFIX} \
      -DCMAKE_PREFIX_PATH=${PREFIX} \
      -DCMAKE_BUILD_TYPE=Debug  \
      -DPython3_EXECUTABLE=${PYTHON} \
      .. 

#     depending on the version of Boost, it might not be detected (I don't know why) in that case add:
#     -DBoost_INCLUDE_DIR=${CONDA_PREFIX}/include \

NBPROC=$(sysctl -a | grep machdep.cpu.thread_count | sed s/"machdep.cpu.thread_count: "//g)
make -j $NBPROC
make install
cd ..
python setup.py install
