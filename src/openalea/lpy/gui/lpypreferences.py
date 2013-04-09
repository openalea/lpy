from openalea.vpltk.qt import qt
import os
from lpyprofiling import AnimatedProfiling, ProfilingWithFinalPlot, ProfilingWithNoPlot

try:
    import openalea.lpy.gui.py2exe_release
except:
    import compile_ui as ui
    ldir    = os.path.dirname(__file__)
    ui.check_ui_generation(os.path.join(ldir, 'lpyprefwidget.ui'))
    del ldir

import lpyprefwidget


class LpyPreferences:
    def __init__(self,lpyeditor):
        self.editor = lpyeditor
        self.widget = None
        self.dialog = None
    def show(self):
        if True:
            self.dialog = qt.QtGui.QDialog(self.editor)
            self.widget = lpyprefwidget.Ui_PreferenceDialog()
            self.widget.setupUi(self.dialog)
            self.widget.toolbarAppEdit.setCurrentIndex(self.editor.getToolBarApp()[0])
            qt.QtCore.QObject.connect(self.widget.toolbarAppEdit,qt.QtCore.SIGNAL('activated(const QString&)'),self.editor.setToolBarApp)
            self.widget.fontFamilyEdit.setCurrentFont(self.editor.codeeditor.currentFont())
            self.widget.fontSizeEdit.setValue(self.editor.codeeditor.currentFont().pointSize())
            qt.QtCore.QObject.connect(self.widget.fontFamilyEdit,qt.QtCore.SIGNAL('currentFontChanged(const QFont&)'),
                            self.editor.codeeditor.setEditionFontFamily)
            qt.QtCore.QObject.connect(self.widget.fontSizeEdit,qt.QtCore.SIGNAL('valueChanged(int)'),
                            self.editor.codeeditor.setEditionFontSize)
            self.widget.spaceForTabEdit.setChecked(self.editor.codeeditor.replaceTab)
            qt.QtCore.QObject.connect(self.widget.spaceForTabEdit,qt.QtCore.SIGNAL('clicked(bool)'),self.editor.codeeditor.setReplaceTab)
            self.widget.tabSizeEdit.setValue(self.editor.codeeditor.tabSize())
            qt.QtCore.QObject.connect(self.widget.tabSizeEdit,qt.QtCore.SIGNAL('valueChanged(int)'),self.editor.codeeditor.setTabSize)
            self.widget.startupReloadEdit.setChecked(self.editor.reloadAtStartup)
            qt.QtCore.QObject.connect(self.widget.startupReloadEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'reloadAtStartup',x))
            self.widget.fileMonitoringEdit.setChecked(self.editor.fileMonitoring)
            qt.QtCore.QObject.connect(self.widget.fileMonitoringEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fileMonitoring',x))
            self.widget.fileBackupEdit.setChecked(self.editor.fileBackupEnabled)
            qt.QtCore.QObject.connect(self.widget.fileBackupEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fileBackupEnabled',x))
            self.widget.codeBackupEdit.setChecked(self.editor.codeBackupEnabled)
            qt.QtCore.QObject.connect(self.widget.codeBackupEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'codeBackupEnabled',x))
            self.widget.historySizeEdit.setValue(self.editor.historymaxsize)
            qt.QtCore.QObject.connect(self.widget.historySizeEdit,qt.QtCore.SIGNAL('valueChanged(int)'),lambda x : setattr(self.editor,'historymaxsize',x))
            self.widget.pycodeDebugEdit.setChecked(self.editor.showPyCode)
            qt.QtCore.QObject.connect(self.widget.pycodeDebugEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'showPyCode',x))
            self.widget.useThreadEdit.setChecked(self.editor.with_thread)
            qt.QtCore.QObject.connect(self.widget.useThreadEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'with_thread',x))
            self.widget.fitViewEdit.setChecked(self.editor.fitAnimationView)
            qt.QtCore.QObject.connect(self.widget.fitViewEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fitAnimationView',x))
            self.widget.visuInfoEdit.setChecked(self.editor.displayMetaInfo)
            qt.QtCore.QObject.connect(self.widget.visuInfoEdit,qt.QtCore.SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'displayMetaInfo',x))
            qt.QtCore.QObject.connect(self.widget.gccPathButton,qt.QtCore.SIGNAL('clicked(bool)'),self.chooseCCompilerPath)
            self.widget.gccPathEdit.setText(self.editor.cCompilerPath)
            qt.QtCore.QObject.connect(self.widget.gccPathEdit,qt.QtCore.SIGNAL('returnPressed()'),self.editor.setCCompilerPath)
            self.setPofilingButton(self.editor.profilingMode)
            qt.QtCore.QObject.connect(self.widget.profilingAnimatedButton,qt.QtCore.SIGNAL('clicked(bool)'),self.setProfilingAnimMode)
            qt.QtCore.QObject.connect(self.widget.profilingFinalPlotButton,qt.QtCore.SIGNAL('clicked(bool)'),self.setProfilingFinalPlotMode)
            qt.QtCore.QObject.connect(self.widget.profilingNoPlotButton,qt.QtCore.SIGNAL('clicked(bool)'),self.setProfilingNoPlotMode)
            self.widget.integratedViewEdit.setChecked(self.editor.use_own_view3D)
            qt.QtCore.QObject.connect(self.widget.integratedViewEdit,qt.QtCore.SIGNAL('clicked(bool)'),self.editor.setIntegratedView3D)
            if not self.editor.withinterpreter:
                self.widget.textOutputBox.setEnabled(False)
            else:
                self.widget.LPyConsoleButton.setChecked(self.editor.shell.isSelfStdOutRedirection() or  self.editor.shell.hasMultipleStdOutRedirection())
                qt.QtCore.QObject.connect(self.widget.LPyConsoleButton,qt.QtCore.SIGNAL('clicked(bool)'),self.setOutputRedirection)
                self.widget.systemConsoleButton.setChecked(self.editor.shell.isSysStdOutRedirection() or  self.editor.shell.hasMultipleStdOutRedirection())
                qt.QtCore.QObject.connect(self.widget.systemConsoleButton,qt.QtCore.SIGNAL('clicked(bool)'),self.setOutputRedirection)
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
    def setOutputRedirection(self):
        self.editor.shell.setOutputRedirection(self.widget.LPyConsoleButton.isChecked(),self.widget.systemConsoleButton.isChecked())
            