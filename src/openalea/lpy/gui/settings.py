from openalea.vpltk.qt.compat import *
from openalea.vpltk.qt import qt
import os


def getSettings():
    settings = qt.QtCore.QSettings(qt.QtCore.QSettings.IniFormat, qt.QtCore.QSettings.UserScope,'OpenAlea','LPy')
    return settings

def restoreState(lpywidget):
  try:
    settings = getSettings()
    settings.beginGroup('history')

    lpywidget.history = [ unicode(i) for i in list(settings.value('RecentFiles')) if not i is None and len(i) > 0]
    try:
        openedfiles = [ unicode(i) for i in list(settings.value('OpenedFiles')) if not i is None and len(i) > 0]
    except:
        openedfiles = ''
    try:
        val = int(settings.value('MaxSize',to_qvariant(lpywidget.historymaxsize)))
        lpywidget.historymaxsize = val
    except:
        pass
    try :
        lastfocus = int(settings.value('LastFocus',to_qvariant(-1)))
    except:
        lastfocus = -1
    settings.endGroup()
    settings.beginGroup('file')
    # settings.value send "true" if value is "True" (and "false" if it is "False")
    lpywidget.reloadAtStartup = settings.value('reloadstartup',lpywidget.reloadAtStartup)=='true'
    lpywidget.fileMonitoring = settings.value('fileMonitoring',lpywidget.fileMonitoring)=='true'
    lpywidget.fileBackupEnabled = settings.value('fileBackup',lpywidget.fileBackupEnabled)=='true'
    lpywidget.codeBackupEnabled = settings.value('codeBackup',lpywidget.codeBackupEnabled)=='true'
    settings.endGroup()
    settings.beginGroup('compilation')
    lpywidget.showPyCode = settings.value('showPythonCode',lpywidget.showPyCode)=='true'
    lpywidget.setCCompilerPath(settings.value('CCompilerPath',str('')))
    settings.endGroup()
    settings.beginGroup('threading')
    lpywidget.with_thread = settings.value('activated',lpywidget.with_thread)=='true'
    settings.endGroup()
    settings.beginGroup('view3D')
    lpywidget.fitAnimationView = settings.value('fitAnimationView',lpywidget.fitAnimationView)=='true' 
    lpywidget.setIntegratedView3D(settings.value('integratedView',lpywidget.use_own_view3D)=='true')
    lpywidget.displayMetaInfo = settings.value('displayMetaInfoAtRun',lpywidget.displayMetaInfo)=='true'
    settings.endGroup()
    settings.beginGroup('profiling')
    try:
        lpywidget.profilingMode = int(settings.value('mode',lpywidget.profilingMode))
    except:
        pass
    settings.endGroup()
    settings.beginGroup('application')
    lpywidget.svnLastRevisionChecked = int(settings.value('svnLastRevisionChecked',lpywidget.svnLastRevisionChecked))
    lpywidget.svnLastDateChecked = float(settings.value('svnLastDateChecked',lpywidget.svnLastDateChecked))
    lpywidget.safeLaunch = settings.value('safeLaunch',False)=='true'
    import sys
    if '--safe' in sys.argv:    lpywidget.safeLaunch = True
    if '--no-safe' in sys.argv:    lpywidget.safeLaunch = False
    settings.endGroup()
    settings.beginGroup('syntax')
    syntaxhlght = settings.value('highlighted',True) == 'true'
    lpywidget.codeeditor.setSyntaxHighLightActivation(syntaxhlght)
    lpywidget.actionSyntax.setChecked(syntaxhlght)
    tabhlght = settings.value('tabview',True) == 'true'
    lpywidget.codeeditor.setTabHighLightActivation(tabhlght)
    lpywidget.actionTabHightlight.setChecked(tabhlght)
    settings.endGroup()
    settings.beginGroup('appearance')
    try:
        nbDock  = int(settings.value('nbMaxDocks'))
        lpywidget.setObjectPanelNb(nbDock,True)
    except:
        pass  
    if not lpywidget.safeLaunch and settings.contains('state'):
        ba = bytearray(settings.value('state'))
        if ba : lpywidget.restoreState(ba,0);
    if settings.contains('geometry'):
        rect = settings.value('geometry')
        if rect:
            maxrect = qt.QtGui.QApplication.desktop().geometry()
            if maxrect.contains(rect) :
                lpywidget.setGeometry(rect)
    tbapp = str(settings.value('toolbarStyle',to_qvariant(lpywidget.getToolBarApp()[1])))
    lpywidget.setToolBarApp(tbapp)
    if settings.contains('editionfont'):
        f = qt.QtGui.QFont()
        fstr = str(from_qvariant(settings.value('editionfont')))
        if fstr != 'default' and f.fromString(fstr):
            #print 'read font',fstr
            lpywidget.codeeditor.setEditionFont(f)
    settings.endGroup()
    settings.beginGroup('stdout')
    lc = settings.value('lpyshell',True)=='true'
    sc = settings.value('sysconsole',False)=='true'
    lpywidget.shell.setOutputRedirection(lc,sc,1)
    settings.endGroup()
    
    settings.beginGroup('stderr')
    lc = settings.value('lpyshell',True)=='true'
    sc = settings.value('sysconsole',False)=='true'
    lpywidget.shell.setOutputRedirection(lc,sc,2)
    settings.endGroup()
    
    settings.beginGroup('edition')
    lpywidget.codeeditor.replaceTab = settings.value('replaceTab',lpywidget.codeeditor.replaceTab)=='true'
    try:
        val = int(settings.value('tabSize',lpywidget.codeeditor.tabSize()))
        lpywidget.codeeditor.setTabSize(val)
    except:
        pass
    settings.endGroup()

    if settings.status() != qt.QtCore.QSettings.NoError:
        raise 'settings error'
    del settings
    
    if lpywidget.reloadAtStartup and len(openedfiles) > 0:
        for f in openedfiles:
            if os.path.exists(f):
                lpywidget.openfile(f)
        if lastfocus != -1 and os.path.exists(openedfiles[lastfocus]):
            try:
                lpywidget.openfile(openedfiles[lastfocus])
            except:
                pass
  except Exception, e:
    print "cannot restore correctly state from ini file:", e

def saveState(lpywidget):
    settings = getSettings()
    settings.beginGroup('history')
    settings.setValue('RecentFiles',to_qvariant(list(lpywidget.history)))
    settings.setValue('OpenedFiles',to_qvariant(list([i.fname for i in lpywidget.simulations if not i.fname is None])))
    settings.setValue('MaxSize',to_qvariant(lpywidget.historymaxsize))
    settings.setValue('LastFocus',to_qvariant(lpywidget.currentSimulationId))
    settings.endGroup()
    settings.beginGroup('file')
    settings.setValue('reloadstartup',to_qvariant(lpywidget.reloadAtStartup))
    settings.setValue('fileMonitoring',to_qvariant(lpywidget.fileMonitoring))
    settings.setValue('fileBackup',to_qvariant(lpywidget.fileBackupEnabled))
    settings.setValue('codeBackup',to_qvariant(lpywidget.codeBackupEnabled))
    settings.endGroup()
    settings.beginGroup('threading')
    settings.setValue('activated',to_qvariant(lpywidget.with_thread)) 
    settings.endGroup()
    settings.beginGroup('view3D')
    settings.setValue('fitAnimationView',to_qvariant(lpywidget.fitAnimationView)) 
    settings.setValue('integratedView',to_qvariant(lpywidget.use_own_view3D))
    settings.setValue('displayMetaInfoAtRun',to_qvariant(lpywidget.displayMetaInfo))
    settings.endGroup()
    settings.beginGroup('compilation')
    
    settings.setValue('showPythonCode',to_qvariant(lpywidget.showPyCode))
    
    settings.setValue('CCompilerPath',to_qvariant(lpywidget.cCompilerPath))
    settings.endGroup()
    settings.beginGroup('edition')
    settings.setValue('replaceTab',to_qvariant(lpywidget.codeeditor.replaceTab)) 
    settings.setValue('tabSize',to_qvariant(lpywidget.codeeditor.tabSize())) 
    settings.endGroup()
    settings.beginGroup('application')
    settings.setValue('svnLastRevisionChecked',to_qvariant(lpywidget.svnLastRevisionChecked))
    settings.setValue('svnLastDateChecked',to_qvariant(lpywidget.svnLastDateChecked))
    if hasattr(lpywidget,'safeLaunch') : settings.setValue('safeLaunch',to_qvariant(lpywidget.safeLaunch))
    settings.endGroup()
    settings.beginGroup('appearance')
    settings.setValue('nbMaxDocks',to_qvariant(lpywidget.getMaxObjectPanelNb()))    
    settings.setValue('state',to_qvariant(lpywidget.saveState(0))) 
    settings.setValue('geometry',to_qvariant(lpywidget.geometry())) 
    settings.setValue('toolbarStyle',to_qvariant(lpywidget.getToolBarApp()[1]))
    if not lpywidget.codeeditor.isFontToDefault():
        settings.setValue('editionfont',to_qvariant(lpywidget.codeeditor.editionFont.toString()))
    else:
        settings.setValue('editionfont','default')
    settings.endGroup()
    if not lpywidget.interpreter is None:
        settings.beginGroup('stdout')
        outinshell = lpywidget.shell.hasMultipleStdOutRedirection() or lpywidget.shell.isSelfStdOutRedirection()
        outinsys   = lpywidget.shell.hasMultipleStdOutRedirection() or lpywidget.shell.isSysStdOutRedirection()
        settings.setValue('lpyshell',to_qvariant(outinshell))
        settings.setValue('sysconsole',to_qvariant(outinsys))
        settings.endGroup()
        settings.beginGroup('stderr')
        errinshell = lpywidget.shell.hasMultipleStdErrRedirection() or lpywidget.shell.isSelfStdErrRedirection()
        errinsys   = lpywidget.shell.hasMultipleStdErrRedirection() or lpywidget.shell.isSysStdErrRedirection()
        settings.setValue('lpyshell',to_qvariant(errinshell))
        settings.setValue('sysconsole',to_qvariant(errinsys))
        settings.endGroup()
    settings.beginGroup('syntax')
    settings.setValue('highlighted',to_qvariant(lpywidget.codeeditor.isSyntaxHighLightActivated()))
    settings.setValue('tabview',to_qvariant(lpywidget.codeeditor.isTabHighLightActivated()))
    settings.endGroup()
    settings.beginGroup('profiling')
    settings.setValue('mode',to_qvariant(lpywidget.profilingMode))
    settings.endGroup()
    if settings.status() != qt.QtCore.QSettings.NoError:
            raise Exception('settings error')