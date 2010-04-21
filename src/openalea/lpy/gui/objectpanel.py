from openalea.plantgl.all import *
#from objectdialog import *
from PyQt4.QtOpenGL import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from OpenGL.GL import *
from OpenGL.GLU import *
import sys, traceback

from objectmanagers import get_managers

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

class TriggerParamFunc:
    def __init__(self,func,value):
        self.func = func
        self.value= value
    def __call__(self):
        self.func(self.value)

        
from objectdialog import ObjectDialog

class ManagerDialogContainer (QObject):
    def __init__(self,panel,manager):
        QObject.__init__(self)
        self.panel = panel
        self.manager = manager
        self.editor   =  None
        self.editorDialog   =  None
        self.editedobjectid = None
    def __transmit_valueChanged__(self):
        self.panel.retrieveObject(self)
        #self.emit(SIGNAL("valueChanged()"))
        
    def __transmit_autoUpdate__(self,enabled):
        self.panel.transmit_autoUpdate(enabled)
        
    def init(self):
        if not self.editor:
            self.editorDialog = ObjectDialog(self.panel)
            self.editor = self.manager.getEditor(self.editorDialog)
            self.editorDialog.setupUi(self.editor)
            if not self.editor:
                return
            QObject.connect(self.editorDialog,SIGNAL("valueChanged()"),self.__transmit_valueChanged__)
            QObject.connect(self.editorDialog,SIGNAL('hidden()'),self.endEditionEvent)
            QObject.connect(self.editorDialog,SIGNAL('AutomaticUpdate(bool)'),self.__transmit_autoUpdate__)
        
    def startObjectEdition(self,obj,id):
        """ used by panel. ask for object edition to start. Use getEditor and  setObjectToEditor """
        self.editedobjectid = id
        if not self.editor:
            self.init()
            if not self.editor:
                QMessageBox.warning(self,"Cannot edit","Cannot edit curve ! Python module (PyQGLViewer) is missing!")
                return
        self.manager.setObjectToEditor(self.editor,obj)
        self.editor.updateGL()
        self.editorDialog.hasChanged = False
        self.editorDialog.show()

    def endObjectEdition(self):
        if self.editor:
            self.editorDialog.hide()
        
    def getEditedObject(self):
        """ used by panel. ask for object edition to start. Use getEditor and  setObjectToEditor """
        if self.editedobjectid is None:
            raise ValueError("No object is currently edited")
        return self.manager.retrieveObjectFromEditor(self.editor),self.editedobjectid

    def endEditionEvent(self):
        """ called when closing editor. """
        self.editedobjectid = None
        
    
class ObjectListDisplay(QGLWidget): 
    """ Display and edit a list of parameter objects """
    
    def __init__(self,parent):
        QGLWidget.__init__(self,parent)
        self.objects = [] # list of pair (manager,obj)
        self.maxthumbwidth = 150
        self.minthumbwidth = 20
        self.thumbwidth = 80
        self.objectthumbwidth = self.thumbwidth*0.9
        self.orientation = Qt.Vertical
        self.setMinimumHeight(self.thumbwidth*len(self.objects))
        self.selection = None
        self.editedobject = None
        self.cursorselection = None
        self.managers = {}
        self.managerDialogs = {}
        for typename, manager in get_managers().items():
            try:
                md = ManagerDialogContainer(self,manager)
                md.init()
                self.managers[typename] = manager
                self.managerDialogs[manager] = md
            except Exception,e:
                exc_info = sys.exc_info()
                traceback.print_exception(*exc_info)
                continue
            #QObject.connect(manager,SIGNAL('valueChanged()'),TriggerParamFunc(self.retrieveObject,manager))
            #QObject.connect(manager,SIGNAL('AutomaticUpdate(bool)'),self.__transmit_autoupdate)
        self.createContextMenu()

    def transmit_autoUpdate(self,enabled):
        if enabled:
            self.emit(SIGNAL('AutomaticUpdate()'))
        
    def createContextMenu(self):
        """ define the context menu """
        self.contextmenu = QMenu(self)
        self.editAction = QAction('Edit',self)
        f = QFont()
        f.setBold(True)
        self.editAction.setFont(f)
        QObject.connect(self.editAction,SIGNAL('triggered(bool)'),self.editSelection)
        self.contextmenu.addAction(self.editAction)
        self.contextmenu.addSeparator()
        subMenu=self.contextmenu.addMenu("New item")
        for k in self.managers.keys():
                subMenu.addAction(k,TriggerParamFunc(self.createDefaultObject,k) )
                #subMenu.addSeparator()
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
        """function setCursorSelection: update the cursorselection parameter of the objectpanel, if the mouse cursor is not placed over an object, it will be None"""
        self.cursorselection = selection            
        self.updateGL()     

   
    def hasSelection(self):
        """function hasSelection: check if an object of the objectListDisplay is selected, return True in this case"""
        return not self.selection is None


    def getSelectedObject(self):
        return self.objects[self.selection]
        
    def getSelectedObjectName(self):
        manager,object = self.objects[self.selection]
        return manager.getName(object)
        
    def setSelectedObjectName(self,name):
        manager,object = self.objects[self.selection]
        return manager.setName(object,name)
        

    def createDefaultObject(self,typename):
        """ adding a new object to the objectListDisplay, a new object will be created following a default rule defined in its manager"""
        manager = self.managers[typename]
        obj = manager.createDefaultObject()
        defname = typename
        mid = retrievemaxidname([manageri.getName(obji) for manageri,obji in self.objects],defname)        
        if not mid is None:
            manager.setName(obj,defname+'_'+str(mid+1))
        else:
            manager.setName(obj,defname)
        self.appendObject(manager,obj)


    def appendObject(self,typename_or_manager,obj):
        """ add the obj and its type to the panel's list """
        if type(typename_or_manager) == str:
            manager = self.managers[typename_or_manager]
        else:
            manager = typename_or_manager
        self.objects.append((manager,obj))
        ln = len(self.objects)
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
            self.setMinimumSize(self.minthumbwidth,(self.thumbwidth*len(self.objects))+b2)
        else:
            b1,b2 = self.getBorderSize()
            self.setMinimumSize(int((self.thumbwidth*len(self.objects))+b2),self.minthumbwidth)


    def copySelection(self):
        """ copy one of the panel's objects"""
        if self.hasSelection() :
            f = self.getSelectedObject()
            fc = f.deepcopy()
            bn = retrievebasename(f.name)
            mid = retrievemaxidname([i.name for i in self.objects],bn)
            if not mid is None:
                fc.name = bn+'_'+str(mid+1)
            self.appendObject(fc)


    def deleteSelection(self):
        """ delete an object from the list """
        if self.hasSelection() :
            self.objects.pop(self.selection)
            self.updateFrameView()
            self.emit(SIGNAL('valueChanged(int)'),self.selection)
            self.setSelection(None)


    def editSelection(self):
        """ Edit the current selection """
        if self.hasSelection() :
            manager,object = self.objects[self.selection]
            managerDialog = self.managerDialogs[manager]
            managerDialog.startObjectEdition(object,self.selection)

    def retrieveObject(self,managerDialog):
        """ Update the panel with edited object """
        object,objectid = managerDialog.getEditedObject()
        if not objectid is None:
            self.objects[objectid] = (managerDialog.manager,object)
            self.updateGL()
            self.emit(SIGNAL('valueChanged(int)'),self.selection)

        
    def init(self):
        """ OpenGL initialisation """
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_BLEND)


    def resizeGL(self,w,h):
        """resizeGL: function handling resizing events"""
        w,h = self.parent().width(),self.parent().height()
        if w > h+50 :
            self.thumbwidth = max(20,min(self.maxthumbwidth,h*0.95))
            self.objectthumbwidth = self.thumbwidth*0.9
            if self.orientation == Qt.Vertical:
                self.setOrientation(Qt.Horizontal)
            else:
                self.updateFrameView()
        else:
            self.thumbwidth = max(20,min(self.maxthumbwidth,w*0.95))
            self.objectthumbwidth = self.thumbwidth*0.9
            if self.orientation == Qt.Horizontal:
                self.setOrientation(Qt.Vertical)
            else:
                self.updateFrameView()


    def paintGL(self):
        """ Paint the different object.
            First it traces the edges of the thumbnail outlines and the name of the object, 
            It also call the function 'displayThumbnail' to draw the thumbnail of the object 
            take into account the orientation of the panel (vertical or horizontal)"""
        
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
        i=0
        b1,b2 = self.getBorderSize()
        for manager,obj in self.objects:
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
            glVertex2f(0,0)
            glVertex2f(self.thumbwidth-1,0)
            glVertex2f(self.thumbwidth-1,self.thumbwidth-1)           
            glVertex2f(0,self.thumbwidth-1)
            glVertex2f(0,0)
            glEnd()
           
            glTranslatef(self.thumbwidth/2,self.thumbwidth/2,0)  
            manager.displayThumbnail(obj,i,self.cursorselection==i,self.objectthumbwidth)
            
            glPopMatrix() 
            
            if self.cursorselection == i:
                glColor4f(1.0,1.0,1.0,1.0)            
            else:
                glColor4f(1.0,1.0,0.0,1.0)
            if self.orientation == Qt.Vertical:
                tx,ty, ty2 = b1,(i*self.thumbwidth)+b2,((i-1)*self.thumbwidth)+b2+3
            else:
                tx,ty, ty2 = (i*self.thumbwidth)+b2,b1, b1-self.thumbwidth+3
            self.drawTextIn(manager.getName(obj),tx,ty,self.thumbwidth)
            glColor4f(1,1,1,1.0)
            self.drawTextIn(manager.typename,tx,ty2,self.thumbwidth,below = True)
            i+=1

    def drawTextIn(self,text,x,y,width, below = False):
            fm = QFontMetrics(self.font())
            tw = fm.width(text)
            th = fm.height()
            mtw = width - 3
            mth = 20
            if below:
                y += th +1
            if mtw < tw:
                tratio = mtw / float(tw)
                lt = len(text)
                nbchar = int(lt * tratio) -3
                text = text[0:nbchar/2]+'...'+text[lt-nbchar/2:]
                tw = fm.width(text)
            px =  (mtw-tw) / 2
            py = width-1-fm.descent()
            if mth > th:
                py -= (mth-th)/2
            self.renderText(x+px,y+py,0,text)
            
    def itemUnderPos(self,pos):
        """function that will return the object under mouseCursor, if no object is present, this wil return None"""
        w = self.width() if self.orientation == Qt.Vertical else self.height()
        posx,posy = pos.x(), pos.y()
        b1,b2 = self.getBorderSize()
        if self.orientation == Qt.Horizontal:
            posx, posy = posy, posx
        if b1 <= posx <= b1 + self.thumbwidth:
          if posy > b2:
            sel = int((posy-b2)/self.thumbwidth)
            if sel < len(self.objects):
                return sel
            else : return None
        else : return None


    def mousePressEvent(self,event):
        """mousePressEvent: function handling mouse press events"""
        if event.button() == Qt.LeftButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            event.accept()
        elif event.button() == Qt.RightButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            QGLWidget.mousePressEvent(self,event)
        else:
            QGLWidget.mousePressEvent(self,event)


    def mouseMoveEvent(self,event):
        """mouse move events, will check constantly if the cursor is placed on one of the Panel's objects"""
        item = self.itemUnderPos(event.pos())
        self.setCursorSelection(item)
        if not item is None:
            self.showMessage("Mouse on item "+str(item)+ " : '"+self.objects[item][0].getName(self.objects[item][1])+"'",2000)


    def mouseDoubleClickEvent(self,event):
        """ mouse double-click events, call editSelection() """
        if event.button() == Qt.LeftButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            self.editSelection()
            event.accept()



    def contextMenuEvent(self, event):
        """ function defining actions to do according to the menu's button chosen"""
        self.editAction.setEnabled(not self.selection is None)
        self.copyAction.setEnabled(not self.selection is None)
        self.deleteAction.setEnabled(not self.selection is None)
        self.contextmenu.exec_(event.globalPos())


    def enterEvent(self,event):
        """ when entering the objectList, reactivate the mouseTracking"""
        self.setMouseTracking(True)
        QGLWidget.enterEvent(self,event)


    def leaveEvent(self,event):
        """ when the mouse is leaving the objectList,  disable the mouseTracking and set the cursorselection to None"""
        self.setMouseTracking(False)
        self.setCursorSelection(None)
        QGLWidget.leaveEvent(self,event)


    def setobjects(self,objects):
        """ 
            if we chose to change the entire objectListDisplay by another, 
            this function is called, sending the new objectListDisplay in parameter
        """
        self.objects = objects
        self.updateFrameView()
        self.setSelection(None)


    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)

class ObjectPanel(QScrollArea): 
    """Class object Panel, will contain and display thumbnails of the ObjectListDisplay"""

    def __init__(self,parent):
        QScrollArea.__init__(self,parent)
        self.view = ObjectListDisplay(self)
        self.name = None
        self.setWidget(self.view)
        self.setWidgetResizable(True)
        QObject.connect(self.view,SIGNAL('selectionChanged(int)'),self.displayName)
        QObject.connect(self.view,SIGNAL('valueChanged(int)'),self.__updateStatus)
        QObject.connect(self.view,SIGNAL('AutomaticUpdate()'),self.__transmit_autoupdate)
        self.setAcceptDrops(True)

    def setStatusBar(self,st):
        self.statusBar = st
        self.view.statusBar = st

    def __updateStatus(self,i=None):
        self.emit(SIGNAL('valueChanged()'))

    def __transmit_autoupdate(self):
        self.emit(SIGNAL('AutomaticUpdate()'))
    
    def setObjectNameEditor(self,objectNameEdit):
        self.objectNameEdit = objectNameEdit
        QObject.connect(self.objectNameEdit,SIGNAL('editingFinished()'),self.updateName)

    def displayName(self,id):
        if id == -1:
            self.objectNameEdit.clear()
        else:
            self.objectNameEdit.setText(self.view.getSelectedObjectName())

    def updateName(self):
        if not (self.view.selection is None or self.view.selection == -1):
            self.view.setSelectedObjectName(str(self.objectNameEdit.text()))
            self.view.updateGL()

    def setObjects(self,objects):
        self.view.setobjects(objects)

    def getObjects(self):
        return self.view.objects

    def dragEnterEvent(self,event):
        event.acceptProposedAction()

    def dropEvent(self,event):
        if event.mimeData().hasUrls() :
            self.fileDropEvent(str(event.mimeData().urls()[0].toLocalFile()))

    def fileDropEvent(self,fname):
        from lpfg_data_import import import_contours
        objects = import_contours(fname)
        if len(objects) > 0:
            manager = self.view.managers['Curve2D']
            self.setobjects(self.getObjects()+[(manager,i) for i in objects])
            self.__updateStatus()
            self.showMessage('import '+str(len(objects))+" object(s) from '"+fname+"'.",5000)

    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            seprint(msg)

