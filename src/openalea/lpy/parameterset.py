

class ParameterSet:
    def __init__(self, **kwd):
        """  Create a ParameterSet """
        self.__dict__.update(kwd)
    
    def rename(self,oldattname,attname):
        """ Rename an attribute of the parameter set """
        setattr(self,attname,getattr(self,oldattname))
        delattr(self,oldattname)
    
    def check(self,attname,defaultvalue):
        """ Check if an attribute exists. If not create it with default value """
        if not hasattr(self,attname):
            setattr(self,attname,defaultvalue)
