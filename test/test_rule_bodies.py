from openalea.lpy import AxialTree, Lsystem


def test_rule_body_accumulates_production_and_builds_an_axial_tree():
    lsystem = Lsystem()
    lsystem.setCode("""
Axiom: A(2)
production:
A(value):
    fragment = makestring (B(value + 1)
                           C(value + 2))
    nproduce (D(fragment[0][0])
              E(fragment[1][0]))
    produce F(value)
""")

    assert lsystem.iterate() == AxialTree("D(3) E(4) F(2)")


def test_rule_body_queries_both_contexts_and_binds_parameters():
    lsystem = Lsystem()
    lsystem.setCode("""
Axiom: A(2) B(3) C(4)
production:
B(value):
    left = {}
    right = {}
    if InLeftContext(A(a), left) and InRightContext(C(c), right):
        produce D(left['a'] + value + right['c'])
""")

    assert lsystem.iterate() == AxialTree("A(2) D(9) C(4)")


def test_rule_body_leaves_python_identifiers_and_strings_untouched():
    lsystem = Lsystem()
    lsystem.setCode(r'''
Axiom: A
production:
A:
    nproduce_value = 2
    makestring_value = "escaped \" produce B"
    assert makestring_value.endswith("produce B")
    produce C(nproduce_value)
''')

    assert lsystem.iterate() == AxialTree("C(2)")
