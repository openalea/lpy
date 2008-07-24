from openalea.lpy import *

def test_nproduce(verbose = False):
    """ Test use of nproduce """
    l=Lsystem('test_nproduce.lpy')
    if verbose: print l.axiom
    res = l.iterate(1)
    if verbose: print res
    assert len(res) == 2 and res[1].name == 'B'
    res = l.iterate(1,1,res)
    if verbose: print res
    assert len(res) == 1 and res[0].name == 'C'
    res = l.iterate(2,1,res)
    if verbose: print res
    assert len(res) == 1 and res[0].name == 'D'
    res = l.iterate(3,1,res)
    if verbose: print res
    assert len(res) == 0 

if __name__ == '__main__':
    import sys
    if '-v' in sys.argv or '--verbose' in sys.argv :
        verbose = True
    else:
        verbose = False
    test_nproduce(verbose)