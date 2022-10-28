from pandas import DataFrame, concat
from numpy import nan

__TABLE = None

def init_table(value = None):
    global __TABLE
    if value is None:
        __TABLE = ParamTable()
    else:
        __TABLE = value

def init_dict_table(value = None):
    init_table(ParamDictTable())

def _get_table():
    global __TABLE
    if __TABLE is None:
        init_table()
    return __TABLE

def get_table():
    return _get_table().dataframe()

class ParamTable:
    def __init__(self):
        self.id = 0
        self.data = DataFrame()

    def dataframe(self):
        return self.data

    def pnames(self):
        return self.data.columns

    def get_new_id(self, att):
        id = self.id
        self.id += 1
        self.data = concat([self.data, DataFrame(dict([(n,[v]) for n,v in att.items()]))], ignore_index=True)
        return id

    def hasattr(self, uid, attname):
        return attname in self.pnames() and self.data.at[uid,attname] != nan

    def setattr(self, uid, attname, value):
        if attname in self.pnames():
            self.data.at[uid,attname] = value
        else :
            values = [(nan if i != uid else value) for i in range(self.id)]
            self.data.insert(len(self.pnames()), attname, values)

    def getattr(self, uid, attname):
        return self.data.at[uid,attname]

    def delattr(self,uid, attname):
        self.data.at[uid,attname] = nan

    def rename(self,oldattname,attname):
        self.data.rename(columns={oldattname:attname})


class ParamDictTable:
    def __init__(self):
        self.id = 0
        self.data = {}

    def dataframe(self):
        return DataFrame(self.data)

    def pnames(self):
        return self.data.keys()

    def get_new_id(self, att):
        id = self.id
        self.id += 1
        for pname, pvalue in att.items():
            self.setattr(id, pname, pvalue)
        return id

    def hasattr(self, uid, attname):
        return attname in self.pnames() and uid in self.data[attname]

    def setattr(self, uid, attname, value):
        self.data.setdefault(attname,{})[uid] = value

    def getattr(self, uid, attname):
        return self.data[attname][uid]

    def delattr(self,uid, attname):
        del self.data[attname][uid]

    def rename(self,oldattname,attname):
        self.data[attname] =  self.data[oldattname]
        del self.data[oldattname]


class TableParameterSet:
    """ A structure that contains and allow to manipulate easily a set of parameters """
    def __init__(self, **kwd):
        """  Create a ParameterSet """
        self.__dict__['uid'] = _get_table().get_new_id(kwd)
        #self.set(**kwd)
    
    def rename(self,oldattname,attname):
        """ Rename an attribute of the parameter set """
        _get_table().rename(oldattname,attname)
    
    def setdefault(self, *args, **kwd):
        """ Check if an attribute exists. If not create it with default value. Can be used as self.setdefault('attname', value) or self.setdefault(attname=value). """
        assert len(args) == 0 or len(kwd) == 0
        if len(args) > 0:
            if len(args) % 2 != 0  : raise ValueError("Should give a list of parameter name and values")
            for i in range(len(args)/2):
                attname = args[2*i]
                defaultvalue = args[2*i+1]
                if not _get_table().hasattr(self.uid,attname):
                    _get_table().setattr(self.uid,attname,defaultvalue)
        elif len(kwd) > 0:
            for attname, defaultvalue in list(kwd.items()):
                if not _get_table().hasattr(self.uid,attname):
                    _get_table().setattr(self.uid,attname,defaultvalue)

    
    def hasattr(self,attname):
        """ Check if an attribute exists.  """
        return _get_table().hasattr(self.uid,attname)
    
    def get(self, attname, defaultvalue=None):
        """ Check if an attribute exists. If not return default value """
        if not _get_table().hasattr(self.uid,attname): return defaultvalue
        return _get_table().getattr(self.uid,attname)

    def set(self, **kwd):
        """ Set a set of parameters """
        for pname, pvalue in kwd.items():
            _get_table().setattr(self.uid, pname,pvalue)
    
    def parameter_names(self):
        """ Gives the name of the parameters """
        return _get_table().pnames()
    
    #def copy(self, deep = True):
    #    """ Return a deep copy of self """
    #    from copy import copy, deepcopy
    #    if deep : return deepcopy(self)
    #    else: return copy(self)

    def __repr__(self):
        return self.__class__.__name__+'('+str(self.uid)+','+','.join([k+'='+repr(_get_table().getattr(self.uid,k)) for k in _get_table().pnames() if _get_table().hasattr(self.uid,k)])+')'

    def __getattr__(self, attname):
        if attname == 'uid' : return self.__dict__['uid']
        #print('getattr', repr(attname))
        return _get_table().getattr(self.uid,attname)

    def __delattr__(self, attname):
        _get_table().delattr(self.uid,attname)

    def __setattr__(self, attname, value):
        if attname == 'uid' : self.__dict__['uid'] = value
        else:
            _get_table().setattr(self.uid, attname, value)

    def __contains__(self, attname):
        return _get_table().hasattr(self.uid,attname)
