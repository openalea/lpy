from openalea.plantgl.all import *
try:
    from openalea.lpy import *
    InitialisationFunctionName = LsysContext.InitialisationFunctionName
except:
    InitialisationFunctionName = '__initialiseContext__'

from PyQt4.QtOpenGL import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from OpenGL.GL import *
from OpenGL.GLU import *

def interpolColor(col1,col2,p1):
    r = col1.red * (1-p1) + col2.red *p1
    g = col1.green * (1-p1) + col2.green *p1
    b = col1.blue * (1-p1) + col2.blue *p1
    return Color3(int(r),int(g),int(b))

    
class MaterialEditor (QGLWidget):
    def __init__(self,parent):
        QGLWidget.__init__(self,parent)
        self.unitsize = 30
        self.sphere = None
        self.spherelist = None
        self.darkcheck = [0.5,0.5,0.5]
        self.lightcheck = [0.9,0.9,0.9]
        self.checklist = None
        self.turtle = PglTurtle()
        self.mousepos = None
        self.menuselection = None
        self.copyselection = None
        self.selectionbegin = None
        self.selectionend = None
    def initializeGL(self):
        glClearColor(1.0,1.0,1.0,1.0)

        glShadeModel(GL_SMOOTH)
        glLightfv(GL_LIGHT0,GL_POSITION,[1000,1000,2000,0])
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,[1.0,1.0,1.0,1.0]) 
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)  

        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_BLEND)

        if self.sphere:
            gluDeleteQuadric(self.sphere)
        self.sphere = gluNewQuadric()
        gluQuadricDrawStyle(self.sphere,GLU_FILL)
        gluQuadricNormals(self.sphere,GLU_SMOOTH)

        self.spherelist = glGenLists(1)
        glNewList(self.spherelist,GL_COMPILE)
        gluSphere(self.sphere,self.unitsize*0.8,80,80)
        glEndList()
        
        checkdepth = -self.unitsize
        self.checklist = glGenLists(1)
        glNewList(self.checklist,GL_COMPILE)
        glDisable(GL_LIGHTING)
        glBegin(GL_QUADS)
        glColor3fv(self.lightcheck)
        glVertex3f(-self.unitsize,self.unitsize,checkdepth)
        glVertex3f(0,self.unitsize,checkdepth)
        glVertex3f(0,0,checkdepth)
        glVertex3f(-self.unitsize,0,checkdepth)

        glVertex3f(0,0,checkdepth)
        glVertex3f(self.unitsize,0,checkdepth)
        glVertex3f(self.unitsize,-self.unitsize,checkdepth)
        glVertex3f(0,-self.unitsize,checkdepth)

        glColor3fv(self.darkcheck)
        glVertex3f(0,self.unitsize,checkdepth)
        glVertex3f(self.unitsize,self.unitsize,checkdepth)
        glVertex3f(self.unitsize,0,checkdepth)
        glVertex3f(0,0,checkdepth)

        glVertex3f(-self.unitsize,0,checkdepth)
        glVertex3f(0,0,checkdepth)
        glVertex3f(0,-self.unitsize,checkdepth)
        glVertex3f(-self.unitsize,-self.unitsize,checkdepth)
        glEnd()
        glEndList()
    def paintGL(self):
        w = self.width()
        if w == 0:
            w = 1
        h = self.height()
        if h == 0:
            h = 1
        cursorselection = -1
        if self.mousepos != None:            
            cursorselection = self.selectedColor(self.mousepos.x(),self.mousepos.y())
        glViewport(0,0,w,h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,w,h,0,-3000,1000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        glShadeModel(GL_SMOOTH)
        nbcitem, nbritem = self.getNbColRow(w,h)
        colorlist = self.turtle.getColorList()
        nbcolors = len(colorlist)
        colindex = 0
        d = Discretizer()
        glr = GLRenderer(d)
        defmat = Material('default')
        selectionbegin = self.selectionbegin
        selectionend = self.selectionend
        if selectionend is None and not selectionbegin is None:
            selectionbegin,selectionend = min(cursorselection,selectionbegin),max(cursorselection,selectionbegin)
        for i in xrange(0,nbcitem):
            for j in xrange(0,nbritem):
                glPushMatrix()
                glTranslate(self.unitsize+2*self.unitsize*i,self.unitsize+2*self.unitsize*j,0)
                glCallList(self.checklist)
                glEnable(GL_LIGHTING)
                if colindex < nbcolors:
                        colorlist[colindex].apply(glr)
                else:
                        defmat.apply(glr)
                if colindex == cursorselection:
                    glScalef(1.3,1.3,1.3)
                elif ( not self.selectionbegin is None and 
                      ( selectionbegin <= colindex <= selectionend ) ):
                    glScalef(1.2,1.2,1.2)
                glCallList(self.spherelist)
                glPopMatrix()
                colindex += 1
    def getNbColRow(self,w,h):
        nbcol = (w / (2*self.unitsize)) +1
        nbrow, rest = divmod(h ,2*self.unitsize)
        if rest > self.unitsize:
            nbrow+=1
        return nbcol,nbrow
    def selectedColor(self,x,y):        
        w = self.width()
        h = self.height()
        nbcol,nbrow = self.getNbColRow(w,h)
        nbcolselection = x // (2*self.unitsize)
        nbrowselection = y // (2*self.unitsize)
        id = nbcolselection*nbrow + nbrowselection    
        return id
    def mouseDoubleClickEvent(self,event):        
        x,y = event.pos().x(),event.pos().y()
        self.edition(self.selectedColor(x,y))
    def mousePressEvent(self,event):        
      if event.button()  == Qt.LeftButton:
        x,y = event.pos().x(),event.pos().y()
        self.selectionbegin = self.selectedColor(x,y)
        self.selectionend = None
    def mouseReleaseEvent(self,event):    
      if event.button()  == Qt.LeftButton:
        x,y = event.pos().x(),event.pos().y()
        self.selectionend = self.selectedColor(x,y)
        if self.selectionbegin == self.selectionend:
            self.showMessage("Click on color "+str(self.selectionbegin),2000)        
            self.selectionbegin == None
            self.selectionend == None
        else:
            self.selectionbegin, self.selectionend = min(self.selectionbegin, self.selectionend),max(self.selectionbegin, self.selectionend)
            self.showMessage("Selected colors from "+str(self.selectionbegin)+' to '+str(self.selectionend),2000)        
    def mouseMoveEvent(self,event):        
        self.mousepos = event.pos()
        cursorselection = self.selectedColor(self.mousepos.x(),self.mousepos.y())
        self.showMessage("Mouse on color "+str(cursorselection),2000) 
        self.updateGL()
    def leaveEvent(self,event):
        self.mousepos = None
        self.setMouseTracking(False)
        self.updateGL()
        QGLWidget.leaveEvent(self,event)
    def enterEvent(self,event):
        self.mousepos = None
        self.setMouseTracking(True)
        QGLWidget.enterEvent(self,event)
    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
    def edition(self,id):
        self.showMessage("Edit color = "+str(id),2000)
        if id >= 0:
            color = self.turtle.getMaterial(id)
            try:
                res = editMaterialInDialog(color)
                if res is None or res == QDialog.Accepted:
                    self.emit(SIGNAL('valueChanged()'))
            except Exception, e:
                print e
                print 'editMaterialInDialog not supported by your version of PlantGL'        
    def contextMenuEvent(self,event):
        self.menuselection = self.selectedColor(event.x(),event.y())
        menu = QMenu("Color Edit",self)
        menu.addAction("Copy",self.copymaterial)        
        menu.addAction("Cut",self.cutmaterial)        
        action = menu.addAction("Paste",self.pastematerial)
        if self.copyselection is None:
            action.setEnabled(False)
        menu.addAction("Remove",self.removematerial)
        action = menu.addAction("Interpolate",self.interpolatematerial)
        if ((self.selectionend is None) or abs(self.selectionend - self.selectionbegin) <3 or
            not ((self.selectionbegin <= self.menuselection <= self.selectionend) or 
                 (self.selectionend <= self.menuselection <= self.selectionbegin)) ) :
            action.setEnabled(False)
        menu.exec_(event.globalPos())
    def copymaterial(self):
        self.copyselection = self.menuselection
        self.cutaction = False
        self.menuselection = None
    def cutmaterial(self):
        self.copyselection = self.menuselection
        self.cutaction = True
        self.menuselection = None
    def pastematerial(self):
        if not self.copyselection is None:  
            color = self.turtle.getMaterial(self.copyselection)
            self.turtle.setMaterial(self.menuselection,Material(color))
            if self.cutaction == True:
                self.turtle.removeColor(self.copyselection)
            self.cutaction = None
            self.emit(SIGNAL('valueChanged()'))
        self.menuselection = None
    def removematerial(self):
        if not self.menuselection is None and len(self.turtle.getColorList()) > self.menuselection:
            if not self.selectionend is None :
                for i in xrange(self.selectionbegin,self.selectionend+1):
                    self.turtle.removeColor(self.selectionbegin)
                self.selectionbegin = None
                self.selectionend = None
            else:
                self.turtle.removeColor(self.menuselection)
            self.emit(SIGNAL('valueChanged()'))
    def interpolatematerial(self):
        if not self.selectionend is None :
            beg = self.selectionbegin
            end = self.selectionend
            self.showMessage("Interpolate colors from "+str(beg)+" to "+str(end),2000)
            deltaselection = end - beg
            ratio = 1/float(deltaselection)
            fmat = self.turtle.getMaterial(beg)
            lmat = self.turtle.getMaterial(end)
            iratio = 0
            for i in xrange(beg+1,end):
                iratio += ratio
                self.turtle.setMaterial(i,Material(interpolColor(fmat.ambient,lmat.ambient,iratio),
                                                   fmat.diffuse * (1-iratio)+lmat.diffuse*iratio,
                                                   interpolColor(fmat.specular,lmat.specular,iratio),
                                                   interpolColor(fmat.emission,lmat.emission,iratio),
                                                   fmat.shininess * (1-iratio)+lmat.shininess*iratio,
                                                   fmat.transparency * (1-iratio)+lmat.transparency*iratio))
            self.selectionbegin,self.selectionend = None,None
            self.emit(SIGNAL('valueChanged()'))

if __name__ == '__main__':
    qapp = QApplication([])
    m = MaterialEditor(None)
    m.show()
    qapp.exec_()
    
