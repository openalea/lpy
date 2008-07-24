from openalea.lpy import *

def test_group():
    """ Test group of rules """
    l = Lsystem('test_group.lpy')
    l.iterate()

if __name__ == '__main__':
    test_group()