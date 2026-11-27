from openalea.lpy import LsysContext, Lsystem


def test_module_declarations_keep_parameters_aliases_and_trailing_comma():
    lsystem = Lsystem()
    lsystem.setCode("""
module GrammarNode(age, height), GrammarAlias=GrammarNode, # declaration list
Axiom: GrammarAlias(1, 2)
""")

    node = lsystem.context()["GrammarNode"]
    assert node.id == lsystem.context()["GrammarAlias"].id
    assert list(node.parameterNames) == ["age", "height"]
    assert len(lsystem.axiom) == 1
    assert [lsystem.axiom[0][0], lsystem.axiom[0][1]] == [1, 2]


def test_module_attributes_keep_nested_commas_and_inheritance():
    lsystem = Lsystem()
    lsystem.setCode("""
module GrammarBase(value) : scale = int(max((1, 3))) # metadata
module GrammarChild(extra), GrammarSibling : base = GrammarBase
Axiom: GrammarChild(1, 2)
""")

    context = lsystem.context()
    context.makeCurrent()
    try:
        base = context["GrammarBase"]
        assert base.scale == 3
        for name in ("GrammarChild", "GrammarSibling"):
            module = context[name]
            assert module.scale == 3
            assert [parent.id for parent in module.bases] == [base.id]
    finally:
        context.done()


def test_context_declare_uses_the_same_module_declaration_grammar():
    context = LsysContext()
    context.declare("GrammarContextNode(value), GrammarContextEmpty(),")
    assert {module.name: list(module.parameterNames) for module in context.declaredModules()} == {
        "GrammarContextNode": ["value"],
        "GrammarContextEmpty": [],
    }


def test_alias_allows_spaces_after_equals():
    lsystem = Lsystem()
    lsystem.setCode("""
module GrammarTarget(value), GrammarSpacedAlias = GrammarTarget
Axiom: GrammarSpacedAlias(7)
""")

    assert lsystem.context()["GrammarSpacedAlias"].id == lsystem.context()["GrammarTarget"].id
    assert lsystem.axiom[0][0] == 7
