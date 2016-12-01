# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/scalarmetaedit.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_ScalarDialog(object):
    def setupUi(self, ScalarDialog):
        ScalarDialog.setObjectName("ScalarDialog")
        ScalarDialog.resize(296, 155)
        self.verticalLayout = QtWidgets.QVBoxLayout(ScalarDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.gridLayout = QtWidgets.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.label = QtWidgets.QLabel(ScalarDialog)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = QtWidgets.QLineEdit(ScalarDialog)
        self.nameEdit.setObjectName("nameEdit")
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = QtWidgets.QLabel(ScalarDialog)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = QtWidgets.QSpinBox(ScalarDialog)
        self.valueEdit.setMaximum(2147483647)
        self.valueEdit.setObjectName("valueEdit")
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(ScalarDialog)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = QtWidgets.QSpinBox(ScalarDialog)
        self.minValueEdit.setMinimum(-2147483647)
        self.minValueEdit.setMaximum(2147483647)
        self.minValueEdit.setObjectName("minValueEdit")
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = QtWidgets.QLabel(ScalarDialog)
        self.label_4.setObjectName("label_4")
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = QtWidgets.QSpinBox(ScalarDialog)
        self.maxValueEdit.setMaximum(2147483647)
        self.maxValueEdit.setProperty("value", 0)
        self.maxValueEdit.setObjectName("maxValueEdit")
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QtWidgets.QSpacerItem(20, 0, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = QtWidgets.QDialogButtonBox(ScalarDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(ScalarDialog)
        self.buttonBox.accepted.connect(ScalarDialog.accept)
        self.buttonBox.rejected.connect(ScalarDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(ScalarDialog)

    def retranslateUi(self, ScalarDialog):
        _translate = QtCore.QCoreApplication.translate
        ScalarDialog.setWindowTitle(_translate("ScalarDialog", "Scalar"))
        self.label.setText(_translate("ScalarDialog", "Name"))
        self.label_2.setText(_translate("ScalarDialog", "value"))
        self.label_3.setText(_translate("ScalarDialog", "Minimum value"))
        self.label_4.setText(_translate("ScalarDialog", "Maximum value"))

