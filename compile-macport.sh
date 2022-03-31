cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DPLANTGL_ROOT=../plantgl/build-cmake/ \
      -DCMAKE_INSTALL_PREFIX=../build-cmake/ \
      -DBoost_INCLUDE_DIR=/opt/local/include \
      -LAH .. 
make -j 8
make install
cd -