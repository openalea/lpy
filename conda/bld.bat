COPY options_conda_win.py options.py

REM scons install
python setup.py install

if errorlevel 1 exit 1

MOVE %LIBRARY_PREFIX%\lib\*pgl*.dll*  %LIBRARY_PREFIX%\bin