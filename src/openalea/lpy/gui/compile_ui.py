from openalea.vpltk.qt import qt
from openalea.vpltk.qt.uic import compileUi, compile_args

import os
import sys

from openalea.vpltk.qt import QT_API, PYQT5_API, PYQT4_API, PYSIDE_API

def get_uifnames_from(fname):
    uiprefix = os.path.splitext(fname)[0]
    pyfname  = uiprefix + '.py'
    return pyfname
    
def get_rcfnames_from(fname):
    rcprefix = os.path.splitext(fname)[0]
    pyfname  = rcprefix + '_rc.py'
    return pyfname

def compile_ui(uifname):
    """ compile a Ui """
    pyfname = get_uifnames_from(uifname)
    fstream = file(pyfname,'w')
    compileUi(uifname, fstream, **compile_args)
    fstream.close()

def compile_rc (rcfname) :
    """ compile a Ressource file """
    pyfname = get_rcfnames_from(rcfname)
    def def_exe(suffix = ''):
        if os.environ[QT_API] in PYQT5_API:
            exe = 'pyrcc5'+suffix
        else:
            exe = 'pyrcc4'+suffix
        return exe

    if os.environ.has_key('CONDA_PREFIX'):
        exe = def_exe()        
    elif sys.platform == 'darwin':
        exe = def_exe('-2.7')
    elif sys.platform == 'posix':
        exe = def_exe()
    else:
        exe = os.path.join(sys.prefix,'pyrcc4.bat')
        if not os.path.exists(exe):
            exe = def_exe()
    cmd = '%s "%s" > "%s"' % (exe,rcfname, pyfname)
    os.system(cmd)

def detect_file_api(fname):
    patternapi = {'PyQt5':PYQT5_API, 'PyQt4':PYQT4_API, 'PySide':PYSIDE_API} 
    txt = file(fname,'r').read()
    for pattern,api in patternapi.items():
        if pattern in txt: return api
    return None

def check_ui_generation(uifname):
    """ check if a py file should regenerated from a ui """
    api = os.environ[QT_API]
    pyfname = get_uifnames_from(uifname)
    if ( os.path.exists(uifname) and 
         not os.path.exists(pyfname) or
         (os.access(pyfname,os.F_OK|os.W_OK) and
         os.stat(pyfname).st_mtime < os.stat(uifname).st_mtime or not api in detect_file_api(pyfname))) :
        print 'Generate Ui', repr(uifname)
        compile_ui(uifname)

def check_rc_generation(rcfname):
    """ check if a py file should regenerated from a ui """
    api = os.environ[QT_API]
    pyfname = get_rcfnames_from(rcfname)
    if (os.path.exists(rcfname) and 
        not os.path.exists(pyfname) or
        (os.access(pyfname,os.F_OK|os.W_OK) and
        os.stat(pyfname).st_mtime < os.stat(rcfname).st_mtime or not api in detect_file_api(pyfname))) :
        print 'Generate Rc', repr(rcfname)
        compile_rc(rcfname)
