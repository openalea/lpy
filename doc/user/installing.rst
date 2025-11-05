Installing Lpy
##############

``L-Py`` distribution is based on the ``conda`` software environment management system.
To install conda, you may refer to its installation page: https://docs.conda.io/projects/conda/en/latest/user-guide/install/



Installing binaries using conda
================================


To install L-Py, you need to create an environment (named lpy in this case) :

.. code-block:: bash

        conda create -n lpy openalea.lpy -c openalea3 -c conda-forge

Then, you need to activate the L-Py environment

.. code-block:: bash

        conda activate lpy

And then run L-Py

.. code-block:: bash

        lpy


Installing from sources
=======================

You should first install all dependencies in a conda environment. The simplest way to do this is to call

.. code-block:: bash
    
    conda create -n lpydev 
    conda activate lpydev
    conda install --only-deps openalea.lpy -c openalea3 -c conda-forge
    conda install 

You should clone the lpy project into your computer

.. code-block:: bash
    
    git clone https://github.com/openalea/lpy.git


Compiling on macOS and Linux
=================

This assumes you installed the usual build tools on Linux, or the Xcode Build Tools on macOS.

You need then to compile lpy with the following command, on macOS and Linux:

.. code-block:: bash
        
        mkdir build ; cd build
        cmake -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} \
              -DCMAKE_PREFIX_PATH=${CONDA_PREFIX} \
              -DCMAKE_BUILD_TYPE=Release  \
              -LAH .. 
        make

You can use ``make -j numproc`` if you have several processors.
To install L-Py on your environment

.. code-block:: bash

        make install
        python setup.py install

to install it into you python system.

To run test,

.. code-block:: bash

        cd test/
        nosetests

To launch the visual editor, you can type in your shell

.. code-block:: bash

        lpy

Compiling on Windows
=======

On Windows you must install **Visual Studio 2019** with Desktop C++ tools.

For your convenience a build script called `windows_build_dev.bat` has been written. If you installed **Visual Studio 2019** with Desktop C++ tools and **miniconda3** at the default location, with your environment called **lpydev**, running the script from the Windows Command Prompt should compile lpy.

If you want to compile manually, open the **Developer Command Prompt for VS 2019** (search for the shortcut in the Start Menu).

Then you should activate **conda** manually in that prompt. If you installed **miniconda3** in the default directory `C:\\Users\\YourName\\miniconda3` and if your environment is named `lpydev`, you can use the command:

.. code-block:: command

        %USERPROFILE%\miniconda3\Scripts\activate.bat %USERPROFILE%\miniconda3\envs\lpydev

Otherwise, adapt the command to the path where you installed miniconda3.

Then you can compile with the following commands:

.. code-block:: command
        
        mkdir build
        cd build
        cmake .. -G "Visual Studio 16 2019" ^
        -Wno-dev ^
        -DCMAKE_INSTALL_PREFIX=%CONDA_PREFIX%\Library ^
        -DCMAKE_PREFIX_PATH=%CONDA_PREFIX%\Library ^
        -DCMAKE_INSTALL_RPATH:STRING=%CONDA_PREFIX%\Library\lib ^
        -DCMAKE_INSTALL_NAME_DIR=%CONDA_PREFIX%\Library\lib ^
        -DPython3_EXECUTABLE=%CONDA_PREFIX%\python.exe
        
        cmake --build . --parallel %NUMBER_OF_PROCESSORS% --config Release --target install
        cd ..

Note: you can only compile using the config **Release** and the target **install** on Windows.

To install L-Py on your environment

.. code-block:: command

        python setup.py install

to install it into you python system.

To run test,

.. code-block:: command

        cd test/
        nosetests

To launch the visual editor, you can type in your shell

.. code-block:: command

        lpy


Notes on dependencies
========================

``L-Py`` core is a ``C++`` library but based on the ``Python`` language.
The communication between both language is made using ``Boost.Python``.
The ``PlantGL`` library is used for the 3D modelling and visualization.
The ``Qt`` library and its python wrappers ``PyQt`` (build with ``SIP``) are used to create the visual interface.
``PyOpenGL`` is used to display and edit the materials.

To compile and install it from sources, the project requires ``cmake`` and ``setuptools``.

To test it, the ``nosetests`` conventions is used.

All these projects have to be correctly installed before compiling ``L-Py``.

Additionally, the ``Cython`` module that make it possible to translate python code into ``C`` code is automatically integrated to the project if detected. You can install it if you want to test this extension.

