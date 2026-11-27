from openalea.lpy import Lsystem


def test_module_string_resolves_longest_active_name_in_each_context():
    long_names = Lsystem()
    long_names.setCode("""
module SyntaxShoot, SyntaxShootT
Axiom: SyntaxShootT(2)SyntaxShoot(3)
production:
SyntaxShootT(value) --> SyntaxShoot(value + 10)
SyntaxShoot(value) --> SyntaxShootT(value + 20)
""")

    short_names = Lsystem()
    short_names.setCode("""
module SyntaxShoot
Axiom: SyntaxShootT(2)
""")

    assert [module.name for module in short_names.axiom] == ["SyntaxShoot", "T"]
    assert [module.name for module in long_names.axiom] == ["SyntaxShootT", "SyntaxShoot"]
    result = long_names.iterate()
    assert [(module.name, module[0]) for module in result] == [
        ("SyntaxShoot", 12),
        ("SyntaxShootT", 23),
    ]


def test_pattern_capture_and_production_variable_use_same_module():
    lsystem = Lsystem()
    lsystem.setCode("""
module SyntaxPayload(value)
Axiom: SyntaxPayload(5)
production:
$SyntaxPayload(captured):
    produce B $captured C
""")

    result = lsystem.iterate()
    assert [module.name for module in result] == ["B", "SyntaxPayload", "C"]
    assert result[1][0] == 5


def test_surface_label_and_explicit_arguments_in_module_strings():
    lsystem = Lsystem()
    lsystem.setCode("""
Axiom: ~l(2)~('r', 3)
production:
~(label, size) --> ~s(size + 1)
""")

    assert [(module[0], module[1]) for module in lsystem.axiom] == [("l", 2), ("r", 3)]
    result = lsystem.iterate()
    assert [(result[i][0], result[i][1]) for i in range(len(result))] == [("s", 3), ("s", 4)]
