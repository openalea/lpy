from openalea.lpy import *


def test_lsystem_as_module():
    lc = LsysContext()
    lc.makeCurrent()
    l = Lsystem()
    l.set("""
test1 = 1
Axiom:A
production:
A --> BA
"""
)
    assert l.test1 == 1
    l.test1 = 2
    assert l.test1 == 2
    assert l.context()['test1'] == 2
    print 'Axiom:',l.axiom 
    l.axiom = 'B'
    print l.axiom, type(l.axiom)
    assert type(l.axiom) == AxialTree and l.axiom == Lstring('B')
        
if __name__ == '__main__':
    test_lsystem_as_module()
    