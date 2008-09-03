try:
    import pkg_resources
    pkg_resources.declare_namespace(__name__)
except ImportError:
    import pkgutil
    __path__ = pkgutil.extend_path(__path__, __name__)


try:
    from __init_path__ import set_path
    set_path()
except:
    pass

import modulefinder
for p in __path__:
   modulefinder.AddPackagePath(__name__, p)
