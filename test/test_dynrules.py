from openalea.lpy import *

code = """
def EndEach(lstring):
    print 'Lstring [',getIterationNb(),']: ',lstring

def End(lstring):
    assert len(lstring) == 1 and lstring[0] == 'C'

Axiom: A

derivation length: 2
production:

A :
    __lsystem__.addRule('''B --> C''')
    produce B

endlsystem
"""

def test_dynrule():
    ''' Test dynamicall addition of rule in the lsystem '''
    l = Lsystem()
    l.set(code)
    l.iterate()