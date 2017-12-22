from openalea.lpy import *


def get_filename(fname, pymodname = __file__):
    import os
    return os.path.join(os.path.dirname(pymodname), fname)

def test_group():
    """ Test group of rules """
    l = Lsystem(get_filename('test_group.lpy'))
    l.iterate()

if __name__ == '__main__':
    test_group()