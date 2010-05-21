from PyQt4.QtCore import QObject, SIGNAL

MouseFocus, Selection, Actived = 1,2,4

class AbstractObjectManager(QObject):
    """ Manage a type of data. Make it possible to name it, display it as thumbnail and edit it"""
    def __init__(self, typename = None):
        """We need the name of the object managed by the editor to link the manager with the right Editor"""
        QObject.__init__(self)
        self.typename =  typename
        
    def setName(self,obj,name):
        obj.name=name

    def getName(self,obj):
        return obj.name

    def displayThumbnail(self,obj,id,mode,objectthumbwidth):
        """ display of an object in the Lpy main window Panel, 
            :param obj: the object to display
            :param id: id of the object in the list
            :param mode: define if object has mode, 
            :param thumbwidth: width of the thumbnail representing the object in the panel
            :param objectthumbwidth: width for the representation of the object in the panel
            Should be reimplemented
        """
        raise NotImplementedError('displayThumbnail')
            
    def getEditor(self,parent_widget):
        """ ask for creation of editor. Should be reimplemented """
        raise NotImplementedError('getEditor')

    def setObjectToEditor(self,editor,obj):
        """ ask for edition of obj with editor. Should be reimplemented """
        raise NotImplementedError('setObjectToEditor')

    def retrieveObjectFromEditor(self,editor):
        """ ask for current value of object being edited """
        raise NotImplementedError('startObjectEdition')

    def createDefaultObject(self):
        """ 
            create a default object of the type handled by the manager.
            requires instanciate a new item in the panel.
            Should be reimplemented
         """
        raise NotImplementedError('createDefaultObject')
        
    def initWriting(self,indentation):
        return ''
        
    def writeObject(self,obj,indentation):
        raise NotImplementedError('writeObject')
    
    def writeObjectToLsysContext(self,obj):
        return obj.name
    
    def canImportData(self,fname):
        return False
    
    def importData(self,fname):
        raise NotImplementedError('importData')        
    
from openalea.plantgl.all import Discretizer, GLRenderer, BBoxComputer, BoundingBox, PyStrPrinter

class AbstractPglObjectManager(AbstractObjectManager):
    def __init__(self, typename = None):
        AbstractObjectManager.__init__(self, typename)
        self.discretizer = Discretizer()
        self.renderer = GLRenderer(self.discretizer)
        self.renderer.renderingMode = GLRenderer.Dynamic
     
    def getBoundingBox(self,obj):
        return BoundingBox(obj)

    def initWriting(self,indentation):
        return indentation+"import openalea.plantgl.all as pgl\n"
        
    def writeObject(self,obj,indentation):
        printer = PyStrPrinter()
        printer.pglnamespace = 'pgl'
        printer.indentation = indentation
        obj.apply(printer)
        return printer.str()
        