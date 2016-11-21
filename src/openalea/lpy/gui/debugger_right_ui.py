# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'debugger_right_ui.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(526, 197)
        self.gridLayout = QtWidgets.QGridLayout(Form)
        self.gridLayout.setContentsMargins(2, 2, 2, 2)
        self.gridLayout.setSpacing(2)
        self.gridLayout.setObjectName("gridLayout")
        self.srcView = QtWidgets.QTextEdit(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.srcView.sizePolicy().hasHeightForWidth())
        self.srcView.setSizePolicy(sizePolicy)
        self.srcView.setMaximumSize(QtCore.QSize(16777215, 50))
        self.srcView.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.srcView.setLineWidth(0)
        self.srcView.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.srcView.setLineWrapMode(QtWidgets.QTextEdit.NoWrap)
        self.srcView.setReadOnly(True)
        self.srcView.setObjectName("srcView")
        self.gridLayout.addWidget(self.srcView, 1, 0, 1, 5)
        self.ruleView = QtWidgets.QLineEdit(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(3)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.ruleView.sizePolicy().hasHeightForWidth())
        self.ruleView.setSizePolicy(sizePolicy)
        self.ruleView.setAlignment(QtCore.Qt.AlignCenter)
        self.ruleView.setObjectName("ruleView")
        self.gridLayout.addWidget(self.ruleView, 2, 0, 1, 5)
        self.destView = QtWidgets.QTextEdit(Form)
        self.destView.setMaximumSize(QtCore.QSize(16777215, 50))
        self.destView.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.destView.setLineWidth(0)
        self.destView.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.destView.setLineWrapMode(QtWidgets.QTextEdit.NoWrap)
        self.destView.setReadOnly(True)
        self.destView.setObjectName("destView")
        self.gridLayout.addWidget(self.destView, 3, 0, 1, 5)
        self.animationDebugSlider = QtWidgets.QSlider(Form)
        self.animationDebugSlider.setMinimum(10)
        self.animationDebugSlider.setMaximum(5000)
        self.animationDebugSlider.setProperty("value", 500)
        self.animationDebugSlider.setOrientation(QtCore.Qt.Horizontal)
        self.animationDebugSlider.setObjectName("animationDebugSlider")
        self.gridLayout.addWidget(self.animationDebugSlider, 4, 3, 1, 1)
        self.progressBar = QtWidgets.QProgressBar(Form)
        self.progressBar.setProperty("value", 0)
        self.progressBar.setAlignment(QtCore.Qt.AlignCenter)
        self.progressBar.setObjectName("progressBar")
        self.gridLayout.addWidget(self.progressBar, 5, 0, 1, 1)
        spacerItem = QtWidgets.QSpacerItem(268, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem, 5, 1, 1, 1)
        self.animateDebugButton = QtWidgets.QPushButton(Form)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/images/icons/play.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.animateDebugButton.setIcon(icon)
        self.animateDebugButton.setObjectName("animateDebugButton")
        self.gridLayout.addWidget(self.animateDebugButton, 5, 3, 1, 1)
        self.nextDebugButton = QtWidgets.QPushButton(Form)
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap(":/images/icons/play-green.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.nextDebugButton.setIcon(icon1)
        self.nextDebugButton.setObjectName("nextDebugButton")
        self.gridLayout.addWidget(self.nextDebugButton, 5, 4, 1, 1)
        spacerItem1 = QtWidgets.QSpacerItem(100, 9, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem1, 6, 0, 1, 1)
        self.endDebugButton = QtWidgets.QPushButton(Form)
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap(":/images/icons/run.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.endDebugButton.setIcon(icon2)
        self.endDebugButton.setObjectName("endDebugButton")
        self.gridLayout.addWidget(self.endDebugButton, 5, 2, 1, 1)
        self.directionLabel = QtWidgets.QLabel(Form)
        self.directionLabel.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.directionLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.directionLabel.setObjectName("directionLabel")
        self.gridLayout.addWidget(self.directionLabel, 0, 4, 1, 1)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.srcView.setHtml(_translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>"))
        self.animateDebugButton.setText(_translate("Form", "Animation"))
        self.nextDebugButton.setText(_translate("Form", "Next"))
        self.endDebugButton.setText(_translate("Form", "Continue"))
        self.directionLabel.setText(_translate("Form", "Forward"))

import lpyresources_rc
