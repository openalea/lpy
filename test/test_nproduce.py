from openalea.lpy import *
from testresources import get_filename

def test_nproduce(verbose = False):
    """ Test use of nproduce """
    l=Lsystem(get_filename('test_nproduce.lpy'))
    if verbose: print l.axiom
    res = l.derive(1)
    if verbose: print res
    assert len(res) == 2 and res[1].name == 'B'
    res = l.derive(res,1,1)
    if verbose: print res
    assert len(res) == 1 and res[0].name == 'C'
    res = l.derive(res,2,1)
    if verbose: print res
    assert len(res) == 1 and res[0].name == 'D'
    res = l.derive(res,3,1)
    if verbose: print res
    assert len(res) == 0 

if __name__ == '__main__':
    import sys
    if '-v' in sys.argv or '--verbose' in sys.argv :
        verbose = True
    else:
        verbose = False
    test_nproduce(verbose)