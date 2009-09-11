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
    ui.check_ui_generation(os.path.join(ldir, 'curvedialogui.ui'))
    del ldir

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from curvedialogui import *
from openalea.plantgl.gui.curveeditor import FuncConstraint,CurveConstraint
    
class CurveDialog(QDialog,Ui_CurveDialog):
    def __init__(self,*args):
        QDialog.__init__(self,*args)
        self.setupUi(self)
        self.autoupdate = False
        QObject.connect(self.cancelButton,SIGNAL('pressed()'),self.reject)
        QObject.connect(self.okButton,SIGNAL('pressed()'),self.__ok)
        QObject.connect(self.applyButton,SIGNAL('pressed()'),self.__valueChanged)
        QObject.connect(self.autoUpdateCheckBox,SIGNAL('toggled(bool)'),self.setAutomaticUpdate)
    def __valueChanged(self):
        self.emit(SIGNAL('valueChanged()'))
    def __ok(self):
        self.__valueChanged()
        self.accept()
    def hideEvent(self,event):
        self.emit(SIGNAL('hidden()'))
    def setFunctionMode(self):
        self.curveView.pointsConstraints = FuncConstraint()
        self.setWindowTitle('Function Editor')
    def setCurveMode(self):
        self.curveView.pointsConstraints = CurveConstraint()
        self.setWindowTitle('Curve Editor')
    def setCurve(self,curve):
        self.curveView.setCurve(curve)
    def getCurve(self):
        return self.curveView.getCurve()
    def getCurve(self):
        return self.curveView.getCurve()
    def setAutomaticUpdate(self,value):
        if self.autoupdate != value:
            self.autoupdate = value
            if value: QObject.connect(self.curveView,SIGNAL('valueChanged()'),self.__valueChanged)
            else:     QObject.disconnect(self.curveView,SIGNAL('valueChanged()'),self.__valueChanged)
