from openalea.lpy import *

class A:
  def __init__(self):
    pass

def test_repr():
    l = LsysContext()
    l.makeCurrent()
    m = ParamModule('B',A())
    a = AxialTree([m])
    print a
    
def test_well_bracketed():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F][[[G]G]FF]')
    assert a.wellBracketed() == True
    a = AxialTree('F[F[F][G]][[F][[[G]G]FF]')
    assert a.wellBracketed() == False

def test_directson():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F]B[[[G]G]FF]')
    assert a.directSon(0) == 13
    assert a.directSon(2) == -1
    assert a.directSon(13) == -1
    
def test_father():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F]B[[[G]G]FF]')
    assert a.father(13) == 0
    assert a.father(2) == 0
    assert a.father(17) == 13     
    assert a.father(21) == 13     
   
def test_complex():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('BA[A[A][CA]][A]B[[[CA]CA]AA]')
    l.setModuleScale('A',1)
    l.setModuleScale('B,C',2)
    print a[0].scale(),a.complex(5)
    assert a.complex(5) == 0
  

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
  