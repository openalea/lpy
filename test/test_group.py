from openalea.lpy import *
from testresources import get_filename


def test_group():
    """ Test group of rules """
    l = Lsystem(get_filename('test_group.lpy'))
    l.iterate()

if __name__ == '__main__':
    test_group()