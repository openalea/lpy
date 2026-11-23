import pytest

from openalea.lpy import AxialTree, Lsystem


@pytest.mark.parametrize("matching_mode", [0, 1, 2, 3])
@pytest.mark.parametrize("direction", ["forward", "backward"])
def test_cyclic_contexts_cross_both_ends(matching_mode, direction):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", matching_mode)
    lsystem.setCode(f"""
{direction}()
SetCyclicLsystem(True)
Axiom: A(2) B C(3)
production:
C(left) < A(value) --> L(left + value)
C(value) > A(right) --> R(value + right)
""")

    assert lsystem.iterate() == AxialTree("L(5) B R(5)")


def test_set_cyclic_is_local_and_can_be_disabled():
    code = """
Axiom: A B
production:
B < A --> L
B > A --> R
"""
    lsystem = Lsystem()
    other = Lsystem()
    lsystem.setCode(code)
    other.setCode(code)

    assert lsystem.iterate() == AxialTree("A B")
    lsystem.setCyclic(True)
    assert lsystem.iterate() == AxialTree("L R")
    assert other.iterate() == AxialTree("A B")
    lsystem.setCyclic(False)
    assert lsystem.iterate() == AxialTree("A B")


@pytest.mark.parametrize("matching_mode", [0, 1])
def test_cyclic_dynamic_contexts_continue_across_the_seam(matching_mode):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", matching_mode)
    lsystem.setCode("""
def StartEach():
    SetCyclicLsystem(True)

Axiom: B A C
production:
B < A > C:
    params = {}
    assert InLeftContext(C, params)
    assert InLeftContext(A, params)
    assert not InLeftContext(C, params)
    assert InRightContext(B, params)
    assert InRightContext(A, params)
    assert not InRightContext(C, params)
    produce M
""")

    assert lsystem.iterate() == AxialTree("B M C")


@pytest.mark.parametrize("matching_mode", [0, 1])
def test_cyclic_contexts_stop_after_one_revolution(matching_mode):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", matching_mode)
    lsystem.setCode("""
SetCyclicLsystem(True)
Axiom: A
production:
A A < A --> X
A > A A --> Y
A < A > A --> M
""")

    assert lsystem.iterate() == AxialTree("M")


def test_cyclic_context_keeps_strict_predecessors_linear():
    lsystem = Lsystem()
    lsystem.setCode("""
SetCyclicLsystem(True)
Axiom: A B
production:
B A --> X
B < A B > A --> Y
""")

    assert lsystem.iterate() == AxialTree("Y")


def test_cyclic_axial_context_skips_branches_at_the_seam():
    lsystem = Lsystem()
    lsystem.setCode("""
SetCyclicLsystem(True)
Axiom: A C [ I ]
production:
C < A --> L
C > A --> R
""")

    assert lsystem.iterate() == AxialTree("L R [ I ]")


@pytest.mark.parametrize("direction, rule", [
    ("forward", "B << A --> X"),
    ("backward", "B >> A --> X"),
])
def test_cyclic_mode_keeps_new_contexts_linear(direction, rule):
    lsystem = Lsystem()
    lsystem.setCode(f"""
{direction}()
SetCyclicLsystem(True)
Axiom: A B
production:
{rule}
""")

    assert lsystem.iterate() == AxialTree("A B")


def test_model_can_disable_cyclic_contexts():
    lsystem = Lsystem()
    lsystem.setCode("""
SetCyclicLsystem(True)

def StartEach():
    SetCyclicLsystem(False)

Axiom: A B
production:
B < A --> X
""")

    assert lsystem.iterate() == AxialTree("A B")


@pytest.mark.parametrize("matching_mode", [0, 1])
def test_cyclic_repetitions_respect_the_context_limit(matching_mode):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", matching_mode)
    lsystem.setCode("""
SetCyclicLsystem(True)
Axiom: A
production:
A x(A, 1) < A --> X
A > x(A, 1) A --> Y
x(A, 1) < A > x(A, 1) --> M
""")

    assert lsystem.iterate() == AxialTree("M")
