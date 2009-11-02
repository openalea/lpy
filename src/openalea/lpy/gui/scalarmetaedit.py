# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'd:\fred\mes documents\develop\vplants\trunk\lpy\src\openalea\lpy\gui\scalarmetaedit.ui'
#
# Created: Mon Nov 02 18:00:20 2009
#      by: PyQt4 UI code generator 4.5.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_ScalarDialog(object):
    def setupUi(self, ScalarDialog):
        ScalarDialog.setObjectName("ScalarDialog")
        ScalarDialog.resize(296, 155)
        self.verticalLayout = QtGui.QVBoxLayout(ScalarDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.label = QtGui.QLabel(ScalarDialog)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = QtGui.QLineEdit(ScalarDialog)
        self.nameEdit.setObjectName("nameEdit")
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = QtGui.QLabel(ScalarDialog)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = QtGui.QSpinBox(ScalarDialog)
        self.valueEdit.setObjectName("valueEdit")
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = QtGui.QLabel(ScalarDialog)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = QtGui.QSpinBox(ScalarDialog)
        self.minValueEdit.setObjectName("minValueEdit")
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = QtGui.QLabel(ScalarDialog)
        self.label_4.setObjectName("label_4")
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = QtGui.QSpinBox(ScalarDialog)
        self.maxValueEdit.setObjectName("maxValueEdit")
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QtGui.QSpacerItem(20, 0, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = QtGui.QDialogButtonBox(ScalarDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(ScalarDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), ScalarDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), ScalarDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(ScalarDialog)

    def retranslateUi(self, ScalarDialog):
        ScalarDialog.setWindowTitle(QtGui.QApplication.translate("ScalarDialog", "Scalar", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("ScalarDialog", "Name", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("ScalarDialog", "value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("ScalarDialog", "Minimum value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("ScalarDialog", "Maximum value", None, QtGui.QApplication.UnicodeUTF8))

