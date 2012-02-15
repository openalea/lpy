from openalea.lpy import *
from openalea.lpy.composition import *
from openalea.plantgl.all import Sequencer

def Lsystem__call__(self,lstring,nbsteps=None):
    if nbsteps is None: nbsteps = self.derivationLength
    return self.derive(0,nbsteps,lstring)
Lsystem.__call__ = Lsystem__call__

def simulation():
  a = Lsystem('aging.lpy')
  b = Lsystem('auxin.lpy')
  interpretation = Lsystem('interpretation.lpy')
  a2b = Lsystem('A2B.lpy')
  b2a = Lsystem('B2A.lpy')  
  a2c = Lsystem('A2C.lpy')
  
  s = Sequencer(0.1)
  for y in xrange(200):
     lstring = a(lstring,1)
     lstring = a2b(lstring)
     #print lstring
     lstring = b(lstring)
     lstring = b2a(lstring)
     
     ilstring = a2c(lstring)     
     interpretation.plot(ilstring)
     s.touch()

def simulation2():
  a = Lsystem('aging.lpy')
  b = Lsystem('auxin.lpy')
  c = Lsystem('interpretation.lpy')
  a2b = Lsystem('A2B.lpy')
  b2a = Lsystem('B2A.lpy')  
  a2c = Lsystem('A2C.lpy')
  
  lsystem = ComposedLsystem([a,a2b,b,b2a],[a2c,c])
  lstring = lsystem.axiom
  s = Sequencer(0.1)
  for i in xrange(10):
    lstring = lsystem.derive(lstring)
    lsystem.plot(lstring)
    s.touch()


if __name__ == '__main__':
    simulation2()