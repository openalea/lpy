from openalea.lpy import *

def test_stringmatching():
    """ Test StringMatching structure """
    a = StringMatching()
    a.addIdentity(2)
    a.addIdentity(3)
    a.append(1,5)
    a.addIdentity(1)
    a.append(1,2)
    b = a.begin()
    for i in xrange(15):
        b.nextValues()
    assert b.values() == (10,15)