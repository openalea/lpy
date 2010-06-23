from openalea.plantgl.gui.curve2deditor import Curve2DEditor,FuncConstraintfrom openalea.lpy.gui.abstractobjectmanager import *
from curve2dmanager import displayLineAsThumbnail
class FunctionManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"Function")
            def displayThumbnail(self,obj,i,focus,objectthumbwidth):        displayLineAsThumbnail(self,obj,i,objectthumbwidth,(1,0,1,1))        
    def createDefaultObject(self,subtype):
        return FuncConstraint.defaultCurve()
    def getEditor(self,parent):        return Curve2DEditor(parent,FuncConstraint())    def setObjectToEditor(self,editor,obj):        """ ask for edition of obj with editor """        from copy import deepcopy                editor.setCurve(deepcopy(obj))    def retrieveObjectFromEditor(self,editor):        """ ask for current value of object being edited """        return editor.getCurve()        def writeObjectToLsysContext(self,obj):        return 'pgl.QuantisedFunction('+obj.name+')'        def canImportData(self,fname):        from os.path import splitext        return splitext(fname)[1] == '.fset'        def importData(self,fname):        from openalea.lpy.gui.lpfg_data_import import import_functions        return import_functions(fname)        def get_managers():    return FunctionManager()