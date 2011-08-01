from openalea.lpy import *
from openalea.plantgl.all import Sequencer

def Lsystem__call__(self,lstring,nbsteps=None):
    if nbsteps is None: nbsteps = self.derivationLength
    return self.derive(0,nbsteps,lstring)
Lsystem.__call__ = Lsystem__call__

def simulation():
  aging = Lsystem('aging.lpy')
  auxin = Lsystem('auxin.lpy')
  interpretation = Lsystem('interpretation.lpy')
  a2b = Lsystem('A2B.lpy')
  b2a = Lsystem('B2A.lpy')  
  a2c = Lsystem('A2C.lpy')
  lstring = aging.axiom
  s = Sequencer(0.1)
  for y in xrange(200):
     lstring = aging(lstring,1)
     lstring = a2b(lstring)
     #print lstring
     lstring = auxin(lstring)
     lstring = b2a(lstring)
     ilstring = a2c(lstring)
     interpretation.plot(ilstring)
     s.touch()

if __name__ == '__main__':
    simulation()