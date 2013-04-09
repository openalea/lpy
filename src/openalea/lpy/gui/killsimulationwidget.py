# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'd:\fred\mes documents\develop\vplants\trunk\lpy\src\openalea\lpy\gui\killsimulationwidget.ui'
#
# Created: Tue Nov 03 14:36:58 2009
#      by: PyQt4 UI code generator 4.5.4
#
# WARNING! All changes made in this file will be lost!

from openalea.vpltk.qt import qt

class Ui_KillSimulationDialog(object):
    def setupUi(self, KillSimulationDialog):
        KillSimulationDialog.setObjectName("KillSimulationDialog")
        KillSimulationDialog.resize(200, 79)
        KillSimulationDialog.setMaximumSize(qt.QtCore.QSize(200, 79))
        KillSimulationDialog.setModal(True)
        self.verticalLayout = qt.QtGui.QVBoxLayout(KillSimulationDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout = qt.QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        spacerItem = qt.QtGui.QSpacerItem(18, 20, qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.label = qt.QtGui.QLabel(KillSimulationDialog)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.timeLabel = qt.QtGui.QLabel(KillSimulationDialog)
        self.timeLabel.setObjectName("timeLabel")
        self.horizontalLayout.addWidget(self.timeLabel)
        self.label_3 = qt.QtGui.QLabel(KillSimulationDialog)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout.addWidget(self.label_3)
        spacerItem1 = qt.QtGui.QSpacerItem(40, 20, qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem1)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = qt.QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        spacerItem2 = qt.QtGui.QSpacerItem(40, 20, qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem2)
        self.okButton = qt.QtGui.QPushButton(KillSimulationDialog)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout_2.addWidget(self.okButton)
        spacerItem3 = qt.QtGui.QSpacerItem(40, 20, qt.QtGui.QSizePolicy.Expanding, qt.QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem3)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        spacerItem4 = qt.QtGui.QSpacerItem(20, 3, qt.QtGui.QSizePolicy.Minimum, qt.QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem4)

        self.retranslateUi(KillSimulationDialog)
        qt.QtCore.QObject.connect(self.okButton, qt.QtCore.SIGNAL("clicked()"), KillSimulationDialog.accept)
        qt.QtCore.QMetaObject.connectSlotsByName(KillSimulationDialog)

    def retranslateUi(self, KillSimulationDialog):
        KillSimulationDialog.setWindowTitle(qt.QtGui.QApplication.translate("KillSimulationDialog", "Kill Simulation", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label.setText(qt.QtGui.QApplication.translate("KillSimulationDialog", "Force quitting in ", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.timeLabel.setText(qt.QtGui.QApplication.translate("KillSimulationDialog", "5", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(qt.QtGui.QApplication.translate("KillSimulationDialog", "sec ...", None, qt.QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(qt.QtGui.QApplication.translate("KillSimulationDialog", "Ok", None, qt.QtGui.QApplication.UnicodeUTF8))

