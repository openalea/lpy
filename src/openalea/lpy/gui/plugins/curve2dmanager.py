try:
    from openalea.plantgl.gui.curve2deditor import Curve2DEditor,Curve2DConstraint
except ImportError as e:
    Curve2DEditor = None
from openalea.plantgl.scenegraph import Polyline2D, BezierCurve2D, NurbsCurve2D, Point2Array, Point3Array
from openalea.lpy.gui.abstractobjectmanager import *
from openalea.plantgl.gui.qt import QtGui, QtWidgets


class TriggerParamFunc:
    def __init__(self,func,*value):
        self.func = func
        self.value= value
    def __call__(self):
        self.func(*self.value)

class Curve2DManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"Curve2D")
        self.focusThumbColor  = (1,1,0,1)
        self.thumbColor  = (0.8,0.8,0,1)

    def render(self, obj):
        import OpenGL.GL as ogl
        pw = obj.width
        obj.width = 1
        ogl.glLineWidth(2)
        obj.apply(self.renderer) 
        obj.width = pw
         
    def getTheme(self):
        return { 'Curve2D' : [ int(self.thumbColor[i] *255) for i in range(3)],
                 'FocusCurve2D' : [ int(self.focusThumbColor[i] *255) for i in range(3)],
                 'FrameColor' : [ int(self.frameColor[i] *255) for i in range(3)] } 
        
    def setTheme(self,theme):
        if 'FocusCurve2D' in theme:
            self.focusThumbColor  =  [ theme['FocusCurve2D'][i] *255 for i in range(3)] + [1]
        if 'Curve2D' in theme:
            self.thumbColor  = [ theme['Curve2D'][i] *255 for i in range(3)] + [1]
        if 'FrameColor' in theme:
            self.frameColor = [ theme['FrameColor'][i] *255 for i in range(3)] + [1]
                 
    def createDefaultObject(self,subtype = None):
        nbP = 4
        if subtype == 'Polyline': 
            return Polyline2D(Point2Array([(-0.5+float(i)/(nbP-1),0) for i in range(nbP)]) )
        if subtype == 'BezierCurve': 
            return BezierCurve2D(Point3Array([(-0.5+float(i)/(nbP-1),0) for i in range(nbP)],1) )
        else: 
            return NurbsCurve2D(Point3Array([(-0.5+float(i)/(nbP-1),0) for i in range(nbP)],1) )

    def reset(self,obj):
        subtype = 'NurbsCurve'        
        if isinstance(obj,Polyline2D): 
            subtype = 'Polyline'
        elif isinstance(obj,BezierCurve2D): 
            subtype = 'BezierCurve'
        return self.createDefaultObject(subtype)
    def getEditor(self,parent):
        if Curve2DEditor:
            return Curve2DEditor(parent,Curve2DConstraint())
        else : return None

    def setObjectToEditor(self,editor,obj):
        """ ask for edition of obj with editor """
        from copy import deepcopy        
        editor.setCurve(deepcopy(obj))

    def retrieveObjectFromEditor(self,editor):
        """ ask for current value of object being edited """
        return editor.getCurve()

    def canImportData(self,fname):
        from os.path import splitext
        return splitext(fname)[1] == '.cset'
    
    def importData(self,fname):
        from openalea.lpy.cpfg_compat.data_import import import_contours
        return import_contours(fname)

    def defaultObjectTypes(self):
        return ['Polyline','BezierCurve','NurbsCurve']
        
    def fillEditorMenu(self,menubar,editor):
        """ Function call to fill the menu of the editor """
        menu = QtWidgets.QMenu('Curve',menubar)
        menu.addAction('Flip Horizontally',TriggerParamFunc(self.flipHorizontallyEditor,editor))
        menu.addAction('Flip Vertically',TriggerParamFunc(self.flipVerticallyEditor,editor))
        menubar.addMenu(menu)
        menu = QtWidgets.QMenu('Theme',menubar)
        menu.addAction('Black',lambda : editor.applyTheme(editor.BLACK_THEME))
        menu.addAction('White',lambda : editor.applyTheme(editor.WHITE_THEME))
        menubar.addMenu(menu)
        menu = QtWidgets.QMenu('Image',menubar)
        menu.addAction('Open',lambda : editor.openImage())
        menu.addAction('Close',lambda : editor.closeImage())
        menubar.addMenu(menu)

    def completeContextMenu(self,menu,obj,widget):
        menu.addAction('Flip Horizontally',TriggerParamFunc(self.flipHorizontally,obj,widget))
        menu.addAction('Flip Vertically',TriggerParamFunc(self.flipVertically,obj,widget))
        
    def flipHorizontallyEditor(self,editor):
        self.flipHorizontally(editor.getCurve(),editor)
        
    def flipHorizontally(self,obj,widget):
        if isinstance(obj,Polyline2D): 
            obj.pointList = [(i.x,-i.y) for i in obj.pointList]
        else:
            obj.ctrlPointList = [(i.x,-i.y,i.z) for i in obj.ctrlPointList]
        widget.updateGL()
        
    def flipVerticallyEditor(self,editor):
        self.flipVertically(editor.getCurve(),editor)
        
    def flipVertically(self,obj,widget):
        if isinstance(obj,Polyline2D): 
            obj.pointList = [(-i.x,i.y) for i in obj.pointList]
        else:
            obj.ctrlPointList = [(-i.x,i.y,i.z) for i in obj.ctrlPointList]
        widget.updateGL()

    def to_json(self, obj):
        import openalea.plantgl.algo.jsonrep  as jr
        res = jr.to_json_rep(obj)
        res['is_function'] = False
        return res

def get_managers():
    return Curve2DManager()
