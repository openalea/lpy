from __version__ import *
from __lpy_kernel__ import *
from parameterset import *

def __mod_getattr__(self,name):
    if self.hasParameter(name): return self.getParameter(name)
    else: return self.__getattribute__(name)

def __mod_setattr__(self,name,value):
    if self.hasParameter(name): self.setParameter(name,value)
    else: self.__dict__[name] = value

ParamModule.__getattr__ = __mod_getattr__
ParamModule.__setattr__ = __mod_setattr__

del __mod_getattr__
del __mod_setattr__

from __lpy_kernel__ import __setCythonAvailable,__setPythonExec

try:
    import pyximport
    pyximport.install()
    __setCythonAvailable(True)
except:
    __setCythonAvailable(False)

from sys import executable
__setPythonExec(executable)
del executable


from openalea.plantgl.scenegraph import deprecated

@deprecated
def father(lstring,pos):
    """ deprecated function. See parent """
    return lstring.parent(pos)

@deprecated
def sons(lstring,pos):
    """ deprecated function. See children """
    return lstring.children(pos)

@deprecated
def lateralSons(lstring,pos):
    """ deprecated function. See lateral_children """
    return lstring.lateral_children(pos)

@deprecated
def directSon(lstring,pos):
    """ deprecated function. See direct_child """
    return lstring.direct_child(pos)

AxialTree.father = father
PatternString.father = father
AxialTree.sons = sons
PatternString.sons = sons
AxialTree.lateralSons = lateralSons
PatternString.lateralSons = lateralSons
AxialTree.directSon = directSon
PatternString.directSon = directSon


@deprecated
def father(node):
    """ deprecated function. See parent """
    return node.parent()

@deprecated
def sons(node):
    """ deprecated function. See children """
    return lstring.children()

@deprecated
def lateralSons(node):
    """ deprecated function. See lateral_children """
    return node.lateral_children()

@deprecated
def directSon(node):
    """ deprecated function. See direct_child """
    return node.direct_child()


NodeModule.father = father
NodeModule.sons = sons
NodeModule.lateralSons = lateralSons
NodeModule.directSon = directSon

Lsystem.iterate = Lsystem.derive
Lsystem.homomorphism = Lsystem.interpret

class Lstring (AxialTree):
    def __init__(self, input = None, lsyscontext = None):
        if lsyscontext: lsyscontext.makeCurrent()
        if input : AxialTree.__init__(self,input)
        else: AxialTree.__init__(self)

class LsystemIterator:
    """ Lsystem iterator """
    def __init__(self, lsystem):
        self.lsystem = lsystem
        self.axiom = self.lsystem.axiom
        self.nbstep = self.lsystem.derivationLength
        self.currentstep = -1
    def next(self):
        if self.currentstep == -1:
            self.axiom = self.lsystem.axiom
            self.currentstep += 1
        if self.currentstep == self.lsystem.derivationLength:
            raise StopIteration()
        else:
            self.axiom = self.lsystem.derive(self.axiom,self.currentstep,1)
            self.currentstep += 1
        return self.axiom
    def __iter__(self):
        return self

def __make_iter(lsystem): return LsystemIterator(lsystem)


Lsystem.__iter__ = __make_iter
del __make_iter

def Lsystem__call__(self,lstring,nbsteps=None):
    if nbsteps is None: nbsteps = self.derivationLength
    return self.derive(0,nbsteps,lstring)
Lsystem.__call__ = Lsystem__call__
del Lsystem__call__

def __lsystem_getattribute__(self,name):
    if self.context().has_key(name): return self.context()[name]
    else: raise AttributeError(name)

__original_lsystem_setattr__ = Lsystem.__setattr__
def __lsystem_setattribute__(self,name,value):
    try :
        self.__getattribute__(name)
    except:
        self.context()[name] = value
    else:
       __original_lsystem_setattr__(self,name,value) # previous method

Lsystem.__getattr__ =  __lsystem_getattribute__
Lsystem.__setattr__ =  __lsystem_setattribute__
