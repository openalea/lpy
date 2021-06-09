from openalea.plantgl.all import NurbsPatch
from openalea.lpy.gui.abstractobjectmanager import *
try:
    from openalea.plantgl.gui.nurbspatcheditor import NurbsObjectEditor
    from PyQGLViewer import Vec
except ImportError as e:
    NurbsObjectEditor = None
from math import pi
from openalea.plantgl.gui.qt import QtGui, QtWidgets

class NurbsPatchManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"NurbsPatch")
        self.focusThumbColor  = (0.0,0.5,1.0,1.0)
        self.thumbColor  = (0.0,0.4,0.8,1.0)
        self.viewAxis = [0,2]

    def createDefaultObject(self,subtype=None):
        du = 1/3.
        dv = 1/3.
        return NurbsPatch([[(0,i*du-0.5,j*dv-0.5,1) for j in range(4)] for i in range(4)], 
                            ustride=10,vstride=10)

    def getEditor(self,parent):
        if not NurbsObjectEditor: return None
        editor = NurbsObjectEditor(parent)
        editor.view.camera().setPosition(Vec(1,0,0.5))
        editor.view.camera().setUpVector(Vec(0,0,1))
        editor.view.camera().setViewDirection(Vec(-1,0,0))
        editor.view.camera().fitSphere(Vec(0,0,0.5),0.8)
        return editor

    def setObjectToEditor(self,editor,obj):
        """ ask for edition of obj with editor """
        from copy import deepcopy        
        editor.setNurbsObject(deepcopy(obj))

    def retrieveObjectFromEditor(self,editor):
        """ ask for current value of object being edited """
        return editor.getNurbsObject()

    def canImportData(self,fname):
        from os.path import splitext
        return splitext(fname)[1] == '.s'
    
    def importData(self,fname):
        from openalea.lpy.cpfg_compat.data_import import import_patch
        return import_patch(fname)
        
    def managePrimitive(self):
        return True

    def fillEditorMenu(self,menubar,editor):
        """ Function call to fill the menu of the editor """
        menu = QtWidgets.QMenu('Init',menubar)
        menu.addAction('3x3',lambda : editor.createDefaultObject(3,3))
        menu.addAction('4x4',lambda : editor.createDefaultObject(4,4))
        menu.addAction('5x5',lambda : editor.createDefaultObject(5,5))
        menu.addAction('Custom',lambda : editor.createCurstomDefaultObject())
        menu.addSeparator()
        menu.addAction('rescale',lambda : editor.rescaleObject())
        menubar.addMenu(menu)          

def get_managers():
    return NurbsPatchManager()