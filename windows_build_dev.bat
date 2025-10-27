@echo off

:: You must install Visual Studio 2019 (Community is fine) with Desktop Development C++ 
:: You must run this script in the Developer Command Prompt for Visual Studio (find the shortcut in your start menu)
:: You should have installed your dependencies with 
:: ==> conda install -c conda-forge -c fredboudon --only-deps openalea.lpy
:: CHECK THE VERSION IT PULLS. Sometimes Conda decides to pull an older version. In that case, specify openalea.lpy=3.8.0
:: You should adjust the path to your conda environment in the variable CONDA_PREFIX

:: Initialize build tools
IF "%VSINSTALLDIR%" == "" CALL "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

:: Initialize conda environment
IF "%CONDA_PREFIX%" == "" CALL "%USERPROFILE%\miniconda3\Scripts\activate.bat" "%USERPROFILE%\miniconda3\envs\lpydev"

echo %CONDA_PREFIX%

set BUILDDIR=build

:: using "clean" as an argument will delete the build directory
if "%1"=="clean" echo "CLEANING BEFORE BUILD."
if "%1"=="clean" rmdir /s /q %BUILDDIR%

set PYTHON=%CONDA_PREFIX%\python.exe
:: BUILD_CONFIG must be Release because conda does not provide debug symbols for Windows.
set BUILD_CONFIG=Release
set LIBRARY_LIB=%CONDA_PREFIX%\Library\lib
set LIBRARY_PREFIX=%CONDA_PREFIX%\Library

if not exist %BUILDDIR% mkdir %BUILDDIR%

cd %BUILDDIR%
cmake --version
cmake .. -G "Visual Studio 17 2022" ^
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

:: You can run the IDE directly by de-commenting this line below.
:: %PYTHON% %CONDA_PREFIX%\Scripts\lpy-script.pyw