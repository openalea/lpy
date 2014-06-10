from openalea.lpy import *

lcode1 = """
VAR = True
def Start():
  global VAR
  assert VAR

Axiom: A(VAR)
"""


def test_namespace_at_creation():
   l = Lsystem()
   d = {'VAR':True}
   l.setCode(lcode1,d)
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
   l.setCode(code)
   l.context().updateNamespace({'VAR':True})
   l.iterate()

def test_to_failed_namespace_at_execution():
    l = Lsystem()
    l.setCode(code,{'VAR':True})
    try:
       l.iterate()
       assert False and 'Has not failed'
    except:
       pass
   
if __name__ == '__main__':
    test_namespace_at_creation()
    test_namespace_at_execution()
    test_to_failed_namespace_at_execution()