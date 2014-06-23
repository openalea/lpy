try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

from openalea.vpltk.qt import qt
    
class ObjectDialog(qt.QtGui.QDialog):
    """the class that will create dialog between the panel and the editor window"""
    def __init__(self, *args):
        """during the init of the dialog we have to know the editor we want to open, the typ variable will allow us to know that"""
        qt.QtGui.QDialog.__init__(self,*args)
        self.hasChanged = False
        self.automaticUpdate = False
        #self.setModal(True)
    
    def setupUi(self,editor):
        self.setObjectName("ObjectDialog")
        self.resize(389, 282)
        self.verticalLayout = qt.QtGui.QVBoxLayout(self)
        self.verticalLayout.setSpacing(2)
        self.verticalLayout.setContentsMargins(2, 2, 2, 2)
        self.verticalLayout.setObjectName("verticalLayout")
        self._menu = qt.QtGui.QMenuBar(self)
        try:
            self._menu.setNativeMenuBar(False)
        except: pass
        self.verticalLayout.addWidget(self._menu)
        self.objectView = editor
        sizePolicy = qt.QtGui.QSizePolicy(qt.QtGui.QSizePolicy.Preferred, qt.QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(5)
        #sizePolicy.setHeightForWidth(self.objectView.sizePolicy().hasHeightForWidth())
        self.objectView.setSizePolicy(sizePolicy)
        self.objectView.setObjectName("objectView")
        self.verticalLayout.addWidget(self.objectView)
        self.horizontalLayout = qt.QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setSizeConstraint(qt.QtGui.QLayout.SetFixedSize)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.autoUpdateCheckBox = qt.QtGui.QCheckBox(self)
        self.autoUpdateCheckBox.setObjectName("autoUpdateCheckBox")
        self.horizontalLayout.addWidget(self.autoUpdateCheckBox)
        spacerItem = qt.QtGui.QSpacerItem(48, 20, qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.okButton = qt.QtGui.QPushButton(self)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout.addWidget(self.okButton)
        self.applyButton = qt.QtGui.QPushButton(self)
        self.applyButton.setObjectName("applyButton")
        self.horizontalLayout.addWidget(self.applyButton)
        self.cancelButton = qt.QtGui.QPushButton(self)
        self.cancelButton.setObjectName("cancelButton")
        self.horizontalLayout.addWidget(self.cancelButton)
        self.verticalLayout.addLayout(self.horizontalLayout)
        
        self.setWindowTitle(qt.QtGui.QApplication.translate("ObjectDialog", "Object Editor", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.autoUpdateCheckBox.setText(qt.QtGui.QApplication.translate("ObjectDialog", "Auto update", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(qt.QtGui.QApplication.translate("ObjectDialog", "Ok", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.applyButton.setText(qt.QtGui.QApplication.translate("ObjectDialog", "Apply", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.cancelButton.setText(qt.QtGui.QApplication.translate("ObjectDialog", "Cancel", None, qt.QtGui.QApplication.UnicodeUTF8))
      
        qt.QtCore.QObject.connect(self.cancelButton,qt.QtCore.SIGNAL('pressed()'),self.reject)
        qt.QtCore.QObject.connect(self.okButton,qt.QtCore.SIGNAL('pressed()'),self.__ok)
        qt.QtCore.QObject.connect(self.applyButton,qt.QtCore.SIGNAL('pressed()'),self.__apply)
        qt.QtCore.QObject.connect(self.autoUpdateCheckBox,qt.QtCore.SIGNAL('toggled(bool)'),self.setAutomaticUpdate)
        qt.QtCore.QObject.connect(self.objectView,qt.QtCore.SIGNAL('valueChanged()'),self.__valueChanged)
        
    def menu(self):
        return self._menu
    
    def __valueChanged(self):
        if self.automaticUpdate:
            self.emit(qt.QtCore.SIGNAL('valueChanged()'))
        else:
            self.hasChanged = True

    def __apply(self):
        self.emit(qt.QtCore.SIGNAL('valueChanged()'))
        self.hasChanged = False
        
    def __ok(self):
        self.emit(qt.QtCore.SIGNAL('valueChanged()'))
        self.hasChanged = False
        self.accept()
        self.close()

    def hideEvent(self,event):
        self.emit(qt.QtCore.SIGNAL('hidden()'))

    def setAutomaticUpdate(self,value):
        """setAutomaticUpdate: checking the autoupdate box will make the qt.QtGui.QDialog send a 'valueChanged()' signal each time it recieve the same Signal from the objectView"""
        if self.automaticUpdate != value:
            self.automaticUpdate = value
            self.applyButton.setEnabled(not self.automaticUpdate)
            self.emit(qt.QtCore.SIGNAL("AutomaticUpdate(bool)"),value)
            if self.automaticUpdate and self.hasChanged :
                self.__apply()
                

    def closeEvent(self,event):
        qt.QtGui.QDialog.closeEvent(self,event)

