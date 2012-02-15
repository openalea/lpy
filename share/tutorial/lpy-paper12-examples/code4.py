from openalea.lpy import *
from openalea.lpy.composition import *


def simulation():
  a = Lsystem('composition/A.lpy')
  b = Lsystem('composition/B.lpy')
  c = Lsystem('composition/interpretation.lpy')
  a2b = Lsystem('composition/A2B.lpy')
  b2a = Lsystem('composition/B2A.lpy')  
  a2c = Lsystem('composition/A2C.lpy')
  
  lsystem = ComposedLsystem([a,a2b,b,b2a],[a2c,c])
  lstring = lsystem.axiom
  print lstring
  lsystem.animate(nbsteps = 10)


if __name__ == '__main__':
    simulation()