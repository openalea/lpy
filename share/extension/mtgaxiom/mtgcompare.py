
from openalea.tree_matching import *
from openalea.tree_matching.mtgimport import mtg2treegraph


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
    print val
    print m.getList(0,0)