from PyQt4.QtCore import *
from PyQt4.QtGui import *
import os

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
            self.dialog = QDialog(self.editor)
            self.widget = lpyprefwidget.Ui_PreferenceDialog()
            self.widget.setupUi(self.dialog)
            self.widget.toolbarAppEdit.setCurrentIndex(self.editor.getToolBarApp()[0])
            QObject.connect(self.widget.toolbarAppEdit,SIGNAL('activated(const QString&)'),self.editor.setToolBarApp)
            self.widget.fontFamilyEdit.setCurrentFont(self.editor.codeeditor.currentFont())
            self.widget.fontSizeEdit.setValue(self.editor.codeeditor.currentFont().pointSize())
            QObject.connect(self.widget.fontFamilyEdit,SIGNAL('currentFontChanged(const QFont&)'),
                            self.editor.codeeditor.setEditionFontFamily)
            QObject.connect(self.widget.fontSizeEdit,SIGNAL('valueChanged(int)'),
                            self.editor.codeeditor.setEditionFontSize)
            self.widget.spaceForTabEdit.setChecked(self.editor.codeeditor.replaceTab)
            QObject.connect(self.widget.spaceForTabEdit,SIGNAL('clicked(bool)'),self.editor.codeeditor.setReplaceTab)
            self.widget.tabSizeEdit.setValue(self.editor.codeeditor.tabSize())
            QObject.connect(self.widget.tabSizeEdit,SIGNAL('valueChanged(int)'),self.editor.codeeditor.setTabSize)
            self.widget.startupReloadEdit.setChecked(self.editor.reloadAtStartup)
            QObject.connect(self.widget.startupReloadEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'reloadAtStartup',x))
            self.widget.fileMonitoringEdit.setChecked(self.editor.fileMonitoring)
            QObject.connect(self.widget.fileMonitoringEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fileMonitoring',x))
            self.widget.fileBackupEdit.setChecked(self.editor.fileBackupEnabled)
            QObject.connect(self.widget.fileBackupEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fileBackupEnabled',x))
            self.widget.codeBackupEdit.setChecked(self.editor.codeBackupEnabled)
            QObject.connect(self.widget.codeBackupEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'codeBackupEnabled',x))
            self.widget.historySizeEdit.setValue(self.editor.historymaxsize)
            QObject.connect(self.widget.historySizeEdit,SIGNAL('valueChanged(int)'),lambda x : setattr(self.editor,'historymaxsize',x))
            self.widget.pycodeDebugEdit.setChecked(self.editor.showPyCode)
            QObject.connect(self.widget.pycodeDebugEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'showPyCode',x))
            self.widget.useThreadEdit.setChecked(self.editor.with_thread)
            QObject.connect(self.widget.useThreadEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'with_thread',x))
            self.widget.fitViewEdit.setChecked(self.editor.fitAnimationView)
            QObject.connect(self.widget.fitViewEdit,SIGNAL('clicked(bool)'),lambda x : setattr(self.editor,'fitAnimationView',x))
            QObject.connect(self.widget.gccPathButton,SIGNAL('clicked(bool)'),self.chooseCCompilerPath)
            self.widget.gccPathEdit.setText(self.editor.cCompilerPath)
            QObject.connect(self.widget.gccPathEdit,SIGNAL('returnPressed()'),self.editor.setCCompilerPath)
        self.dialog.show()
    def chooseCCompilerPath(self):
        p = QFileDialog.getExistingDirectory(self.editor, "Choose Compiler Path", self.editor.cCompilerPath )
        if len(p)> 0:
            self.widget.gccPathEdit.setText(p)
            self.editor.setCCompilerPath(p)
    def reSetCCompilerPath(self):
        self.editor.setCCompilerPath(self.widget.gccPathEdit.text())