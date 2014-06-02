

class ParameterSet:
    """ A structure that contains and allow to manipulate easily a set of parameters """
    def __init__(self, **kwd):
        """  Create a ParameterSet """
        self.__dict__.update(kwd)
    
    def rename(self,oldattname,attname):
        """ Rename an attribute of the parameter set """
        setattr(self,attname,getattr(self,oldattname))
        delattr(self,oldattname)
    
    def setdefault(self, *args, **kwd):
        """ Check if an attribute exists. If not create it with default value. Can be used as self.setdefault('attname', value) or self.setdefault(attname=value). """
        assert len(args) == 0 or len(kwd) == 0
        if len(args) > 0:
            if len(args) % 2 != 0  : raise ValueError("Should give a list of parameter name and values")
            for i in xrange(len(args)/2):
                attname = args[2*i]
                defaultvalue = args[2*i+1]
                if not hasattr(self,attname):
                    setattr(self,attname,defaultvalue)
        elif len(kwd) > 0:
            for attname, defaultvalue in kwd.items():
                if not hasattr(self,attname):
                    setattr(self,attname,defaultvalue)

    
    def hasattr(self,attname):
        """ Check if an attribute exists.  """
        return hasattr(self,attname)
    
    def get(self, attname, defaultvalue=None):
        """ Check if an attribute exists. If not return default value """
        if not hasattr(self,attname): return defaultvalue
        return getattr(self,attname)

    def set(self, **kwd):
        """ Set a set of parameters """
        self.__dict__.update(kwd)
    
    def parameter_names(self):
        """ Gives the name of the parameters """
        self.__dict__.keys()
    
    def copy(self, deep = True):
        """ Return a deep copy of self """
        from copy import copy, deepcopy
        if deep : return deepcopy(self)
        else: return copy(self)

    def __repr__(self):
        return self.__class__.__name__+'('+','.join([k+'='+repr(v) for k,v in self.__dict__.items()])+')'

    def __getitem__(self, attname):
        return getattr(self,attname)

    def __delitem__(self, attname):
        delattr(self,attname)

    def __setitem__(self, attname, value):
        setattr(self, attname, value)

    def __contains__(self, attname):
        return hasattr(self,attname)
