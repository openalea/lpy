@echo off

:: You must install Visual Studio 2019 (Community is fine) with Desktop Development C++ 
:: You must run this script in the Developer Command Prompt for Visual Studio (find the shortcut in your start menu)
:: You should have installed your dependencies with 
:: ==> conda install -c conda-forge -c fredboudon --only-deps openalea.lpy
:: CHECK THE VERSION IT PULLS. Sometimes Conda decides to pull an older version. In that case, specify openalea.lpy=3.8.0
:: You should adjust the path to your conda environment in the variable CONDA_PREFIX

set CONDA_PREFIX=%USERPROFILE%\miniconda3\envs\lpydev
set BUILDDIR=build

:: using "clean" as an argument will delete the build directory
if "%1"=="clean" echo "CLEANING BEFORE BUILD."
if "%1"=="clean" rmdir /s /q %BUILDDIR%
if "%1"=="clean" mkdir %BUILDDIR%

set PYTHON=%CONDA_PREFIX%\python.exe
:: BUILD_CONFIG must be Release because conda does not provide debug symbols for Windows.
set BUILD_CONFIG=Release
set LIBRARY_LIB=%CONDA_PREFIX%\Library\lib
set LIBRARY_PREFIX=%CONDA_PREFIX%\Library

cd %BUILDDIR%

cmake .. -G "Visual Studio 16 2019" ^
-Wno-dev ^
-DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% ^
-DCMAKE_PREFIX_PATH=%LIBRARY_PREFIX% ^
-DCMAKE_INSTALL_RPATH:STRING=%LIBRARY_LIB% ^
-DCMAKE_INSTALL_NAME_DIR=%LIBRARY_LIB% ^
-DPython3_EXECUTABLE=%PYTHON%


if errorlevel 1 exit /b
::echo "Can't generate CMake config"

cd ..

cd %BUILDDIR%
cmake --build . --parallel %NUMBER_OF_PROCESSORS% --config %BUILD_CONFIG% --target install

if errorlevel 1 exit /b
:: echo "Can't build generated CMake config"

cd ..
%PYTHON% setup.py install

:: if you want to run the program, you must have your conda environment activated
:: it won't not be the case if you're running this script in a simple Developer Command Prompt.

:: %PYTHON% %CONDA_PREFIX%\Scripts\lpy-script.pyw