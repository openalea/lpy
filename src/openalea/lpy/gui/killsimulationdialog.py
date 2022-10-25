try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

from openalea.plantgl.gui.qt import qt    
from .killsimulationwidget import Ui_KillSimulationDialog

from openalea.plantgl.gui.qt.QtCore import QTimer
from openalea.plantgl.gui.qt.QtWidgets import QDialog

class KillSimulationDialog (QDialog,Ui_KillSimulationDialog):
    def __init__(self,parent):
        QDialog.__init__(self,parent)
        self.setupUi(self)
        self.setModal(True)
        self.timer = QTimer(self)
        self.timer.setInterval(1000)
        self.timer.setSingleShot(True)
        self.timer.timeout.connect(self.step)
        self.accepted.connect( self.finishProcess)
        self.rejected.connect( self.timer.stop)
        self.timeout = 5
        self.condition = None
        self.killer = None
    def run(self,cond, killer, timeout = 5, initialtimeout = 0.1):
        self.condition = cond
        self.killer = killer
        self.timeout = timeout
        if self.condition():
            QTimer.singleShot(int(1000*initialtimeout),self.initTiming)
    def initTiming(self):
        if self.condition():
            self.step()
            self.exec_()
    def step(self):
        if self.condition():
            if self.timeout > 0:
                self.timeLabel.setText(str(self.timeout))
                self.timeout -= 1
                self.timer.start()
            else:
                self.timeLabel.setText(str(self.timeout))                
                self.accept()
        else:
            self.accept()
    def finishProcess(self):
        self.timer.stop()
        if self.condition():
            self.killer()
