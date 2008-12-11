from openalea.lpy import *
from openalea.plantgl.all import *


def sc2dict(s):
    d = {}
    for i in s:
        if not d.has_key(i.id):
            d[i.id] = []
        d[i.id].append(i)
    return d


lcode = """
Axiom:A
production:
A --> A[+A]A[-A]
homomorphism:
maximum depth:2
A --> BCB
B --> F
C --> f@Of
endlsystem
"""


def test_generateScene():
    """ Test Lsystem generation of a scene using homomorphism """
    l = Lsystem()
    l.set(lcode)
    a = l.iterate()
    sc = l.sceneInterpretation(a)
    assert len(sc) == 4*3
    d = sc2dict(sc)
    assert len(d) == 4

if __name__ == '__main__':
    test_generateScene()
