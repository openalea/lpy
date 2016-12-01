# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/fboudon/Develop/oagit/lpy/src/openalea/lpy/gui/killsimulationwidget.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_KillSimulationDialog(object):
    def setupUi(self, KillSimulationDialog):
        KillSimulationDialog.setObjectName("KillSimulationDialog")
        KillSimulationDialog.resize(200, 79)
        KillSimulationDialog.setMaximumSize(QtCore.QSize(200, 79))
        KillSimulationDialog.setModal(True)
        self.verticalLayout = QtWidgets.QVBoxLayout(KillSimulationDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        spacerItem = QtWidgets.QSpacerItem(18, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.label = QtWidgets.QLabel(KillSimulationDialog)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.timeLabel = QtWidgets.QLabel(KillSimulationDialog)
        self.timeLabel.setObjectName("timeLabel")
        self.horizontalLayout.addWidget(self.timeLabel)
        self.label_3 = QtWidgets.QLabel(KillSimulationDialog)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout.addWidget(self.label_3)
        spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem1)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        spacerItem2 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem2)
        self.okButton = QtWidgets.QPushButton(KillSimulationDialog)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout_2.addWidget(self.okButton)
        spacerItem3 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem3)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        spacerItem4 = QtWidgets.QSpacerItem(20, 3, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem4)

        self.retranslateUi(KillSimulationDialog)
        self.okButton.clicked.connect(KillSimulationDialog.accept)
        QtCore.QMetaObject.connectSlotsByName(KillSimulationDialog)

    def retranslateUi(self, KillSimulationDialog):
        _translate = QtCore.QCoreApplication.translate
        KillSimulationDialog.setWindowTitle(_translate("KillSimulationDialog", "Kill Simulation"))
        self.label.setText(_translate("KillSimulationDialog", "Force quitting in "))
        self.timeLabel.setText(_translate("KillSimulationDialog", "5"))
        self.label_3.setText(_translate("KillSimulationDialog", "sec ..."))
        self.okButton.setText(_translate("KillSimulationDialog", "Ok"))

