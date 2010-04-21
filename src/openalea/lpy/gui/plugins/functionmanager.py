from openalea.plantgl.gui.curve2deditor import Curve2DEditor,FuncConstraintfrom openalea.lpy.gui.abstractobjectmanager import *
from curve2dmanager import displayLineAsThumbnail
class FunctionManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"Function")
            def displayThumbnail(self,obj,i,focus,objectthumbwidth):        displayLineAsThumbnail(self,obj,i,focus,objectthumbwidth,(1,0,1,0))        
    def createDefaultObject(self):
        return FuncConstraint.defaultCurve()
    def getEditor(self,parent):        return Curve2DEditor(parent,FuncConstraint())    def setObjectToEditor(self,editor,obj):        """ ask for edition of obj with editor """        from copy import deepcopy                editor.setCurve(deepcopy(obj))    def retrieveObjectFromEditor(self,editor):        """ ask for current value of object being edited """        return editor.getCurve()        def writeObjectToLsysContext(self,obj):        return 'pgl.QuantisedFunction('+obj.name+')'        def get_managers():    return FunctionManager()