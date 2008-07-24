from openalea.lpy import *

code = """
l = None
started = False

def Start():
    global started
    started = True
    assert not l is None
    assert l.isRunning()
    canIterate = False
    try:
        l.iterate()
        canIterate = True
    except:
        pass
    assert canIterate == False and 'Can call recursivelly iterate on the same Lsystem'

Axiom:A
production:
A --> A
endlsystem
"""

def test_recursecall():
    """ Test blocking of Lsystem when recursivelly called (multithread support)."""
    l = Lsystem()
    l.set(code)
    l.context()['l'] = l
    l.iterate() 
    assert l.context()['started'] == True
    l.clear()

if __name__ == '__main__':
    test_recursecall()