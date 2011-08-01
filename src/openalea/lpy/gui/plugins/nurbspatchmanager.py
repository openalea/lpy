from openalea.plantgl.all import NurbsPatch
from openalea.plantgl.gui.nurbspatcheditor import NurbsPatchEditor
from openalea.lpy.gui.abstractobjectmanager import *
from OpenGL.GL import *
from math import pi
from PyQGLViewer import Vec

class NurbsPatchManager(AbstractPglObjectManager):
    """see the doc of the objectmanager abtsract class to undesrtand the implementation of the functions"""
    def __init__(self):
        AbstractPglObjectManager.__init__(self,"NurbsPatch")

    def displayThumbnail(self,obj,id,focus,objectthumbwidth):
        b = self.getBoundingBox(obj)
        lsize = b.getSize()
        msize = lsize[lsize.getMaxAbsCoord()]
        scaling = objectthumbwidth/(2*msize)
        x0c = -b.getCenter()[0]*scaling
        z0c = -b.getCenter()[2]*scaling
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)
        if 2*abs(z0c) <= objectthumbwidth:
            glColor4f(0.5,0.5,0.5,0)
            glLineWidth(1)
            glBegin(GL_LINE_STRIP)                
            glVertex2f(-objectthumbwidth/2.,-z0c)
            glVertex2f(objectthumbwidth/2.,-z0c)
            glEnd()                
        if 2*abs(x0c) <= objectthumbwidth:
            glColor4f(0.5,0.5,0.5,0)
            glLineWidth(1)
            glBegin(GL_LINE_STRIP)
            glVertex2f(x0c,-objectthumbwidth/2.)
            glVertex2f(x0c,objectthumbwidth/2.)
            glEnd()                
        glRotatef(90,1,0,0)
        glRotatef(-90,0,0,1)
        glScalef(scaling,scaling,scaling)
        glTranslatef(*-b.getCenter())
        if focus:
            glColor4f(0.0,0.5,1.0,1.0)
        else:
            glColor4f(0.0,0.4,0.8,1.0)
        glLineWidth(1)
        obj.apply(self.renderer)

    def createDefaultObject(self,subtype=None):
        return NurbsPatch([[(0,-0.5+j/3.,i/3.,1) for j in range(4)] for i in range(4)])

    def getEditor(self,parent):
        editor = NurbsPatchEditor(parent)
        editor.camera().setPosition(Vec(1,0,0.5))
        editor.camera().setUpVector(Vec(0,0,1))
        editor.camera().setViewDirection(Vec(-1,0,0))
        editor.camera().fitSphere(Vec(0,0,0.5),0.8)
        return editor

    def setObjectToEditor(self,editor,obj):
        """ ask for edition of obj with editor """
        from copy import deepcopy        
        editor.setNurbsPatch(deepcopy(obj))

    def retrieveObjectFromEditor(self,editor):
        """ ask for current value of object being edited """
        return editor.getNurbsPatch()

    def canImportData(self,fname):
        from os.path import splitext
        return splitext(fname)[1] == '.s'
    
    def importData(self,fname):
        from openalea.lpy.cpfg_compat.data_import import import_patch
        return import_patch(fname)
        
    def managePrimitive(self):
        return True
        

def get_managers():
    return NurbsPatchManager()