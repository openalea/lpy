# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/logdialog.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_LogDialog(object):
    def setupUi(self, LogDialog):
        LogDialog.setObjectName("LogDialog")
        LogDialog.resize(416, 230)
        self.verticalLayout = QtWidgets.QVBoxLayout(LogDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.groupBox = QtWidgets.QGroupBox(LogDialog)
        self.groupBox.setObjectName("groupBox")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.groupBox)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.logEdit = QtWidgets.QPlainTextEdit(self.groupBox)
        self.logEdit.setObjectName("logEdit")
        self.horizontalLayout.addWidget(self.logEdit)
        self.verticalLayout.addWidget(self.groupBox)
        self.buttonBox = QtWidgets.QDialogButtonBox(LogDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(LogDialog)
        self.buttonBox.accepted.connect(LogDialog.accept)
        self.buttonBox.rejected.connect(LogDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(LogDialog)

    def retranslateUi(self, LogDialog):
        _translate = QtCore.QCoreApplication.translate
        LogDialog.setWindowTitle(_translate("LogDialog", "Dialog"))
        self.groupBox.setTitle(_translate("LogDialog", "Log"))

