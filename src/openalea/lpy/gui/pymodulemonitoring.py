
class ModuleMonitor:
    def __init__(self):
        import sys
        self.modules = set()
        self.sysmodules = sys.modules

    def _get_current_modules(self):
        return set([k for k,m in self.sysmodules.items() if not m is None])

    def start(self):
        self.modules = set()
        self.goon()

    def goon(self):
        self._initialmodules = self._get_current_modules()

    def stop(self):
        if not hasattr(self,'_initialmodules') : return
        self.modules.update(self._get_current_modules() - self._initialmodules)
        del self._initialmodules

    def reloadall(self, verbose = True):
        for m in self.modules:
            module = self.sysmodules[m]
            if module:
                if verbose: print 'Reload',repr(m)
                reload(module)

class ModuleMonitorWatcher:
    def __init__(self, modulemonitor):
        self.modulemonitor = modulemonitor
        self.modulemonitor.start()
    def __del__(self):
        self.modulemonitor.stop()



def get_python_library():
    import distutils.sysconfig
    import glob
    import os
    import pkgutil
    import sys    

    # Get list of the loaded source modules on sys.path.
    modules = { 
        module
        for _, module, package in list(pkgutil.iter_modules())
        if package is False
    }

    # Glob all the 'top_level.txt' files installed under site-packages.
    site_packages = glob.iglob(os.path.join(os.path.dirname(os.__file__) 
                    + '/site-packages', '*-info', 'top_level.txt'))

    # Read the files for the import names and remove them from the modules list.
    modules -= {open(txt).read().strip() for txt in site_packages}

    # Get the system packages.
    system_modules = set(sys.builtin_module_names)

    # Get the just the top-level packages from the python install.
    python_root = distutils.sysconfig.get_python_lib(standard_lib=True)
    _, top_level_libs, _ = list(os.walk(python_root))[0]

    return sorted(top_level_libs + list(modules | system_modules))

def check_local_modules(dir = '.'):
    import glob, os, sys
    import os.path as op
    pylocalmodules = glob.glob('*.pyc')
    pylocalmodules = set([op.join(op.abspath(dir),f) for f in pylocalmodules])
    result = []
    for modname, module in sys.modules.items():
        if not module is None and ('__file__' in module.__dict__) and (op.abspath(module.__file__) in pylocalmodules):
            result.append(modname)
    return result

def reload_local_modules(dir = '.', verbose = False):
    import sys
    for mod in check_local_modules(dir):
        if verbose: print 'Reload', mod
        reload(sys.modules[mod])
