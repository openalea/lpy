from openalea.lpy import *
from openalea.plantgl.all import Scene

def test_addHomRule():
    """ Regression around 010708 """
    s="PANNN[+ANNN][-ANNN]AN"
    l = Lsystem()
    tree = AxialTree(s) 
    l.addInterpretationRule('N --> F', 0)
    geom_tree = l.interpret(tree)
    scene = l.sceneInterpretation(geom_tree)
    assert len(scene) == 10, "len(scene) == %d instead of 10"%len(scene)
    undeclare('P,A,N')
    assert len(LsysContext.currentContext().declaredModules()) == 0

if __name__ == '__main__':
    test_addHomRule()

