from __version__ import *
from __lpy_kernel__ import *
 

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
