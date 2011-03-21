
from openalea.tree_matching import *
from openalea.tree_matching.mtgimport import mtg2treegraph
import openalea.mtg.algo as algo

class MyNodeCost (NodeCost):
  def __init__(self):
     NodeCost.__init__(self)  
  def getInsertionCost(self,a):  return 1
  def getDeletionCost(self,b):   return 1
  def getChangingCost(self,a,b): return 0
 

def compare(mtg1,mtg2,scale1=1,scale2=1):
    tree1,idmap1 = mtg2treegraph(mtg1,scale1)
    print len(idmap1)
    tree2,idmap2 = mtg2treegraph(mtg2,scale2)
    print len(idmap2)
    node_cost = MyNodeCost()
    m = Matching(tree1,tree2,node_cost,1)
    val = m.match()
    res = m.getList(0,0)
    idmap1 = dict([(j,i) for i,j in idmap1.iteritems()])
    idmap2 = dict([(j,i) for i,j in idmap2.iteritems()])
    res = [(idmap1[i],idmap2[j],k) for i,j,k in res]
    return val,res

def getproportion(mtg,roots,matched,rebuildclasses = 'WZ'):
   proportions = {}
   for r in roots:
     nbelements = 0
     nbmatched = 0
     for vtx in algo.descendants(mtg,r):
       nbelements += 1
       if vtx in matched:
           nbmatched += 1
     print r,nbmatched,nbelements,nbmatched/float(nbelements)
     proportions[r]=(nbmatched/float(nbelements))
   return proportions

def setproportions(axialtree,mapping,proportions):
   pass