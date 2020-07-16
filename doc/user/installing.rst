Installing Lpy
##############

``L-Py`` distribution is based on the ``conda`` software environment management system.
To install conda, you may refer to its installation page: https://docs.conda.io/projects/conda/en/latest/user-guide/install/



Installing binaries using conda
================================


To install L-Py, you need to create an environment (named lpy in this case) :

.. code-block:: bash

        conda create -n lpy openalea.lpy -c fredboudon -c conda-forge

The package is retrieved from the ``fredboudon`` channel (developement) and its dependencies will be taken from ``conda-forge`` channel.

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
    conda install --only-deps openalea.lpy -c fredboudon -c conda-forge
    conda install 

You should clone the lpy project into your computer

.. code-block:: bash
    
    git clone https://github.com/fredboudon/lpy.git

You need then to compile lpy with the following command

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

