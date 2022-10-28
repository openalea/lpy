from openalea.lpy import *

code = """
Axiom: A(0)
production:
interpretation:
A(x): produce F(x)@0(2*x)
"""
def test_interpret():
    l = Lsystem()
    l.setCode(code)
    ls = l.interpret(l.axiom)