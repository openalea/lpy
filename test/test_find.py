from openalea.lpy import *

def test_find():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('ABAABBACBABA')
    assert a.find('C') == 7
