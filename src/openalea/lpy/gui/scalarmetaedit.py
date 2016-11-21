# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/scalarmetaedit.ui'
#
# Created by: PyQt4 UI code generator 4.11.4
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

class Ui_ScalarDialog(object):
    def setupUi(self, ScalarDialog):
        ScalarDialog.setObjectName(_fromUtf8("ScalarDialog"))
        ScalarDialog.resize(296, 155)
        self.verticalLayout = QtGui.QVBoxLayout(ScalarDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = QtGui.QLabel(ScalarDialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = QtGui.QLineEdit(ScalarDialog)
        self.nameEdit.setObjectName(_fromUtf8("nameEdit"))
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = QtGui.QLabel(ScalarDialog)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = QtGui.QSpinBox(ScalarDialog)
        self.valueEdit.setMaximum(2147483647)
        self.valueEdit.setObjectName(_fromUtf8("valueEdit"))
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = QtGui.QLabel(ScalarDialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = QtGui.QSpinBox(ScalarDialog)
        self.minValueEdit.setMinimum(-2147483647)
        self.minValueEdit.setMaximum(2147483647)
        self.minValueEdit.setObjectName(_fromUtf8("minValueEdit"))
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = QtGui.QLabel(ScalarDialog)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = QtGui.QSpinBox(ScalarDialog)
        self.maxValueEdit.setMaximum(2147483647)
        self.maxValueEdit.setProperty("value", 0)
        self.maxValueEdit.setObjectName(_fromUtf8("maxValueEdit"))
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = QtGui.QSpacerItem(20, 0, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = QtGui.QDialogButtonBox(ScalarDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(ScalarDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), ScalarDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), ScalarDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(ScalarDialog)

    def retranslateUi(self, ScalarDialog):
        ScalarDialog.setWindowTitle(_translate("ScalarDialog", "Scalar", None))
        self.label.setText(_translate("ScalarDialog", "Name", None))
        self.label_2.setText(_translate("ScalarDialog", "value", None))
        self.label_3.setText(_translate("ScalarDialog", "Minimum value", None))
        self.label_4.setText(_translate("ScalarDialog", "Maximum value", None))

