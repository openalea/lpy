from openalea.lpy import *

lsysbasecode = '''
module U,E : scale = 1 
module I,L : scale = 2 
Axiom: 
  nproduce E L U I I I 
  for i in xrange(1):
    nproduce U I I I
  nproduce E L
'''
#        0  1  2  3  4  5  6    7   8  9   10    11
#        E  L  U  I  I  I  U    I   I  I   E     L
res_1 = [2, 2, 6, 6, 6, 6, 10, 10, 10, 10, None, None]
res_2 = [1, 3, 3, 4, 5, 7, 7,   8, 9,  11, 11,   None]


def i_successor_at_scale(i):
    l = Lsystem()
    l.setCode(lsysbasecode)
    l.makeCurrent()
    lstring = l.axiom
    m = lstring[i]
    pred = lstring.successor_at_scale(i,1)
    pred2 = lstring.successor_at_scale(i,2)
    res = i,m,pred,(lstring[pred] if not pred is None else pred),pred2,(lstring[pred2] if not pred2 is None else pred2)
    l.done()
    return res



def test_successor_at_scale(assertion = True):
    l = Lsystem()
    l.setCode(lsysbasecode)
    l.makeCurrent()
    lstring = l.axiom
    for i,m in enumerate(lstring):
        pred = lstring.successor_at_scale(i,1)
        pred2 = lstring.successor_at_scale(i,2)
        if assertion:
            assert pred == res_1[i] and pred2 == res_2[i]
        else :
          print i,m,
          print '\t',pred,
          if not pred is None: print lstring[pred],
          print '\t',pred == res_1[i],
          print '\t',pred2,
          if not pred2 is None: print lstring[pred2],
          print '\t',pred2 == res_2[i]

if __name__ == '__main__':
    test_successor_at_scale(False)
