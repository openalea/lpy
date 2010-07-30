from openalea.plantgl.all import *
#from objectdialog import *
from PyQt4.QtOpenGL import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from OpenGL.GL import *
from OpenGL.GLU import *
import sys, traceback, os
from math import sin, pi

from objectmanagers import get_managers

def retrieveidinname(name,prefix):
    if name == prefix: return 1
    postfix = name[len(prefix):]
    if postfix[0] in '_ ':
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
    def __init__(self,func,*value):
        self.func = func
        self.value= value
    def __call__(self):
        self.func(*self.value)

        
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
        
    def __transmit_autoUpdate__(self,enabled):
        self.panel.transmit_autoUpdate(enabled)
        
    def init(self):
        if not self.editor:
            self.editorDialog = ObjectDialog(self.panel)
            self.editor = self.manager.getEditor(self.editorDialog)
            self.editorDialog.setupUi(self.editor)
            self.editorDialog.setWindowTitle(self.manager.typename+' Editor')
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
        if not self.editedobjectid is None:
            return self.manager.retrieveObjectFromEditor(self.editor),self.editedobjectid

    def endEditionEvent(self):
        """ called when closing editor. """
        self.editedobjectid = None
    
    def isVisible(self):
        """ Tell whether editor is visible """
        return (not (self.editorDialog is None)) and self.editorDialog.isVisible()
    
class ObjectListDisplay(QGLWidget): 
    """ Display and edit a list of parameter objects """
    
    def __init__(self,parent, panelmanager = None):
        QGLWidget.__init__(self,parent)
        
        # global manager of all the panels
        self.panelmanager = panelmanager 
        
        # objects and their managers
        self.objects = [] # list of pair (manager,obj) 
        self.managers = {} # dict of managers of object
        self.managerDialogs = {} # dialog for editor corresponding to manager
        
        # loading managers
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
        
        # selection
        self.selection = None
        self.editedobject = None
        self.cursorselection = None
        self.selectionPositionBegin = None
        self.selectionPositionCurrent = None
        
        # Geometry
        self.active = True
        self.maxthumbwidth = 150
        self.minthumbwidth = 20
        self.thumbwidth = 80
        self.cornersize = 5
        self.objectthumbwidth = self.thumbwidth*0.9
        self.orientation = Qt.Vertical
        self.setMinimumHeight(self.thumbwidth*len(self.objects))
    
        # BackGround
        self.normalBgColor = 0          # in grey level
        self.inactiveBgColor = 0.4      # in grey level
        self.bgObject = None
        
        # OpenGL object
        self.discretizer = Discretizer()
        self.renderer = GLRenderer(self.discretizer)
        self.borderList = None
        self.selectedBorderList = None
        self.backGroundList = None

        self.createContextMenuActions()
        
    def isActive(self):
        return self.active
        
    def setActive(self,enabled):
        if self.active != enabled:
            self.active = enabled
            self.generateDisplayList()
            if not self.active:
                self.setSelection(None)
            self.emit(SIGNAL('valueChanged(int)'),-1)

    def transmit_autoUpdate(self,enabled):
        if enabled:
            self.emit(SIGNAL('AutomaticUpdate()'))
    

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
        self.setToolTip('' if selection is None else self.getCursorSelectionObjectName())
        self.updateGL()     

   
    def hasSelection(self):
        """function hasSelection: check if an object of the objectListDisplay is selected, return True in this case"""
        return not (self.selection is None or self.selection == -1)


    def getSelectedObject(self):
        return self.objects[self.selection]
        
    def getSelectedObjectName(self):
        manager,object = self.objects[self.selection]
        return manager.getName(object)
        
    def getCursorSelectionObjectName(self):
        manager,object = self.objects[self.cursorselection]
        return manager.getName(object)
        
    def setSelectedObjectName(self,name):
        manager,object = self.objects[self.selection]
        dialogmanager = self.managerDialogs[manager]
        edition = dialogmanager.getEditedObject()
        if edition:
            editedobj,objid = edition
            if objid == self.selection:
                manager.setName(editedobj,name)
                dialogmanager.startObjectEdition(editedobj,objid)
        manager.setName(object,name)
        self.emit(SIGNAL('valueChanged(int)'),self.selection)
        
        

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
        from copy import deepcopy
        if self.hasSelection() :
            f = self.getSelectedObject()
            fc = (f[0],deepcopy(f[1]))
            self.panelmanager.setClipboard(fc)

    def copySelectionName(self):
        """ copy the name of the panel selection"""
        
        from copy import deepcopy
        if self.hasSelection() :
            name = self.getSelectedObjectName()
            QApplication.clipboard().setText(name)

    def cutSelection(self):
        """ copy one of the panel's objects"""
        if self.hasSelection() :
            self.panelmanager.setClipboard(self.getSelectedObject())
            self.deleteObject(self.selection)
    
    def paste(self):
        if self.panelmanager.hasClipboard():
            manager,obj = self.panelmanager.getClipboard()
            manager.setName(obj,self.computeNewName(manager.getName(obj)))
            self.appendObject(manager,obj,self.selection)
        
    def computeNewName(self,basename):
        bn = retrievebasename(basename)
        allnames = []
        for panel in self.panelmanager.getObjectPanels():
            allnames += [ manager.getName(obj) for manager,obj in panel.getObjects() ]
        mid = retrievemaxidname(allnames,bn)
        if not mid is None:
            return bn+'_'+str(mid+1)
        return bn
        
    def deleteSelection(self):
        """ delete an object from the list """
        if self.hasSelection() :
            self.deleteObject(self.selection)
            self.setSelection(None)
            
    def deleteObject(self,i):
        """ delete an object from the list """
        self.objects.pop(i)
        self.updateFrameView()
        self.emit(SIGNAL('valueChanged(int)'),i)

    def renameSelection(self):
        """ rename an object in the list """
        if self.hasSelection() :
            self.emit(SIGNAL('renameRequest(int)'),self.selection)

    def resetSelection(self):
        """ reset an object in the list """
        if self.hasSelection() :
            manager,obj = self.getSelectedObject()
            name = manager.getName(obj)
            obj = manager.reset(obj)
            manager.setName(obj,name)
            self.objects[self.selection] = (manager,obj)
            self.emit(SIGNAL('valueChanged(int)'),self.selection)
            managerDialog = self.managerDialogs[manager]
            if managerDialog.isVisible() and managerDialog.getEditedObject()[1] == self.selection:
                self.editSelection()
            
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

    def sendSelectionTo(self,panelname):
        targetpanel = self.panelmanager.getPanel(panelname)
        if targetpanel and self.hasSelection():
            object = self.objects[self.selection]
            self.deleteSelection()
            targetpanel.appendObjects([object])        

    def sendSelectionToNewPanel(self):
        self.panelmanager.createNewPanel()
        self.sendSelectionTo(self.panelmanager.getObjectPanels()[-1].name)
    
    def resizeGL(self,w,h):
        """ resizing events"""
        w,h = self.parent().width(),self.parent().height()
        if w == 0 or h == 0: return
        if w > h+50 :
            self.thumbwidth = max(20,min(self.maxthumbwidth,h*0.95))
            self.objectthumbwidth = self.thumbwidth*0.7
            if self.orientation == Qt.Vertical:
                self.setOrientation(Qt.Horizontal)
            else:
                self.updateFrameView()
        else:
            self.thumbwidth = max(20,min(self.maxthumbwidth,w*0.95))
            self.objectthumbwidth = self.thumbwidth*0.7
            if self.orientation == Qt.Horizontal:
                self.setOrientation(Qt.Vertical)
            else:
                self.updateFrameView()
        self.computeBackGround(w,h)
        self.generateDisplayList()
    
    def generateDisplayList(self):
        if self.borderList is  None:
            self.borderList = glGenLists(1)
        cornersize = self.cornersize
        thumbwidth = self.thumbwidth
        
        # list for simple thumbnail
        glNewList(self.borderList,GL_COMPILE)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
        activeGreyColor = 0.25
        inactiveGreyColor = self.inactiveBgColor + 0.05
        selectionGreyColor = 0.4
        enhancementColor = 0.3
        if self.active:
           bc = activeGreyColor
           ec = bc + enhancementColor
        else:
           bc = inactiveGreyColor
           ec = bc
        glBegin(GL_QUADS)
        glColor4f(bc,bc,bc,0.5)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glColor4f(ec,ec,ec,0.5)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glEnd()
        glLineWidth(1)
        glColor4f(0.9,0.9,0.9,1.0)
        glBegin(GL_LINE_STRIP)
        glVertex2f(cornersize,0)
        glVertex2f(thumbwidth-cornersize-1,0)
        glVertex2f(thumbwidth-1,cornersize)
        glVertex2f(thumbwidth-1,thumbwidth-cornersize-1)           
        glVertex2f(thumbwidth-cornersize-1,thumbwidth-1)           
        glVertex2f(cornersize,thumbwidth-1)
        glVertex2f(0,thumbwidth-cornersize-1)
        glVertex2f(0,cornersize)
        glVertex2f(cornersize,0)
        glEnd()
        glEndList()
        
        # list for selected thumbnail
        if self.selectedBorderList is  None:
            self.selectedBorderList = glGenLists(1)
        glNewList(self.selectedBorderList,GL_COMPILE)
        glBegin(GL_QUADS)
        glColor4f(selectionGreyColor,selectionGreyColor,selectionGreyColor,0.5)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glColor4f(selectionGreyColor+enhancementColor,selectionGreyColor+enhancementColor,selectionGreyColor+enhancementColor,0.5)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glEnd()
        glLineWidth(3)
        glColor4f(0.5,0.5,0.5,1.0)
        glBegin(GL_LINE_STRIP)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glVertex2f(0,0)
        glEnd()
        glLineWidth(1)
        glColor4f(1.0,1.0,1.0,1.0)
        glBegin(GL_LINE_STRIP)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glVertex2f(0,0)
        glEnd()
        glEndList()

        if self.backGroundList is  None:
            self.backGroundList = glGenLists(1)
        glNewList(self.backGroundList,GL_COMPILE)
        self.drawBackGround(self.width(),self.height())
        glEndList()
        
    def computeBackGround(self,w,h):
        if self.orientation == Qt.Vertical:
            self.bgheigth = w * .3
        else:
            self.bgheigth = h * .3
        self.bgwidth = 300
        nbpoint = 50
        midheigth =  max(self.bgheigth-30,self.bgheigth*0.7)
        bottomheigth = max(self.bgheigth-60,self.bgheigth*0.4)
        def heigth(i,nbpoint,maxheigth=self.bgheigth,midheigth=midheigth,bottomheigth=bottomheigth):
            if i < nbpoint/2:
                return midheigth + ((maxheigth - midheigth)    *sin(pi*2*i/float(nbpoint-1)))
            else:
                return midheigth + ((midheigth - bottomheigth) *sin(pi*2*i/float(nbpoint-1)))
                
        points = ([ (self.bgwidth *i / float(nbpoint-1), heigth(i,nbpoint), 0) for i in xrange(nbpoint)]+
                  [ (self.bgwidth *i / float(nbpoint-1), 0, 0) for i in xrange(nbpoint)])
        indices = [ (i,i+1,nbpoint+i+1,nbpoint+i) for i in xrange(nbpoint-1)]
        self.bgObject = QuadSet(points,indices)
            
    def drawBackGround(self,w,h):
        glPushMatrix()
        if self.active:
            c = self.normalBgColor+0.1
        else:
            c = self.inactiveBgColor+0.05
        c2 = c+0.02
        if self.orientation == Qt.Vertical:
            glTranslatef(0,0,-10)
            glScalef(-1,1,1)
            glRotatef(90,0,0,1)
            nb = h/(self.bgwidth)
        else:
            glTranslatef(0,h,-10)
            glScalef(1,-1,1)
            nb = w/(self.bgwidth)
        for i in xrange(int(nb)+1):
            glColor4f(c,c,c,1.0)
            self.bgObject.apply(self.renderer)
            glColor4f(c2,c2,c2,1.0)
            glTranslatef(self.bgwidth,0,0)
        glPopMatrix()
        
    def paintGL(self):
        """ Paint the different object.
            First it traces the edges of the thumbnail outlines and the name of the object, 
            It also call the function 'displayThumbnail' to draw the thumbnail of the object 
            take into account the orientation of the panel (vertical or horizontal)"""
        w = self.width()
        h = self.height()
        if w == 0 or h == 0: return
        if self.active:
            glClearColor(self.normalBgColor,self.normalBgColor,self.normalBgColor,1.0)
        else:
            glClearColor(self.inactiveBgColor,self.inactiveBgColor,self.inactiveBgColor,1.0)
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
        glViewport(0,0,w,h)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,w,h,0,-1000,1000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity()
        glCallList(self.backGroundList)
        self.drawBackGround(w,h)
        i=0
        b1,b2 = self.getBorderSize()
        for manager,obj in self.objects:

            glPushMatrix()
            if self.orientation == Qt.Vertical:
                glTranslatef(b1,(i*self.thumbwidth)+b2,0)
            else:
                glTranslatef((i*self.thumbwidth)+b2,b1,0)
            
            if (not self.selectionPositionCurrent is None) and (not self.selectionPositionBegin is None) and (self.selection==i):
                decal = self.selectionPositionCurrent - self.selectionPositionBegin
                glTranslatef(decal.x(),decal.y(),0)
            else : decal = QPoint(0,0)

            if self.selection == i:
                glCallList(self.selectedBorderList)
            else:
                glCallList(self.borderList)
           
            glTranslatef(self.thumbwidth/2,self.thumbwidth/2,0)                  
            manager.displayThumbnail(obj,i,self.cursorselection==i,self.objectthumbwidth)
            
            glPopMatrix()
            if self.active:
                if self.cursorselection == i:
                    glColor4f(1.0,1.0,1.0,1.0)            
                else:
                    glColor4f(1.0,1.0,0.0,1.0)
            else:
                glColor4f(0.8,0.8,0.8,1.0)
            if self.orientation == Qt.Vertical:
                tx,ty, ty2 = b1,(i*self.thumbwidth)+b2,((i-1)*self.thumbwidth)+b2+3
            else:
                tx,ty, ty2 = (i*self.thumbwidth)+b2,b1, b1-self.thumbwidth+3
            self.drawTextIn(manager.getName(obj),tx+decal.x(),ty+decal.y(),self.thumbwidth)
            if self.active:
                glColor4f(1,1,1,1.0)
            self.drawTextIn(manager.typename,tx+decal.x(),ty2+decal.y(),self.thumbwidth,below = True)
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
            #glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
            self.renderText(x+px,y+py,QString(text))
            
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
            if self.active:
                self.selectionPositionBegin = event.pos()
                self.setSelection(self.itemUnderPos(event.pos()))
            event.accept()
        elif event.button() == Qt.RightButton:
            if self.active:
                self.setSelection(self.itemUnderPos(event.pos()))
            QGLWidget.mousePressEvent(self,event)
        else:
            QGLWidget.mousePressEvent(self,event)


    def mouseMoveEvent(self,event):
        """mouse move events, will check constantly if the cursor is placed on one of the Panel's objects"""
        if self.active:
            item = self.itemUnderPos(event.pos())
            self.setCursorSelection(item)
            if event.buttons() & Qt.LeftButton:
                self.selectionPositionCurrent = event.pos()
                if not item is None and not self.selection is None and item != self.selection:
                    self.objects[item],self.objects[self.selection] = self.objects[self.selection],self.objects[item]
                    if self.orientation == Qt.Vertical:
                        self.selectionPositionBegin -= QPoint(0,self.thumbwidth*(self.selection-item))
                        print self.thumbwidth*(self.selection-item),0
                    else:
                        self.selectionPositionBegin -= QPoint(self.thumbwidth*(self.selection-item),0)
                    self.updateGL()
                    self.selection = item 
            if not item is None:
                self.showMessage("Mouse on item "+str(item)+ " : '"+self.objects[item][0].getName(self.objects[item][1])+"'",2000)
                
    def mouseReleaseEvent(self,event):
        self.selectionPositionBegin = None
        self.selectionPositionCurrent = None
        QGLWidget.mouseReleaseEvent(self,event)
        self.updateGL()

    def mouseDoubleClickEvent(self,event):
        """ mouse double-click events, call editSelection() """
        if self.active and event.button() == Qt.LeftButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            self.editSelection()
            event.accept()

    def createContextMenuActions(self):
        """ define the context menu """
        self.editAction = QAction('Edit',self)
        f = QFont()
        f.setBold(True)
        self.editAction.setFont(f)
        QObject.connect(self.editAction,SIGNAL('triggered(bool)'),self.editSelection)
        self.newItemMenu = QMenu("New item",self)
        for mname, manager in self.managers.items():
            subtypes = manager.defaultObjectTypes()
            if not subtypes is None and len(subtypes) == 1:
                mname = subtypes[0]
                subtypes = None
            if subtypes is None:
                self.newItemMenu.addAction(mname,TriggerParamFunc(self.createDefaultObject,manager) )
            else:
                subtypeMenu = self.newItemMenu.addMenu(mname)
                for subtype in subtypes: 
                    subtypeMenu.addAction(subtype,TriggerParamFunc(self.createDefaultObject,manager,subtype) )
                
        self.copyAction = QAction('Copy',self)
        QObject.connect(self.copyAction,SIGNAL('triggered(bool)'),self.copySelection)
        self.cutAction = QAction('Cut',self)
        QObject.connect(self.cutAction,SIGNAL('triggered(bool)'),self.cutSelection)
        self.pasteAction = QAction('Paste',self)
        QObject.connect(self.pasteAction,SIGNAL('triggered(bool)'),self.paste)
        self.deleteAction = QAction('Delete',self)
        QObject.connect(self.deleteAction,SIGNAL('triggered(bool)'),self.deleteSelection)
        self.copyNameAction = QAction('Copy Name',self)
        QObject.connect(self.copyNameAction,SIGNAL('triggered(bool)'),self.copySelectionName)
        self.renameAction = QAction('Rename',self)
        QObject.connect(self.renameAction,SIGNAL('triggered(bool)'),self.renameSelection)
        self.resetAction = QAction('Reset',self)
        QObject.connect(self.resetAction,SIGNAL('triggered(bool)'),self.resetSelection)


    def createContextMenu(self):
        """ define the context menu """
        contextmenu = QMenu(self)
        contextmenu.addAction(self.editAction)
        contextmenu.addSeparator()
        contextmenu.addMenu(self.newItemMenu)
        contextmenu.addSeparator()
        contextmenu.addAction(self.copyAction)
        contextmenu.addAction(self.cutAction)
        contextmenu.addAction(self.pasteAction)
        contextmenu.addSeparator()
        contextmenu.addAction(self.resetAction)
        contextmenu.addSeparator()
        contextmenu.addAction(self.renameAction)
        contextmenu.addAction(self.copyNameAction)
        contextmenu.addSeparator()
        contextmenu.addAction(self.deleteAction)
        if self.hasSelection():
            manager,object = self.objects[self.selection]
            manager.completeContextMenu(contextmenu,object)
            if self.panelmanager :
                panels = self.panelmanager.getObjectPanels()
                if len(panels) > 1:
                    contextmenu.addSeparator()
                    sendToMenu = contextmenu.addMenu('Send To')
                    for panel in panels:
                        if not panel is self.dock:
                            sendToAction = QAction(panel.name,contextmenu)
                            QObject.connect(sendToAction,SIGNAL('triggered(bool)'),TriggerParamFunc(self.sendSelectionTo,panel.name))
                            sendToMenu.addAction(sendToAction)
                    sendToNewAction = QAction('New Panel',contextmenu)
                    QObject.connect(sendToNewAction,SIGNAL('triggered(bool)'),self.sendSelectionToNewPanel)
                    sendToMenu.addSeparator()
                    sendToMenu.addAction(sendToNewAction)                
        contextmenu.addSeparator()
        self.panelmanager.completeMenu(contextmenu,self.dock)
        #contextmenu.addAction(self.newPanelAction)
        return contextmenu
            

    def contextMenuEvent(self, event):
        """ function defining actions to do according to the menu's button chosen"""
        selcond = not self.selection is None
        contextmenu = self.createContextMenu()
        self.newItemMenu.setEnabled(self.active)
        self.editAction.setEnabled(selcond)
        self.copyAction.setEnabled(selcond)
        self.cutAction.setEnabled(selcond)
        self.pasteAction.setEnabled(self.panelmanager.hasClipboard() and self.active)
        self.renameAction.setEnabled(selcond)
        self.copyNameAction.setEnabled(selcond)
        self.deleteAction.setEnabled(selcond)
        self.resetAction.setEnabled(selcond)
        contextmenu.exec_(event.globalPos())


    def enterEvent(self,event):
        """ when entering the objectList, reactivate the mouseTracking"""
        self.setMouseTracking(True)
        QGLWidget.enterEvent(self,event)


    def leaveEvent(self,event):
        """ when the mouse is leaving the objectList,  disable the mouseTracking and set the cursorselection to None"""
        self.setMouseTracking(False)
        self.setCursorSelection(None)
        QGLWidget.leaveEvent(self,event)

    def getObjects(self):
        """ 
            Return the object list
        """
        return self.objects
    
    def getObjectsCopy(self):
        """ 
            Return the object list
        """
        from copy import deepcopy
        return [(m,deepcopy(o)) for m,o in self.objects]
    
    def setObjects(self,objects):
        """ 
            if we chose to change the entire objectListDisplay by another, 
            this function is called, sending the new objectListDisplay in parameter
        """
        self.objects = objects
        self.updateFrameView()
        self.setSelection(None)

    def appendObjects(self,objects):
        """ 
            if we chose to change the entire objectListDisplay by another, 
            this function is called, sending the new objectListDisplay in parameter
        """
        nbelem = len(self.objects)
        self.objects = self.objects+objects
        self.updateFrameView()
        self.setSelection(None)
        self.emit(SIGNAL('valueChanged(int)'),nbelem)

    def createDefaultObject(self,manager,subtype = None):
        """ adding a new object to the objectListDisplay, a new object will be created following a default rule defined in its manager"""
        obj = manager.createDefaultObject(subtype)
        manager.setName(obj,self.computeNewName('parameter'))
        self.appendObject(manager,obj,self.selection)
        self.renameSelection()


    def appendObject(self,typename_or_manager,obj, pos = None):
        """ add the obj and its type to the panel's list """
        if type(typename_or_manager) == str:
            manager = self.managers[typename_or_manager]
        else:
            manager = typename_or_manager
        if pos is None:
            self.objects.append((manager,obj))
            pos = len(self.objects) - 1
        else:
            self.objects.insert(pos,(manager,obj))
        self.updateFrameView()
        self.emit(SIGNAL('valueChanged(int)'),pos)
        self.setSelection(pos)
    
    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)



class LpyObjectPanelDock (QDockWidget):
    def __init__(self,parent,name,panelmanager = None):    
        QDockWidget.__init__(self,parent)
        self.panelmanager = panelmanager
        self.setObjectName(name.replace(' ','_'))
        self.setName(name)
        self.dockWidgetContents = QWidget()
        self.dockWidgetContents.setObjectName(name+"DockWidgetContents")
        self.verticalLayout = QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(name+"verticalLayout")
        
        self.objectpanel = QScrollArea(self.dockWidgetContents)
        self.view = ObjectListDisplay(self,panelmanager)
        self.view.dock = self
        self.objectpanel.setWidget(self.view)
        self.objectpanel.setWidgetResizable(True)
        self.objectpanel.setObjectName(name+"panelarea")
        
        self.verticalLayout.addWidget(self.objectpanel)
        self.objectNameEdit = QLineEdit(self.dockWidgetContents)
        self.objectNameEdit.setObjectName(name+"NameEdit")
        self.verticalLayout.addWidget(self.objectNameEdit)        
        self.objectNameEdit.hide()
        self.setWidget(self.dockWidgetContents)
        
        QObject.connect(self.view,SIGNAL('valueChanged(int)'),self.__updateStatus)
        QObject.connect(self.view,SIGNAL('AutomaticUpdate()'),self.__transmit_autoupdate)
        QObject.connect(self.view,SIGNAL('selectionChanged(int)'),self.endNameEditing)
        QObject.connect(self.view,SIGNAL('renameRequest(int)'),self.displayName)
        QObject.connect(self.objectNameEdit,SIGNAL('editingFinished()'),self.updateName)
        self.dockNameEdition = False
        self.setAcceptDrops(True)
    
    
    def dragEnterEvent(self,event):
        event.acceptProposedAction()

    def dropEvent(self,event):
        if event.mimeData().hasUrls() :
            self.fileDropEvent(str(event.mimeData().urls()[0].toLocalFile()))

    def fileDropEvent(self,fname):
        for manager in self.view.managers.itervalues():
            if manager.canImportData(fname):
                objects = manager.importData(fname)
                self.view.appendObjects([(manager,i) for i in objects])    
                self.showMessage('import '+str(len(objects))+" object(s) from '"+fname+"'.",5000)
                return

    def endNameEditing(self,id):
        if id != -1 and self.objectNameEdit.isVisible():
            self.displayName(-1)
    
    def displayName(self,id):
        if id == -1:
            self.objectNameEdit.clear()
            self.objectNameEdit.hide()
        else:
            self.objectNameEdit.show()
            self.objectNameEdit.setText(self.view.getSelectedObjectName())
            self.objectNameEdit.setFocus()

    def updateName(self):
        if not self.dockNameEdition :
            if self.view.hasSelection():
                self.view.setSelectedObjectName(str(self.objectNameEdit.text()))
                self.view.updateGL()
                self.objectNameEdit.hide()
        else :
            self.setName(self.objectNameEdit.text())
            self.objectNameEdit.hide()            
            self.dockNameEdition = False
        
    def setObjects(self,objects):
        self.view.setObjects(objects)

    def appendObjects(self,objects):
        self.view.appendObjects(objects)

    def getObjects(self):
        return self.view.objects

    def getObjectsCopy(self):
        return self.view.getObjectsCopy()

    def setStatusBar(self,st):
        self.objectpanel.statusBar = st
        self.view.statusBar = st

    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)    
    def __updateStatus(self,i=None):
        self.emit(SIGNAL('valueChanged()'))

    def __transmit_autoupdate(self):
        self.emit(SIGNAL('AutomaticUpdate()'))
        
    def setName(self,name):
        self.name = name
        self.setWindowTitle(name)
        
    def rename(self):
        self.dockNameEdition = True
        self.objectNameEdit.show()
        self.objectNameEdit.setText(self.name)
        self.objectNameEdit.setFocus()
    
    def getInfo(self):
        visibility = True
        if not self.isVisible() :
            if self.parent().isVisible() :
                visibility = False
            else:
                visibility = getattr(self,'previousVisibility',True)
        return {'name':str(self.name),'active':bool(self.view.isActive()),'visible':visibility }
        
    def setInfo(self,info):
        self.setName(info['name'])
        if info.has_key('active'):
            self.view.setActive(info['active'])        
        if info.has_key('visible'):
            self.previousVisibility = info['visible']
            self.setVisible(info['visible'])

class ObjectPanelManager(QObject):
    def __init__(self,parent):
        QObject.__init__(self,parent)
        self.parent = parent
        self.vparameterView = self.parent.vparameterView        
        self.panels  = []
        self.unusedpanels = []
        self.vparameterView.addAction("New Panel",self.createNewPanel)
        self.vparameterView.addSeparator()
        self.clipboard = None
    def setClipboard(self,obj):
        self.clipboard = obj
    def getClipboard(self):
        obj = self.clipboard
        self.clipboard = None
        return obj
    def hasClipboard(self):
        return not self.clipboard is None
    def getObjectPanels(self):
        return self.panels
    def getMaxObjectPanelNb(self):
        return len(self.panels)+len(self.unusedpanels)
    def setObjectPanelNb(self,nb, new_visible = True):
        nbpanel = len(self.panels)
        if nb < nbpanel:
            newunusedpanels = self.panels[nb:]
            self.unusedpanels = [(panel,panel.isVisible()) for panel in newunusedpanels]+self.unusedpanels
            self.panels = self.panels[:nb]
            for panel in newunusedpanels:
                panel.hide()
                self.vparameterView.removeAction(panel.toggleViewAction())
        else:
            nbtoadd = nb - nbpanel
            nbunusedpanels = len(self.unusedpanels)
            nbreused = min(nbtoadd,nbunusedpanels)
            if nbreused > 0:
                for i in xrange(nbreused):
                    npanel,visible = self.unusedpanels.pop(0)
                    self.panels.append(npanel)
                    if visible:
                        npanel.show()
                    self.vparameterView.addAction(npanel.toggleViewAction())
            if nbtoadd-nbunusedpanels > 0:
                for i in xrange(nbtoadd-nbunusedpanels):
                    npanel = LpyObjectPanelDock(self.parent,"Panel "+str(i+nbpanel+nbunusedpanels),self)
                    npanel.setStatusBar(self.parent.statusBar())
                    QObject.connect(npanel, SIGNAL('valueChanged()'),self.parent.projectEdited)
                    QObject.connect(npanel, SIGNAL('valueChanged()'),self.parent.projectParameterEdited)
                    QObject.connect(npanel, SIGNAL('AutomaticUpdate()'),self.parent.projectAutoRun)
                    self.panels.append(npanel)
                    self.parent.addDockWidget(Qt.LeftDockWidgetArea,npanel)
                    self.vparameterView.addAction(npanel.toggleViewAction())
                    if new_visible:
                        npanel.show()
                    #self.restoreDockWidget(npanel)
    def completeMenu(self,menu,panel):
        panelmenu = QMenu("Panel",menu)
        menu.addSeparator()
        menu.addMenu(panelmenu)
        panelAction = QAction('Rename',panelmenu)
        QObject.connect(panelAction,SIGNAL('triggered(bool)'),panel.rename)
        panelmenu.addAction(panelAction)
        panelAction = QAction('Delete',panelmenu)
        QObject.connect(panelAction,SIGNAL('triggered(bool)'),TriggerParamFunc(self.deletePanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = QAction('New',panelmenu)
        QObject.connect(panelAction,SIGNAL('triggered(bool)'),TriggerParamFunc(self.createNewPanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = QAction('Duplicate',panelmenu)
        QObject.connect(panelAction,SIGNAL('triggered(bool)'),TriggerParamFunc(self.duplicatePanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = QAction('Disable' if panel.view.isActive() else 'Enable',panelmenu)
        QObject.connect(panelAction,SIGNAL('triggered(bool)'),TriggerParamFunc(panel.view.setActive,not panel.view.isActive()))
        panelmenu.addAction(panelAction)
    def createNewPanel(self,above = None):
        nb = len(self.panels)+1
        self.setObjectPanelNb(nb)
        npanel = self.panels[-1]
        npanel.setObjects([])
        npanel.show()
        npanel.setName(self.computeNewPanelName('Panel'))
        if not above is None:
            self.parent.tabifyDockWidget(above,npanel)
    def duplicatePanel(self,source):
        nb = len(self.panels)+1
        self.setObjectPanelNb(nb)
        npanel = self.panels[-1]
        npanel.setObjects(source.getObjectsCopy())
        npanel.show()
        npanel.setName(self.computeNewPanelName(source.name))
        self.parent.tabifyDockWidget(source,npanel)
        source.setActive(False)
    def getPanel(self,panelname):
        for panel in self.panels:
            if panel.name == panelname:
                return panel
    def deletePanel(self,panel):
        self.panels.pop( self.panels.index(panel) )
        panel.hide()
        self.vparameterView.removeAction(panel.toggleViewAction())
        self.parent.projectEdited()
    def computeNewPanelName(self,basename):
        bn = retrievebasename(basename)
        mid = retrievemaxidname([panel.name for panel in self.panels],bn)
        if not mid is None:
            return bn+' '+str(mid+1)
        return bn
        