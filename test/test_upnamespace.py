from openalea.lpy import *

code = """
def Start():
  global VAR
  assert VAR

Axiom: A(VAR)
"""


def test_namespace_at_creation():
   l = Lsystem()
   d = {'VAR':True}
   l.set(code,d)
   l.iterate()

   l = Lsystem()
   l.set(code,d)
   l.iterate()

code = """
VAR = False

def Start():
  global VAR
  assert VAR

Axiom: A(VAR)
"""

def test_namespace_at_execution():
   l = Lsystem()
   l.set(code)
   l.context().updateNamespace({'VAR':True})
   l.iterate()

def test_to_failed_namespace_at_execution():
    l = Lsystem()
    l.set(code,{'VAR':True})
    try:
       l.iterate()
       assert False and 'Has not failed'
    except:
       pass
   
