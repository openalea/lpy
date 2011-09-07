from openalea.lpy import *

resfname = 'result-%i.pkl'
def dump(obj,fname = resfname):
   import pickle
   f = file(fname,'w')
   pickle.dump(obj,f)
   f.close()

def load(fname = resfname):
   import pickle
   f = file(fname,'r')
   return pickle.load(f)

def check_dump(fname = resfname):
   import os
   return os.path.exists(fname)

def evalreconstruction(i):
    l = Lsystem("walnut4.lpy",{'RAMIF':i})
    l.iterate()
    return l.context()['result']

def evalrange(i=0, j=101,step = 1):
    return [evalreconstruction(k) for k in xrange(i,j,step)]

def getevaluation(i=0, j=101,step = 1):
    fname = resfname%step
    if not check_dump(fname):
        res = evalrange(i, j,step)
        dump(res,fname)
        return res
    else:
        return load(fname)


def plotresult(i=0, j=101,step = 1):
    import matplotlib.pyplot as mpl
    from numpy import arange
    res = getevaluation(i, j,step)
    x = [k/100. for k in range(i,j,step)]
    nbcurve = len(res[0])
    nres = [[] for i in xrange(nbcurve)]
    mres = []
    maxofmin = -1,0.01
    for kindex,kres in enumerate(res):
        minv = min(kres.values())
        if minv > maxofmin[1]:
           maxofmin = kindex, minv
        lres = [(i,j) for i,j in kres.items()]
        lres.sort(lambda x,y: cmp(x[0],y[0]))
        for i,v in enumerate(lres):
            nres[i].append(v[1])
        mres.append(sum([j for i,j in lres])/nbcurve)
    print maxofmin
    for y in nres:
       mpl.plot(x,y)
    mpl.plot(x,mres,linewidth=2)
    mpl.ylim(0.5,1)
    mpl.xlim(0,1)
    mpl.axhline(0.8)
    mpl.axvline(0.77)
    mpl.xticks(arange(0,1.1,0.1))
    mpl.yticks(arange(0.5,1.04,0.05))
    mpl.show()

if __name__ == '__main__':
    plotresult()
    