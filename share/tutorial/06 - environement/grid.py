import openalea.plantgl.all  as pgl
import numpy as np


class Grid3D(pgl.Grid3Indexing):
    def __init__(self, voxelsize = (1,1,1), origin = (0,0,0), upper = (10,10,10), default_value = 0):
        pgl.Grid3Indexing.__init__(self, voxelsize, origin, upper)
        self.values = np.ones(self.size()) * default_value

    def getValueAt(self, pos):
        return self.values[self.cellIdFromPoint(pos)]

    def setValueAt(self, pos, value):
        self.values[self.cellIdFromPoint(pos)] = value

    def __contains__(self,pos):
        return pgl.BoundingBox(self.getLowerCorner(),self.getUpperCorner()-(1e-5,1e-5,1e-5)).contains(pos)
    
    def get27Neighbors(self,idx):
        import itertools
        result = []
        for i,j,k in itertools.product((-1,0,1),(-1,0,1),(-1,0,1)):
           nidx = (idx[0]+i,idx[1]+j,idx[2]+k)
           if i == j == k == 0 : continue
           for i,v in enumerate(nidx):
             if v < 0 or v >= self.getGridSize()[i]:
               continue
           result.append(nidx)
        return result
    
    def __getitem__(self,cid):
        return self.values[cid]

class WaterSoilGrid(Grid3D):
    def __init__(self, voxelsize = (1,1,1), origin = (0,0,0), upper = (10,10,10), default_value = 0):
        Grid3D.__init__(self, voxelsize , origin , upper , default_value )
        self.maxvalue = None
    
       
    def representation(self):
       mat = pgl.Material('mblue',(0,0,200),transparency=0.8)
       mat2 = pgl.Material('mred',(200,0,0),transparency=0.8)
       if not self.maxvalue: self.maxvalue = max(self.values)
       if self.maxvalue <= 0: return pgl.Scene()
       sc = pgl.Scene()
       for i, v in enumerate(self.values):
           size = v/(2*self.maxvalue)
           if size > 1e-2:
             sc += pgl.Shape(pgl.Translated(self.getVoxelCenterFromId(i),pgl.Box(self.getVoxelSize()*size)),mat,i)
           else:
             sc += pgl.Shape(pgl.Translated(self.getVoxelCenterFromId(i),pgl.Box(self.getVoxelSize()*0.1)),mat2,i)
       return sc

def nbgs(idx):
    ref = list(idx)
    res = []
    dim = grid.dimensions()
    for d in range(3):
      if ref[d] > 0 : 
        lres = list(ref)
        lres[d] -= 1
        res.append(lres)
      if ref[d] < dim[d]-1 : 
        lres = list(ref)
        lres[d] += 1
        res.append(lres)
    return res
