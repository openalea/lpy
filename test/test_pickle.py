from openalea.lpy import *
import pickle

from testresources import active_context, get_filename

def te_st_pickle_lsystem():
    l = Lsystem(get_filename("../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy"))
    buffer = pickle.dumps(l)
    print(buffer)
    nl = pickle.loads(buffer)
    assert l.axiom == nl.axiom

def test_pickle_lstring_as_str():
    l = Lsystem(get_filename("../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy"))
    ls = l.derive(10)
    buffer = pickle.dumps(str(ls))
    print(buffer)
    nls = pickle.loads(buffer)
    assert str(ls) == nls

def test_pickle_lstring():
    l = Lsystem(get_filename("../share/tutorial/04 - simple-plant-archi/02 - random-tree.lpy"))
    ls = l.derive(10)
    buffer = pickle.dumps(ls)
    print(buffer)
    with active_context(l.context()):
        nls = pickle.loads(buffer)
    assert ls == nls

def test_pickle_axial_tree_preserves_named_modules_and_python_arguments():
    context = LsysContext()
    with active_context(context):
        context.declare("PicklePayload")
        payload = {"text": "quotes, [brackets] and \n", "values": [1, None]}
        module = ParamModule("PicklePayload")
        module.append(("pair", 2))
        module.append(payload)
        tree = AxialTree("[")
        tree.append(module)
        tree.append(module)
        tree.append(AxialTree("]"))

        restored = pickle.loads(pickle.dumps(tree))
        assert pickle.loads(pickle.dumps(AxialTree())) == AxialTree()

    assert [item.name for item in restored] == ["[", "PicklePayload", "PicklePayload", "]"]
    assert restored[1].args == [("pair", 2), payload]
    assert restored[1][1] is restored[2][1]


if __name__ == '__main__':
    test_pickle_lstring()