
import numpy as np

class ParamTable:
    def __init__(self, process, indexname):
        self.indexname = indexname
        self.process = process
        self.id = len(self._getattr(indexname))

    def pnames(self):
        names = self.process.__xsimlab_state_keys__.keys() 
        names = [n[len(self.indexname)+1:] for n in names if n.startswith(self.indexname+'_')] 
        return names

    def _getattr(self, attname, prefix = None):
        try :
            return getattr(self.process, prefix+'_'+attname if prefix else attname)
        except KeyError as ke:
            value = np.array([])
            setattr(self.process,prefix+'_'+attname if prefix else attname,value)
            return value
        
    def get_new_id(self, att):
        id = self.id
        setattr(self.process, self.indexname, np.append(getattr(self.process, self.indexname), [id]))
        for attname, attvalue in att.items():
            self.setattr(id, attname,  attvalue)
        self.id += 1
        return id

    def hasattr(self, uid, attname):
        if not attname in self.pnames() : return False
        v = self._getattr(attname, self.indexname)
        if len(v) <= uid : return False
        else : return v[uid] != np.nan

    def setattr(self, uid, attname, value):
        if attname in self.pnames():
            try:
                data = getattr(self.process, self.indexname+'_'+attname)
            except KeyError as ke:
                setattr(self.process, self.indexname+'_'+attname, [(np.nan if i != uid else value) for i in range(self.id+1)])
                return
            if len(data) > uid:
                data[uid] = value
            else:
                setattr(self.process, self.indexname+'_'+attname, np.append(data, [(np.nan if i != uid else value) for i in range(len(data), self.id+1)]))
        else :
            raise IndexError(attname)
            #values = [(np.nan if i != uid else value) for i in range(self.id+1)]
            # setattr(self.process, attname, values)

    def getattr(self, uid, attname):
        data = self._getattr(attname, self.indexname)
        if len(data) > uid:
            return data[uid]
        else:
            return np.nan

    def delattr(self,uid, attname):
        self.setattr(uid, attname, np.nan)


class ArrayParameterSet:
    table = None
    
    """ A structure that contains and allow to manipulate easily a set of parameters """
    def __init__(self, **kwd):
        """  Create a ParameterSet """
        #self.__dict__['__ptable'] = self.table
        self.__dict__['uid'] = self._get_table().get_new_id(kwd)
    
    def _get_table(self):
        return self.__class__.table
                
    def setdefault(self, *args, **kwd):
        """ Check if an attribute exists. If not create it with default value. Can be used as self.setdefault('attname', value) or self.setdefault(attname=value). """
        assert len(args) == 0 or len(kwd) == 0
        if len(args) > 0:
            if len(args) % 2 != 0  : raise ValueError("Should give a list of parameter name and values")
            for i in range(len(args)/2):
                attname = args[2*i]
                defaultvalue = args[2*i+1]
                if not self._get_table().hasattr(self.uid,attname):
                    self._get_table().setattr(self.uid,attname,defaultvalue)
        elif len(kwd) > 0:
            for attname, defaultvalue in list(kwd.items()):
                if not self._get_table().hasattr(self.uid,attname):
                    self._get_table().setattr(self.uid,attname,defaultvalue)

    
    def hasattr(self,attname):
        """ Check if an attribute exists.  """
        return self._get_table().hasattr(self.uid,attname)
    
    def get(self, attname, defaultvalue=None):
        """ Check if an attribute exists. If not return default value """
        if not self._get_table().hasattr(self.uid,attname): return defaultvalue
        return self._get_table().getattr(self.uid,attname)

    def set(self, **kwd):
        """ Set a set of parameters """
        for pname, pvalue in kwd.items():
            self._get_table().setattr(self.uid, pname,pvalue)
        return self
    
    def parameter_names(self):
        """ Gives the name of the parameters """
        return self._get_table().pnames()

    def __repr__(self):
        return self._get_table().indexname+'('+str(self.uid)+','+','.join(
                [k+'='+repr(self._get_table().getattr(self.uid,k)) 
                    for k in self._get_table().pnames() if self._get_table().hasattr(self.uid,k)])+')'

    def __getattr__(self, attname):
        if attname == 'uid' : return self.__dict__['uid']
        return self._get_table().getattr(self.uid,attname)

    def __delattr__(self, attname):
        self._get_table().delattr(self.uid,attname)

    def __setattr__(self, attname, value):
        if attname == 'uid' : self.__dict__['uid'] = value
        else:
            self._get_table().setattr(self.uid, attname, value)

    def __contains__(self, attname):
        return self._get_table().hasattr(self.uid,attname)


APS = ArrayParameterSet
