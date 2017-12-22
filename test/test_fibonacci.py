from openalea.lpy import *
from testresources import get_filename


def test_backward():
    """ Computation of the fibonnacci series using fast transfer in backward direction """
    l = Lsystem(get_filename('fibonacci.lpy'))
    a = l.iterate()
    print a
    assert a[2][0] == 6765, "Lpy failed to compute fibonacci test"
    assert a[3][0] == 4181, "Lpy failed to compute fibonacci test"
    assert a[21][0] == 1, "Lpy failed to compute fibonacci test"

if __name__ == '__main__':
    test_backward()
