from openalea.mtg.io import *
import openalea.mtg.plantframe as plantframe
import openalea.mtg.algo as algo 
from openalea.mtg import aml, dresser
from openalea.plantgl.all import norm,Vector3
from numpy import mean

def flatten(g):
    microroot = next(g.component_roots_at_scale_iter(g.root,g.max_scale()))
    
    g = g.sub_tree(microroot,True)
    g, props = colored_tree(g,colors={1:list(g.vertices(scale=g.max_scale()))})
    
    #f = file('debug.txt','w')
    #f.write(str(g))
    #f.close()
    return g

def read_mtg(fn = 'walnut.mtg' ,drf = 'walnut.drf'):
    g = read_mtg_file(fn)

    topdia = lambda x: g.property('TopDia').get(x)

    dressing_data = dresser.dressing_data_from_file(drf)
    pf = plantframe.PlantFrame(g,  TopDiameter=topdia, 
                               DressingData = dressing_data)
    pf.propagate_constraints()

    diameters = pf.algo_diameter()
    toppositions = pf.points
    
    g.properties()['TopDiameter']=diameters
    g.properties()['TopPosition']= dict([ (k,Vector3(v)) for k,v in toppositions.items()])
    
    g = flatten(g)
    return g

def color_last_year_node(g):
    def year_ancestors(i):
        ancestors = [i]
        assert g.label(i)[0] == 'U'
        while g.label(g.parent(i))[0] == 'U' :
            i = g.parent(i)
            ancestors.append(i)
        return ancestors
    leaves = [vtx for vtx in g.vertices(scale=1) if g.nb_children(vtx) == 0 and g.label(vtx)[0] == 'U']
    gu = [year_ancestors(leaf) for leaf in leaves]
    toppos = g.property('TopPosition')
    def nodepos(i):
        try:
            return toppos[i]
        except:
            return toppos[g.parent(i)]
    
    def nodelength(i):
        try:
           return norm(toppos[i]-nodepos(g.parent(i)))
        except:
           return 0
    
    gul = [sum([nodelength(i) for i in ui]) for ui in gu]
    avg_length_gu = mean(gul)
    print('**', avg_length_gu, min(gul), max(gul))
    leavesly = [g.parent(i[-1]) for i in gu]
    
    def last_year_ancestors(i):
        ancestors = [i]
        l = 0
        p = nodepos(i)
        while l < avg_length_gu:
            i = g.parent(i)
            if i:
                ancestors.append(i)
                try:
                  np = toppos[i]
                  l += norm(p-np)
                  p = np
                except:
                  pass
            else:
               break
        return ancestors
    lygus = [last_year_ancestors(i) for i in leavesly]
    labels = g.property('label')
    for lygu in reversed(lygus):
        for i in lygu:
          if len([j for j in g.children(i) if labels[j][0] in 'S']) == 0:
            assert labels[i][0] in 'SV'
            labels[i] = 'V'+labels[i][1:]


def construct_lstring(g):
    idp = dict([(i,i) for i in g.property('TopPosition').keys()])
    g.properties()['mtgid'] = idp
    paramnames = ['TopPosition','TopDiameter']
    
    params = { 'S': paramnames+['mtgid'], 'U' : paramnames, 'V' : paramnames }
    lstring = mtg2axialtree(g, params)
    return lstring

def construct_walnut_lstring():
    g = read_mtg()
    color_last_year_node(g)
    return construct_lstring(g)
    
if __name__ == '__m_ain__':
    g = read_mtg()
    color_last_year_node(g)
    lstring = construct_lstring(g)
    # print len(lstring)
    # print len([0 for i in lstring if not i.name in '[]'])
    # print lstring[0:10]
    # print lstring[5]
    # print len(lstring[5])

