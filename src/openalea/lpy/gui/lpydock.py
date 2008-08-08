from PyQt4.QtCore import Qt
from PyQt4.QtGui import QApplication, QIcon, QPixmap
from code import InteractiveInterpreter as Interpreter
import shell

def initDocks(lpywidget):
    for dock in [lpywidget.materialDock, lpywidget.parametersDock, lpywidget.descriptionDock]:
        lpywidget.addDockWidget(Qt.LeftDockWidgetArea,dock)
        action = dock.toggleViewAction()
        lpywidget.menuView.addAction(action)        
    lpywidget.addDockWidget(Qt.LeftDockWidgetArea,lpywidget.helpDock)
    action = lpywidget.helpDock.toggleViewAction()
    action.setShortcut(QApplication.translate("MainWindow", "F1", None, QApplication.UnicodeUTF8))
    lpywidget.helpDock.hide()
    lpywidget.helpDock.setFloating(True)
    icon = QIcon()
    icon.addPixmap(QPixmap(":/images/icons/book.png"),QIcon.Normal,QIcon.Off)
    action.setIcon(icon)
    lpywidget.menuHelp.addSeparator()
    lpywidget.menuHelp.addAction(action)
    lpywidget.tabifyDockWidget(lpywidget.materialDock,lpywidget.parametersDock)
    lpywidget.tabifyDockWidget(lpywidget.parametersDock,lpywidget.descriptionDock)
    if lpywidget.withinterpreter :
        shellclass = shell.get_shell_class()
        lpywidget.interpreter = Interpreter()
        lpywidget.shell = shellclass(lpywidget.interpreter, parent=lpywidget.interpreterDock)    
        lpywidget.interpreterDock.setWidget(lpywidget.shell)
        action = lpywidget.interpreterDock.toggleViewAction()
        action.setShortcut(QApplication.translate("MainWindow", "Ctrl+P", None, QApplication.UnicodeUTF8))
        lpywidget.menuView.addSeparator()
        lpywidget.menuView.addAction(action)
        #lpywidget.interpreter.locals['lsystem'] = lpywidget.lsystem
        #lpywidget.interpreter.locals['tree'] = lpywidget.tree
        lpywidget.interpreter.locals['window'] = lpywidget
        lpywidget.interpreter.locals['clear'] = lpywidget.shell.clear
        lpywidget.interpreter.runcode('from openalea.plantgl.all import *')
        lpywidget.interpreter.runcode('from openalea.lpy import *')
        lpywidget.addDockWidget(Qt.BottomDockWidgetArea,lpywidget.interpreterDock)
    else:
        lpywidget.interpreter = None
