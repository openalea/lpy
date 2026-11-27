import pytest

from openalea.lpy import Lsystem

from testresources import assert_axial_tree


@pytest.mark.parametrize("in_production", [False, True])
@pytest.mark.parametrize("axiom_keyword", ["Axiom", "axiom"])
def test_derivation_length_directive_evaluates_python(in_production, axiom_keyword):
    lsystem = Lsystem()
    directive = "derivation length: steps + 1"
    before = "" if in_production else directive
    after = directive if in_production else ""
    lsystem.setCode(f"""
steps = 2
{axiom_keyword}: A(0)
{before}
production:
{after}
A(x) --> A(x + 1)
""")

    assert lsystem.derivationLength == 3
    assert_axial_tree(lsystem, lsystem.iterate(), "A(3)")


@pytest.mark.parametrize("section", ["interpretation", "homomorphism"])
def test_section_directives_with_function_axiom_and_depths(section):
    lsystem = Lsystem()
    lsystem.setCode(f"""
Axiom:
    nproduce A
derivation length: 1
production:
A --> B
decomposition:
maximum depth: 1 + 1
B --> C
C --> D
D --> E
{section}:
maximum depth: 1
D --> F
F --> G
endlsystem
after_rules = 42
""")

    result = lsystem.iterate()
    assert_axial_tree(lsystem, result, "D")
    assert_axial_tree(lsystem, lsystem.interpret(result), "F")
    assert lsystem.context()["after_rules"] == 42


def test_group_and_endgroup_select_rules():
    lsystem = Lsystem()
    lsystem.setCode("""
def Start():
    useGroup(2)
Axiom: A B
production:
group 1:
A --> C
group 1 + 1:
A --> D
endgroup
B --> E
""")

    assert_axial_tree(lsystem, lsystem.iterate(), "D E")


@pytest.mark.parametrize("directive", ["ignore: X", "consider: A B"])
def test_context_directives_apply_and_reset_per_rule(directive):
    lsystem = Lsystem()
    lsystem.setCode(f"""
Axiom: A X B A X C
production:
{directive}
A < B --> D
{directive.split(':')[0]}:
A < C --> E
""")

    assert_axial_tree(lsystem, lsystem.iterate(), "A X D A X C")


@pytest.mark.parametrize("directive", ["ignore: X", "consider: A B"])
def test_context_directives_before_production_apply_to_rules(directive):
    lsystem = Lsystem()
    lsystem.setCode(f"""
Axiom: A X B
{directive}
production:
A < B --> C
""")

    assert_axial_tree(lsystem, lsystem.iterate(), "A X C")


def test_lpyimport_loads_rules_and_python_namespace(tmp_path):
    imported = tmp_path / "imported.lpy"
    imported.write_text("""
imported_value = 7
production:
A --> B(2)
""")
    lsystem = Lsystem()
    lsystem.setCode(f"""
lpyimport {imported.with_suffix("")}
Axiom: A
derivation length: 2
production:
B(value) --> C(value + imported_value)
""")

    assert lsystem.context()["imported_value"] == 7
    assert_axial_tree(lsystem, lsystem.iterate(), "C(9)")


def test_pastefile_expands_directives_before_parsing(tmp_path):
    source = tmp_path / "directives.lpy"
    source.write_text("Axiom: A(4)\nderivation length: 2\n")
    lsystem = Lsystem()
    lsystem.setCode(f"""
%pastefile {source}
production:
A(value) --> A(value + 1)
""")

    assert lsystem.derivationLength == 2
    assert_axial_tree(lsystem, lsystem.iterate(), "A(6)")
