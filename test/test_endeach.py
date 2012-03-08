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
    return lstring

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

lcodeH = """
nbH = 0
nbWantedH = 2 # At min, one for display of axiom, one for iter 1

def check():
    assert  nbH == nbWantedH
    
Axiom: B A 

derivation length: 1
production:

A --> A

homomorphism:

B:
  global nbH
  nbH += 1
  #print getIterationNb(), nbH, nbWantedH
  assert nbH <= nbWantedH

endlsystem
"""

from plotredirection import PlotRedirection

lcodeH1 = """
def EndEach(lstring, scene):
    return lstring, scene
"""

def test_endeach_with_return_2():
    """ Test use of EndEach with return of two argument """
    p = PlotRedirection()
    l = Lsystem()
    l.set(lcodeH1+lcodeH)
    l.context()['nbWantedH'] = 2 # suppose to reuse the scene
    l.animate()
    l.context()['check']()

lcodeH2 = """
def EndEach(lstring, scene):
    return lstring
"""

def test_endeach_with_return_lstring():
    """ Test use of EndEach with return of one  lstring argument """
    p = PlotRedirection()
    l = Lsystem()
    l.set(lcodeH2+lcodeH)
    l.context()['nbWantedH'] = 3 # should recompute the scene
    l.animate()
    l.context()['check']()

lcodeH3 = """
def EndEach(lstring, scene):
    return scene
"""

def test_endeach_with_return_scene():
    """ Test use of EndEach with return of one scene argument """
    p = PlotRedirection()
    l = Lsystem()
    l.set(lcodeH3+lcodeH)
    l.context()['nbWantedH'] = 2 # suppose to reuse the scene
    l.animate()
    l.context()['check']()

lcodeH4 = """
def EndEach(lstring, scene):
    pass
"""

def test_endeach_with_no_return():
    """ Test use of EndEach with no return """
    p = PlotRedirection()
    l = Lsystem()
    l.set(lcodeH4+lcodeH)
    l.context()['nbWantedH'] = 2 # suppose to reuse the scene
    l.animate()
    l.context()['check']()


lcodeH5 = """
def EndEach(lstring, scene):
    return lstring,None
"""

def test_endeach_with_return_none():
    """ Test use of EndEach with return of none for scene"""
    p = PlotRedirection()
    l = Lsystem()
    l.set(lcodeH5+lcodeH)
    l.context()['nbWantedH'] = 3 # suppose to recompute scene
    l.animate()
    l.context()['check']()


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
