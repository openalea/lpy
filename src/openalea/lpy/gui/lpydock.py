from PyQt4.QtCore import Qt
from PyQt4.QtGui import QApplication, QIcon, QPixmap,QWidget,QDockWidget
from code import InteractiveInterpreter as Interpreter
import shell
import debugger_ui


class DebugWidget(QWidget,debugger_ui.Ui_Form):
    def __init__(self,parent):
        QWidget.__init__(self,parent)
        debugger_ui.Ui_Form.__init__(self)
        self.setupUi(self)

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
    # debug dock
    lpywidget.debugDock = QDockWidget("Debugger",lpywidget)
    lpywidget.debugDock.setObjectName("LpyDebugger")
    lpywidget.debugWidget = DebugWidget(lpywidget)
    lpywidget.debugWidget.setEnabled(False)
    lpywidget.debugDock.setWidget(lpywidget.debugWidget)
    lpywidget.addDockWidget(Qt.BottomDockWidgetArea,lpywidget.debugDock)
    action = lpywidget.debugDock.toggleViewAction()
    lpywidget.menuView.addAction(action)
    lpywidget.debugDock.hide()
    #interpreter dock
    if lpywidget.withinterpreter :
        shellclass = shell.get_shell_class()
        lpywidget.interpreter = Interpreter()
        lpywidget.shell = shellclass(lpywidget.interpreter, parent=lpywidget.interpreterDock)    
        lpywidget.interpreterDock.setWidget(lpywidget.shell)
        action = lpywidget.interpreterDock.toggleViewAction()
        action.setShortcut(QApplication.translate("MainWindow", "Ctrl+P", None, QApplication.UnicodeUTF8))
        lpywidget.menuView.addSeparator()
        lpywidget.menuView.addAction(action)
        lpywidget.interpreter.locals['window'] = lpywidget
        lpywidget.interpreter.locals['clear'] = lpywidget.shell.clear
        lpywidget.interpreter.runcode('from openalea.plantgl.all import *')
        lpywidget.interpreter.runcode('from openalea.lpy import *')
        lpywidget.addDockWidget(Qt.BottomDockWidgetArea,lpywidget.interpreterDock)
        lpywidget.tabifyDockWidget(lpywidget.debugDock,lpywidget.interpreterDock)
    else:
        lpywidget.interpreter = None
