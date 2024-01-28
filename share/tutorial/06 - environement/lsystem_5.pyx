from openalea.lpy import *
from openalea.plantgl.all import *
from random import *


maxorder = 3
maxlength = 12
cmaxlength = 1
maxcambiumgrowth = 240
rotationtime = 100
maxgrowth = 300
maxiter  = 480. if ROTATION else maxgrowth

lateral_delay = 20
dx = 0.1
da = 0.2
dt = 1
maxwidth = 1.0
CINC = True
INCANG = True
Enviro = False
minl, maxl = 0,0

def getmidheigth(t):
    return midheigth(min(maxgrowth,getIterationNb())/float(maxgrowth))

def getmaxheigth(t):
    return 2+ 2*heigth(min(maxgrowth,getIterationNb())/float(maxgrowth))

def getmaxradius(t):
    return 2 + 2*radius(min(maxgrowth,getIterationNb())/float(maxgrowth))*(t/10.)
    
def StartEach():
    global Enviro
    Enviro = False
    pass

from openalea.plantgl.all import Viewer,eStatic,eAnimatedScene, eAnimatedPrimitives

from openalea.plantgl.light.light import diffuseInterception

def End():
    if isAnimationEnabled():
        print (minl,maxl)
        f = open('interception.txt','w')
        f.write(str(minl)+'\n')
        f.write(str(maxl)+'\n')
        f.close()

import os
def Start():
    global minl, maxl
    if os.path.exists('interception.txt'):
        f = open('interception.txt','r')
        minl = float(f.readline())
        maxl = float(f.readline())
        f.close()
        

def EndEach(lstring,scene):
   if isAnimationEnabled():
      global minl, maxl # read from initial simu
      Viewer.display(scene) 
      shapes = dict([(sh.id,sh) for sh in scene])
      res = diffuseInterception(scene)   # compute light from turtle dome
      for k,v in res.items(): # res contains pair of id, light level
         if lstring[k].name in 'AB': # check for module A and B
           v /= surface(shapes[k])   # normalized 
           lstring[k][3] = v         # set ligth level into param 4 
           if minl > v: minl = v
           if maxl < v: maxl = v


def do_houpppier(p,s,c)   :
    seed(s)
    p2 = (p[0],p[1])
    t = norm(p2)/1.8 
    maxr = getmaxradius(t)
    args = [maxr-uniform(0,maxr/3) for i in range(4)]
    maxh = getmaxheigth(t)
    midh = maxh*getmidheigth(t)
    args += [midh+uniform(-0.5,0.5) for i in range(4)]
    args += [(0,0,0),(0,0,maxh)]
    ah = AsymmetricHull(*args)
    l = 17 if (c < 0 or maxl <= minl) else 2+(11 * ((c-minl)/(maxl-minl)) )   
    pproduce(59,int(l),-midh,ah)


ROT = ModuleClass.get('ROT') # module ROT

__axiom__ = [121,136,120,133,135,18,(67,0,0,1),(66,0.025),(137,2,20),12,(96,0,0,0,-1)]

__derivation_length__ =  maxiter
# production:

def __p_0_0_N_l_t__P_p_A_x_o_s_c_(l,t,p,x,o,s,c) :
  global cmaxlength
  if l >= 2.0:
    if o == 0:
        cmaxlength = x+1
    pproduce(58)
    if x < maxlength :
        if o < maxorder and x % 2 == 0 and x < maxlength-1: 
            pproduce(57,lateral_delay,x+1,o+1,randint(1,1000))
        return pproduce(56,x+1,o,s,c)
    else:
        return pproduce(55,x,o,s,c)

def __p_0_1_D_t_x_o_s_(t,x,o,s) :
    if t > 0:
        return pproduce(54,t-1,x,o,s)
    else :
        return pproduce(53,x,o,s)

def __p_0_2_N_x_t_(x,t) :
    global CINC
    if x < 2:
        x += dx
    if CINC:
        t +=dt
        if t >= maxcambiumgrowth:
           CINC = False
    return pproduce(52,x,t)

def __p_0_3___a_(a) :
   global INCANG
   if a > 120:
       INCANG = False
   elif a < 55:
       INCANG = True
   if INCANG:
       a += da
   else:
       a -= da
   pproduce(51,a)

# interpretation:

def __h_0_0_N_x_t_(turtle,x,t) :
    if t > maxcambiumgrowth:
        t = maxcambiumgrowth
    dnt = min(20,x*10)
    lowt = max(t-dnt,0)
    w2 = width(lowt/(maxcambiumgrowth+1.))
    w1 = width(t/(maxcambiumgrowth+1.))
    pproduce(50,maxwidth*w1)
    nb,reminder = divmod(x,0.2)
    if nb > 0:
      for i in range(int(nb)):
        lr = maxwidth*(w1+(w2-w1)*((i+1)/nb))
        #print lr
        pproduce(49,lr)
        
    if reminder>0: return pproduce(48,reminder,w1)

def __h_0_1_N_ox_t__P_p_A_x_o_s_c_(turtle,ox,t,p,x,o,s,c) :
        do_houpppier(p,s,c)
        pass

def __h_0_2__P_p_B_x_o_s_c_(turtle,p,x,o,s,c) :
    do_houpppier(p,s,c)
    pass

def __h_0_3_ROT(turtle,) :
    if getIterationNb() > 300:
        return pproduce(47,2*(getIterationNb() - 300))

def __h_0_4_SOIL(turtle,) :
    if Enviro:
        s = 20
        return pproduce(46,QuadSet([(-s,s,0),(s,s,0),(s,-s,0),(-s,-s,0)],[range(4)],texCoordList = [(0,1),(1,1),(1,0),(0,0)]))
# endlsystem

