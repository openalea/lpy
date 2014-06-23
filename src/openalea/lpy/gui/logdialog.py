# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/fred/Develop/vplants/lpy/src/openalea/lpy/gui/logdialog.ui'
#
# Created: Mon Jun 23 16:12:14 2014
#      by: PyQt4 UI code generator 4.10.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_LogDialog(object):
    def setupUi(self, LogDialog):
        LogDialog.setObjectName(_fromUtf8("LogDialog"))
        LogDialog.resize(416, 230)
        self.verticalLayout = QtGui.QVBoxLayout(LogDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.groupBox = QtGui.QGroupBox(LogDialog)
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.groupBox)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.logEdit = QtGui.QPlainTextEdit(self.groupBox)
        self.logEdit.setObjectName(_fromUtf8("logEdit"))
        self.horizontalLayout.addWidget(self.logEdit)
        self.verticalLayout.addWidget(self.groupBox)
        self.buttonBox = QtGui.QDialogButtonBox(LogDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(LogDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), LogDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), LogDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(LogDialog)

    def retranslateUi(self, LogDialog):
        LogDialog.setWindowTitle(_translate("LogDialog", "Dialog", None))
        self.groupBox.setTitle(_translate("LogDialog", "Log", None))

