from openalea.lpy import *


def test_iterator():
    l = Lsystem()
    l.set("""
Axiom:A
derivation length: 4
production:
A --> BA
"""
)
    for lstring in l:
        print lstring

        
if __name__ == '__main__':
    test_iterator()
    