from openalea.lpy import *

def test_axiom():
    l = Lsystem()
    l.setCode('''
module AA
Axiom: AA
''')
    print('Axiom:',l.axiom)
    assert (len(l.axiom) == 1)


if __name__ == '__main__':
    test_axiom()
