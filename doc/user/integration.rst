L-Py in scripts or in third party applications
###############################################

Manipulation of Lsystems
=====================================

Using the openlea.lpy module, Lsystems can be manipulated directly using the 'Lsystem' object. Main actions are the creation and the derivation of an Lsystem.

Creation
--------

To create an Lsystem, a file or a string containing the code can be used.

.. code-block:: python

    from openalea.lpy import *

    l = Lsystem("myfile.lpy")

or 

.. code-block:: python

    l = Lsystem()
    l.setCode(mycode)

To configure the creation of an Lsystems by setting predefined variables, it is possible to pass as an argument a dictionnary of variables.

.. code-block:: python

    variables = {'VAR1' : VALUE1 , ... }
    l = Lsystem("myfile.lpy", variables)

In such case, the variables contained in the dictionnary may overwrite graphically defined variables if they have similar names or variable in the model defined using the ``extern`` command. 

The ``extern`` command can be used to set variables that are defined with default value in the Lsystem but can be redefined externally. For instance, in the following example, the ``VAR1``is redefined externally.

.. code-block:: python

    # myfile.lpy
    extern(VAR1 = VALUE1)
    Axiom: F(VAR1)

.. code-block:: python
    
    # application of the lsystem
    l = Lsystem("myfile.lpy", {'VAR1' : VALUE2})

Derivation
----------

The standard way to apply derivation on an Lstring using a Lsystem is to use the function ``derive``. Its parameters are the lstring on which derivation is applied, the iteration number to which the derivation corresponds and the number of iterations to apply. Default values for lstring, iteration number and number of iterations are the axiom, 0 and derivation length respectivelly. An extra optimization parameter makes it possible to indicates if the lstring has already been used for interpretation (and thus environmental modules are completed). 

Thus an Lsystem can be simulated with the following code


.. code-block:: python

    l = Lsystem("myfile.lpy")
    lstring = l.derive()

To have all intermediate lstrings, the following code can be used

.. code-block:: python

    l = Lsystem("myfile.lpy")
    lstring = l.axiom
    for i in xrange(l.derivationLength):
        lstring = l.derive(lstring, i, 1)

A more compact version using iterator is 

.. code-block:: python

    lsystem = Lsystem("myfile.lpy")
    for lstring in lsystem:
        pass


Note that the function ``interpret`` of the Lsystem makes it possible to apply interpretation rules on an Lstring and return the resulting interpretation string.

.. code-block:: python

    lsystem = Lsystem("myfile.lpy")
    for lstring in lsystem:
        ilstring = lsystem.interpret(lstring)



Graphical output
----------------

A visual interpretation can be made using a 3D turtle. For this the function ``turtle_interpretation`` of the Lsystem can be used. Custom turtle deriving from PlantGL ``Turtle`` class can be defined and used. By default, a ``PglTurtle`` is used which output PlantGL primitives. 
A computation of the PlantGL representation at each step can thus be defined in the following way:

.. code-block:: python

    from openalea.lpy import *
    from openalea.plantgl.all import *

    lsystem = Lsystem("myfile.lpy")
    for lstring in lsystem:
        t = PglTurtle()
        lsystem.turtle_interpretation(lstring, t)
        scene = t.getScene()


Note that an interpretation of the lstring as a PlantGL scene can be computed directly with the ``sceneInterpretation`` of the Lsystem object.

.. code-block:: python

    from openalea.lpy import *

    lsystem = Lsystem("myfile.lpy")
    for lstring in lsystem:
        scene = lsystem.sceneInterpretation(lstring)


Plotting directly the 3D scene from the Lstring is also possible with the function ``plot`` of the Lsystem object. 

.. code-block:: python

    from openalea.lpy import *

    lsystem = Lsystem("myfile.lpy")
    for lstring in lsystem:
        lsystem.plot(lstring)


In such case, the viewer used to plot the 3D scene can be parameterized (By default it is the PlantGLViewer). For this the function ``registerPlotter`` of the lpy module can be used. It allows to register a plotter whose following interface are expected

.. code-block:: python

        class Plotter:
           def __init__(self): 
                pass
           def plot(self,scene): 
                pass
           def save(self, fname, format): 
                """ Save the view of the 3D scene in fname with the given format (PNG, JPG) """
                pass
           def selection(self): 
                """Should return a list of id of selected elements """
                pass
           def waitSelection(self, txt): 
                """ Wait for selection of elements with the following text """
                pass

From the Lsystem point of view, it is possible to use the function ``animate`` or ``record`` to plot at each step the 3D interpretation of the Lsystem using the defined plotter.

Graphical Parameters Manipulation
=====================================

The L-Py GUI makes it possible to define graphical parameters by the user and usable within the simulation. For this, some code are defined at the end of the Lsystem code. In addition to the variables defined in its namespace, it is possible to have access to the predefined graphical parameters of an Lsystems using some globals variables:

.. code-block:: python
    
    lsystem = Lsystem("myfile.lpy")
    # A list of scalar object that defined type, value and bounds
    # of every graphical scalar parameters
    print lsystem.__scalars__ 

    # A list of information on graphical objects. 
    # It is a list of panel. 
    # A panel is a tuple with panelinfo and list of objets. 
    # Objects are defined as a tuple with a type and the object. 
    # Panelinfo is a dictionnary of properties.
    print lsystem.__parameterset__ 

To write parameters at the end of an Lsystem code, it is possible to use function defined in openalea.lpy.simu_environ

.. code-block:: python
    
    from openalea.lpy.simu_environ import initialisationFunction
    from openalea.lpy.gui.scalar import IntegerScalar

    lc = LsysContext()
    # Setting the options of Lsystem execution
    lc.options.setSelection('Module declaration',1)

    # Defining graphical scalar parameter
    scalars = [IntegerScalar('default_scalar', 1, 0, 100)]

    initcode = initialisationFunction(lc, scalars = scalars, 
                           visualparameters = None,
                           colorlist = None,
                           referencedir = savedir)


