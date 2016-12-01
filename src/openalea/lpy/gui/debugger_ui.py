# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/debugger_ui.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(286, 250)
        self.gridLayout_4 = QtWidgets.QGridLayout(Form)
        self.gridLayout_4.setContentsMargins(0, 2, 0, 0)
        self.gridLayout_4.setHorizontalSpacing(2)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.tabWidget = QtWidgets.QTabWidget(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy)
        self.tabWidget.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.tabWidget.setObjectName("tabWidget")
        self.tabArgument = QtWidgets.QWidget()
        self.tabArgument.setGeometry(QtCore.QRect(0, 0, 280, 221))
        self.tabArgument.setObjectName("tabArgument")
        self.gridLayout = QtWidgets.QGridLayout(self.tabArgument)
        self.gridLayout.setContentsMargins(4, 4, 4, 4)
        self.gridLayout.setObjectName("gridLayout")
        self.argTable = QtWidgets.QTreeView(self.tabArgument)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Expanding)
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
        self.argTable.setObjectName("argTable")
        self.gridLayout.addWidget(self.argTable, 0, 0, 1, 1)
        self.tabWidget.addTab(self.tabArgument, "")
        self.tabGlobals = QtWidgets.QWidget()
        self.tabGlobals.setGeometry(QtCore.QRect(0, 0, 280, 223))
        self.tabGlobals.setObjectName("tabGlobals")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.tabGlobals)
        self.gridLayout_2.setContentsMargins(4, 4, 4, 4)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.globalTable = QtWidgets.QTreeView(self.tabGlobals)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Expanding)
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
        self.globalTable.setObjectName("globalTable")
        self.gridLayout_2.addWidget(self.globalTable, 0, 0, 1, 1)
        self.tabWidget.addTab(self.tabGlobals, "")
        self.gridLayout_4.addWidget(self.tabWidget, 1, 1, 1, 1)

        self.retranslateUi(Form)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabArgument), _translate("Form", "Arguments"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabGlobals), _translate("Form", "Globals"))

import lpyresources_rc
