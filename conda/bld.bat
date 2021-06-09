:: Working Dir
mkdir build
cd build

:: Build
cmake -G "NMake Makefiles" -DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% -DCMAKE_BUILD_TYPE=Release -DPython3_EXECUTABLE=%PYTHON% -LAH ..
if errorlevel 1 exit 1
nmake
if errorlevel 1 exit 1
nmake install
if errorlevel 1 exit 1

:: Install Python Files
cd ..
%PYTHON% setup.py install
if errorlevel 1 exit 1
