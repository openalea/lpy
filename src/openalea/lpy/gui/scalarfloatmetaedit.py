# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'c:\openalea\vplants\lpy\src\openalea\lpy\gui\scalarfloatmetaedit.ui'
#
# Created: Thu Nov 29 08:57:19 2012
#      by: PyQt4 UI code generator 4.8.6
#
# WARNING! All changes made in this file will be lost!

from openalea.vpltk.qt import qt

try:
    _fromUtf8 = qt.QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_FloatScalarDialog(object):
    def setupUi(self, FloatScalarDialog):
        FloatScalarDialog.setObjectName(_fromUtf8("FloatScalarDialog"))
        FloatScalarDialog.resize(301, 179)
        FloatScalarDialog.setWindowTitle(qt.QtGui.QApplication.translate("FloatScalarDialog", "Scalar", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout = qt.QtGui.QVBoxLayout(FloatScalarDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.gridLayout = qt.QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = qt.QtGui.QLabel(FloatScalarDialog)
        self.label.setText(qt.QtGui.QApplication.translate("FloatScalarDialog", "Name", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = qt.QtGui.QLineEdit(FloatScalarDialog)
        self.nameEdit.setObjectName(_fromUtf8("nameEdit"))
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = qt.QtGui.QLabel(FloatScalarDialog)
        self.label_2.setText(qt.QtGui.QApplication.translate("FloatScalarDialog", "value", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = qt.QtGui.QDoubleSpinBox(FloatScalarDialog)
        self.valueEdit.setObjectName(_fromUtf8("valueEdit"))
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = qt.QtGui.QLabel(FloatScalarDialog)
        self.label_3.setText(qt.QtGui.QApplication.translate("FloatScalarDialog", "Minimum value", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = qt.QtGui.QDoubleSpinBox(FloatScalarDialog)
        self.minValueEdit.setObjectName(_fromUtf8("minValueEdit"))
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = qt.QtGui.QLabel(FloatScalarDialog)
        self.label_4.setText(qt.QtGui.QApplication.translate("FloatScalarDialog", "Maximum value", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = qt.QtGui.QDoubleSpinBox(FloatScalarDialog)
        self.maxValueEdit.setObjectName(_fromUtf8("maxValueEdit"))
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.label_5 = qt.QtGui.QLabel(FloatScalarDialog)
        self.label_5.setText(qt.QtGui.QApplication.translate("FloatScalarDialog", "Decimals", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.gridLayout.addWidget(self.label_5, 4, 0, 1, 1)
        self.decimalEdit = qt.QtGui.QSpinBox(FloatScalarDialog)
        self.decimalEdit.setMinimum(1)
        self.decimalEdit.setMaximum(10)
        self.decimalEdit.setProperty("value", 2)
        self.decimalEdit.setObjectName(_fromUtf8("decimalEdit"))
        self.gridLayout.addWidget(self.decimalEdit, 4, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = qt.QtGui.QSpacerItem(20, 0, qt.QtGui.QSizePolicy.Minimum, qt.QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = qt.QtGui.QDialogButtonBox(FloatScalarDialog)
        self.buttonBox.setOrientation(qt.QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(qt.QtGui.QDialogButtonBox.Cancel|qt.QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(FloatScalarDialog)
        qt.QtCore.QObject.connect(self.buttonBox, qt.QtCore.SIGNAL(_fromUtf8("accepted()")), FloatScalarDialog.accept)
        qt.QtCore.QObject.connect(self.buttonBox, qt.QtCore.SIGNAL(_fromUtf8("rejected()")), FloatScalarDialog.reject)
        qt.QtCore.QMetaObject.connectSlotsByName(FloatScalarDialog)

    def retranslateUi(self, FloatScalarDialog):
        pass

