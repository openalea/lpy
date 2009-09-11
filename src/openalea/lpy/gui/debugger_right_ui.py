# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'd:\fred\mes documents\develop\vplants\trunk\lpy\src\openalea\lpy\gui\debugger_right_ui.ui'
#
# Created: Fri Jul 24 23:51:06 2009
#      by: PyQt4 UI code generator 4.4.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(526,197)
        self.gridLayout = QtGui.QGridLayout(Form)
        self.gridLayout.setMargin(2)
        self.gridLayout.setSpacing(2)
        self.gridLayout.setObjectName("gridLayout")
        self.srcView = QtGui.QTextEdit(Form)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.srcView.sizePolicy().hasHeightForWidth())
        self.srcView.setSizePolicy(sizePolicy)
        self.srcView.setMaximumSize(QtCore.QSize(16777215,50))
        self.srcView.setFrameShape(QtGui.QFrame.StyledPanel)
        self.srcView.setLineWidth(0)
        self.srcView.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.srcView.setLineWrapMode(QtGui.QTextEdit.NoWrap)
        self.srcView.setReadOnly(True)
        self.srcView.setObjectName("srcView")
        self.gridLayout.addWidget(self.srcView,1,0,1,5)
        self.ruleView = QtGui.QLineEdit(Form)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(3)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.ruleView.sizePolicy().hasHeightForWidth())
        self.ruleView.setSizePolicy(sizePolicy)
        self.ruleView.setAlignment(QtCore.Qt.AlignCenter)
        self.ruleView.setObjectName("ruleView")
        self.gridLayout.addWidget(self.ruleView,2,0,1,5)
        self.destView = QtGui.QTextEdit(Form)
        self.destView.setMaximumSize(QtCore.QSize(16777215,50))
        self.destView.setFrameShape(QtGui.QFrame.StyledPanel)
        self.destView.setLineWidth(0)
        self.destView.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.destView.setLineWrapMode(QtGui.QTextEdit.NoWrap)
        self.destView.setReadOnly(True)
        self.destView.setObjectName("destView")
        self.gridLayout.addWidget(self.destView,3,0,1,5)
        self.animationDebugSlider = QtGui.QSlider(Form)
        self.animationDebugSlider.setMinimum(10)
        self.animationDebugSlider.setMaximum(5000)
        self.animationDebugSlider.setProperty("value",QtCore.QVariant(500))
        self.animationDebugSlider.setOrientation(QtCore.Qt.Horizontal)
        self.animationDebugSlider.setObjectName("animationDebugSlider")
        self.gridLayout.addWidget(self.animationDebugSlider,4,3,1,1)
        self.progressBar = QtGui.QProgressBar(Form)
        self.progressBar.setProperty("value",QtCore.QVariant(0))
        self.progressBar.setAlignment(QtCore.Qt.AlignCenter)
        self.progressBar.setObjectName("progressBar")
        self.gridLayout.addWidget(self.progressBar,5,0,1,1)
        spacerItem = QtGui.QSpacerItem(268,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem,5,1,1,1)
        self.animateDebugButton = QtGui.QPushButton(Form)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/images/icons/play.png"),QtGui.QIcon.Normal,QtGui.QIcon.Off)
        self.animateDebugButton.setIcon(icon)
        self.animateDebugButton.setObjectName("animateDebugButton")
        self.gridLayout.addWidget(self.animateDebugButton,5,3,1,1)
        self.nextDebugButton = QtGui.QPushButton(Form)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/images/icons/play-green.png"),QtGui.QIcon.Normal,QtGui.QIcon.Off)
        self.nextDebugButton.setIcon(icon)
        self.nextDebugButton.setObjectName("nextDebugButton")
        self.gridLayout.addWidget(self.nextDebugButton,5,4,1,1)
        spacerItem1 = QtGui.QSpacerItem(100,9,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem1,6,0,1,1)
        self.endDebugButton = QtGui.QPushButton(Form)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/images/icons/run.png"),QtGui.QIcon.Normal,QtGui.QIcon.Off)
        self.endDebugButton.setIcon(icon)
        self.endDebugButton.setObjectName("endDebugButton")
        self.gridLayout.addWidget(self.endDebugButton,5,2,1,1)
        self.directionLabel = QtGui.QLabel(Form)
        self.directionLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        self.directionLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.directionLabel.setObjectName("directionLabel")
        self.gridLayout.addWidget(self.directionLabel,0,4,1,1)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.srcView.setHtml(QtGui.QApplication.translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.animateDebugButton.setText(QtGui.QApplication.translate("Form", "Animation", None, QtGui.QApplication.UnicodeUTF8))
        self.nextDebugButton.setText(QtGui.QApplication.translate("Form", "Next", None, QtGui.QApplication.UnicodeUTF8))
        self.endDebugButton.setText(QtGui.QApplication.translate("Form", "Continue", None, QtGui.QApplication.UnicodeUTF8))
        self.directionLabel.setText(QtGui.QApplication.translate("Form", "Forward", None, QtGui.QApplication.UnicodeUTF8))

import lpyresources_rc
