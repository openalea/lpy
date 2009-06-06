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