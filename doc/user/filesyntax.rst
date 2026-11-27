File syntax
###########

The Lpy file format is based on the python language which is extended with L-system particular constructs.


.. _FileSyntax_Canvas_Of_LPy_File:

Canvas of L-Py file
===================

.. code-block:: python

    # pure python code
    def func():
       # python code
       nproduce lstring # it is possible to use the nproduce statement
                        # in this part of the file

    module A,B   # declaration of module name

    Axiom: lstring # declaration of axiom

    derivation length: int # default = 1
                           # number of derivation step to perform
    production: # beginning of production rules

    pattern :          # a production rule. Start with successor given as a pattern of module to replace
       python code     # rule core are pure python code with production statement
       produce lstring # production statement giving the new string pattern to produce

    # simple rules can be expressed this way
    pattern --> new_pattern

    homomorphism: # beginning of homomorphism rules.
                  # They are called before plotting the string or
                  # application of rule with query modules (?[PHUR])
    maximum depth: int # default = 1
                       # number of homomorphism recursive step to perform.
                       # should be defined only once

    decomposition: # beginning of decomposition rules.
                   # These rules are applied recursively after each production step
                   # usefull to decompose a module into a structure
    maximum depth: int # default = 1
                       # number of decomposition recursive step to perform.
                       # should be defined only once

    group: int  # all following rules will be assign to this group
                # to activate a group of rule see command useGroup
                # by default group 0 is active

    production: # again all types of rule can be defined
    homomorphism:
    decomposition:

    endgroup # following rules will be assign to default 0 group

    production: # again all types of rule can be defined
    homomorphism:
    decomposition:

    endlsystem # end of rules definition

    # pure python code is again possible


Rule headers
============

A rule header contains a predecessor and may include contexts from the
original string (< or >) and from the string
being produced (<< or >>). A header can use a new left context or a new right
context. For example, A(a) < D(d) << B(b) > C(c) matches B with original left
context A, newly produced left context D, and original right context C.

The declarative grammar covers the first line of production, decomposition
and interpretation rules, including block, arrow and static forms.

.. literalinclude:: rule_header.lark
   :language: text

LPy resolves module names using the active module table and interprets the
arguments. Angle brackets inside arguments and quoted strings belong to the
fragment. Square brackets at the top level are LPy branch symbols.

With Lark installed, run the check from the repository root.

.. code-block:: console

    python test/check_rule_header_grammar.py

The C++ parser uses a production table generated from the context rules
above. It uses LPy's fragment reader and keeps the table in the source tree
so building LPy requires no Lark installation. After changing the context
rules, run the same command with ``--write`` to regenerate the table.

The check scans candidate rule lines in tracked LPy files without executing
the models. It reports context forms, terminators and rejected candidates
with their source locations.

Module declarations
===================

A module declaration can list names, parameter names and aliases. Attributes
after the colon apply to the whole declaration line. LPy reads nested
expressions in attribute values with its Python fragment reader.

.. literalinclude:: module_declaration.lark
   :language: text

The C++ parser shares the generated syntax between the module directive and
the declaration list accepted by the context API. The verification script
checks the table and scans tracked LPy sources. With Lark installed, run it
with ``--write`` to regenerate the table.

.. code-block:: console

    python test/check_module_declaration_grammar.py

.. _FileSyntax_LSystem_Specific_Declaration:

L-System specific declaration
=============================

The declarative grammar describes directive keywords and the shape of their
payloads. Section transitions and evaluation of payloads are handled by LPy.

.. literalinclude:: lsystem_directives.lark
   :language: text

The C++ parser uses a table generated from this grammar. The table is checked
into the source tree and requires no Lark installation when building or using
LPy. With Lark installed, check the table or regenerate it after grammar
changes with the following commands.

.. code-block:: console

    python test/check_directive_grammar.py
    python test/check_directive_grammar.py --write

+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**module name**             |*Declaration of module name.*                                                                                                                                                                            |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**consider: name**          |*Symbol to consider.*                                                                                                                                                                                    |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**ignore: name**            |*Symbol to ignore.*                                                                                                                                                                                      |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**group id:**               |*Following rules will be associated to group id. By default rules are associated to group 0.*                                                                                                            |
|                            |*The rules of this group will always be applied. To additionnally activate other groups, use the command useGroup.*                                                                                          |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**Axiom: Lstring**          |*Declaration of the axiom of the Lsystem*                                                                                                                                                                |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**produce Lstring**         |*Produce an Lstring and return.*                                                                                                                                                                         |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**nproduce Lstring**        |*Produce an Lstring whithout returning.*                                                                                                                                                                 |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**derivation length: value**|*Number of derivation to do (default=1).*                                                                                                                                                                |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**production:**             |*Start of the production rules declaration.*                                                                                                                                                             |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**homomorphism:**           |*Start of the interpretation rules declaration.*                                                                                                                                                         |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**interpretation:**         |*Start of the interpretation rules declaration.*                                                                                                                                                         |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**decomposition:**          |*Start of the decomposition rules declaration.*                                                                                                                                                          |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**maximum depth:**          |*Number of decomposition or interpretation recursive call to do (default=1).*                                                                                                                            |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**endgroup**                |*Reactivate default group 0.*                                                                                                                                                                            |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|**endlsystem**              |*End of lsystem rules declaration.*                                                                                                                                                                      |
+----------------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
