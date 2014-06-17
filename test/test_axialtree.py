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

def test_substring():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F][[[G]G]FF]')
    assert a[0].name == 'F'
    assert a[-2].name == 'F'
    assert a[-3:-1] == AxialTree('FF')
    assert a[-3:] == AxialTree('FF]')

def test_sons():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F]B[[[G]G]FF]')
    assert a.sons(0) == [2,11,13]
    assert a.sons(2) == [4,7]
    assert a.sons(4) == None

def test_sons2():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F]F')
    assert a.sons(0) == [2,4]
    assert a.sons(2) == None
    assert a.sons(4) == None

def test_directson():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('F[F[F][G]][F]B[[[G]G]FF]')
    assert a.directSon(0) == 13
    assert a.directSon(2) == None
    assert a.directSon(13) == None

   
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
    assert a.complex(5) == 0

def test_successor_at_level():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('BA[A[A][CA]][A]B[[[CA]CA]AA]')
    l.setModuleScale('B,C',1)
    l.setModuleScale('A',2)
    print a.successor_at_level(0,1)
    assert a.successor_at_level(0,1) == 15
    

def test_successor_at_scale():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('BA[A[A][CA]][A]B[[[CA]CA]AA]ABA')
    l.setModuleScale('B,C',1)
    l.setModuleScale('A',2)
    assert a.successor_at_scale(0,1) == 15
    assert a.successor_at_scale(15,1) == 29
    a = AxialTree('BA[[A][CA][A]A]BA[[[CA]CA]AA]')
    print a.directSon(1),a.directSon(15),a.successor_at_scale(1,2)
    assert a.successor_at_scale(1,2) == 16

def test_successor_at_scale2():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('ELU(0,0)I(0,0)I(1,1)I(0,2)U(1,1)I(0,3)I(0,4)I(0,5)U(0,2)I(0,6)I(0,7)I(0,8)EL')
    l.setModuleScale('U,E',1)
    l.setModuleScale('I,L',2)
    assert a.successor_at_scale(2,1) == 6
    assert a.successor_at_scale(10,1) == 14
    
def test_predecessor_at_scale():
    l = LsysContext()
    l.makeCurrent()
    a = AxialTree('BA[A[A][CA]][A]B[[[CA]CA]AA]')
    l.setModuleScale('B,C',1)
    l.setModuleScale('A',2)
    assert a.predecessor_at_scale(15,1) == 0
    print a.predecessor_at_scale(25,2)
    assert a.predecessor_at_scale(25,2) == 1
  

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
  