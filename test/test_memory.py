from openalea.pylsystems import *

#def test_String( nb = 1, size = 100):
#    """ Test creation of str """
#    for i in xrange(nb):
#        a = 'F'*size
#        del a

#maxsize = 10000
#maxlength = 8
maxsize = 100
maxlength = 3


def test_AT( nb = maxlength, size = maxsize):
    """ Test creation of AxialTree """
    for i in xrange(nb):
        a = AxialTree('F'*size)
        del a

def test_PAT( nb = maxlength, size = maxsize):
    """ Test creation of Parametric AxialTree """
    for i in xrange(nb):
        a = AxialTree()
        for j in xrange(size):
            a += 'F('+str(j)+')'
        del a

def test_LsRule():
    """ Test creation of LsysRule """
    res = 'F[+F]F[-F]F'
    l = LsysRule()
    l.set('F --> '+res)
    #l.compile()
    assert l.compiled(), "LsysRule.compile did not work."
    assert l.forwardCompatible() == True and l.backwardCompatible() == True 
    assert l.isContextFree() == True and l.nbContexts() == 0
    assert l.nbParameters() == 0
    assert len(l()) == len(res)
    assert len(l.process('FF')) == 2*len(res)
    assert l.match('F',0)[1] == 1
    assert l.reverse_match('FF',1)[1] == 0

def lnLs(l = 8):
    if l == 0: return 1
    elif l == 1: return 11
    else:
        res = 6
        a = 5
        for i in xrange(l-2):
            res += a*6
            a *= 5
        res += a*11
        return res
        
def test_Ls( nb = 1, length = maxlength):
    """ Test creation and execution of Lsystems """
    l = Lsystem()
    l.derivationLength = length
    l.axiom = AxialTree('F')
    l.addProdRule('F --> F[+F]F[-F]F',0)
    ln = lnLs(length)
    for i in range(nb):
        a = l.iterate()
        assert len(a) == ln
        del a
    del l

def test_PLs( nb = 1, length = maxlength):
    """ Test creation and execution of Parametric Lsystems """
    l = Lsystem()
    l.derivationLength = length
    l.axiom = AxialTree('F(0)')
    l.addProdRule('F(x) --> F(x+1)[+F(x)]F(x+1)[-F(x)]F(x+1)',0)
    ln = lnLs(length)
    for i in range(nb):
        a = l.iterate()
        assert len(a) == ln
        del a
    del l

if __name__ == '__main__':
    test_AT()
    test_PAT()
    test_LsRule()
    test_Ls()
    test_PLs()
