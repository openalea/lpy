L-Systems
#########

.. _LSystems_Introduction:

Introduction
============

Lindenmayer [Lin1968] proposed a model of development based on *rewriting rules* or *productions*. This model,
known as *L-systems*, originally provided a formal description of the development of simple multicellular
organisms and was later extended to higher plants [Prusinkiewicz, 89]. For this, the structure of the systems
is represented as a *string* of *modules*, i.e. a label and some parameters, representing the different components
and their states. The rewriting rules operate on components of a plant; for example an apex, an internode,
or a leaf and describe their evolution in time.

A rule consists of two components, a *predecessor* and a *successor*. During a *derivation step*, the predecessor
is replaced by the successor. Even very simple L-systems can produce plant-like structures. The difference between
L-systems and other rewritting system such as Chomsky grammars lies in the method of applying productions.
In Chomsky grammars productions are applied sequentially, whereas in L-systems they are applied in parallel and
simultaneously replace all letters in a given string.


.. _LSystems_String_Representation:

String Representation
=====================

Lindenmayer developed a string notation that makes it easy to specify productions and carry out
simulations. Each component of the system is represented as a module. A module is characterized
by a label, for instance **A**, and possibly a set of parameters. For instance, an apex of age
10 can thus be represented by the module **A(10)**, an internode of length 20 and radius 3 by **I(20,3)**.
To represent a linear structure, a *string*, called *L-string* can be created as a serie of modules. For branched
structures such as trees, special modules [ and ] are used to represent begin and end of branches.
After each module in the string, its lateral successors enclosed in square brackets are first given
followed by its axial successor. For instance in the structure **A[B]C**, **A** carries a lateral module **B**
and is followed by module **C**.


.. _LSystems_Geometric_Representation_Of_The_String:

Geometric representation of the string
======================================

To graphically represent the string, it is possible to use a LOGO like turtle that will
parse sequentially the string and interpret some of the modules as geometric commands
[Prusinkiewicz, 86]. The commands will be additioned to create the geometry. For instance,
module **F** makes the turtle draw a cylinder and move forward, **+** and **-** to rotate on the right
or left. You can read section [Predefined Symbols] to have the list of interpretable modules.


.. _LSystems_Rules:

Rules
=====

Production rules are composed of a *predecessor* which represents the pattern of modules to match
and replace, and *successor* which is expressed as python code containing production statements.

.. code-block:: python

    # simple rule
    predecessor --> successor
    # equivalent to
    predecessor : produce successor
    # more complex rule
    predecessor :
      if condition:
         produce successor
      else:
         produce successor2

As an example,

.. code-block:: python

    I(l) --> I(l+dl)
    A(t):
      if t < APEX_DURATION:
        produce I(INITIAL_LENGTH)
      else:
        produce  # empty production

The pattern given in the successor is expressed as a set of modules represented by their labels and
variables for the parameters. A pattern is matched to a module or a set of modules of a string only
if labels and numbers of parameters are the same. If matched, the variables are assigned to the actual
values in the string of the parameters and the code of the successor is called.

As one can see in the example the production of the successor can be conditional. The successor can
produce an empty string (``produce`` or ``produce *``) meaning that the successor is replaced by nothing. The
production can also produce nothing. In this case the application will supposed to have failed and
other rules can be tested. If no rule matches a module of the string, then identity rule will be applied
and the module will be replaced by itself.

The replacement of a successor can depend on the neighbours of the modules to replace. In this case,
the rule is called context sensitive and the successor, now called *strict successor* is augmented with
pattern for neighbour modules (i.e. the contexts). Lpy supports 4 types of contexts.

- left and right contexts, i.e. neighbour modules on the left and on the right.

.. code-block:: bash

    left_context < strict_predecessor > right_context  --> successor

For instance, A < B > C –> D or I < A –> IA

- new left and right contexts, i.e. futur neighbour modules in the new string on the left and on the right.

.. code-block:: bash

    new_left_context << strict_predecessor --> successor
                        strict_predecessor >> new_right_context   --> successor

Of course, the ``new_left_context`` is only available if the rule are applied from left to right on
the string and ``new_right_context`` from right to left.

These contexts can then be combined.

Different types of Rules
------------------------

- Production rules are intended to express the development of the modelled structure. They are applied in parallel on the L-string.

- Decomposition rules are intended to decompose recursivelly a module into an L-string using a set of possibly recursive rules. To avoid infinite recursion a maximum depth of recursion can be specified.

- Interpretation rules allows to specify the geometric interpretation of symbols used in a given simulation. For this, a mapping to interpretable symbols can be made. Recursive rules can be used and similarly to decomposition rules, a maximum depth of recursion can be specified. A Turtle object is managed by the simulation and L-Py translates automatically some predefined modules into the corresponding method call onto the Turtle object. It is also possible to directly access the Turtle object using the option 'Turtle in Interpretation rules'. In such case, the turtle is accessible using the ``turtle`` variable and any of its methods can be called.

.. code-block:: python

    Internode(t):
        turtle.F(t)
