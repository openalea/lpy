from . import debugger_ui
from . import debugger_right_ui
from .objectpanel import LpyObjectPanelDock
from .lpyshell import set_shell_widget

from openalea.plantgl.gui.qt import qt
from openalea.plantgl.gui.qt.QtCore import Qt, QCoreApplication, QTimer
from openalea.plantgl.gui.qt.QtGui import QIcon, QPixmap
from openalea.plantgl.gui.qt.QtWidgets import QApplication, QDockWidget, QSplitter, QWidget
_translate = QCoreApplication.translate

class DebugLeftWidget(QWidget,debugger_ui.Ui_Form):
    def __init__(self,parent):
        QWidget.__init__(self,parent)
        debugger_ui.Ui_Form.__init__(self)
        self.setupUi(self)

class DebugRightWidget(QWidget,debugger_right_ui.Ui_Form):
    def __init__(self,parent):
        QWidget.__init__(self,parent)
        debugger_right_ui.Ui_Form.__init__(self)
        self.setupUi(self)

def showMessage(self,msg,timeout):
    if hasattr(self,'statusBar'):
        self.statusBar.showMessage(msg,timeout)
    else:
        print(msg)

def initDocks(lpywidget):
    prevdock = None
    st = lpywidget.statusBar()
    for i,dock in enumerate([lpywidget.materialDock, lpywidget.scalarDock, lpywidget.descriptionDock, lpywidget.parametersDock]):
        lpywidget.addDockWidget(Qt.LeftDockWidgetArea,dock)
        #lpywidget.menuView.addAction(dock.toggleViewAction())
        dock.statusBar = st
        dock.showMessage = showMessage
        if not prevdock is None:
            lpywidget.tabifyDockWidget(prevdock,dock)
        prevdock = dock
    lpywidget.menuView.addAction(lpywidget.materialDock.toggleViewAction())
    lpywidget.menuView.addAction(lpywidget.scalarDock.toggleViewAction())
    lpywidget.vparameterView = lpywidget.menuView.addMenu("Graphical Parameters")
    lpywidget.menuView.addSeparator()
    lpywidget.menuView.addAction(lpywidget.descriptionDock.toggleViewAction())
    lpywidget.menuView.addAction(lpywidget.parametersDock.toggleViewAction())
   
    lpywidget.addDockWidget(Qt.LeftDockWidgetArea,lpywidget.helpDock)
    action = lpywidget.helpDock.toggleViewAction()
    action.setShortcut(_translate("MainWindow", "F1"))
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
    lpywidget.debugWidget = QSplitter(Qt.Horizontal,lpywidget)
    lpywidget.debugWidget.left = DebugLeftWidget(lpywidget.debugWidget)
    lpywidget.debugWidget.addWidget(lpywidget.debugWidget.left)
    lpywidget.debugWidget.right = DebugRightWidget(lpywidget.debugWidget)
    lpywidget.debugWidget.addWidget(lpywidget.debugWidget.right)
    lpywidget.debugWidget.setEnabled(False)
    lpywidget.debugDock.setWidget(lpywidget.debugWidget)
    lpywidget.addDockWidget(Qt.BottomDockWidgetArea,lpywidget.debugDock)    
    action = lpywidget.debugDock.toggleViewAction()
    lpywidget.menuView.addSeparator()
    lpywidget.menuView.addAction(action)
    lpywidget.debugDock.hide()
    #profiler dock
    lpywidget.addDockWidget(Qt.BottomDockWidgetArea,lpywidget.profilerDock)    
    action = lpywidget.profilerDock.toggleViewAction()
    lpywidget.menuView.addAction(action)
    lpywidget.tabifyDockWidget(lpywidget.profilerDock,lpywidget.debugDock)
    lpywidget.profilerDock.hide()
    #interpreter dock
    if lpywidget.withinterpreter :
        #try:
        set_shell_widget(lpywidget)
        #except:
        #    lpywidget.withinterpreter = False
        #    lpywidget.interpreter = None
        #    lpywidget.interpreterDock.hide()

    if lpywidget.withinterpreter:
        action = lpywidget.interpreterDock.toggleViewAction()
        action.setShortcut(QCoreApplication.translate("MainWindow", "Ctrl+P"))
        lpywidget.menuView.addSeparator()
        lpywidget.menuView.addAction(action)
            
        lpywidget.addDockWidget(Qt.BottomDockWidgetArea,lpywidget.interpreterDock)
        lpywidget.tabifyDockWidget(lpywidget.debugDock,lpywidget.interpreterDock)
    else:
        lpywidget.interpreter = None

def initShell(lpywidget):
    return
    lpywidget.interpreter.locals['window'] = lpywidget
    lpywidget.shell.run_code('from openalea.plantgl.all import *')
    lpywidget.shell.run_code('from openalea.lpy import *')
