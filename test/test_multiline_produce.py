from openalea.lpy import *

def test_multi_line():
    l = Lsystem('test_multiline_produce.lpy')
    l.iterate()