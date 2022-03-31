from openalea.plantgl.gui.qt import QT_API, PYQT5_API, PYQT4_API, PYSIDE_API
from openalea.plantgl.gui.qt.uic import compileUi, compile_args

import os
import sys


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
    fstream = open(pyfname,'w')
#    compile_args["from_imports"] = ""
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

    if 'CONDA_PREFIX' in os.environ:
        exe = def_exe()        
    elif sys.platform == 'darwin':
        exe = def_exe('-'+str(sys.version_info[0])+'.'+str(sys.version_info[1]))
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
    txt = open(fname,'r').read()
    for pattern,api in list(patternapi.items()):
        if pattern in txt: return api
    return None

def is_generation_required(srcfile, generatedfile):
    api = os.environ[QT_API]
    if not os.path.exists(srcfile) : return False
    if not os.path.exists(generatedfile) : return True
    if not os.access(generatedfile,os.F_OK|os.W_OK) : return False
    if len(open(generatedfile).read()) == 0 : return True
    if os.stat(generatedfile).st_mtime < os.stat(srcfile).st_mtime : return True
    if not api in detect_file_api(generatedfile) : return True
    return False

def check_ui_generation(uifname):
    """ check if a py file should regenerated from a ui """
    pyfname = get_uifnames_from(uifname)
    if is_generation_required(uifname, pyfname):
        print('Generate Ui', repr(uifname))
        compile_ui(uifname)

def check_rc_generation(rcfname):
    """ check if a py file should regenerated from a ui """
    pyfname = get_rcfnames_from(rcfname)
    if is_generation_required(rcfname, pyfname):
        print('Generate Rc', repr(rcfname))
        compile_rc(rcfname)
