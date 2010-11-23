from PyQt4.QtCore import QSettings, QVariant, QRect, QStringList
from PyQt4.QtGui import QApplication, QFont
import os


def getSettings():
    settings = QSettings(QSettings.IniFormat, QSettings.UserScope,'OpenAlea','LPy')
    return settings

def restoreState(lpywidget):
  #try:
    settings = getSettings()
    settings.beginGroup('history')
    lpywidget.history = [ str(i) for i in settings.value('RecentFiles').toStringList() if not i is None and len(i) > 0]
    openedfiles = [ str(i) for i in settings.value('OpenedFiles').toStringList() if not i is None and len(i) > 0]    
    val, ok = settings.value('MaxSize',QVariant(lpywidget.historymaxsize)).toInt()
    if ok: lpywidget.historymaxsize = val
    lastfocus, ok = settings.value('LastFocus',QVariant(-1)).toInt()
    if not ok: lastfocus = -1
    settings.endGroup()
    settings.beginGroup('file')
    lpywidget.reloadAtStartup = settings.value('reloadstartup',QVariant(lpywidget.reloadAtStartup)).toBool()
    lpywidget.fileMonitoring = settings.value('fileMonitoring',QVariant(lpywidget.fileMonitoring)).toBool()
    lpywidget.fileBackupEnabled = settings.value('fileBackup',QVariant(lpywidget.fileBackupEnabled)).toBool()
    lpywidget.codeBackupEnabled = settings.value('codeBackup',QVariant(lpywidget.codeBackupEnabled)).toBool()
    settings.endGroup()
    settings.beginGroup('compilation')
    lpywidget.showPyCode = settings.value('showPythonCode',QVariant(lpywidget.showPyCode)).toBool() 
    lpywidget.setCCompilerPath(settings.value('CCompilerPath',QVariant('')).toString())
    settings.endGroup()
    settings.beginGroup('threading')
    lpywidget.with_thread = settings.value('activated',QVariant(lpywidget.with_thread)).toBool() 
    settings.endGroup()
    settings.beginGroup('view3D')
    lpywidget.fitAnimationView = settings.value('fitAnimationView',QVariant(lpywidget.fitAnimationView)).toBool() 
    lpywidget.setIntegratedView3D(settings.value('integratedView',QVariant(lpywidget.use_own_view3D)).toBool())
    settings.endGroup()
    settings.beginGroup('profiling')
    lpywidget.profilingMode = settings.value('mode',QVariant(lpywidget.profilingMode)).toInt() 
    settings.endGroup()
    settings.beginGroup('syntax')
    syntaxhlght = settings.value('highlighted',QVariant(True)).toBool()
    lpywidget.codeeditor.setSyntaxHighLightActivation(syntaxhlght)
    lpywidget.actionSyntax.setChecked(syntaxhlght)
    tabhlght = settings.value('tabview',QVariant(True)).toBool()
    lpywidget.codeeditor.setTabHighLightActivation(tabhlght)
    lpywidget.actionTabHightlight.setChecked(tabhlght)
    settings.endGroup()
    settings.beginGroup('appearance')
    nbDock, nbDockValid  = settings.value('nbMaxDocks').toInt()
    if nbDockValid:
        lpywidget.setObjectPanelNb(nbDock,True)
    if settings.contains('state'):
        ba = settings.value('state').toByteArray() 
        if ba: lpywidget.restoreState(ba,0);
    if settings.contains('geometry'):
        rect = settings.value('geometry').toRect()
        if rect:
            maxrect = QApplication.desktop().geometry()
            if maxrect.contains(rect) :
                lpywidget.setGeometry(rect)
    tbapp = str(settings.value('toolbarStyle',QVariant(lpywidget.getToolBarApp()[1])).toString())
    lpywidget.setToolBarApp(tbapp)
    if settings.contains('editionfont'):
        f = QFont()
        fstr = settings.value('editionfont').toString()
        if str(fstr) != 'default' and f.fromString(fstr):
            print 'read font'
            lpywidget.codeeditor.setEditionFont(f)
    settings.endGroup()
    settings.beginGroup('pythonshell')
    lpywidget.shell.multipleStdOutRedirection(not settings.value('uniqueStdout',QVariant(True)).toBool())
    settings.endGroup()
    
    settings.beginGroup('edition')
    lpywidget.codeeditor.replaceTab = settings.value('replaceTab',QVariant(lpywidget.codeeditor.replaceTab)).toBool()
    val,ok = settings.value('tabSize',QVariant(lpywidget.codeeditor.tabSize())).toInt()
    if ok: lpywidget.codeeditor.setTabSize(val)
    settings.endGroup()
    if settings.status() != QSettings.NoError:
        raise 'settings error'
    del settings
    
    if lpywidget.reloadAtStartup and len(openedfiles) > 0:
        for f in openedfiles:
            if os.path.exists(f):
                lpywidget.openfile(f)
        if lastfocus != -1:
            try:
                lpywidget.openfile(openedfiles[lastfocus])
            except:
                pass
  #except:
  #  print "cannot restore state from ini file"    
    
def saveState(lpywidget):
    settings = getSettings()
    settings.beginGroup('history')
    settings.setValue('RecentFiles',QVariant(QStringList(lpywidget.history)))
    settings.setValue('OpenedFiles',QVariant(QStringList([i.fname for i in lpywidget.simulations if not i.fname is None])))
    settings.setValue('MaxSize',QVariant(lpywidget.historymaxsize))
    settings.setValue('LastFocus',QVariant(lpywidget.currentSimulationId))
    settings.endGroup()
    settings.beginGroup('file')
    settings.setValue('reloadstartup',QVariant(lpywidget.reloadAtStartup))
    settings.setValue('fileMonitoring',QVariant(lpywidget.fileMonitoring))
    settings.setValue('fileBackup',QVariant(lpywidget.fileBackupEnabled))
    settings.setValue('codeBackup',QVariant(lpywidget.codeBackupEnabled))
    settings.endGroup()
    settings.beginGroup('threading')
    settings.setValue('activated',QVariant(lpywidget.with_thread)) 
    settings.endGroup()
    settings.beginGroup('view3D')
    settings.setValue('fitAnimationView',QVariant(lpywidget.fitAnimationView)) 
    settings.setValue('integratedView',QVariant(lpywidget.use_own_view3D))
    settings.endGroup()
    settings.beginGroup('compilation')
    settings.setValue('showPythonCode',QVariant(lpywidget.showPyCode))
    settings.setValue('CCompilerPath',QVariant(lpywidget.cCompilerPath))
    settings.endGroup()
    settings.beginGroup('edition')
    settings.setValue('replaceTab',QVariant(lpywidget.codeeditor.replaceTab)) 
    settings.setValue('tabSize',QVariant(lpywidget.codeeditor.tabSize())) 
    settings.endGroup()
    settings.beginGroup('appearance')
    settings.setValue('nbMaxDocks',QVariant(lpywidget.getMaxObjectPanelNb()))    
    settings.setValue('state',QVariant(lpywidget.saveState(0))) 
    settings.setValue('geometry',QVariant(lpywidget.geometry())) 
    settings.setValue('toolbarStyle',QVariant(lpywidget.getToolBarApp()[1]))
    if not lpywidget.codeeditor.isFontToDefault():
        settings.setValue('editionfont',QVariant(lpywidget.codeeditor.editionFont))
    else:
        settings.setValue('editionfont',QVariant('default'))
    settings.endGroup()
    if not lpywidget.interpreter is None:
        settings.beginGroup('pythonshell')
        settings.setValue('uniqueStdout',QVariant(lpywidget.shell.hasMultipleStdOutRedirection()))
        settings.endGroup()
    settings.beginGroup('syntax')
    settings.setValue('highlighted',QVariant(lpywidget.codeeditor.isSyntaxHighLightActivated()))
    settings.setValue('tabview',QVariant(lpywidget.codeeditor.isTabHighLightActivated()))
    settings.endGroup()
    settings.beginGroup('profiling')
    settings.setValue('mode',QVariant(lpywidget.profilingMode))
    settings.endGroup()
    if settings.status() != QSettings.NoError:
            raise Exception('settings error')
