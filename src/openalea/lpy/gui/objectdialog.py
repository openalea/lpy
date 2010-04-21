try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False


from PyQt4.QtGui import *
from PyQt4.QtCore import *
import PyQt4.QtGui as QtGui

    
class ObjectDialog(QDialog):
    """the class that will create dialog between the panel and the editor window"""
    def __init__(self, *args):
        """during the init of the dialog we have to know the editor we want to open, the typ variable will allow us to know that"""
        QDialog.__init__(self,*args)
        self.hasChanged = False
        self.automaticUpdate = False
        #self.setModal(True)
    
    def setupUi(self,editor):
        self.setObjectName("ObjectDialog")
        self.resize(389, 282)
        self.verticalLayout = QtGui.QVBoxLayout(self)
        self.verticalLayout.setSpacing(2)
        self.verticalLayout.setMargin(2)
        self.verticalLayout.setObjectName("verticalLayout")
        self.objectView = editor
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.objectView.sizePolicy().hasHeightForWidth())
        self.objectView.setSizePolicy(sizePolicy)
        self.objectView.setObjectName("objectView")
        self.verticalLayout.addWidget(self.objectView)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setSizeConstraint(QtGui.QLayout.SetFixedSize)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.autoUpdateCheckBox = QtGui.QCheckBox(self)
        self.autoUpdateCheckBox.setObjectName("autoUpdateCheckBox")
        self.horizontalLayout.addWidget(self.autoUpdateCheckBox)
        spacerItem = QtGui.QSpacerItem(48, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.okButton = QtGui.QPushButton(self)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout.addWidget(self.okButton)
        self.applyButton = QtGui.QPushButton(self)
        self.applyButton.setObjectName("applyButton")
        self.horizontalLayout.addWidget(self.applyButton)
        self.cancelButton = QtGui.QPushButton(self)
        self.cancelButton.setObjectName("cancelButton")
        self.horizontalLayout.addWidget(self.cancelButton)
        self.verticalLayout.addLayout(self.horizontalLayout)
        
        self.setWindowTitle(QtGui.QApplication.translate("ObjectDialog", "Object Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.autoUpdateCheckBox.setText(QtGui.QApplication.translate("ObjectDialog", "Auto update", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(QtGui.QApplication.translate("ObjectDialog", "Ok", None, QtGui.QApplication.UnicodeUTF8))
        self.applyButton.setText(QtGui.QApplication.translate("ObjectDialog", "Apply", None, QtGui.QApplication.UnicodeUTF8))
        self.cancelButton.setText(QtGui.QApplication.translate("ObjectDialog", "Cancel", None, QtGui.QApplication.UnicodeUTF8))
      
        QObject.connect(self.cancelButton,SIGNAL('pressed()'),self.reject)
        QObject.connect(self.okButton,SIGNAL('pressed()'),self.__ok)
        QObject.connect(self.applyButton,SIGNAL('pressed()'),self.__apply)
        QObject.connect(self.autoUpdateCheckBox,SIGNAL('toggled(bool)'),self.setAutomaticUpdate)
        QObject.connect(self.objectView,SIGNAL('valueChanged()'),self.__valueChanged)

    def __valueChanged(self):
        if self.automaticUpdate:
            self.emit(SIGNAL('valueChanged()'))
        else:
            self.hasChanged = True

    def __apply(self):
        self.emit(SIGNAL('valueChanged()'))
        self.hasChanged = False
        
    def __ok(self):
        self.emit(SIGNAL('valueChanged()'))
        self.hasChanged = False
        self.accept()
        self.close()

    def hideEvent(self,event):
        self.emit(SIGNAL('hidden()'))

    def setAutomaticUpdate(self,value):
        """setAutomaticUpdate: checking the autoupdate box will make the QDialog send a 'valueChanged()' signal each time it recieve the same Signal from the objectView"""
        if self.automaticUpdate != value:
            self.automaticUpdate = value
            self.applyButton.setEnabled(not self.automaticUpdate)
            self.emit(SIGNAL("AutomaticUpdate(bool)"),value)
            if self.automaticUpdate and self.hasChanged :
                self.__apply()
                

    def closeEvent(self,event):
        QDialog.closeEvent(self,event)

