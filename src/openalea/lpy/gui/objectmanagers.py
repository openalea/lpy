from glob import glob
from os.path import join,dirname,basename,splitext
import warnings
import sys, traceback

def __get_plugins_path():
    p = dirname(__file__)
    return join(p,'plugins')

def __read_manager_plugins():
    oldpathes = list(sys.path)
    sys.path.insert(0,"plugins")
    managers = []
    pattern = join(__get_plugins_path(),'*.py')
    listplugins = glob(pattern)
    listplugins = [ splitext(basename(i))[0] for i in listplugins]
    listplugins = [ i  for i in listplugins if i[:2] != '__']
    for plugin in listplugins:
        plugname = plugin
        try:
            mod = __import__(plugname)
        except ImportError,e :
            exc_info = sys.exc_info()
            traceback.print_exception(*exc_info)
            warnings.warn("Cannot import "+plugin+" : "+str(e))
            continue
        try:
            lmanagers = getattr(mod, 'get_managers')()
            try:
                iter(lmanagers)
                managers += lmanagers
                for m in lmanagers:
                    print "import manager '"+m.typename+"' from plugin '"+plugin+"'"
            except:
                managers.append(lmanagers)
                print "import manager '"+lmanagers.typename+"' from plugin '"+plugin+"'"
        except Exception, e:
            exc_info = sys.exc_info()
            traceback.print_exception(*exc_info)
            warnings.warn("Cannot import "+plugin+" : "+str(e))
    sys.path  = oldpathes
    return managers

# global manager list value
__MANAGERS = None

def get_managers():
    global __MANAGERS
    if not __MANAGERS : 
        managers = __read_manager_plugins()        
        __MANAGERS = dict([(m.typename,m) for m in managers])
    return __MANAGERS
