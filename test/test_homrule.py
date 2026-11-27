from openalea.lpy import AxialTree, Lsystem

from testresources import active_context


def test_addHomRule():
    lsystem = Lsystem()
    context = lsystem.context()
    with active_context(context):
        tree = AxialTree("PANNN[+ANNN][-ANNN]AN")
    lsystem.addInterpretationRule("N --> F", 0)
    interpreted = lsystem.interpret(tree)
    assert len(lsystem.sceneInterpretation(interpreted)) == 10
    with active_context(context):
        context.undeclare("P,A,N")
        assert len(context.declaredModules()) == 0


if __name__ == '__main__':
    test_addHomRule()
