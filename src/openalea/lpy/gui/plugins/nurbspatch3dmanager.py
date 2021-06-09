from openalea.lpy.gui.abstractobjectmanager import *
try:
    from openalea.plantgl.gui.nurbspatcheditor import NurbsObjectEditor
    from PyQGLViewer import Vec
except ImportError as e:
    NurbsPatchEditor = None
from math import pi
from openalea.plantgl.all import Scene, Polyline, NurbsPatch3D, Vector4
from openalea.plantgl.gui.qt import QtGui, QtWidgets

def view(patch):
    import itertools

    patchView = Scene()

    opoints = patch.ctrlPointMatrix
    for i0,i1 in [(0,1),(1,2),(0,2)]:
        for index in itertools.product(*[list(range(d)) for d in [opoints.shape[i0],opoints.shape[i1]]]):
            i = [slice(None,None),slice(None,None),slice(None,None)]
            i[i0] = index[0]
            i[i1] = index[1]
            points = opoints[tuple(i)]
            pid = ((i[2]*10000 if not isinstance(i[2],slice) else 0)+
                   (i[1]*100 if not isinstance(i[1],slice) else 0)+
                   (i[0] if not isinstance(i[0],slice) else 0))
            patchView.add(Polyline([p.project() for p in points]))

    return patchView

class NurbsPatch3DManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"NurbsPatch3D")
        self.focusThumbColor  = (0.0,1.0,0.5,1.0)
        self.thumbColor  = (0.0,0.8,0.4,1.0)
        self.viewAxis = [0,2]

    def getBoundingBox(self, obj):
        return obj.getBoundingBox()

    def render(self, obj):
        mrep = view(obj) 
        mrep.applyGeometryOnly(self.renderer)
        

    def createDefaultObject(self,subtype=None):
        res = NurbsPatch3D.default(3,3,3)
        res.name = ''
        return res

    def getEditor(self,parent):
        if not NurbsObjectEditor: return None
        editor = NurbsObjectEditor(parent, 3)
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
        from copy import deepcopy
        return deepcopy(editor.getNurbsObject())

         
    def managePrimitive(self):
        return True

    def initWriting(self,indentation):
        return indentation+"import openalea.plantgl.scenegraph.nurbspatch_nd as pglnp\n"
        
    def writeObject(self,obj,indentation):
        res = indentation+obj.name+ ' = pglnp.' +repr(obj)+'\n'
        res += indentation+obj.name+'.name = '+repr(obj.name)+'\n'
        return res

    def fillEditorMenu(self,menubar,editor):
        """ Function call to fill the menu of the editor """
        menu = QtWidgets.QMenu('Init',menubar)
        menu.addAction('3x3x3',lambda : editor.createDefaultObject(3,3,3))
        menu.addAction('4x4x4',lambda : editor.createDefaultObject(4,4,4))
        menu.addAction('5x5x5',lambda : editor.createDefaultObject(5,5,5))
        menu.addAction('Custom',lambda : editor.createCurstomDefaultObject())
        menu.addSeparator()
        menu.addAction('rescale',lambda : editor.rescaleObject())
        menubar.addMenu(menu)       

def get_managers():
    return NurbsPatch3DManager()