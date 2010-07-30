from glob import glob
from os.path import join,dirname,basename,splitext
import warnings
import sys, traceback
try:
    import py2exe_release
except: 
    py2exe_release = False

def __get_plugins_path():
    p = dirname(__file__)
    return join(p,'plugins')

def __read_manager_plugins():
    managers = []
    oldpathes = list(sys.path)
    if not py2exe_release:
        pluginpath = __get_plugins_path()
        sys.path.insert(0,pluginpath)
        pattern = join(pluginpath,'*.py')
        listplugins = glob(pattern)
        listplugins = [ splitext(basename(i))[0] for i in listplugins]
        listplugins = [ i  for i in listplugins if i[:2] != '__']    
    else:
        import plugins.curve2dmanager as cm
        import plugins.functionmanager as fm
        import plugins.nurbspatchmanager as nm
        listplugins = [cm,fm,nm]
    for plugin in listplugins:
        plugname = plugin
        try:
            if not py2exe_release:
                mod = __import__(plugname)
            else:
                mod = plugin
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
                # for m in lmanagers:
                #    print "import manager '"+m.typename+"' from plugin '"+plugin+"'"
            except:
                managers.append(lmanagers)
                #print "import manager '"+lmanagers.typename+"' from plugin '"+plugin+"'"
        except Exception, e:
            exc_info = sys.exc_info()
            traceback.print_exception(*exc_info)
            print dir(mod)
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
