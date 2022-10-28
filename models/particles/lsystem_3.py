from openalea.lpy import *
from random import *
from openalea.plantgl.all import *

gravity =  -Vector3.OZ * 9.80665
cdef float mass = 0.05
cdef float step = 0.2
g = gravity*mass*step
cdef int maxparticule = 2000
cdef int maxtime = 500
cdef int maxcol = 35
cdef int maxrad = 8

from time import perf_counter
timer = 0

def Start():
  global timer
  timer = clock()

def End():
  print ('time=',perf_counter()-timer)

__axiom__ = [1,(52,0),1,(49,0.5),(4,1),2,1,(49,maxrad),(4,1),2,2,(88,0)]

__derivation_length__ =  maxtime
# production:

def __p_0_0_A_x_(x) :
  cdef int nbprod
  if x < maxparticule:
    nbprod = randint(0,4)
    for i in xrange(nbprod):
      pproduce(9,Vector3(0,0,0),Vector3((random()/2)-.25,(random()/2)-.25,3+2*random()))
    return pproduce(10,x+nbprod)
  else:
    return pproduce(11)

def __p_0_1_P_pos_speed_t_(pos,speed,t) :
  speed += g
  pos += speed * step
  if pos.z < 0:
     if norm(pos) > maxrad:
         return pproduce(12)
     pos.z *= -1
     speed.z *= -1
     speed *= 0.8
     if norm(speed) < 1e-4:
        return pproduce(13)
  return pproduce(14,pos,speed,t+1)

# homomorphism:


def __h_0_0_P_pos_speed_t_(turtle,pos,speed,t) : return pproduce(15,int(min(35,1+maxcol*t/200)),pos.x,pos.y,pos.z,max(0.1,0.3-0.001*t))

# endlsystem

