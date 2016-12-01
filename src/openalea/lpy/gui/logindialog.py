# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/logindialog.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_LoginDialog(object):
    def setupUi(self, LoginDialog):
        LoginDialog.setObjectName("LoginDialog")
        LoginDialog.resize(262, 93)
        self.gridLayout = QtWidgets.QGridLayout(LoginDialog)
        self.gridLayout.setObjectName("gridLayout")
        self.LoginLabel = QtWidgets.QLabel(LoginDialog)
        self.LoginLabel.setObjectName("LoginLabel")
        self.gridLayout.addWidget(self.LoginLabel, 0, 0, 1, 1)
        self.loginEdit = QtWidgets.QLineEdit(LoginDialog)
        self.loginEdit.setObjectName("loginEdit")
        self.gridLayout.addWidget(self.loginEdit, 0, 1, 1, 1)
        self.label = QtWidgets.QLabel(LoginDialog)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 1, 0, 1, 1)
        self.passEdit = QtWidgets.QLineEdit(LoginDialog)
        self.passEdit.setEchoMode(QtWidgets.QLineEdit.Password)
        self.passEdit.setObjectName("passEdit")
        self.gridLayout.addWidget(self.passEdit, 1, 1, 1, 1)
        self.buttonBox = QtWidgets.QDialogButtonBox(LoginDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridLayout.addWidget(self.buttonBox, 2, 0, 1, 2)

        self.retranslateUi(LoginDialog)
        self.buttonBox.accepted.connect(LoginDialog.accept)
        self.buttonBox.rejected.connect(LoginDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(LoginDialog)

    def retranslateUi(self, LoginDialog):
        _translate = QtCore.QCoreApplication.translate
        LoginDialog.setWindowTitle(_translate("LoginDialog", "Dialog"))
        self.LoginLabel.setText(_translate("LoginDialog", "Login :"))
        self.label.setText(_translate("LoginDialog", "Password :"))

