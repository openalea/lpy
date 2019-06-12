L-Py Turtle advanced primitives
###############################

Using PlantGL primitives
~~~~~~~~~~~~~~~~~~~~~~~~

Drawing PlantGL shapes
======================

In order to draw more complex but predifined shapes, certain methods use PlantGL primitives  These methods are **~** and **@g**.
**@g** draws the PlantGL shape in argument. **~** is more complicated, it takes in argument a geometric shape saved in the Turtle.
A special primitive is already predifined : **~l**. It draws a leaf.

.. code-block:: python

	Axiom: ~l 	#(Fig. A)

	Axiom: @g(Sphere(radius=3)) 	#(Fig. B)

	execContext().turtle.setSurface('t',Sphere(radius=3))
	Axiom: ~t(5) 	#(Fig. C)

+--------------------------------+--------------------------------+--------------------------------+
| .. image:: ../_images/leaf.png |                                |                                |
+--------------------------------+--------------------------------+--------------------------------+

Miscellaneous
~~~~~~~~~~~~~

In this section, several less common tools can be found.

SectionResolution
=================

**SectionResolution** allows to change the resolution of all following shapes. **Be careful !** If the resolution is too low, the program may not work properly.

.. code-block:: python

	Axiom: F SectionResolution(4) +F@O(0.5) 	#(Fig. A)

	Axiom: F SectionResolution(60) +F@O(0.5) 	#(Fig. B)

Download the example : :download:`resolution.lpy <../_downloads/resolution.lpy>`

+---------------------------------------+---------------------------------------+
| .. image:: ../_images/resolution1.png | .. image:: ../_images/resolution2.png |
|    :scale: 60%                        |    :scale: 60%                        |
+---------------------------------------+---------------------------------------+

ScreenProjection
=================

After using **@2D** (or **StartScreenProjection**), the following shapes will be drawn on the screen coordinates system (in two dimensions).
The examples below belong to the same axiom, only the camera's orientation is different. It confirms that the shape is in the screen system.

.. code-block:: python

	Axiom: Frame @2DF(0.6)@O(0.2)

Download the example : :download:`screen.lpy <../_downloads/screen.lpy>`

+-------------------------------+-------------------------------+
| .. image:: ../_images/2D1.png | .. image:: ../_images/2D2.png |
|    :scale: 70%                |    :scale: 70%                |
+-------------------------------+-------------------------------+

To switch back to the original coordinates system, **@3D** (or **EndScreenProjection**) can be used.

InterpolateColors
=================

There is an other way to color shapes using **InterpolateColors**. This method mixes up two colors in one.
There are three arguments, the first and the second are the index of materials and the last (optional) sets a priority to the first or the second color in order to make the final color.
There are two examples below.

.. code-block:: python

	Step = 20
	DIncr = 1.0 / Step

	Axiom:
	  d = 0.0
	  for i in range(Step):
	    nproduce InterpolateColors(3, 5, d) F(2.0/Step) 	#(Fig. A)
	    d += DIncr
	  produce ;(2) @O(0.15)

	#Other example

	Step = 4
	DIncr = 1.0 / Step

	Axiom:
	  d = 0.0
	  for i in range(Step):
	    nproduce InterpolateColors(2, 5, d) F(2.0/Step)      #(Fig. B)
	    d += DIncr
	  produce ;(1) @O(0.15)

Download the example : :download:`InterpolateColors.lpy <../_downloads/InterpolateColors.lpy>`

+---------------------------------------------+----------------------------------------------+
| .. image:: ../_images/interpolateColors.png | .. image:: ../_images/interpolateColors2.png | 
|    :scale: 50%                              |    :scale: 50%                               |
+---------------------------------------------+----------------------------------------------+

Advanced rotation primitives
============================

There are other primitives that can be used to rotate the Turtle.

**TurnAround** or **|**, turn the Turtle for 180° around the Up vector. It produce the same result as **+(180)** or **(-180)**

.. code-block:: python

	Axiom: Frame(2) | Frame(2)

+--------------------------------------+
| .. image:: ../_images/turnAround.png |
+--------------------------------------+

Requests
========

These methods allow to get some informations about the Turtle and store it in variables in order to use it after. Except **GetFrame**, 
it all can take three floats or one vector in arguments. If done, arguments are filled with values requested.

- **GetPos** or **?P**, collect the Turtle's Position vector informations.

- **GetHead** or **?H**, collect the Turtle's Head vector informations.

- **GetUp** or **?U**, collect the Turtle's Head vector informations.

- **GetLeft** or **?L**, collect the Turtle's Left vector informations.

- **GetRight** or **?R**, collect the Turtle's Right vector informations.

**GetFrame** or **?F**, collect the Turtle's Frame vector informations. 
It can take four vectors in arguments and fill it with the Position vector, the Head vector, the Up vector and the Left vector.

Rewriting shapes
~~~~~~~~~~~~~~~~

To clear the viewer, the primitive **None** can be written in the Axiom part.

.. code-block:: python

	Axiom: None

*Work in progress*
