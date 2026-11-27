import pytest

from openalea.lpy import Lsystem

from testresources import assert_axial_tree


@pytest.mark.parametrize("matching_mode", [0, 1])
@pytest.mark.parametrize("has_left_context", [False, True])
def test_backward_in_left_context_stays_at_start(matching_mode, has_left_context):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", matching_mode)
    left_context = "B " if has_left_context else ""
    lsystem.setCode(f"""
backward()
Axiom: {left_context}A
production:
A:
    params = {{}}
    assert InLeftContext(B, params) == {has_left_context}
    assert InLeftContext(None, params)
    assert InLeftContext(None, params)
    assert not InLeftContext(A, params)
    produce M
""")

    assert_axial_tree(lsystem, lsystem.iterate(), f"{left_context}M")


@pytest.mark.parametrize("pattern", ["C x(B, 1) < A", "C x(B, 1) A"])
def test_backward_repetition_followed_by_required_module_at_start(pattern):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", 0)
    lsystem.setCode(f"""
backward()
Axiom: B A
production:
{pattern} --> X
x(B, 1) A --> M
""")

    assert_axial_tree(lsystem, lsystem.iterate(), "M")


@pytest.mark.parametrize("matching_mode", [0, 1, 2, 3])
def test_backward_new_right_context_reads_the_produced_string(matching_mode):
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", matching_mode)
    lsystem.setCode("""
backward()
Axiom: A R A
production:
A >> B(value) --> M(value)
R --> B(7)
""")

    assert_axial_tree(lsystem, lsystem.iterate(), "M(7) B(7) A")


def test_backward_multiscale_new_right_context_uses_predecessor_scale():
    lsystem = Lsystem()
    lsystem.context().options.setSelection("String matching", 3)
    lsystem.setCode("""
backward()
module A: scale = 1
module B: scale = 1
module I: scale = 2

Axiom: A R
production:
A >> B(value) --> M(value)
R --> I B(7)
""")

    assert_axial_tree(lsystem, lsystem.iterate(), "M(7) I B(7)")
