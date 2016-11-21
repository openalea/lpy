# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/killsimulationwidget.ui'
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

class Ui_KillSimulationDialog(object):
    def setupUi(self, KillSimulationDialog):
        KillSimulationDialog.setObjectName(_fromUtf8("KillSimulationDialog"))
        KillSimulationDialog.resize(200, 79)
        KillSimulationDialog.setMaximumSize(QtCore.QSize(200, 79))
        KillSimulationDialog.setModal(True)
        self.verticalLayout = QtGui.QVBoxLayout(KillSimulationDialog)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        spacerItem = QtGui.QSpacerItem(18, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.label = QtGui.QLabel(KillSimulationDialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        self.timeLabel = QtGui.QLabel(KillSimulationDialog)
        self.timeLabel.setObjectName(_fromUtf8("timeLabel"))
        self.horizontalLayout.addWidget(self.timeLabel)
        self.label_3 = QtGui.QLabel(KillSimulationDialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.horizontalLayout.addWidget(self.label_3)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem1)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem2)
        self.okButton = QtGui.QPushButton(KillSimulationDialog)
        self.okButton.setObjectName(_fromUtf8("okButton"))
        self.horizontalLayout_2.addWidget(self.okButton)
        spacerItem3 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem3)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        spacerItem4 = QtGui.QSpacerItem(20, 3, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem4)

        self.retranslateUi(KillSimulationDialog)
        QtCore.QObject.connect(self.okButton, QtCore.SIGNAL(_fromUtf8("clicked()")), KillSimulationDialog.accept)
        QtCore.QMetaObject.connectSlotsByName(KillSimulationDialog)

    def retranslateUi(self, KillSimulationDialog):
        KillSimulationDialog.setWindowTitle(_translate("KillSimulationDialog", "Kill Simulation", None))
        self.label.setText(_translate("KillSimulationDialog", "Force quitting in ", None))
        self.timeLabel.setText(_translate("KillSimulationDialog", "5", None))
        self.label_3.setText(_translate("KillSimulationDialog", "sec ...", None))
        self.okButton.setText(_translate("KillSimulationDialog", "Ok", None))

