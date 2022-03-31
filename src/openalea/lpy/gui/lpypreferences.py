from openalea.plantgl.gui.qt import qt
import os
from .lpyprofiling import AnimatedProfiling, ProfilingWithFinalPlot, ProfilingWithNoPlot

from . import generate_ui
from . import lpyprefwidget


from openalea.plantgl.gui.qt.QtCore import QObject, pyqtSignal
from openalea.plantgl.gui.qt.QtWidgets import QDialog


class LpyPreferences:
    def __init__(self,lpyeditor):
        self.editor = lpyeditor
        self.widget = None
        self.dialog = None
    def show(self):
        if True:
            self.dialog = QDialog(self.editor)
            self.widget = lpyprefwidget.Ui_PreferenceDialog()
            self.widget.setupUi(self.dialog)
            self.widget.toolbarAppEdit.setCurrentIndex(self.editor.getToolBarApp()[0])
            self.widget.toolbarAppEdit.activated.connect(self.editor.setToolBarApp) # QObject.connect(self.widget.toolbarAppEdit,SIGNAL('activated(const QString&)'),self.editor.setToolBarApp)
            self.widget.fontFamilyEdit.setCurrentFont(self.editor.codeeditor.currentFont())
            self.widget.fontSizeEdit.setValue(self.editor.codeeditor.currentFont().pointSize())
            self.widget.fontFamilyEdit.currentFontChanged.connect(self.editor.codeeditor.setEditionFontFamily)
            self.widget.fontSizeEdit.valueChanged.connect(self.editor.codeeditor.setEditionFontSize)
            self.widget.spaceForTabEdit.setChecked(self.editor.codeeditor.replaceTab)
            self.widget.spaceForTabEdit.clicked.connect(self.editor.codeeditor.setReplaceTab) # QObject.connect(self.widget.spaceForTabEdit,SIGNAL('clicked(bool)'),self.editor.codeeditor.setReplaceTab)
            self.widget.tabSizeEdit.setValue(self.editor.codeeditor.tabSize())
            self.widget.tabSizeEdit.valueChanged.connect(self.editor.codeeditor.setTabSize) # QObject.connect(self.widget.tabSizeEdit,SIGNAL('valueChanged(int)'),self.editor.codeeditor.setTabSize)
            self.widget.startupReloadEdit.setChecked(self.editor.reloadAtStartup)
            self.widget.startupReloadEdit.clicked.connect(lambda x : setattr(self.editor,'reloadAtStartup',x)) # QObject.connect(self.widget.startupReloadEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'reloadAtStartup',x))
            self.widget.fileMonitoringEdit.setChecked(self.editor.fileMonitoring)
            self.widget.fileMonitoringEdit.clicked.connect(lambda x : setattr(self.editor,'fileMonitoring',x)) # QObject.connect(self.widget.fileMonitoringEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fileMonitoring',x))
            self.widget.fileBackupEdit.setChecked(self.editor.fileBackupEnabled)
            self.widget.fileBackupEdit.clicked.connect(lambda x : setattr(self.editor,'fileBackupEnabled',x)) # QObject.connect(self.widget.fileBackupEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fileBackupEnabled',x))
            self.widget.codeBackupEdit.setChecked(self.editor.codeBackupEnabled)
            self.widget.codeBackupEdit.clicked.connect(lambda x : setattr(self.editor,'codeBackupEnabled',x)) # QObject.connect(self.widget.codeBackupEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'codeBackupEnabled',x))
            self.widget.historySizeEdit.setValue(self.editor.historymaxsize)
            self.widget.historySizeEdit.valueChanged.connect(lambda x : setattr(self.editor,'historymaxsize',x)) # QObject.connect(self.widget.historySizeEdit,SIGNAL('valueChanged(int)'),lambda x : setattr(self.editor,'historymaxsize',x))
            self.widget.pycodeDebugEdit.setChecked(self.editor.showPyCode)
            self.widget.pycodeDebugEdit.clicked.connect(lambda x : setattr(self.editor,'showPyCode',x)) # QObject.connect(self.widget.pycodeDebugEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'showPyCode',x))
            self.widget.useThreadEdit.setChecked(self.editor.with_thread)
            self.widget.useThreadEdit.clicked.connect(lambda x : setattr(self.editor,'with_thread',x)) # QObject.connect(self.widget.useThreadEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'with_thread',x))
            self.widget.fitViewAnimateEdit.setChecked(self.editor.fitAnimationView)
            self.widget.fitViewAnimateEdit.clicked.connect(lambda x : setattr(self.editor,'fitAnimationView',x)) # QObject.connect(self.widget.fitViewEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fitAnimationView',x))
            self.widget.fitViewRunEdit.setChecked(self.editor.fitRunView)
            self.widget.fitViewRunEdit.clicked.connect(lambda x : setattr(self.editor,'fitRunView',x)) # QObject.connect(self.widget.fitViewEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fitAnimationView',x))
            self.widget.visuInfoEdit.setChecked(self.editor.displayMetaInfo)
            self.widget.visuInfoEdit.clicked.connect(lambda x : setattr(self.editor,'displayMetaInfo',x)) # QObject.connect(self.widget.visuInfoEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'displayMetaInfo',x))
            self.widget.gccPathButton.clicked.connect(self.chooseCCompilerPath) # QObject.connect(self.widget.gccPathButton,SIGNAL('clicked(bool)'),self.chooseCCompilerPath)
            self.widget.gccPathEdit.setText(self.editor.cCompilerPath)
            self.widget.gccPathEdit.returnPressed.connect(self.editor.setCCompilerPath) # QObject.connect(self.widget.gccPathEdit,SIGNAL('returnPressed()'),self.editor.setCCompilerPath)
            self.setPofilingButton(self.editor.profilingMode)
            self.widget.profilingAnimatedButton.clicked.connect(self.setProfilingAnimMode) # QObject.connect(self.widget.profilingAnimatedButton,SIGNAL('clicked(bool)'),self.setProfilingAnimMode)
            self.widget.profilingFinalPlotButton.clicked.connect(self.setProfilingFinalPlotMode) # QObject.connect(self.widget.profilingFinalPlotButton,SIGNAL('clicked(bool)'),self.setProfilingFinalPlotMode)
            self.widget.profilingNoPlotButton.clicked.connect(self.setProfilingNoPlotMode) # QObject.connect(self.widget.profilingNoPlotButton,SIGNAL('clicked(bool)'),self.setProfilingNoPlotMode)
            self.widget.integratedViewEdit.setChecked(self.editor.use_own_view3D)
            self.widget.integratedViewEdit.clicked.connect(self.editor.setIntegratedView3D) # QObject.connect(self.widget.integratedViewEdit,SIGNAL('clicked(bool)'),self.editor.setIntegratedView3D)
            #if not self.editor.withinterpreter:
            self.widget.textOutputBox.setEnabled(False)
            #else:
            #    self.widget.LPyConsoleButton.setChecked(self.editor.shellwidget.isSelfStdOutRedirection() or  self.editor.shellwidget.hasMultipleStdOutRedirection())
            #    self.widget.LPyConsoleButton.clicked.connect(self.setOutputRedirection) # QObject.connect(self.widget.LPyConsoleButton,SIGNAL('clicked(bool)'),self.setOutputRedirection)
            #    self.widget.systemConsoleButton.setChecked(self.editor.shellwidget.isSysStdOutRedirection() or  self.editor.shellwidget.hasMultipleStdOutRedirection())
            #    self.widget.systemConsoleButton.clicked.connect(self.setOutputRedirection) # QObject.connect(self.widget.systemConsoleButton,SIGNAL('clicked(bool)'),self.setOutputRedirection)
        self.dialog.show()
    def chooseCCompilerPath(self):
        p = QFileDialog.getExistingDirectory(self.editor, "Choose Compiler Path", self.editor.cCompilerPath )
        if len(p)> 0:
            self.widget.gccPathEdit.setText(p)
            self.editor.setCCompilerPath(p)
    def reSetCCompilerPath(self):
        self.editor.setCCompilerPath(self.widget.gccPathEdit.text())
    def setProfilingAnimMode(self,enabled):
        if enabled :
            self.editor.profilingMode = AnimatedProfiling
    def setProfilingFinalPlotMode(self,enabled):
        if enabled :
            self.editor.profilingMode = ProfilingWithFinalPlot
    def setProfilingNoPlotMode(self,enabled):
        if enabled :
            self.editor.profilingMode = ProfilingWithNoPlot
    def setPofilingButton(self,value):
        if value == AnimatedProfiling:
            self.widget.profilingAnimatedButton.setChecked(True)
        elif value == ProfilingWithFinalPlot:
            self.widget.profilingFinalPlotButton.setChecked(True)
        else:
            self.widget.profilingNoPlotButton.setChecked(True)
    #def setOutputRedirection(self):
    #    self.editor.shellwidget.setOutputRedirection(self.widget.LPyConsoleButton.isChecked(),self.widget.systemConsoleButton.isChecked())
            
