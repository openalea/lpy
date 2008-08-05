from PyQt4.QtCore import QSettings, QVariant, QRect, QStringList
from PyQt4.QtGui import QApplication


def getSettings():
    settings = QSettings(QSettings.IniFormat, QSettings.UserScope,'OpenAlea','LPy')
    return settings

def restoreState(lpywidget):
    settings = getSettings()
    settings.beginGroup('history')
    lpywidget.history = [ str(i) for i in settings.value('RecentFiles').toStringList() if not i is None and len(i) > 0]
    settings.endGroup()
    settings.beginGroup('threading')
    lpywidget.with_thread = settings.value('activated',QVariant(False)).toBool() 
    settings.endGroup()
    settings.beginGroup('animation')
    lpywidget.fitAnimationView = settings.value('fitview',QVariant(True)).toBool() 
    settings.endGroup()
    settings.beginGroup('syntax')
    syntaxhlght = settings.value('highlighted',QVariant(True)).toBool()
    lpywidget.codeeditor.setSyntaxHighLightActivation(syntaxhlght)
    lpywidget.actionSyntax.setChecked(syntaxhlght)
    settings.endGroup()
    settings.beginGroup('appearance')
    ba = settings.value('state').toByteArray() 
    if ba:
        lpywidget.restoreState(ba,0);
    rect = settings.value('geometry').toRect()
    if rect:
        maxrect = QApplication.desktop().geometry()
        if maxrect.contains(rect) :
            lpywidget.setGeometry(rect)
    settings.endGroup()
    if settings.status() != QSettings.NoError:
        raise 'settings error'
    del settings
    
    
def saveState(lpywidget):
    settings = getSettings()
    settings.beginGroup('history')
    settings.setValue('RecentFiles',QVariant(QStringList(lpywidget.history)))
    settings.endGroup()
    settings.beginGroup('threading')
    settings.setValue('activated',QVariant(lpywidget.with_thread)) 
    settings.endGroup()
    settings.beginGroup('appearance')
    settings.setValue('state',QVariant(lpywidget.saveState(0))) 
    settings.setValue('geometry',QVariant(lpywidget.geometry())) 
    settings.endGroup()
    if not lpywidget.interpreter is None:
        settings.beginGroup('pythonshell')
        settings.setValue('visible',QVariant(lpywidget.interpreterDock.isVisible()))
        settings.endGroup()
    settings.beginGroup('syntax')
    settings.setValue('highlighted',QVariant(lpywidget.codeeditor.isSyntaxHighLightActivated()))
    settings.endGroup()
    if settings.status() != QSettings.NoError:
            raise Exception('settings error')
