from openalea.vpltk.qt import qt
import os

def getTmpLpyDir():
    tmpdir = os.path.join(str(qt.QtCore.QDir.tempPath()),'lpy','new')
    if not os.path.exists(tmpdir):
        os.makedirs(tmpdir)
    return tmpdir

tmpfileid = None
prefix = 'lpytmp_'
suffix = '.lpy'
    
def getNewTmpLpyFile():
    global tmpfileid
    tmpdir = getTmpLpyDir()
    if tmpfileid is None:        
        tmpfiles = os.listdir(tmpdir)
        tmpfiles = [f for f in tmpfiles if (prefix in f) and (suffix in f)]
        if len(tmpfiles) > 0:
            tmpfileid = max([int(f[len(prefix):-len(suffix)]) for f in tmpfiles])
        else:
            tmpfileid = -1
    tmpfileid += 1
    return os.path.abspath(os.path.join(tmpdir,prefix+str(tmpfileid)+suffix))

 
def getPreviousTmpLpyFiles():
    tmpdir = getTmpLpyDir()
    tmpfiles = os.listdir(tmpdir)
    tmpfiles = [os.path.abspath(os.path.join(tmpdir,f)) for f in tmpfiles if (prefix in f) and (suffix in f)]
    return tmpfiles