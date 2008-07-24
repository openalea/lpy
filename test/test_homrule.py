from openalea.pylsystems import *
from openalea.plantgl.all import Scene

def test_addHomRule():
    """ Regression around 010708 """
    s="PANNN[+ANNN][-ANNN]AN"
    tree = AxialTree(s) 
    l = Lsystem()
    l.addHomRule('N --> F', 0)
    geom_tree = l.homomorphism(tree)
    scene = l.sceneInterpretation(geom_tree)
    assert len(scene) == 10, "len(scene) == %d instead of 10"%len(scene)

if __name__ == '__main__':
    test_addHomRule()

