import glob
import os

import pytest

from openalea.lpy import Lsystem

HERE = os.path.dirname(os.path.abspath(__file__))


def exec_lpy_tst(lfile):
    l = Lsystem(lfile)
    l.iterate()


@pytest.mark.parametrize('lfile', sorted(glob.glob(os.path.join(HERE, 'lpytest', '*.lpy'))),
                         ids=os.path.basename)
def test_lpy_tests(lfile):
    """ Test all lpy examples from lpytest/ repository """
    exec_lpy_tst(lfile)


def test_diese_bug():
    """ Test diese bug """
    with pytest.raises(Exception):
        exec_lpy_tst(os.path.join(HERE, 'diese_bug.lpy'))
