from openalea.lpy import *

def test_well_bracketed():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F][[[G]G]FF]')
    assert a.wellBracketed() == True
    a = AxialTree('F[F[F][G]][[F][[[G]G]FF]')
    assert a.wellBracketed() == False

def test_directson():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F]B[[[G]G]FF]')
    assert a.directSon(0) == 13
    #assert a.directSon(2) == -1    