# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'c:\openalea\vplants\lpy\src\openalea\lpy\gui\debugger_right_ui.ui'
#
# Created: Thu Nov 29 08:57:18 2012
#      by: PyQt4 UI code generator 4.8.6
#
# WARNING! All changes made in this file will be lost!

from openalea.vpltk.qt import qt

try:
    _fromUtf8 = qt.QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName(_fromUtf8("Form"))
        Form.resize(526, 197)
        Form.setWindowTitle(qt.QtGui.QApplication.translate("Form", "Form", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.gridLayout = qt.QtGui.QGridLayout(Form)
        self.gridLayout.setContentsMargins(2, 2, 2, 2)
        self.gridLayout.setSpacing(2)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.srcView = qt.QtGui.QTextEdit(Form)
        sizePolicy = qt.QtGui.QSizePolicy(qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.srcView.sizePolicy().hasHeightForWidth())
        self.srcView.setSizePolicy(sizePolicy)
        self.srcView.setMaximumSize(qt.QtCore.QSize(16777215, 50))
        self.srcView.setFrameShape(qt.QtGui.QFrame.StyledPanel)
        self.srcView.setLineWidth(0)
        self.srcView.setVerticalScrollBarPolicy(qt.QtCore.Qt.ScrollBarAlwaysOff)
        self.srcView.setLineWrapMode(qt.QtGui.QTextEdit.NoWrap)
        self.srcView.setReadOnly(True)
        self.srcView.setHtml(qt.QtGui.QApplication.translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.srcView.setObjectName(_fromUtf8("srcView"))
        self.gridLayout.addWidget(self.srcView, 1, 0, 1, 5)
        self.ruleView = qt.QtGui.QLineEdit(Form)
        sizePolicy = qt.QtGui.QSizePolicy(qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(3)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.ruleView.sizePolicy().hasHeightForWidth())
        self.ruleView.setSizePolicy(sizePolicy)
        self.ruleView.setAlignment(qt.QtCore.Qt.AlignCenter)
        self.ruleView.setObjectName(_fromUtf8("ruleView"))
        self.gridLayout.addWidget(self.ruleView, 2, 0, 1, 5)
        self.destView = qt.QtGui.QTextEdit(Form)
        self.destView.setMaximumSize(qt.QtCore.QSize(16777215, 50))
        self.destView.setFrameShape(qt.QtGui.QFrame.StyledPanel)
        self.destView.setLineWidth(0)
        self.destView.setVerticalScrollBarPolicy(qt.QtCore.Qt.ScrollBarAlwaysOff)
        self.destView.setLineWrapMode(qt.QtGui.QTextEdit.NoWrap)
        self.destView.setReadOnly(True)
        self.destView.setObjectName(_fromUtf8("destView"))
        self.gridLayout.addWidget(self.destView, 3, 0, 1, 5)
        self.animationDebugSlider = qt.QtGui.QSlider(Form)
        self.animationDebugSlider.setMinimum(10)
        self.animationDebugSlider.setMaximum(5000)
        self.animationDebugSlider.setProperty("value", 500)
        self.animationDebugSlider.setOrientation(qt.QtCore.Qt.Horizontal)
        self.animationDebugSlider.setObjectName(_fromUtf8("animationDebugSlider"))
        self.gridLayout.addWidget(self.animationDebugSlider, 4, 3, 1, 1)
        self.progressBar = qt.QtGui.QProgressBar(Form)
        self.progressBar.setProperty("value", 0)
        self.progressBar.setAlignment(qt.QtCore.Qt.AlignCenter)
        self.progressBar.setObjectName(_fromUtf8("progressBar"))
        self.gridLayout.addWidget(self.progressBar, 5, 0, 1, 1)
        spacerItem = qt.QtGui.QSpacerItem(268, 20, qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem, 5, 1, 1, 1)
        self.animateDebugButton = qt.QtGui.QPushButton(Form)
        self.animateDebugButton.setText(qt.QtGui.QApplication.translate("Form", "Animation", None, qt.QtGui.QApplication.UnicodeUTF8))
        icon = qt.QtGui.QIcon()
        icon.addPixmap(qt.QtGui.QPixmap(_fromUtf8(":/images/icons/play.png")), qt.QtGui.QIcon.Normal, qt.QtGui.QIcon.Off)
        self.animateDebugButton.setIcon(icon)
        self.animateDebugButton.setObjectName(_fromUtf8("animateDebugButton"))
        self.gridLayout.addWidget(self.animateDebugButton, 5, 3, 1, 1)
        self.nextDebugButton = qt.QtGui.QPushButton(Form)
        self.nextDebugButton.setText(qt.QtGui.QApplication.translate("Form", "Next", None, qt.QtGui.QApplication.UnicodeUTF8))
        icon1 = qt.QtGui.QIcon()
        icon1.addPixmap(qt.QtGui.QPixmap(_fromUtf8(":/images/icons/play-green.png")), qt.QtGui.QIcon.Normal, qt.QtGui.QIcon.Off)
        self.nextDebugButton.setIcon(icon1)
        self.nextDebugButton.setObjectName(_fromUtf8("nextDebugButton"))
        self.gridLayout.addWidget(self.nextDebugButton, 5, 4, 1, 1)
        spacerItem1 = qt.QtGui.QSpacerItem(100, 9, qt.QtGui.QSizePolicy.Minimum, qt.QtGui.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem1, 6, 0, 1, 1)
        self.endDebugButton = qt.QtGui.QPushButton(Form)
        self.endDebugButton.setText(qt.QtGui.QApplication.translate("Form", "Continue", None, qt.QtGui.QApplication.UnicodeUTF8))
        icon2 = qt.QtGui.QIcon()
        icon2.addPixmap(qt.QtGui.QPixmap(_fromUtf8(":/images/icons/run.png")), qt.QtGui.QIcon.Normal, qt.QtGui.QIcon.Off)
        self.endDebugButton.setIcon(icon2)
        self.endDebugButton.setObjectName(_fromUtf8("endDebugButton"))
        self.gridLayout.addWidget(self.endDebugButton, 5, 2, 1, 1)
        self.directionLabel = qt.QtGui.QLabel(Form)
        self.directionLabel.setFrameShape(qt.QtGui.QFrame.StyledPanel)
        self.directionLabel.setText(qt.QtGui.QApplication.translate("Form", "Forward", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.directionLabel.setAlignment(qt.QtCore.Qt.AlignCenter)
        self.directionLabel.setObjectName(_fromUtf8("directionLabel"))
        self.gridLayout.addWidget(self.directionLabel, 0, 4, 1, 1)

        self.retranslateUi(Form)
        qt.QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        pass

import lpyresources_rc
