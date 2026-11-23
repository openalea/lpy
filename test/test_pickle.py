from openalea.lpy import *
import pickle

def te_st_pickle_lsystem():
    l = Lsystem(r'../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy')
    buffer = pickle.dumps(l)
    print(buffer)
    nl = pickle.loads(buffer)
    assert l.axiom == nl.axiom

def test_pickle_lstring_as_str():
    l = Lsystem(r'../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy')
    ls = l.derive(10)
    buffer = pickle.dumps(str(ls))
    print(buffer)
    nls = pickle.loads(buffer)
    assert str(ls) == nls

def test_pickle_lstring():
    l = Lsystem(r'../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy')
    ls = l.derive(10)
    buffer = pickle.dumps(ls)
    print(buffer)
    nls = pickle.loads(buffer)
    assert str(ls) == str(nls)

if __name__ == '__main__':
    test_pickle_lstring()