Tropism in L-Py: ``Elasticity`` and ``ElasticityPerLengthUnit``
===============================================================

Overview
--------

In L-Py, a turtle draws a plant structure by interpreting an L-system string. The turtle has a position and a local orientation; its **heading** vector, :math:`\mathbf h`, specifies the direction in which it grows. An ``F(L)`` module draws a segment and advances the turtle.

**Tropism** progressively reorients the turtle towards a preferred direction, :math:`\mathbf t` (for example, the upward vertical :math:`\mathbf t=(0,0,1)`). The two L-Py modules discussed here select different ways of applying this reorientation:

-  ``Elasticity(e)`` applies a correction **at each ``F`` instruction**, independently of its length.
-  ``ElasticityPerLengthUnit(k)`` applies a correction **continuously along the distance traveled** by ``F``.

The examples below assume that the turtle’s tropism direction is configured as the upward vertical, that the initial heading is :math:`120^\circ` from that direction, and that positive coefficients cause the heading to approach the tropism. They describe the integrated ``ElasticityPerLengthUnit`` behavior developed in this implementation; availability and exact behavior depend on the installed L-Py/PlantGL version.

1. A branch built from a series of segments
-------------------------------------------

Consider a branch of total length :math:`L=10`, represented by :math:`n` successive ``F`` modules, each of length :math:`\Delta s=L/n`. The following L-Py production generates those segments:

.. code:: python

   module Stem

   Axiom: [ +(120) Elasticity(0.1) Stem(10, 10) ]

   derivation length: 1

   production:

   Stem(length, dl):
       for i in range(int(round(length / dl))):
           nproduce F(dl)

Here, ``Stem(10,10)`` generates one ``F(10)``. Changing it to ``Stem(10,1)`` generates ten consecutive ``F(1)`` modules. The ``+(120)`` command establishes the initial inclination; the tropism is applied when ``F`` is interpreted, not when ``+`` is executed. The ``[`` and ``]`` delimit a turtle branch.

To use the continuous mode, **only change the elasticity module**:

.. code:: python

   Axiom: [ +(120) ElasticityPerLengthUnit(0.1) Stem(10, 1) ]

This makes it straightforward to compare the same L-system geometry under the two tropism models.

2. Discrete tropism: ``Elasticity(e)``
--------------------------------------

In the discrete mode, the turtle is rotated towards the tropism direction **once for every ``F`` module**. If :math:`\theta_i` is the angle between the heading and the tropism just before the :math:`i`-th segment, the correction is

.. math::


   \Delta\theta_i=e\theta_i,
   \qquad
   \boxed{\theta_{i+1}=(1-e)\theta_i.}

The turtle then draws a straight segment of the requested length :math:`\Delta s` in the corrected direction. For :math:`0\le e\le1`, each correction reduces the angular deviation without overshooting.

After :math:`n` segments,

.. math::


   \boxed{\theta_n=\theta_0(1-e)^n.}

**The result depends on the number of ``F`` modules, not just the total length.** For example:

.. code:: python

   # One correction, one straight segment
   Axiom: [ +(120) Elasticity(0.1) Stem(10, 10) ]

.. code:: python

   # Ten corrections, ten straight segments
   Axiom: [ +(120) Elasticity(0.1) Stem(10, 1) ]

With :math:`\theta_0=120^\circ` and :math:`e=0.1`, the first version ends at :math:`108^\circ` from the tropism; the second ends at approximately :math:`41.84^\circ`. Subdividing the same branch therefore changes both its shape and final orientation.

3. Continuous tropism: ``ElasticityPerLengthUnit(k)``
-----------------------------------------------------

In the continuous mode, :math:`k` is a coefficient **per unit of length**, with dimensions :math:`\mathrm{length}^{-1}`. The angle decreases as a function of the curvilinear distance :math:`s` along the branch:

.. math::


   \frac{d\theta}{ds}=-k\theta,
   \qquad
   \boxed{\theta(s)=\theta_0e^{-ks}.}

After a total curvilinear length :math:`L`,

.. math::


   \boxed{\theta_L=\theta_0e^{-kL}.}

The dimensionless quantity :math:`kL` measures the accumulated tropic response. The characteristic reorientation length is :math:`1/k`: after traveling this distance, the angular deviation is reduced by a factor of :math:`e\approx2.718` (Euler’s number, not the discrete elasticity coefficient).

For example:

.. code:: python

   # One segment representing a continuously bending trajectory
   Axiom: [ +(120) ElasticityPerLengthUnit(0.1) Stem(10, 10) ]

.. code:: python

   # Ten segments approximating the same trajectory
   Axiom: [ +(120) ElasticityPerLengthUnit(0.1) Stem(10, 1) ]

Both end at the same angle,

.. math::


   \theta_L=120^\circ e^{-0.1\times10}\approx44.15^\circ.

**The endpoint and final heading are independent of the segmentation** (up to floating-point rounding), provided the tropism direction and :math:`k` remain constant. The displayed shape does change: one ``F(10)`` produces a single chord, whereas ten ``F(1)`` modules produce a ten-segment polyline that follows the curved trajectory more closely.

What does an individual ``F`` draw?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A continuous tropism produces a curved *underlying trajectory*. For each ``F(\Delta s)``, the turtle computes the displacement obtained by integrating the changing heading:

.. math::


   \boxed{\Delta\mathbf p=\int_0^{\Delta s}\mathbf h(s)\,ds.}

The displayed segment joins the start and end positions of that displacement. Its **chord length** is :math:`\|\Delta\mathbf p\|`, generally shorter than the requested **curvilinear length** :math:`\Delta s`. After drawing the chord, the turtle retains the **final tangent orientation** of the underlying curve, rather than the chord’s direction. This ensures that the next ``F`` continues the same integrated trajectory.

Mathematical expression of the displacement (optional)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For a unit tropism direction :math:`\mathbf t` and an initial unit heading :math:`\mathbf h_0`, define

.. math::


   \mathbf a=\frac{\mathbf h_0\times\mathbf t}{\|\mathbf h_0\times\mathbf t\|},
   \qquad \mathbf x=\mathbf a\times\mathbf t.

For :math:`0<\theta_0<\pi`, the heading at distance :math:`s` is

.. math::


   \mathbf h(s)=-\sin(\theta(s))\mathbf x+\cos(\theta(s))\mathbf t.

With :math:`\theta_1=\theta_0e^{-k\Delta s}`, the exact displacement is

.. math::


   \boxed{
   \Delta\mathbf p=
   \frac{\operatorname{Si}(\theta_1)-\operatorname{Si}(\theta_0)}{k}\mathbf x
   +\frac{\operatorname{Ci}(\theta_0)-\operatorname{Ci}(\theta_1)}{k}\mathbf t.
   }

Here :math:`\operatorname{Si}` and :math:`\operatorname{Ci}` are the sine and cosine integrals. **The negative transverse sign is important:** with :math:`\mathbf x=\mathbf a\times\mathbf t`, the initial heading is :math:`\mathbf h_0=-\sin(\theta_0)\mathbf x+\cos(\theta_0)\mathbf t`. Aligned and exactly opposite headings require special handling of the rotation axis; they do not affect the main comparison between the two modes.

4. Numerical comparison
-----------------------

Assume an initial angle :math:`\theta_0=120^\circ`, a total length :math:`L=10`, discrete elasticity :math:`e=0.1`, and continuous elasticity :math:`k=0.1` per unit length.

+----------------------------------+-----------------+-------------------------+----------------------+
| L-Py elasticity module           | ``Stem`` call   | Number of ``F`` modules | Final angle          |
+==================================+=================+=========================+======================+
| ``Elasticity(0.1)``              | ``Stem(10,10)`` | 1                       | :math:`108.00^\circ` |
+----------------------------------+-----------------+-------------------------+----------------------+
| ``Elasticity(0.1)``              | ``Stem(10,1)``  | 10                      | :math:`41.84^\circ`  |
+----------------------------------+-----------------+-------------------------+----------------------+
| ``ElasticityPerLengthUnit(0.1)`` | ``Stem(10,10)`` | 1                       | :math:`44.15^\circ`  |
+----------------------------------+-----------------+-------------------------+----------------------+
| ``ElasticityPerLengthUnit(0.1)`` | ``Stem(10,1)``  | 10                      | :math:`44.15^\circ`  |
+----------------------------------+-----------------+-------------------------+----------------------+

The two coefficients have different meanings and units. To obtain the **same angular correction** over one step of length :math:`\Delta s`, use

.. math::


   \boxed{e=1-e^{-k\Delta s}.}

This matches the orientations at segment boundaries, but does **not** make the segment endpoints identical: the discrete mode advances along the corrected heading, whereas the continuous mode integrates all intermediate headings.

5. Side-by-side L-Py example
----------------------------

The following example creates four branches: two using discrete tropism and two using continuous tropism. Each pair has the same total curvilinear length but a different number of ``F`` modules.

.. code:: python

   module Stem

   Axiom:
       nproduce [ @M(-6,0,0) +(120) Elasticity(0.1) Stem(10,10) ]
       nproduce [ @M(-2,0,0) +(120) Elasticity(0.1) Stem(10,1) ]
       nproduce [ @M( 2,0,0) +(120) ElasticityPerLengthUnit(0.1) Stem(10,10) ]
       nproduce [ @M( 6,0,0) +(120) ElasticityPerLengthUnit(0.1) Stem(10,1) ]

   derivation length: 1

   production:

   Stem(length, dl):
       for i in range(int(round(length / dl))):
           nproduce F(dl)

Configure the turtle’s tropism direction to point upward before running the example. The four branches illustrate the key difference: **with ``Elasticity``, changing the number of segments changes the total bending; with ``ElasticityPerLengthUnit``, it changes only the polygonal approximation of the same continuously bent trajectory.**

Summary
-------

+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+
|                             | ``Elasticity(e)``                         | ``ElasticityPerLengthUnit(k)``                                           |
+=============================+===========================================+==========================================================================+
| Meaning                     | Tropic response per ``F`` instruction     | Tropic response per unit length                                          |
+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+
| Parameter units             | Dimensionless                             | Inverse length                                                           |
+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+
| Angle update                | :math:`\theta_{i+1}=(1-e)\theta_i`        | :math:`\theta(s)=\theta_0e^{-ks}`                                        |
+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+
| Segment rendering           | Straight segment after angular correction | Straight chord of integrated displacement                                |
+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+
| Dependence on segment count | Final shape **and** orientation change    | Polygonal shape changes; endpoint and final orientation remain unchanged |
+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+
| Best suited for             | Growth models defined in discrete steps   | Growth models defined by physical distance                               |
+-----------------------------+-------------------------------------------+--------------------------------------------------------------------------+

The distinction is particularly important when an L-system uses different numbers of ``F`` modules to represent the same branch: ``ElasticityPerLengthUnit`` makes the tropic response independent of that discretization.
