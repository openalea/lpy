try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

# Generate GUI if necessary
if not py2exe_release:
    import os
    import compile_ui as ui
    ldir    = os.path.dirname(__file__)
    ui.check_ui_generation(os.path.join(ldir, 'killsimulationwidget.ui'))
    del ldir

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from killsimulationwidget import Ui_KillSimulationDialog


class KillSimulationDialog (QDialog,Ui_KillSimulationDialog):
    def __init__(self,parent):
        QDialog.__init__(self,parent)
        self.setupUi(self)
        self.setModal(True)
        self.timer = QTimer(self)
        self.timer.setInterval(1000)
        self.timer.setSingleShot(True)
        QObject.connect(self.timer,SIGNAL('timeout()'),self.step)
        QObject.connect(self, SIGNAL("accepted()"), self.finishProcess)
        QObject.connect(self, SIGNAL("rejected()"), self.timer.stop)
        self.timeout = 5
        self.condition = None
        self.killer = None
    def run(self,cond, killer, timeout = 5, initialtimeout = 0.1):
        self.condition = cond
        self.killer = killer
        self.timeout = timeout
        if self.condition():
            QTimer.singleShot(1000*initialtimeout,self.initTiming)
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