from openalea.lpy import *
import pickle

def test_pickle_lsystem():
    l = Lsystem(r'../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy')
    ls = l.derive(10)
    buffer = pickle.dumps(l)
    print(buffer)
    nl = pickle.loads(buffer)
    assert l.derive(ls,1) == nl.derive(ls,1)

def test_pickle_lstring():
    l = Lsystem(r'../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy')
    ls = l.derive(10)
    buffer = pickle.dumps(ls)
    print(buffer)
    nls = pickle.loads(buffer)
    assert str(ls) == str(nls)

if __name__ == '__main__':
    test_pickle_lstring()