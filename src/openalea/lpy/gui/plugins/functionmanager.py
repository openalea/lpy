try:
    from openalea.plantgl.gui.curve2deditor import Curve2DEditor,FuncConstraint
except ImportError as e:
    Curve2DEditor = None
from openalea.lpy.gui.abstractobjectmanager import *
from openalea.plantgl.gui.qt import QtGui, QtWidgets
from openalea.plantgl.all import QuantisedFunction

class FunctionManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"Function")
        self.thumbColor = (1,0,1,1)
        self.focusThumbColor = (0.8,0,0.8,1)

    def render(self, obj):
        import OpenGL.GL as ogl
        pw = obj.width
        obj.width = 1
        ogl.glLineWidth(2)
        obj.apply(self.renderer) 
        obj.width = pw

    def getObjectForLsysContext(self,obj):
        return QuantisedFunction(obj)
        
    def createDefaultObject(self,subtype=None):
        import openalea.plantgl.all as pgl
        nbP = 4
        return pgl.NurbsCurve2D(pgl.Point3Array([(float(i)/(nbP-1),0) for i in range(nbP)],1) )

    def getEditor(self,parent):
        if Curve2DEditor:
            return Curve2DEditor(parent,FuncConstraint())
        else: return None

    def setObjectToEditor(self,editor,obj):
        """ ask for edition of obj with editor """
        from copy import deepcopy        
        editor.setCurve(deepcopy(obj))

    def retrieveObjectFromEditor(self,editor):
        """ ask for current value of object being edited """
        return editor.getCurve()
    
    def writeObjectToLsysContext(self,obj):
        return 'pgl.QuantisedFunction('+obj.name+')'
    
    def canImportData(self,fname):
        from os.path import splitext
        ext = splitext(fname)[1]
        return  ext == '.fset' or ext == '.func'
    
    def importData(self,fname):
        from openalea.lpy.cpfg_compat.data_import import import_functions, import_function
        from os.path import splitext
        ext = splitext(fname)[1]
        if ext == '.fset':  return import_functions(fname)
        else: return import_function(fname)
    
    def fillEditorMenu(self,menubar,editor):
        """ Function call to fill the menu of the editor """
        menu = QtWidgets.QMenu('Theme',menubar)
        menu.addAction('Black',lambda : editor.applyTheme(editor.BLACK_THEME))
        menu.addAction('White',lambda : editor.applyTheme(editor.WHITE_THEME))
        menubar.addMenu(menu)

    def to_json(self, obj):
        import openalea.plantgl.algo.jsonrep  as jr
        res = jr.to_json_rep(obj)
        res['is_function'] = True
        return res

       
def get_managers():
    return FunctionManager()
