try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

from openalea.plantgl.gui.qt import qt

from openalea.plantgl.gui.qt.QtCore import QObject, pyqtSignal
from openalea.plantgl.gui.qt.QtWidgets import QApplication, QCheckBox, QDialog, QHBoxLayout, QLayout, QMenuBar, QPushButton, QSizePolicy, QSpacerItem, QVBoxLayout

    
class ObjectDialog(QDialog):
    """the class that will create dialog between the panel and the editor window"""
    valueChanged = pyqtSignal()
    hidden = pyqtSignal()
    AutomaticUpdate = pyqtSignal(bool)

    def __init__(self, *args):
        """during the init of the dialog we have to know the editor we want to open, the typ variable will allow us to know that"""
        QDialog.__init__(self,*args)
        self.hasChanged = False
        self.automaticUpdate = False
        #self.setModal(True)
    
    def setupUi(self,editor):
        self.setObjectName("ObjectDialog")
        self.resize(389, 282)
        self.verticalLayout = QVBoxLayout(self)
        self.verticalLayout.setSpacing(2)
        self.verticalLayout.setContentsMargins(2, 2, 2, 2)
        self.verticalLayout.setObjectName("verticalLayout")
        self._menu = QMenuBar(self)
        try:
            self._menu.setNativeMenuBar(False)
        except: pass
        self.verticalLayout.addWidget(self._menu)
        self.objectView = editor
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(5)
        #sizePolicy.setHeightForWidth(self.objectView.sizePolicy().hasHeightForWidth())
        self.objectView.setSizePolicy(sizePolicy)
        self.objectView.setObjectName("objectView")
        self.verticalLayout.addWidget(self.objectView)
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setSizeConstraint(QLayout.SetFixedSize)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.autoUpdateCheckBox = QCheckBox(self)
        self.autoUpdateCheckBox.setObjectName("autoUpdateCheckBox")
        self.horizontalLayout.addWidget(self.autoUpdateCheckBox)
        spacerItem = QSpacerItem(48, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.okButton = QPushButton(self)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout.addWidget(self.okButton)
        self.applyButton = QPushButton(self)
        self.applyButton.setObjectName("applyButton")
        self.horizontalLayout.addWidget(self.applyButton)
        self.cancelButton = QPushButton(self)
        self.cancelButton.setObjectName("cancelButton")
        self.horizontalLayout.addWidget(self.cancelButton)
        self.verticalLayout.addLayout(self.horizontalLayout)
        
        self.setWindowTitle("ObjectDialog")
        self.autoUpdateCheckBox.setText("Auto update")
        self.okButton.setText("Ok")
        self.applyButton.setText("Apply")
        self.cancelButton.setText("Cancel")
      
        self.cancelButton.pressed.connect(self.reject)
        self.okButton.pressed.connect(self.__ok)
        self.applyButton.pressed.connect(self.__apply)
        self.autoUpdateCheckBox.toggled.connect(self.setAutomaticUpdate)
        self.objectView.valueChanged.connect(self.__valueChanged)
        
    def menu(self):
        return self._menu
    
    def __valueChanged(self):
        if self.automaticUpdate:
            self.valueChanged.emit()
        else:
            self.hasChanged = True

    def __apply(self):
        self.valueChanged.emit()
        self.hasChanged = False
        
    def __ok(self):
        self.valueChanged.emit()
        self.hasChanged = False
        self.accept()
        self.close()

    def hideEvent(self,event):
        self.hidden.emit()

    def setAutomaticUpdate(self,value):
        """setAutomaticUpdate: checking the autoupdate box will make the QDialog send a 'valueChanged()' signal each time it recieve the same Signal from the objectView"""
        if self.automaticUpdate != value:
            self.automaticUpdate = value
            self.applyButton.setEnabled(not self.automaticUpdate)
            self.AutomaticUpdate.emit(value)
            if self.automaticUpdate and self.hasChanged :
                self.__apply()
                

    def closeEvent(self,event):
        QDialog.closeEvent(self,event)

