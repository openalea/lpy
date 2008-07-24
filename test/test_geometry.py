from openalea.lpy import *
from openalea.plantgl.all import *


def get_primitive(g):
    while isinstance(g,Transformed):
        g = g.geometry
    return g

lcode_cyl = """
Axiom: F
"""

def test_cylinder():
    """ Test creation of a cylinder with pylsystems """
    l = Lsystem()
    l.set(lcode_cyl)
    a = l.axiom
    sc = l.sceneInterpretation(a)
    if len(sc) != 1 :
        raise Exception("Invalid size of scene : %i instead of 1." % len(sc))
    g = get_primitive(sc[0].geometry)
    if type(g) != Cylinder:
        raise Exception ("Type of shape is %s" % g.__class__.__name__)

lcode_sf = """
Axiom: L
production:
homomorphism:
L --> ~l(1)
endlsystem
"""

def test_surface():
    """ Test creation of a surface with pylsystems """
    l = Lsystem()
    l.set(lcode_sf)
    a = l.axiom
    sc = l.sceneInterpretation(a)
    if len(sc) != 1 :
        raise Exception("Invalid size of scene : %i instead of 1." % len(sc))
    g = get_primitive(sc[0].geometry)
    if type(g) != TriangleSet:
        raise Exception ("Type of shape is %s" % g.__class__.__name__)

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
