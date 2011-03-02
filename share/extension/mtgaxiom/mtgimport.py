from openalea.mtg.io import *
import openalea.mtg.plantframe as plantframe
import openalea.mtg.algo as algo 
from openalea.mtg import aml, dresser


def flatten(g):
    microroot = g.component_roots_at_scale(g.root,g.max_scale()).next()
    print microroot
    
    g = g.sub_tree(microroot,True)
    g, props = colored_tree(g,colors={1:list(g.vertices())})
    
    f = file('debug.txt','w')
    f.write(str(g))
    f.close()
    print len(g)
    return g

def construct_lstring():
    
    fn = r'walnut.mtg'
    drf = r'walnut.drf'

    g = read_mtg_file(fn)

    topdia = lambda x: g.property('TopDia').get(x)

    dressing_data = dresser.dressing_data_from_file(drf)
    pf = plantframe.PlantFrame(g,  TopDiameter=topdia, 
                               DressingData = dressing_data)
    pf.propagate_constraints()

    diameters = pf.algo_diameter()
    toppositions = pf.points
    
    g.properties()['TopDiameter']=diameters
    g.properties()['TopPosition']=toppositions
    
    g = flatten(g)
    
    paramnames = ['TopPosition','TopDiameter']
    
    params = { 'S': paramnames, 'U' : paramnames }
    lstring = mtg2axialtree(g, params)
    return lstring

if __name__ == '__main__':
    lstring = construct_lstring()
    print len(lstring)
    print len([0 for i in lstring if not i.name in '[]'])
    print lstring[0:10]
    print lstring[5]
    print len(lstring[5])
