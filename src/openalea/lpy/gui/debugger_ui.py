# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/pradal/devlp/release/vplants/lpy/src/openalea/lpy/gui/debugger_ui.ui'
#
# Created: Fri Oct 17 16:56:35 2014
#      by: PyQt4 UI code generator 4.11.1
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

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName(_fromUtf8("Form"))
        Form.resize(286, 250)
        self.gridLayout_4 = QtGui.QGridLayout(Form)
        self.gridLayout_4.setContentsMargins(0, 2, 0, 0)
        self.gridLayout_4.setHorizontalSpacing(2)
        self.gridLayout_4.setObjectName(_fromUtf8("gridLayout_4"))
        self.tabWidget = QtGui.QTabWidget(Form)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy)
        self.tabWidget.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tabArgument = QtGui.QWidget()
        self.tabArgument.setGeometry(QtCore.QRect(0, 0, 280, 221))
        self.tabArgument.setObjectName(_fromUtf8("tabArgument"))
        self.gridLayout = QtGui.QGridLayout(self.tabArgument)
        self.gridLayout.setMargin(4)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.argTable = QtGui.QTreeView(self.tabArgument)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.argTable.sizePolicy().hasHeightForWidth())
        self.argTable.setSizePolicy(sizePolicy)
        self.argTable.setMidLineWidth(1)
        self.argTable.setRootIsDecorated(False)
        self.argTable.setUniformRowHeights(True)
        self.argTable.setSortingEnabled(True)
        self.argTable.setAnimated(True)
        self.argTable.setAllColumnsShowFocus(True)
        self.argTable.setExpandsOnDoubleClick(False)
        self.argTable.setObjectName(_fromUtf8("argTable"))
        self.gridLayout.addWidget(self.argTable, 0, 0, 1, 1)
        self.tabWidget.addTab(self.tabArgument, _fromUtf8(""))
        self.tabGlobals = QtGui.QWidget()
        self.tabGlobals.setGeometry(QtCore.QRect(0, 0, 280, 223))
        self.tabGlobals.setObjectName(_fromUtf8("tabGlobals"))
        self.gridLayout_2 = QtGui.QGridLayout(self.tabGlobals)
        self.gridLayout_2.setMargin(4)
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.globalTable = QtGui.QTreeView(self.tabGlobals)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.globalTable.sizePolicy().hasHeightForWidth())
        self.globalTable.setSizePolicy(sizePolicy)
        self.globalTable.setRootIsDecorated(False)
        self.globalTable.setUniformRowHeights(True)
        self.globalTable.setItemsExpandable(True)
        self.globalTable.setSortingEnabled(True)
        self.globalTable.setAnimated(True)
        self.globalTable.setAllColumnsShowFocus(True)
        self.globalTable.setExpandsOnDoubleClick(False)
        self.globalTable.setObjectName(_fromUtf8("globalTable"))
        self.gridLayout_2.addWidget(self.globalTable, 0, 0, 1, 1)
        self.tabWidget.addTab(self.tabGlobals, _fromUtf8(""))
        self.gridLayout_4.addWidget(self.tabWidget, 1, 1, 1, 1)

        self.retranslateUi(Form)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(_translate("Form", "Form", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabArgument), _translate("Form", "Arguments", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabGlobals), _translate("Form", "Globals", None))

import lpyresources_rc
