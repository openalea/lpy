
from openalea.tree_matching import *
from openalea.tree_matching.mtgmatching import MtgMatching
import openalea.mtg.algo as algo

class MyNodeCost (NodeCost):
  def __init__(self):
     NodeCost.__init__(self)  
  def getInsertionCost(self,a):  return 1
  def getDeletionCost(self,b):   return 1
  def getChangingCost(self,a,b): return 0
 

def compare(mtg1,mtg2,scale1=1,scale2=1,root1=None,root2=None):
    node_cost = MyNodeCost()
    m = MtgMatching(mtg1,mtg2,scale1,scale2,node_cost,1,root1,root2)
    val = m.match()
    res = m.getList()
    return val,res

def getproportion(mtg,roots,initialmtg,initialroot,matched,rebuildclasses = 'WZ'):
   proportions = {}
   for r,ir,m in zip(roots,initialroot,matched):
     p = getpropforsubtree(mtg,r,initialmtg,ir,m,rebuildclasses) 
     proportions[r]=p
   return proportions
     
def getpropforsubtree(mtg,root,initialmtg,initialroot,matched,rebuildclasses = 'WZ'):
     nbelements1 = len(list(algo.descendants(mtg,root)))
     nbelements2 = len(list(algo.descendants(initialmtg,initialroot)))
     nbmatched = len(matched)
     return 2*nbmatched/float(nbelements1+nbelements2)
     nbtotelem = nbelements1+nbelements2-nbmatched
     print root,nbmatched,nbelements1,nbelements2,nbmatched/float(nbtotelem)
     return nbmatched/float(nbtotelem)

