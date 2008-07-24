from openalea.plantgl.all import *
from openalea.lpy import *

def test_enviro():
    """ Test whether environment module ?P works """
    l = Lsystem('p_h_enviro.lpy')
    res = l.iterate()
    assert res[1][0] == Vector3(0,0,10) and "homomorphism not applied before ?P"