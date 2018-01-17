from openalea.lpy import *
from testresources import get_filename

def test_multi_line():
    l = Lsystem(get_filename('test_multiline_produce.lpy'))
    l.iterate()