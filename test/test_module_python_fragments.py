from openalea.lpy import Lsystem


def test_axiom_module_arguments_with_escaped_quotes():
    lsystem = Lsystem()
    lsystem.set(r'''
Axiom: A("double\")", 'single\')', {'key': [(1, 2)]}) B
''')

    axiom = lsystem.axiom
    assert len(axiom) == 2
    assert axiom[0][0] == 'double")'
    assert axiom[0][1] == "single')"
    assert axiom[0][2] == {"key": [(1, 2)]}


def test_production_splits_only_top_level_commas():
    lsystem = Lsystem()
    lsystem.set(r'''
Axiom: A
production:
A:
    values = (7, 8)
    produce B(dict(key=[(1, 2), (3, 4)]), ['it\'s, fine', "a\",b"], *values)
''')

    result = lsystem.iterate()
    assert len(result) == 1
    assert result[0][0] == {"key": [(1, 2), (3, 4)]}
    assert result[0][1] == ["it's, fine", 'a",b']
    assert result[0][2] == 7
    assert result[0][3] == 8


def test_production_handles_comments_and_triple_quoted_strings():
    lsystem = Lsystem()
    lsystem.set("""
Axiom: A
production:
A:
    produce B([1,  # Ignore these delimiters: ) ], "
               2], '''text "quote" with ), and ]''')
""")

    result = lsystem.iterate()
    assert result[0][0] == [1, 2]
    assert result[0][1] == 'text "quote" with ), and ]'
