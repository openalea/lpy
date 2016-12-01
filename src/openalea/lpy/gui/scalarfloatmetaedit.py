# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/scalarfloatmetaedit.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_FloatScalarDialog(object):
    def setupUi(self, FloatScalarDialog):
        FloatScalarDialog.setObjectName("FloatScalarDialog")
        FloatScalarDialog.resize(301, 179)
        self.verticalLayout = QtWidgets.QVBoxLayout(FloatScalarDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.gridLayout = QtWidgets.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.label = QtWidgets.QLabel(FloatScalarDialog)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = QtWidgets.QLineEdit(FloatScalarDialog)
        self.nameEdit.setObjectName("nameEdit")
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = QtWidgets.QLabel(FloatScalarDialog)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = QtWidgets.QDoubleSpinBox(FloatScalarDialog)
        self.valueEdit.setObjectName("valueEdit")
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(FloatScalarDialog)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = QtWidgets.QDoubleSpinBox(FloatScalarDialog)
        self.minValueEdit.setObjectName("minValueEdit")
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = QtWidgets.QLabel(FloatScalarDialog)
        self.label_4.setObjectName("label_4")
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = QtWidgets.QDoubleSpinBox(FloatScalarDialog)
        self.maxValueEdit.setObjectName("maxValueEdit")
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.label_5 = QtWidgets.QLabel(FloatScalarDialog)
        self.label_5.setObjectName("label_5")
        self.gridLayout.addWidget(self.label_5, 4, 0, 1, 1)
        self.decimalEdit = QtWidgets.QSpinBox(FloatScalarDialog)
        self.decimalEdit.setMinimum(1)
        self.decimalEdit.setMaximum(10)
        self.decimalEdit.setProperty("value", 2)
        self.decimalEdit.setObjectName("decimalEdit")
        self.gridLayout.addWidget(self.decimalEdit, 4, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QtWidgets.QSpacerItem(20, 0, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = QtWidgets.QDialogButtonBox(FloatScalarDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(FloatScalarDialog)
        self.buttonBox.accepted.connect(FloatScalarDialog.accept)
        self.buttonBox.rejected.connect(FloatScalarDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(FloatScalarDialog)

    def retranslateUi(self, FloatScalarDialog):
        _translate = QtCore.QCoreApplication.translate
        FloatScalarDialog.setWindowTitle(_translate("FloatScalarDialog", "Scalar"))
        self.label.setText(_translate("FloatScalarDialog", "Name"))
        self.label_2.setText(_translate("FloatScalarDialog", "value"))
        self.label_3.setText(_translate("FloatScalarDialog", "Minimum value"))
        self.label_4.setText(_translate("FloatScalarDialog", "Maximum value"))
        self.label_5.setText(_translate("FloatScalarDialog", "Decimals"))

