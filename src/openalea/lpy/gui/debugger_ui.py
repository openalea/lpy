# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'c:\openalea\vplants\lpy\src\openalea\lpy\gui\debugger_ui.ui'
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
        Form.resize(286, 250)
        Form.setWindowTitle(qt.QtGui.QApplication.translate("Form", "Form", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.gridLayout_4 = qt.QtGui.QGridLayout(Form)
        self.gridLayout_4.setContentsMargins(0, 2, 0, 0)
        self.gridLayout_4.setHorizontalSpacing(2)
        self.gridLayout_4.setObjectName(_fromUtf8("gridLayout_4"))
        self.tabWidget = qt.QtGui.QTabWidget(Form)
        sizePolicy = qt.QtGui.QSizePolicy(qt.QtGui.QSizePolicy.Minimum, qt.QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy)
        self.tabWidget.setMaximumSize(qt.QtCore.QSize(16777215, 16777215))
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tabArgument = qt.QtGui.QWidget()
        self.tabArgument.setGeometry(qt.QtCore.QRect(0, 0, 280, 221))
        self.tabArgument.setObjectName(_fromUtf8("tabArgument"))
        self.gridLayout = qt.QtGui.QGridLayout(self.tabArgument)
        self.gridLayout.setMargin(4)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.argTable = qt.QtGui.QTreeView(self.tabArgument)
        sizePolicy = qt.QtGui.QSizePolicy(qt.QtGui.QSizePolicy.MinimumExpanding, qt.QtGui.QSizePolicy.Expanding)
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
        self.tabGlobals = qt.QtGui.QWidget()
        self.tabGlobals.setGeometry(qt.QtCore.QRect(0, 0, 280, 223))
        self.tabGlobals.setObjectName(_fromUtf8("tabGlobals"))
        self.gridLayout_2 = qt.QtGui.QGridLayout(self.tabGlobals)
        self.gridLayout_2.setMargin(4)
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.globalTable = qt.QtGui.QTreeView(self.tabGlobals)
        sizePolicy = qt.QtGui.QSizePolicy(qt.QtGui.QSizePolicy.MinimumExpanding, qt.QtGui.QSizePolicy.Expanding)
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
        qt.QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabArgument), qt.QtGui.QApplication.translate("Form", "Arguments", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabGlobals), qt.QtGui.QApplication.translate("Form", "Globals", None, qt.QtGui.QApplication.UnicodeUTF8))

import lpyresources_rc
