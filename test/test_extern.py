from openalea.lpy import *

code = """
extern(a=1)

assert 'a' in globals()
assert 'a' in __externs__
"""

def test_extern_default():
    l = Lsystem()
    l.setCode(code+'assert a == 1')

def test_extern_externalset():
    l = Lsystem()
    l.setCode(code+'assert a == 2',parameters={'a':2})

code2 = """
extern('b')

assert 'b' in globals()
assert 'b' in __externs__
assert b == 1
"""

def test_extern_external_check():
    l = Lsystem()
    l.setCode(code2,parameters={'b':1})
