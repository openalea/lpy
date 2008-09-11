from openalea.lpy import *

def isLstring(lstring):
    return type(lstring) == AxialTree

lcode = """
from openalea.plantgl.all import *

ok = False

def End():
    assert ok == True
    
Axiom: A
production:
A --> A
endlsystem
"""

lcode1 = """
def EndEach():
    global ok
    ok = True
"""

def test_simple_endeach():
    """ Test use of EndEach without argument """
    l = Lsystem()
    l.set(lcode1+lcode)
    l.iterate()

lcode2 = """
def EndEach(lstring):
    assert isLstring(lstring)
    global ok
    ok = True
"""

def test_endeach_with_one_arg():
    """ Test use of EndEach with one argument """
    l = Lsystem()
    l.set(lcode2+lcode)
    l.context()['isLstring'] = isLstring
    l.iterate()

lcode3 = """
def EndEach(lstring, scene):
    assert isLstring(lstring)
    assert type(scene) == Scene
    global ok
    ok = True
"""

def test_endeach_with_two_args():
    """ Test use of EndEach with two argument """
    l = Lsystem()
    l.set(lcode3+lcode)
    l.context()['isLstring'] = isLstring
    l.iterate()

lcode4 = """
def EndEach(lstring):
    assert len(lstring) == getIterationNb()+1
    lstring += 'B'
    global ok
    ok = True

derivation length : 5
"""
    
def test_endeach_lstring_modif():
    """ Test modification of lstring in EndEach """
    l = Lsystem()
    l.set(lcode4+lcode)
    l.iterate()
    
lcode5 = """
class A:
  def __init__(self):
    pass

def EndEach(lstring):
    str(lstring)
    repr(lstring)

Axiom: B(A())

derivation length: 1
production:

B(a) --> B(a)

homomorphism:


endlsystem
"""

def test_endeach_print_lstring():
    """ Test print of lstring in EndEach """
    l = Lsystem()
    l.set(lcode5)
    l.iterate()

if __name__ == '__main__':
    import traceback as tb
    test_func = [ (n,v) for n,v in globals().items() if 'test' in n]
    test_func.sort(lambda x,y : cmp(x[1].func_code.co_firstlineno,y[1].func_code.co_firstlineno))
    for tfn,tf in test_func:
        print tfn
        try:
            tf()
        except:
            tb.print_exc()
