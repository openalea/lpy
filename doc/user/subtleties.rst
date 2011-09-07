Subtelties with L-Py 
####################

Axiom parameters that change after simulation
=============================================

Python makes it possible to create complex structure for instance to contains parameter of a module::

    1   class ApexParameters:
    2       def __init__(self,age=0):
    3          self.age = age
    4       def __str__(self):
    5           return 'ApexParameters(age='+str(self.age)+')'

It can then be instantiated in the axiom and modified during the simulation::

    6   module Apex
    7   Axiom: Apex(ApexParameters(age=0))
    8   dt = 1
    9   derivation length: 2
    10  Apex(p) : 
    11     p.age += dt
    12     produce Node() Apex(p)

Creating the lsystem and running it the first time will produce the expected result.::

    >>> l = Lsystem('mycode.lpy')
    >>> lstring = l.derive()
    >>> print lstring
    Node()Node()Apex(ApexParameters(age=2))

However, a strange behavior occurs when asking for the axiom.::

    >>> print l.axiom
    Apex(ApexParameters(age=2))

In fact, this behavior is due to the way python manage object. By default, complex objects
are not copied but simply shared between variables.::

    >>> a = ApexParameters(age=0)
    >>> b = a
    >>> b.age = 2
    >>> print a.age
    2

The variables ``b`` and ``a`` point toward the same object in memory. This one will be destroyed only when no variable
will be pointing to it. This is the same for the module of the string. In rule of lines 10-12, the parameter object of 
the ``Apex`` module of the string at derivation step t (line 10) is actually shared with the new ``Apex`` module of the produced string
for step t+1 (line 12). In the middle (line 11), it is modified and thus both string of step t and t+1 will be affected.
This produces side effects that lead to modification of the axiom in our case. With the ``L-Py`` graphic interface, this
can also occur when running 2 times the same simulation. The second simulation can look strange since it was started with an
axiom with modified parameters values.

To avoid that, an explicit copy of the parameter should be done. Rules will have the following shape.::

    10  Apex(p) : 
    11     from copy import deepcopy
    12     p = deepcopy(p)
    13     p.age += dt
    14     produce Node() Apex(p)


sproduce and undeclared modules
===============================

The ``sproduce`` function makes it possible to produce modules generated algorithmically.::

    m = ParamModule('toto')
    sproduce (Lstring([m]))

Advantage is that modules can be created on the fly. However, if you then try to math the produced module with
rules such as the following one, it may not work.::

    toto --> F

The reason is that by default L-Py assume module names of minimal length and thus the previous rule is understood
as the 4 modules t, o, t and o should be transformed into one F. If it was used with production of the type::

    produce toto

it would have work since consistently, the previous production would have been understood by ``L-Py`` as produce the 4 modules
t, o, t and o. However if you use ``ParamModule('toto')``, then you declare that only one module exists and it has a composed name 'toto'.
toto being different from t, o, t and o, matching will not occur.
To resolve this, you have to make ``L-Py`` understand that toto is actually a module name and not four letters. For this module declaration
are available.::

    module toto

Thus the rule::

    toto --> F

will be correctly interpreted as one module toto being transformed into one F.