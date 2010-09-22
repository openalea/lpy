from openalea.lpy import *

lcode = """
from openalea.plantgl.all import *

nbiter = 3

def EndEach(lstring,scene):
    assert isAnimationEnabled() == animation
    if not isFrameDisplayed(): assert scene is None
    if isAnimationEnabled()  : assert isFrameDisplayed()
    else : assert isFrameDisplayed() == (getIterationNb() == (nbiter-1))
    
Axiom: A
derivation length: nbiter
production:
A --> A
endlsystem
"""

from plotredirection import PlotRedirection

def test_framedisplayed_in_run():
    """ Test isFrameDisplayed value in case of run """
    l = Lsystem()
    l.set(lcode)
    l.context()['animation'] = False
    l.iterate()

def test_framedisplayed_in_animate():
    """ Test isFrameDisplayed value in case of animate """
    p = PlotRedirection()
    l = Lsystem()
    l.set(lcode)
    l.context()['animation'] = True
    l.animate()
