import pytest

from openalea.lpy import AxialTree, Lsystem


@pytest.mark.parametrize("direction, first_rule, header, expected", [
    ("forward", "A(x) --> D(x + 10)", "A(a) < D(d) << B(b) > C(c)",
     "D(11) M(1,11,2,3) C(3)"),
    ("backward", "C(x) --> D(x + 10)", "A(a) < B(b) >> D(d) > C(c)",
     "A(1) M(1,13,2,3) D(13)"),
])
@pytest.mark.parametrize("body", [" --> M(a,d,b,c)", ":\n    produce M(a,d,b,c)"])
def test_rule_header_binds_original_and_produced_contexts(
    direction, first_rule, header, expected, body
):
    lsystem = Lsystem()
    lsystem.setCode(f"""
{direction}()
Axiom: A(1) B(2) C(3)
production:
{first_rule}
{header}{body}
""")

    assert lsystem.iterate() == AxialTree(expected)


def test_rule_header_requires_both_original_contexts():
    lsystem = Lsystem()
    lsystem.setCode("""
Axiom: A(1) B(2) C(3) B(4) A(5)
production:
A(a) < B(b) > C(c) --> M(a,b,c)
""")

    assert lsystem.iterate() == AxialTree("A(1) M(1,2,3) C(3) B(4) A(5)")


@pytest.mark.parametrize("body", [" -static-> M", ": @static\n    produce M"])
def test_static_rule_header_keeps_its_context(body):
    lsystem = Lsystem()
    lsystem.setCode(f"""
Axiom: A B C B
production:
A < B > C{body}
""")

    assert lsystem.iterate() == AxialTree("A M C B")


def test_new_left_context_can_precede_original_left_context_in_header():
    lsystem = Lsystem()
    lsystem.setCode("""
Axiom: A(1) B(2)
production:
A(x) --> D(x + 10)
D(d) << A(a) < B(b) --> M(a,d,b)
""")

    assert lsystem.iterate() == AxialTree("D(11) M(1,11,2)")
