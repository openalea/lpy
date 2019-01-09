Installing Lpy
##############

Dependencies
============

``L-Py`` core is a ``C++`` library but based on the ``Python`` language.
The communication between both language is made using ``Boost.Python``.
The ``PlantGL`` library is used for the 3D modelling.
The ``Qt`` library and its python wrappers ``PyQt`` (build with ``SIP``) are used to create the visual interface.
``PyOpenGL`` is used to display and edit the materials.

To compile and install it from sources, the project requires ``scons`` and ``setuptools`` and their
openalea extension ``OpenAlea.SconsX`` and ``OpenAlea.Deploy``.

To test it, the ``nosetests`` conventions is used.

All these projects have to be correctly installed before compiling ``L-Py``.

Additionally, the ``Cython`` module that make it possible to translate python code into ``C`` code is automatically integrated to the project if detected. You can install it if you want to test this extension.

Installing for Linux & Mac
===================

First, you need to install conda: https://conda.io/docs/user-guide/install/index.html

When it's done, create an environment for L-Py:

.. code-block:: bash

        conda create -n lpy openalea.lpy -c openalea

Then, activate the L-Py environment

.. code-block:: bash

        source activate lpy

And run L-Py

.. code-block:: bash

        lpy

Installing binaries (Windows)
===================

You can download binaries of this project via the OpenAlea Installer after loggin on the Inria Gforge.

Installing from sources
=======================

You should first install all dependencies.
Then in a shell, go in the project home directory and simply type

.. code-block:: bash

        scons

to compile. You can use option ``-j numproc`` if you have several processors.
Then type

.. code-block:: bash

        python setup.py install

to install it into you python system.

To run test,

.. code-block:: bash

        cd test/
        nosetests

To launch the visual editor, on windows you can click on ``[Start]/Programs/OpenAlea/Lpy``. On linux,
just type

.. code-block:: bash

        lpy
