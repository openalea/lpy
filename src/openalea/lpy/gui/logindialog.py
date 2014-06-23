# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/home/fred/Develop/vplants/lpy/src/openalea/lpy/gui/logindialog.ui'
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

class Ui_LoginDialog(object):
    def setupUi(self, LoginDialog):
        LoginDialog.setObjectName(_fromUtf8("LoginDialog"))
        LoginDialog.resize(262, 93)
        self.gridLayout = QtGui.QGridLayout(LoginDialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.LoginLabel = QtGui.QLabel(LoginDialog)
        self.LoginLabel.setObjectName(_fromUtf8("LoginLabel"))
        self.gridLayout.addWidget(self.LoginLabel, 0, 0, 1, 1)
        self.loginEdit = QtGui.QLineEdit(LoginDialog)
        self.loginEdit.setObjectName(_fromUtf8("loginEdit"))
        self.gridLayout.addWidget(self.loginEdit, 0, 1, 1, 1)
        self.label = QtGui.QLabel(LoginDialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 1, 0, 1, 1)
        self.passEdit = QtGui.QLineEdit(LoginDialog)
        self.passEdit.setEchoMode(QtGui.QLineEdit.Password)
        self.passEdit.setObjectName(_fromUtf8("passEdit"))
        self.gridLayout.addWidget(self.passEdit, 1, 1, 1, 1)
        self.buttonBox = QtGui.QDialogButtonBox(LoginDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 2, 0, 1, 2)

        self.retranslateUi(LoginDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), LoginDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), LoginDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(LoginDialog)

    def retranslateUi(self, LoginDialog):
        LoginDialog.setWindowTitle(_translate("LoginDialog", "Dialog", None))
        self.LoginLabel.setText(_translate("LoginDialog", "Login :", None))
        self.label.setText(_translate("LoginDialog", "Password :", None))

