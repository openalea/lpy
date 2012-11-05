# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'd:\fred\mes documents\develop\vplants\trunk\lpy\src\openalea\lpy\gui\scalarfloatmetaedit.ui'
#
# Created: Wed Oct 31 17:48:59 2012
#      by: PyQt4 UI code generator 4.9.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_FloatScalarDialog(object):
    def setupUi(self, FloatScalarDialog):
        FloatScalarDialog.setObjectName(_fromUtf8("FloatScalarDialog"))
        FloatScalarDialog.resize(301, 179)
        self.verticalLayout = QtGui.QVBoxLayout(FloatScalarDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = QtGui.QLabel(FloatScalarDialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = QtGui.QLineEdit(FloatScalarDialog)
        self.nameEdit.setObjectName(_fromUtf8("nameEdit"))
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = QtGui.QLabel(FloatScalarDialog)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = QtGui.QDoubleSpinBox(FloatScalarDialog)
        self.valueEdit.setObjectName(_fromUtf8("valueEdit"))
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = QtGui.QLabel(FloatScalarDialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = QtGui.QDoubleSpinBox(FloatScalarDialog)
        self.minValueEdit.setObjectName(_fromUtf8("minValueEdit"))
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = QtGui.QLabel(FloatScalarDialog)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = QtGui.QDoubleSpinBox(FloatScalarDialog)
        self.maxValueEdit.setObjectName(_fromUtf8("maxValueEdit"))
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.label_5 = QtGui.QLabel(FloatScalarDialog)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.gridLayout.addWidget(self.label_5, 4, 0, 1, 1)
        self.decimalEdit = QtGui.QSpinBox(FloatScalarDialog)
        self.decimalEdit.setMinimum(1)
        self.decimalEdit.setMaximum(10)
        self.decimalEdit.setProperty("value", 2)
        self.decimalEdit.setObjectName(_fromUtf8("decimalEdit"))
        self.gridLayout.addWidget(self.decimalEdit, 4, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QtGui.QSpacerItem(20, 0, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = QtGui.QDialogButtonBox(FloatScalarDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(FloatScalarDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), FloatScalarDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), FloatScalarDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(FloatScalarDialog)

    def retranslateUi(self, FloatScalarDialog):
        FloatScalarDialog.setWindowTitle(QtGui.QApplication.translate("FloatScalarDialog", "Scalar", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("FloatScalarDialog", "Name", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("FloatScalarDialog", "value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("FloatScalarDialog", "Minimum value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("FloatScalarDialog", "Maximum value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("FloatScalarDialog", "Decimals", None, QtGui.QApplication.UnicodeUTF8))

