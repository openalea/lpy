# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'c:\openalea\vplants\lpy\src\openalea\lpy\gui\scalarmetaedit.ui'
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

class Ui_ScalarDialog(object):
    def setupUi(self, ScalarDialog):
        ScalarDialog.setObjectName(_fromUtf8("ScalarDialog"))
        ScalarDialog.resize(296, 155)
        ScalarDialog.setWindowTitle(qt.QtGui.QApplication.translate("ScalarDialog", "Scalar", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.verticalLayout = qt.QtGui.QVBoxLayout(ScalarDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.gridLayout = qt.QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = qt.QtGui.QLabel(ScalarDialog)
        self.label.setText(qt.QtGui.QApplication.translate("ScalarDialog", "Name", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.nameEdit = qt.QtGui.QLineEdit(ScalarDialog)
        self.nameEdit.setObjectName(_fromUtf8("nameEdit"))
        self.gridLayout.addWidget(self.nameEdit, 0, 1, 1, 1)
        self.label_2 = qt.QtGui.QLabel(ScalarDialog)
        self.label_2.setText(qt.QtGui.QApplication.translate("ScalarDialog", "value", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.valueEdit = qt.QtGui.QSpinBox(ScalarDialog)
        self.valueEdit.setMaximum(2147483647)
        self.valueEdit.setObjectName(_fromUtf8("valueEdit"))
        self.gridLayout.addWidget(self.valueEdit, 1, 1, 1, 1)
        self.label_3 = qt.QtGui.QLabel(ScalarDialog)
        self.label_3.setText(qt.QtGui.QApplication.translate("ScalarDialog", "Minimum value", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.minValueEdit = qt.QtGui.QSpinBox(ScalarDialog)
        self.minValueEdit.setMinimum(-2147483647)
        self.minValueEdit.setMaximum(2147483647)
        self.minValueEdit.setObjectName(_fromUtf8("minValueEdit"))
        self.gridLayout.addWidget(self.minValueEdit, 2, 1, 1, 1)
        self.label_4 = qt.QtGui.QLabel(ScalarDialog)
        self.label_4.setText(qt.QtGui.QApplication.translate("ScalarDialog", "Maximum value", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxValueEdit = qt.QtGui.QSpinBox(ScalarDialog)
        self.maxValueEdit.setMaximum(2147483647)
        self.maxValueEdit.setProperty("value", 0)
        self.maxValueEdit.setObjectName(_fromUtf8("maxValueEdit"))
        self.gridLayout.addWidget(self.maxValueEdit, 3, 1, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout)
        spacerItem = qt.QtGui.QSpacerItem(20, 0, qt.QtGui.QSizePolicy.Minimum, qt.QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.buttonBox = qt.QtGui.QDialogButtonBox(ScalarDialog)
        self.buttonBox.setOrientation(qt.QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(qt.QtGui.QDialogButtonBox.Cancel|qt.QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(ScalarDialog)
        qt.QtCore.QObject.connect(self.buttonBox, qt.QtCore.SIGNAL(_fromUtf8("accepted()")), ScalarDialog.accept)
        qt.QtCore.QObject.connect(self.buttonBox, qt.QtCore.SIGNAL(_fromUtf8("rejected()")), ScalarDialog.reject)
        qt.QtCore.QMetaObject.connectSlotsByName(ScalarDialog)

    def retranslateUi(self, ScalarDialog):
        pass

