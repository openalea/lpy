import glob
import os

import pytest

from openalea.lpy import Lsystem

HERE = os.path.dirname(os.path.abspath(__file__))
EXAMPLES = sorted(glob.glob(os.path.join(HERE, os.path.pardir, 'share', 'examples', '*.lpy')))


@pytest.mark.parametrize('lfile', EXAMPLES, ids=os.path.basename)
def test_share_examples(lfile):
    """ Test all lpy examples from share/ repository """
    l = Lsystem(lfile)
    l.iterate()
