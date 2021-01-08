from openalea.plantgl.gui.qt.QtCore import QObject

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

    def getObjectForLsysContext(self,obj):
        return obj

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
            
    def reset(self,obj):
        return self.createDefaultObject()
        
    def getEditor(self,parent_widget):
        """ ask for creation of editor. Should be reimplemented """
        raise NotImplementedError('getEditor')

    def fillEditorMenu(self,menubar,editor):
        """ Function call to fill the menu of the editor """
        pass
        
    def setObjectToEditor(self,editor,obj):
        """ ask for edition of obj with editor. Should be reimplemented """
        raise NotImplementedError('setObjectToEditor')

    def retrieveObjectFromEditor(self,editor):
        """ ask for current value of object being edited """
        raise NotImplementedError('startObjectEdition')
    
    def defaultObjectTypes(self):
        """ ask for type of object managed by this manager. Several are possible. None means that typename should be used. """
        return None
        
    def createDefaultObject(self, objtype = None):
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
    
    def completeContextMenu(self,menu,obj,widget):
        pass
    
    def managePrimitive(self):
        return False
    
    def getTheme(self):
        """ get the color theme currenlty used """
        return {}
    
    def setTheme(self,theme):
        """ get the color theme acccording to the theme dict """
        pass
    
    def to_json(self, obj):
        raise NotImplementedError('jsonObject')


from openalea.plantgl.all import Discretizer, GLRenderer, BBoxComputer, BoundingBox, PyStrPrinter

class AbstractPglObjectManager(AbstractObjectManager):
    def __init__(self, typename = None):
        AbstractObjectManager.__init__(self, typename)
        self.discretizer = Discretizer()
        self.renderer = GLRenderer(self.discretizer)
        self.renderer.renderingMode = GLRenderer.Dynamic
        self.frameColor = (0.5,0.5,0.5,1.0)
        self.focusThumbColor  = (1,1,0,1)
        self.thumbColor  = (0.8,0.8,0,1)
        self.viewAxis = [0,1]

     
    def getBoundingBox(self,obj):
        return BoundingBox(obj)

    def initWriting(self,indentation):
        return indentation+"import openalea.plantgl.all as pgl\n"
        
    def writeObject(self,obj,indentation):
        printer = PyStrPrinter()
        printer.pglnamespace = 'pgl'
        printer.indentation = indentation
        printer.line_between_object = 0
        obj.apply(printer)
        return printer.str()
        
    def to_json(self, obj):
        import openalea.plantgl.algo.jsonrep  as jr
        return jr.to_json_rep(obj)

    def displayThumbnail(self,obj,id,focus,objectthumbwidth):
        import OpenGL.GL as ogl
        self.discretizer.clear()
        b = self.getBoundingBox(obj)
        lsize = b.getSize()
        msize = lsize[lsize.getMaxAbsCoord()]
        scaling = objectthumbwidth/(2*msize)
        x0c = -b.getCenter()[self.viewAxis[0]]*scaling
        y0c = -b.getCenter()[self.viewAxis[1]]*scaling
        if 2*abs(y0c) <= objectthumbwidth:
            ogl.glColor4f(*self.frameColor)
            ogl.glLineWidth(1)
            ogl.glBegin(ogl.GL_LINE_STRIP)                
            ogl.glVertex2f(-objectthumbwidth/2.,-y0c)
            ogl.glVertex2f(objectthumbwidth/2.,-y0c)
            ogl.glEnd()                
        if 2*abs(x0c) <= objectthumbwidth:
            ogl.glColor4f(*self.frameColor)
            ogl.glLineWidth(1)
            ogl.glBegin(ogl.GL_LINE_STRIP)
            ogl.glVertex2f(x0c,-objectthumbwidth/2.)
            ogl.glVertex2f(x0c,objectthumbwidth/2.)
            ogl.glEnd()
        if self.viewAxis[1] == 2:
            ogl.glRotatef(90,1,0,0)
            ogl.glRotatef(-90,0,0,1)
            ogl.glScalef(scaling,scaling,scaling)
        else:
            ogl.glScalef(scaling,-scaling,scaling)
        ogl.glTranslatef(*-b.getCenter())
        if focus:
            ogl.glColor4f(*self.focusThumbColor)
        else:
            ogl.glColor4f(*self.thumbColor)
        ogl.glPushAttrib(ogl.GL_POLYGON_BIT)
        ogl.glPolygonMode(ogl.GL_FRONT_AND_BACK,ogl.GL_LINE)
        ogl.glLineWidth(1)
        self.render(obj)
        ogl.glPopAttrib()

    def render(self, obj):
        obj.apply(self.renderer) 


def curveJsonRepresentation(obj):
    result = dict(name=obj.name, type=obj.__class__.__name__)
    if hasattr(obj,'pointList'):
        result['points'] = list(map(list,obj.pointList))
    else:
        result['points'] = list(map(list,obj.ctrlPointList))
    
    if hasattr(obj,'degree') and not obj.isDegreeToDefault():
        result['degree'] = obj.degree

    return result
