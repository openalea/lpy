from openalea.lpy import *

def test_modclasstable():
    """ Test access to module class table """
    cl = ModuleClassTable.get().getClasses()
    ncl = ModuleClassTable.get().getNames()
    nclnb2 = sum([1+len(m.aliases) for m in cl])
    assert len(ncl) == nclnb2
    ids = [i.id for i in ModuleClassTable.get().getClasses()]
    ids.sort()
    #cl.sort(lambda x,y : cmp(x.id,y.id))
    #print cl
    assert ids == range(len(ids)) and "All predefined modules are not registered or other modules are still registered"

lcode1 = """
module BABA

Axiom: BABA
"""

lcode2 = """
module BA

Axiom: BABA
"""

def test_modnamespace():
    """ Test creation of axiom with declared module name and lsyscontext namespace of module declaration """
    l1 = Lsystem()
    l1.set(lcode1)
    assert len(l1.axiom) == 1
    l2 = Lsystem()
    l2.set(lcode2)    
    assert len(l2.axiom) == 2


lcode3 = """
matched = False
def end():
    assert matched

module BABA,DADA


Axiom: BABA
production:
BABA :
    global matched
    matched = True
    produce DADA
"""

def test_querymodname():
    """ Test creation of lstring pattern with declared module name """
    l = Lsystem()
    l.set(lcode3)
    r = l.iterate()
    assert len(r) == 1

def test_nodeclaration():
    """ Test mandatory module declaration """
    l = Lsystem()
    l.context().options.setSelection('Module declaration','Mandatory')
    try:
        l.set('Axiom: H')
        failed = False
    except:
        failed = True
    assert failed and "Mandatory module declaration do not work"
    
def test_nodeclaration_withdeclaredmodule():
    """ Test mandatory module declaration with module declared in another namespace """
    lp = Lsystem()
    #lp.set('Axiom: H')
    lp.context().declare('H')
    l = Lsystem()
    l.context().options.setSelection('Module declaration','Mandatory')
    try:
        l.set('Axiom: H')
        failed = False
    except:
        failed = True
    assert failed and "Mandatory module declaration do not work"
    
def test_nodeclaration_withgloballydeclaredmodule():
    """ Test mandatory module declaration with module declared in python namespace """
    lc = LsysContext()
    lc.declare('H')
    lc.makeCurrent()
    l = Lsystem()
    l.context().options.setSelection('Module declaration','Mandatory')
    try:
        l.set('Axiom: H')
        failed = False
    except:
        failed = True
    assert failed and "Mandatory module declaration do not work"
    #lc.done()
 
def test_alias_declared():
    """ Test name of module originally declared as alias """
    a = AxialTree('GetUp')
    assert len(a) == 1
    a = AxialTree('repexp')
    assert len(a) == 1 and a == AxialTree('x')


def test_scale_declaration():
    """ Test if we had a scale property declaration to a class """
    defaultscale = ModuleClass.DEFAULT_SCALE
    a = AxialTree('T')
    assert a[0].mclass.scale == defaultscale
    a[0].mclass.scale = 5
    assert a[0].mclass.scale == 5
    l = LsysContext()
    l.makeCurrent()
    b = AxialTree('T')
    assert b[0].mclass.scale == defaultscale
    l.done()
    assert b[0].mclass.scale == 5
 
 
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
