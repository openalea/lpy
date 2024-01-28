import json
from treex import *

def tree_to_json( t , path ):
    d = t.dict_of_ids_with_attributes()
    d2 = {}
    for a in d.keys():
        d2_a = {}

        d2_a['children id'] = [str(cid) for cid in d[a]['children id']]
        d2_a['parent id'] = str(d[a]['parent id'])

        for b in d[a].keys():
            if b not in ['children id','parent id'] and b not in Tree.computable_attributes:
                d2_a[b] = d[a][b]
        d2[str(a)] = d2_a

    metadata_dict = {'type':'tree' , 'origin':'treex' , 'axial':False , 'size':t.get_attribute('size')}
    if 'successor' in t.get_attribute().keys():
        metadata_dict['axial'] = True

    mega_dict = {'metadata' : metadata_dict , 'object' : d2}

    with open(path,'w') as json_file:
        json.dump(mega_dict,json_file,indent=2)


def __json_dict_to_tree(dico , cle):
    dic = {}
    for a in dico[cle].keys():
        if a not in ['children id','parent id'] and a not in Tree.computable_attributes:
            dic[a] = dico[cle][a]
    t = Tree(dic)

    for c in dico[cle]['children id']:
        s = __json_dict_to_tree(dico , c)
        t.add_subtree(s)

    return t


def json_to_tree( path ):
    with open(path,'r') as json_file:
        d = json.load(json_file)

    for a in d['object'].keys():
        if d['object'][a]['parent id'] == 'None':
            root_tree = a
            break
    return __json_dict_to_tree(d['object'] , root_tree)


##########################################################################################


def assign_axis_to_tree(t , axis_name = 'successor' , axis = True):
    """
    This method allows to add axis to a tree

    Parameters
    ----------
    t: Tree

    axis_name: str
        name of the attribute corresponding to the axis level in the tree

    axis: bool
        if True, t is the priviledged child of its parent called successor

    Returns
    -------
    None

    See Also
    --------
    tree_to_lstring(...)

    """
    t.add_attribute_to_id(axis_name , axis)
    compteur = 0
    for child in t.my_children:
        if compteur == 0:
            assign_axis_to_tree(child,axis_name , True)
        else:
            assign_axis_to_tree(child,axis_name , False)
        compteur += 1


##########################################################################################

### EXAMPLE 1 ###
t = gen_random_tree(10)
assign_axis_to_tree(t)

t.add_attribute_to_id('test_json1' , 'abc')
t.add_attribute_to_id('test_json2' , 3.25)
t.add_attribute_to_id('test_json3' , {'cle':3})
t.add_attribute_to_id('test_json4' , ['abc' , 3.25 , {'cle':[1,'abc']}])

tree_to_json(t , 'ex1.json')

s = json_to_tree('ex1.json')

### EXAMPLE 2 ###
file = open('lilac180-medium2.txt')
lstr = file.read()
file.close()

t = lstring_to_tree(lstr, axis_name = 'successor')

tree_to_json(t,'ex2.json')