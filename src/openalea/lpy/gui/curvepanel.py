from openalea.plantgl.all import *
from curvedialog import CurveDialog
from PyQt4.QtOpenGL import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from OpenGL.GL import *
from OpenGL.GLU import *

def retrieveidinname(name,prefix):
    if name == prefix: return 1
    postfix = name[len(prefix):]
    if postfix[0] == '_':
        postfix = postfix[1:]
    try:
        return int(postfix)
    except:
        return None

def retrievemaxidname(names,prefix):
    previousids = [ retrieveidinname(name,prefix) for name in names if len(name) >= len(prefix) and prefix == name[0:len(prefix)]]
    mid = None
    if len(previousids) > 0:
        mid = max(previousids)
    return mid

def retrievebasename(name):
    lastindex = len(name)-1
    i = lastindex
    while i >= 0 and name[i].isdigit():
        i -= 1
    if i == lastindex or i <= 0:
        return name
    if name[i] == '_' and i >= 1:
        return name[:i]
    return name
    
class CurveListDisplay(QGLWidget): 
    def __init__(self,parent):
        QGLWidget.__init__(self,parent)
        self.curves = []
        self.maxthumbwidth = 150
        self.minthumbwidth = 20
        self.thumbwidth = 80
        self.curvethumbwidth = self.thumbwidth*0.9
        self.orientation = Qt.Vertical
        self.setMinimumHeight(self.thumbwidth*len(self.curves))
        self.discretizer = Discretizer()
        self.renderer = GLRenderer(self.discretizer)
        self.renderer.renderingMode = GLRenderer.Dynamic
        self.selection = None
        self.editedcurves = None
        self.cursorselection = None
        if not CurveDialog is None:
            self.editor = CurveDialog(self)            
            QObject.connect(self.editor,SIGNAL('valueChanged()'),self.retrieveCurve)
            QObject.connect(self.editor,SIGNAL('hidden()'),self.endEditionEvent)
        else:
            self.editor = None
        self.createContextMenu()
        self.sphere = Sphere(radius=3,slices=64,stacks=64)
    def setFunctionMode(self):
        if self.editor:
            self.editor.setFunctionMode()
    def createContextMenu(self):
        self.contextmenu = QMenu(self)
        self.editAction = QAction('Edit',self)
        f = QFont()
        f.setBold(True)
        self.editAction.setFont(f)
        QObject.connect(self.editAction,SIGNAL('triggered(bool)'),self.editSelection)
        self.contextmenu.addAction(self.editAction)
        self.contextmenu.addSeparator()
        self.contextmenu.addAction("New item",self.createNewDefaultCurve)
        self.contextmenu.addSeparator()
        self.copyAction = QAction('Copy',self)
        QObject.connect(self.copyAction,SIGNAL('triggered(bool)'),self.copySelection)
        self.contextmenu.addAction(self.copyAction)
        self.deleteAction = QAction('Delete',self)
        QObject.connect(self.deleteAction,SIGNAL('triggered(bool)'),self.deleteSelection)
        self.contextmenu.addAction(self.deleteAction)
    def setOrientation(self,orientation):
        self.orientation  = orientation
        self.updateFrameView()
    def setSelection(self,selection):
        if self.selection != selection:
            self.selection = selection
            self.cursorselection = selection
            self.emit(SIGNAL('selectionChanged(int)'),selection if not selection is None else -1)
        self.updateGL()
    def setCursorSelection(self,selection):
        self.cursorselection = selection            
        self.updateGL()        
    def hasSelection(self):
        return not self.selection is None
    def getSelectedCurve(self):
        return self.curves[self.selection]
    def createNewDefaultCurve(self):
        if self.editor:
            n = self.editor.newDefaultCurve()
        else:
            n = NurbsCurve2D([(0,0,1),(1/3.,0,1),(2/3.,0,1),(1,0,1)])
        defname = 'Default'
        mid = retrievemaxidname([i.name for i in self.curves],defname)
        if not mid is None:
            n.name = defname+'_'+str(mid+1)
        else:
            n.name = defname
        self.appendCurve(n)
    def appendCurve(self,curve):
        self.curves.append(curve)
        ln = len(self.curves)
        self.updateFrameView()
        self.emit(SIGNAL('valueChanged(int)'),ln-1)
        self.setSelection(ln-1)
    def getBorderSize(self):
        if self.orientation == Qt.Vertical:
            decal = max( 0, (self.width() - self.thumbwidth) / 2)
        else:
            decal = max( 0, (self.height() - self.thumbwidth) / 2 )      
        ldecal = decal
        if ldecal > 15:
            ldecal = 15
        return decal,ldecal
    def updateFrameView(self):
        if self.orientation == Qt.Vertical:
            b1,b2 = self.getBorderSize()
            self.setMinimumSize(self.minthumbwidth,(self.thumbwidth*len(self.curves))+b2)
        else:
            b1,b2 = self.getBorderSize()
            self.setMinimumSize(int((self.thumbwidth*len(self.curves))+b2),self.minthumbwidth)
    def copySelection(self):
        if self.hasSelection() :
            f = self.getSelectedCurve()
            fc = f.deepcopy()
            bn = retrievebasename(f.name)
            mid = retrievemaxidname([i.name for i in self.curves],bn)
            if not mid is None:
                fc.name = bn+'_'+str(mid+1)
            self.appendCurve(fc)
    def deleteSelection(self):
        if self.hasSelection() :
            self.curves.pop(self.selection)
            self.updateFrameView()
            self.emit(SIGNAL('valueChanged(int)'),self.selection)
            self.setSelection(None)
    def editSelection(self):
        if self.hasSelection() :
            self.editedcurve = self.selection
            if self.editor:
                self.editor.setCurve(self.getSelectedCurve().deepcopy())
                self.editor.show()
            else:
                QMessageBox.warning(self,"Cannot edit","Cannot edit curve ! Python module (PyQGLViewer) is missing!")
    def init(self):
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_BLEND)
    def resizeGL(self,w,h):
        w,h = self.parent().width(),self.parent().height()
        if w > h+50 :
            self.thumbwidth = max(20,min(self.maxthumbwidth,h*0.95))
            self.curvethumbwidth = self.thumbwidth*0.9
            if self.orientation == Qt.Vertical:
                self.setOrientation(Qt.Horizontal)
            else:
                self.updateFrameView()
        else:
            self.thumbwidth = max(20,min(self.maxthumbwidth,w*0.95))
            self.curvethumbwidth = self.thumbwidth*0.9
            if self.orientation == Qt.Horizontal:
                self.setOrientation(Qt.Vertical)
            else:
                self.updateFrameView()
    def paintGL(self):
        glClearColor(0.0,0.0,0.0,1.0)
        w = self.width()
        if w == 0:  w = 1
        h = self.height()
        if h == 0: h = 1
        glViewport(0,0,w,h)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,w,h,0,-1000,1000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity()
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        i = 0
        b1,b2 = self.getBorderSize()
        for curve in self.curves:
            glPushMatrix()
            if self.orientation == Qt.Vertical:
                glTranslatef(b1,(i*self.thumbwidth)+b2,0)
            else:
                glTranslatef((i*self.thumbwidth)+b2,b1,0)                
            if self.selection == i:
                glLineWidth(3)
                glColor4f(1.0,1.0,1.0,0.0)
            else:
                glLineWidth(1)
                glColor4f(0.9,0.9,0.9,0.0)
            glBegin(GL_LINE_STRIP)
            #glColor4f(0.9,0.0,0.0,0.0)
            glVertex2f(0,0)
            #glColor4f(0.9,0.9,0.9,0.0)

            glVertex2f(self.thumbwidth-1,0)
            glVertex2f(self.thumbwidth-1,self.thumbwidth-1)
           
            glVertex2f(0,self.thumbwidth-1)
            glVertex2f(0,0)
            glEnd()

            if self.cursorselection == i:
                glColor4f(1.0,0.0,0.0,1.0)
            else:
                glColor4f(1.0,1.0,1.0,1.0)            
            tname = curve.name
            fm = QFontMetrics(self.font())
            tw = fm.width(tname)
            th = fm.height()
            mtw = self.thumbwidth - 3
            mth = 20
            if mtw < tw:
                tratio = mtw / float(tw)
                lt = len(tname)
                nbchar = int(lt * tratio) -3
                tname = tname[0:nbchar/2]+'...'+tname[lt-nbchar/2:]
                tw = fm.width(tname)
            px =  (mtw-tw) / 2
            py = self.thumbwidth-1-fm.descent()
            if mth > th:
                py -= (mth-th)/2
            self.renderText(px,py,0,tname)
            
            glTranslatef(self.thumbwidth/2,(self.thumbwidth/2)-10,0)
            b = BoundingBox(curve)
            lsize = b.getSize()
            msize = lsize[lsize.getMaxAbsCoord()]
            scaling = self.curvethumbwidth/(2*msize)
            scaling = min(self.curvethumbwidth/(2*(lsize.x+1e-4)),(self.curvethumbwidth-20)/(2*(lsize.y+1e-4)))
            x0c = -b.getCenter()[0]*scaling
            y0c = -b.getCenter()[1]*scaling
            if 2*abs(y0c) <= self.curvethumbwidth:
                glColor4f(0.5,0.5,0.5,0)
                glLineWidth(1)
                glBegin(GL_LINE_STRIP)                
                glVertex2f(-self.curvethumbwidth/2.,-y0c)
                glVertex2f(self.curvethumbwidth/2.,-y0c)
                glEnd()                
            if 2*abs(x0c) <= self.curvethumbwidth:
                glColor4f(0.5,0.5,0.5,0)
                glLineWidth(1)
                glBegin(GL_LINE_STRIP)
                glVertex2f(x0c,-self.curvethumbwidth*0.4)
                glVertex2f(x0c,self.curvethumbwidth*0.4)
                glEnd()                
            glScalef(scaling,-scaling,1)
            glTranslatef(*-b.getCenter())
            pw = curve.width
            curve.width = 1
            if self.cursorselection == i:
                glPushMatrix()
                glTranslatef(0,0,-8)
                glLineWidth(6)
                glColor4f(0.5,0.5,0.5,0.5)
                curve.apply(self.renderer)
                glPopMatrix()
                glPushMatrix()
                glTranslatef(0,0,-4)
                glLineWidth(4)
                glColor4f(0.0,0.0,0.0,0.0)
                curve.apply(self.renderer)
                glPopMatrix()
            glColor4f(1,1,0.0,0.0)
            glLineWidth(2)
            curve.apply(self.renderer)
            curve.width = pw
            glPopMatrix()  
            i += 1
    def itemUnderPos(self,pos):
        w = self.width() if self.orientation == Qt.Vertical else self.height()
        posx,posy = pos.x(), pos.y()
        b1,b2 = self.getBorderSize()
        if self.orientation == Qt.Horizontal:
            posx, posy = posy, posx
        if b1 <= posx <= b1 + self.thumbwidth:
          if posy > b2:
            sel = int((posy-b2)/self.thumbwidth)
            if sel < len(self.curves):
                return sel
            else : return None
        else : return None
    def mousePressEvent(self,event):
        if event.button() == Qt.LeftButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            event.accept()
        elif event.button() == Qt.RightButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            QGLWidget.mousePressEvent(self,event)
        else:
            QGLWidget.mousePressEvent(self,event)
    def mouseMoveEvent(self,event):
        item = self.itemUnderPos(event.pos())
        self.setCursorSelection(item)
        if not item is None:
            self.showMessage("Mouse on item "+str(item)+ " : '"+self.curves[item].name+"'",2000)
    def mouseDoubleClickEvent(self,event):
        if event.button() == Qt.LeftButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            self.editSelection()
            event.accept()
    def retrieveCurve(self):
        curveid = self.editedcurve
        if not curveid is None:
            initialWidth = self.curves[curveid].width
            self.curves[curveid] = self.editor.getCurve()
            self.curves[curveid].width = initialWidth
            self.discretizer.clear()
            self.updateGL()
            self.emit(SIGNAL('valueChanged(int)'),self.selection)
    def endEditionEvent(self):
        self.editedcurve = None
    def contextMenuEvent(self, event):
        self.editAction.setEnabled(not self.selection is None)
        self.copyAction.setEnabled(not self.selection is None)
        self.deleteAction.setEnabled(not self.selection is None)
        self.contextmenu.exec_(event.globalPos())
    def enterEvent(self,event):
        self.setMouseTracking(True)
        QGLWidget.enterEvent(self,event)
    def leaveEvent(self,event):
        self.setMouseTracking(False)
        self.setCursorSelection(None)
        QGLWidget.leaveEvent(self,event)
    def setCurves(self,curves):
        self.curves = curves
        self.updateFrameView()
        self.setSelection(None)
    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)

class CurvePanel(QScrollArea): 
    def __init__(self,parent):
        QScrollArea.__init__(self,parent)
        self.view = CurveListDisplay(self)
        self.setWidget(self.view)
        self.setWidgetResizable(True)
        QObject.connect(self.view,SIGNAL('selectionChanged(int)'),self.displayName)
        QObject.connect(self.view,SIGNAL('valueChanged(int)'),self.__updateStatus)
        self.setAcceptDrops(True)
    def setStatusBar(self,st):
        self.statusBar = st
        self.view.statusBar = st
    def __updateStatus(self,i=None):
        self.emit(SIGNAL('valueChanged()'))
    def setCurveNameEditor(self,curveNameEdit):
        self.curveNameEdit = curveNameEdit
        QObject.connect(self.curveNameEdit,SIGNAL('editingFinished()'),self.updateName)
    def displayName(self,id):
        if id == -1:
            self.curveNameEdit.clear()
        else:
            self.curveNameEdit.setText(self.view.getSelectedCurve().name)
    def updateName(self):
        if not (self.view.selection is None or self.view.selection == -1):
            self.view.getSelectedCurve().name = str(self.curveNameEdit.text())
            self.view.updateGL()
    def setCurves(self,curves):
        self.view.setCurves(curves)
    def getCurves(self):
        return self.view.curves
    def dragEnterEvent(self,event):
        event.acceptProposedAction()
    def dropEvent(self,event):
        if event.mimeData().hasUrls() :
            self.fileDropEvent(str(event.mimeData().urls()[0].toLocalFile()))
    def fileDropEvent(self,fname):
        from lpfg_data_import import import_contours
        curves = import_contours(fname)
        if len(curves) > 0:
            self.setCurves(self.getCurves()+curves)
            self.__updateStatus()
            self.showMessage('import '+str(len(curves))+" curve(s) from '"+fname+"'.",5000)
    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)

class FunctionPanel(CurvePanel): 
    def __init__(self,parent):
        CurvePanel.__init__(self,parent)
        self.view.setFunctionMode()
    def setFunctions(self,curves):
        self.setCurves(curves)
    def getFunctions(self):
        return self.getCurves()
    def fileDropEvent(self,fname):
        pass
