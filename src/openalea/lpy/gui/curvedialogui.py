# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'd:\fred\mes documents\develop\vplants\trunk\lpy\src\openalea\lpy\gui\curvedialogui.ui'
#
# Created: Fri Sep 11 14:57:28 2009
#      by: PyQt4 UI code generator 4.4.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_CurveDialog(object):
    def setupUi(self, CurveDialog):
        CurveDialog.setObjectName("CurveDialog")
        CurveDialog.resize(389,282)
        self.verticalLayout = QtGui.QVBoxLayout(CurveDialog)
        self.verticalLayout.setSpacing(2)
        self.verticalLayout.setMargin(2)
        self.verticalLayout.setObjectName("verticalLayout")
        self.curveView = CurveEditor(CurveDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred,QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.curveView.sizePolicy().hasHeightForWidth())
        self.curveView.setSizePolicy(sizePolicy)
        self.curveView.setObjectName("curveView")
        self.verticalLayout.addWidget(self.curveView)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setSizeConstraint(QtGui.QLayout.SetFixedSize)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.autoUpdateCheckBox = QtGui.QCheckBox(CurveDialog)
        self.autoUpdateCheckBox.setObjectName("autoUpdateCheckBox")
        self.horizontalLayout.addWidget(self.autoUpdateCheckBox)
        spacerItem = QtGui.QSpacerItem(48,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.okButton = QtGui.QPushButton(CurveDialog)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout.addWidget(self.okButton)
        self.applyButton = QtGui.QPushButton(CurveDialog)
        self.applyButton.setObjectName("applyButton")
        self.horizontalLayout.addWidget(self.applyButton)
        self.cancelButton = QtGui.QPushButton(CurveDialog)
        self.cancelButton.setObjectName("cancelButton")
        self.horizontalLayout.addWidget(self.cancelButton)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(CurveDialog)
        QtCore.QMetaObject.connectSlotsByName(CurveDialog)

    def retranslateUi(self, CurveDialog):
        CurveDialog.setWindowTitle(QtGui.QApplication.translate("CurveDialog", "Curve Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.autoUpdateCheckBox.setText(QtGui.QApplication.translate("CurveDialog", "Auto update", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(QtGui.QApplication.translate("CurveDialog", "Ok", None, QtGui.QApplication.UnicodeUTF8))
        self.applyButton.setText(QtGui.QApplication.translate("CurveDialog", "Apply", None, QtGui.QApplication.UnicodeUTF8))
        self.cancelButton.setText(QtGui.QApplication.translate("CurveDialog", "Cancel", None, QtGui.QApplication.UnicodeUTF8))

from curveedit import CurveEditor
