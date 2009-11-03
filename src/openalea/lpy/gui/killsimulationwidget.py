# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'd:\fred\mes documents\develop\vplants\trunk\lpy\src\openalea\lpy\gui\killsimulationwidget.ui'
#
# Created: Tue Nov 03 14:36:58 2009
#      by: PyQt4 UI code generator 4.5.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_KillSimulationDialog(object):
    def setupUi(self, KillSimulationDialog):
        KillSimulationDialog.setObjectName("KillSimulationDialog")
        KillSimulationDialog.resize(200, 79)
        KillSimulationDialog.setMaximumSize(QtCore.QSize(200, 79))
        KillSimulationDialog.setModal(True)
        self.verticalLayout = QtGui.QVBoxLayout(KillSimulationDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        spacerItem = QtGui.QSpacerItem(18, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.label = QtGui.QLabel(KillSimulationDialog)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.timeLabel = QtGui.QLabel(KillSimulationDialog)
        self.timeLabel.setObjectName("timeLabel")
        self.horizontalLayout.addWidget(self.timeLabel)
        self.label_3 = QtGui.QLabel(KillSimulationDialog)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout.addWidget(self.label_3)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem1)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem2)
        self.okButton = QtGui.QPushButton(KillSimulationDialog)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout_2.addWidget(self.okButton)
        spacerItem3 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem3)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        spacerItem4 = QtGui.QSpacerItem(20, 3, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem4)

        self.retranslateUi(KillSimulationDialog)
        QtCore.QObject.connect(self.okButton, QtCore.SIGNAL("clicked()"), KillSimulationDialog.accept)
        QtCore.QMetaObject.connectSlotsByName(KillSimulationDialog)

    def retranslateUi(self, KillSimulationDialog):
        KillSimulationDialog.setWindowTitle(QtGui.QApplication.translate("KillSimulationDialog", "Kill Simulation", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("KillSimulationDialog", "Force quitting in ", None, QtGui.QApplication.UnicodeUTF8))
        self.timeLabel.setText(QtGui.QApplication.translate("KillSimulationDialog", "5", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("KillSimulationDialog", "sec ...", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(QtGui.QApplication.translate("KillSimulationDialog", "Ok", None, QtGui.QApplication.UnicodeUTF8))

