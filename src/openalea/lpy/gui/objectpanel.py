from openalea.plantgl.all import *
#from objectdialog import *
from openalea.vpltk import qt
try:
    from PyQt4 import QtOpenGL
except:
    from PySide import QtOpenGL    
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
    name = str(name)
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

class ManagerDialogContainer (qt.QtCore.QObject):
    def __init__(self,panel,manager):
        qt.QtCore.QObject.__init__(self)
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
            if not self.editor: return
            self.editorDialog.setupUi(self.editor)
            self.editorDialog.setWindowTitle(self.manager.typename+' Editor')
            self.manager.fillEditorMenu(self.editorDialog.menu(),self.editor)
            qt.QtCore.QObject.connect(self.editorDialog,qt.QtCore.SIGNAL("valueChanged()"),self.__transmit_valueChanged__)
            qt.QtCore.QObject.connect(self.editorDialog,qt.QtCore.SIGNAL('hidden()'),self.endEditionEvent)
            qt.QtCore.QObject.connect(self.editorDialog,qt.QtCore.SIGNAL('AutomaticUpdate(bool)'),self.__transmit_autoUpdate__)
            
    def startObjectEdition(self,obj,id):
        """ used by panel. ask for object edition to start. Use getEditor and  setObjectToEditor """
        self.editedobjectid = id
        if not self.editor:
            self.init()
            if not self.editor:
                qt.QtGui.QMessageBox.warning(self,"Cannot edit","Cannot edit curve ! Python module (PyQGLViewer) is missing!")
                return
        self.manager.setObjectToEditor(self.editor,obj)
        try:
            self.editor.updateGL()
        except:
            pass
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


class ObjectListDisplay(QtOpenGL.QGLWidget): 
    """ Display and edit a list of parameter objects """
    class Theme:
        def __init__(self):
            self.values = {}
            
    BLACK_THEME = {'backGroundColor' : (0,0,0),
                      'waveColor' : (25,25,25),
                      'inactiveBackGroundColor' : (102,102,102),
                      'inactiveWaveColor' : (115,115,115),
                      'topText' : (255,255,255),
                      'selectedTopText' : (255,255,255),
                      'bottomText' : (255,255,0),
                      'selectedBottomText' : (255,255,255),
                      'inactiveText' : (204,204,204),
                      'thumbnailBackGround' : (64,64,64),
                      'thumbnailBackGround2' : (140,140,140),
                      'inactiveThumbnailBackGround' : (115,115,115),
                      'selectedThumbnailBackGround' : (102,102,102),
                      'selectedThumbnailBackGround2' : (179,179,179),
                      'thumbnailLine' : (180,180,180),
                      'thumbnailSelectedLine' : (255,255,255),
                      'thumbnailLineShadow' : (122,122,122)}
                      
    WHITE_THEME = {'backGroundColor' : (255,255,255),
                   'waveColor' : (255,255,255), #?(230,230,230),
                   'inactiveBackGroundColor' : (102,102,102),
                   'inactiveWaveColor' : (115,115,115),
                   'topText' : (0,0,0),
                   'selectedTopText' : (0,0,0),
                   'bottomText' : (200,0,0),
                   'selectedBottomText' : (255,0,0),
                   'inactiveText' : (204,204,204),
                   'thumbnailBackGround' : (250,250,250),
                   'thumbnailBackGround2' : (250,250,250),
                   'inactiveThumbnailBackGround' : (115,115,115),
                   'selectedThumbnailBackGround' : (200,200,200),
                   'selectedThumbnailBackGround2' : (200,200,200),
                   'thumbnailLine' : (60,60,60),
                   'thumbnailSelectedLine' : (0,0,0),
                   'thumbnailLineShadow' : (122,122,122),
                   'Curve2D' : (0,0,220), 'FocusCurve2D' : (0,0,255), }
    
    THEMES = { "Black" : BLACK_THEME, "White": WHITE_THEME }
    
    def __init__(self,parent, panelmanager = None):
        QtOpenGL.QGLWidget.__init__(self,parent)
        
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
        self.orientation = qt.QtCore.Qt.Vertical
        self.setMinimumHeight(self.thumbwidth*len(self.objects))
    
        # BackGround
        self.bgObject = None
        
        # OpenGL object
        self.discretizer = Discretizer()
        self.renderer = GLRenderer(self.discretizer)
        self.borderList = None
        self.selectedBorderList = None
        self.backGroundList = None

        self.createContextMenuActions()
        self.theme = self.Theme()
        self.setTheme(self.BLACK_THEME)

    def setTheme(self,theme):
        self.theme.values.update(theme)
        
        for name,value in self.theme.values.items():
            setattr(self.theme,name,[i/255. for i in value]+[0.5 if 'humbnailBackGround' in name else 1.0])
        
        for m in self.managers.values():
            m.setTheme(theme)
    
    def getTheme(self):
        from copy import deepcopy
        theme = deepcopy(self.theme.values)
        
        for m in self.managers.values():
            theme.update(m.getTheme())
    
    def applyTheme(self,theme):
        self.setTheme(theme)
        self.generateDisplayList()
        self.updateGL()
        
    def isActive(self):
        return self.active
        
    def setActive(self,enabled):
        if self.active != enabled:
            self.active = enabled
            self.generateDisplayList()
            if not self.active:
                self.setSelection(None)
            self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),-1)

    def transmit_autoUpdate(self,enabled):
        if enabled:
            self.emit(qt.QtCore.SIGNAL('AutomaticUpdate()'))
    

    def setOrientation(self,orientation):
        self.orientation  = orientation
        self.updateFrameView()


    def setSelection(self,selection):
        if self.selection != selection:
            self.selection = selection
            self.cursorselection = selection
            self.emit(qt.QtCore.SIGNAL('selectionChanged(int)'),selection if not selection is None else -1)
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
        self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),self.selection)
        
        

    def getBorderSize(self):
        if self.orientation == qt.QtCore.Qt.Vertical:
            decal = max( 0, (self.width() - self.thumbwidth) / 2)
        else:
            decal = max( 0, (self.height() - self.thumbwidth) / 2 )      
        ldecal = decal
        if ldecal > 15:
            ldecal = 15
        return decal,ldecal


    def updateFrameView(self):
        if self.orientation == qt.QtCore.Qt.Vertical:
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
            qt.QtGui.QApplication.clipboard().setText(name)

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
        self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),i)

    def renameSelection(self):
        """ rename an object in the list """
        if self.hasSelection() :
            self.emit(qt.QtCore.SIGNAL('renameRequest(int)'),self.selection)

    def resetSelection(self):
        """ reset an object in the list """
        if self.hasSelection() :
            manager,obj = self.getSelectedObject()
            name = manager.getName(obj)
            obj = manager.reset(obj)
            manager.setName(obj,name)
            self.objects[self.selection] = (manager,obj)
            self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),self.selection)
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
            self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),objectid)

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
            if self.orientation == qt.QtCore.Qt.Vertical:
                self.setOrientation(qt.QtCore.Qt.Horizontal)
            else:
                self.updateFrameView()
        else:
            self.thumbwidth = max(20,min(self.maxthumbwidth,w*0.95))
            self.objectthumbwidth = self.thumbwidth*0.7
            if self.orientation == qt.QtCore.Qt.Horizontal:
                self.setOrientation(qt.QtCore.Qt.Vertical)
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
        # activeGreyColor = 0.25
        # inactiveGreyColor = self.inactiveBgColor + 0.05
        # selectionGreyColor = 0.4
        # enhancementColor = 0.3
        if self.active:
           bc = self.theme.thumbnailBackGround
           ec = self.theme.thumbnailBackGround2
        else:
           bc = self.theme.inactiveThumbnailBackGround
           ec = bc
        #bc[3] = 0.5
        #ec[3] = 0.5
        glBegin(GL_QUADS)
        
        glColor4fv(bc)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glColor4fv(ec)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glEnd()
        glLineWidth(2)
        glColor4fv(self.theme.thumbnailLine)
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
        
        bc = self.theme.selectedThumbnailBackGround
        ec = self.theme.selectedThumbnailBackGround2
        #bc[3] = 0.5
        #ec[3] = 0.5
        # list for selected thumbnail
        if self.selectedBorderList is  None:
            self.selectedBorderList = glGenLists(1)
        glNewList(self.selectedBorderList,GL_COMPILE)
        glBegin(GL_QUADS)
        glColor4fv(bc)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glColor4fv(ec)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glEnd()
        
        glLineWidth(3)
        glColor4fv(self.theme.thumbnailLineShadow)
        glBegin(GL_LINE_STRIP)
        glVertex2f(0,0)
        glVertex2f(thumbwidth-1,0)
        glVertex2f(thumbwidth-1,thumbwidth-1)
        glVertex2f(0,thumbwidth-1)
        glVertex2f(0,0)
        glEnd()
        glLineWidth(1)
        glColor4fv(self.theme.thumbnailSelectedLine)
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
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
        self.drawBackGround(self.width(),self.height())
        glEndList()
        
    def computeBackGround(self,w,h):
        if self.orientation == qt.QtCore.Qt.Vertical:
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
            c = self.theme.waveColor
        else:
            c = self.theme.inactiveWaveColor
        if self.orientation == qt.QtCore.Qt.Vertical:
            glTranslatef(0,0,-10)
            glScalef(-1,1,1)
            glRotatef(90,0,0,1)
            nb = h/(self.bgwidth)
        else:
            glTranslatef(0,h,-10)
            glScalef(1,-1,1)
            nb = w/(self.bgwidth)
        for i in xrange(int(nb)+1):
            glColor4fv(c)
            self.bgObject.apply(self.renderer)
            glTranslatef(self.bgwidth,0,0)
        glPopMatrix()
        
    def paintGL(self):
        """ Paint the different object.
            First it traces the edges of the thumbnail outlines and the name of the object, 
            It also call the function 'displayThumbnail' to draw the thumbnail of the object 
            take into account the orientation of the panel (vertical or horizontal)"""
        if not self.isVisible(): return
        w = self.width()
        h = self.height()
        if w == 0 or h == 0: return
        if self.active:
            bgcol = self.theme.backGroundColor
        else:
            bgcol = self.theme.inactiveBackGroundColor
        glClearColor(*bgcol)
        try:
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        except: # the visible attribute is not correctly set on mac. The widget is not really visible and the initialization failed.
            return
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
            if self.orientation == qt.QtCore.Qt.Vertical:
                glTranslatef(b1,(i*self.thumbwidth)+b2,0)
            else:
                glTranslatef((i*self.thumbwidth)+b2,b1,0)
            
            if (not self.selectionPositionCurrent is None) and (not self.selectionPositionBegin is None) and (self.selection==i):
                decal = self.selectionPositionCurrent - self.selectionPositionBegin
                glTranslatef(decal.x(),decal.y(),0)
            else : decal = qt.QtCore.QPoint(0,0)

            if self.selection == i:
                glCallList(self.selectedBorderList)
            else:
                glCallList(self.borderList)
           
            glTranslatef(self.thumbwidth/2,self.thumbwidth/2,0)                  
            manager.displayThumbnail(obj,i,self.cursorselection==i,self.objectthumbwidth)
            
            glPopMatrix()
            if not self.active:
                glColor4fv(self.theme.inactiveText)
            else:
                if self.cursorselection == i:
                    glColor4fv(self.theme.selectedBottomText)
                else:
                    glColor4fv(self.theme.bottomText)
            if self.orientation == qt.QtCore.Qt.Vertical:
                tx,ty, ty2 = b1,(i*self.thumbwidth)+b2,((i-1)*self.thumbwidth)+b2+3
            else:
                tx,ty, ty2 = (i*self.thumbwidth)+b2,b1, b1-self.thumbwidth+3
            self.drawTextIn(manager.getName(obj),tx+decal.x(),ty+decal.y(),self.thumbwidth)
            if self.active:
                if self.cursorselection == i:
                    glColor4fv(self.theme.selectedTopText)
                else:
                    glColor4fv(self.theme.topText)
            self.drawTextIn(manager.typename,tx+decal.x(),ty2+decal.y(),self.thumbwidth,below = True)
            i+=1            


    def drawTextIn(self,text,x,y,width, below = False):
            fm = qt.QtGui.QFontMetrics(self.font())
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
            self.renderText(x+px,y+py,str(text))
            
    def itemUnderPos(self,pos):
        """function that will return the object under mouseCursor, if no object is present, this wil return None"""
        w = self.width() if self.orientation == qt.QtCore.Qt.Vertical else self.height()
        posx,posy = pos.x(), pos.y()
        b1,b2 = self.getBorderSize()
        if self.orientation == qt.QtCore.Qt.Horizontal:
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
        if event.button() == qt.QtCore.Qt.LeftButton:
            if self.active:
                self.selectionPositionBegin = event.pos()
                self.setSelection(self.itemUnderPos(event.pos()))
            event.accept()
        elif event.button() == qt.QtCore.Qt.RightButton:
            if self.active:
                self.setSelection(self.itemUnderPos(event.pos()))
            QtOpenGL.QGLWidget.mousePressEvent(self,event)
        else:
            QtOpenGL.QGLWidget.mousePressEvent(self,event)


    def mouseMoveEvent(self,event):
        """mouse move events, will check constantly if the cursor is placed on one of the Panel's objects"""
        if self.active:
            item = self.itemUnderPos(event.pos())
            self.setCursorSelection(item)
            if event.buttons() & qt.QtCore.Qt.LeftButton:
                self.selectionPositionCurrent = event.pos()
                if not item is None and not self.selection is None and item != self.selection:
                    self.objects[item],self.objects[self.selection] = self.objects[self.selection],self.objects[item]
                    if self.orientation == qt.QtCore.Qt.Vertical:
                        self.selectionPositionBegin -= qt.QtCore.QPoint(0,self.thumbwidth*(self.selection-item))
                    else:
                        self.selectionPositionBegin -= qt.QtCore.QPoint(self.thumbwidth*(self.selection-item),0)
                    self.updateGL()
                    self.selection = item 
            if not item is None:
                self.showMessage("Mouse on item "+str(item)+ " : '"+self.objects[item][0].getName(self.objects[item][1])+"'",2000)
                
    def mouseReleaseEvent(self,event):
        self.selectionPositionBegin = None
        self.selectionPositionCurrent = None
        QtOpenGL.QGLWidget.mouseReleaseEvent(self,event)
        self.updateGL()

    def mouseDoubleClickEvent(self,event):
        """ mouse double-click events, call editSelection() """
        if self.active and event.button() == qt.QtCore.Qt.LeftButton:
            self.setSelection(self.itemUnderPos(event.pos()))
            self.editSelection()
            event.accept()

    def createContextMenuActions(self):
        """ define the context menu """
        self.editAction = qt.QtGui.QAction('Edit',self)
        f = qt.QtGui.QFont()
        f.setBold(True)
        self.editAction.setFont(f)
        qt.QtCore.QObject.connect(self.editAction,qt.QtCore.SIGNAL('triggered(bool)'),self.editSelection)
        self.newItemMenu = qt.QtGui.QMenu("New item",self)
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
                
        self.copyAction = qt.QtGui.QAction('Copy',self)
        qt.QtCore.QObject.connect(self.copyAction,qt.QtCore.SIGNAL('triggered(bool)'),self.copySelection)
        self.cutAction = qt.QtGui.QAction('Cut',self)
        qt.QtCore.QObject.connect(self.cutAction,qt.QtCore.SIGNAL('triggered(bool)'),self.cutSelection)
        self.pasteAction = qt.QtGui.QAction('Paste',self)
        qt.QtCore.QObject.connect(self.pasteAction,qt.QtCore.SIGNAL('triggered(bool)'),self.paste)
        self.deleteAction = qt.QtGui.QAction('Delete',self)
        qt.QtCore.QObject.connect(self.deleteAction,qt.QtCore.SIGNAL('triggered(bool)'),self.deleteSelection)
        self.copyNameAction = qt.QtGui.QAction('Copy Name',self)
        qt.QtCore.QObject.connect(self.copyNameAction,qt.QtCore.SIGNAL('triggered(bool)'),self.copySelectionName)
        self.renameAction = qt.QtGui.QAction('Rename',self)
        qt.QtCore.QObject.connect(self.renameAction,qt.QtCore.SIGNAL('triggered(bool)'),self.renameSelection)
        self.resetAction = qt.QtGui.QAction('Reset',self)
        qt.QtCore.QObject.connect(self.resetAction,qt.QtCore.SIGNAL('triggered(bool)'),self.resetSelection)
        self.savePanelImageAction = qt.QtGui.QAction('Save Image',self)
        qt.QtCore.QObject.connect(self.savePanelImageAction,qt.QtCore.SIGNAL('triggered(bool)'),self.saveImage)
        

    def createContextMenu(self):
        """ define the context menu """
        contextmenu = qt.QtGui.QMenu(self)
        contextmenu.addAction(self.editAction)
        contextmenu.addSeparator()
        contextmenu.addMenu(self.newItemMenu)
        contextmenu.addSeparator()
        contextmenu.addAction(self.copyAction)
        contextmenu.addAction(self.cutAction)
        contextmenu.addAction(self.pasteAction)
        contextmenu.addSeparator()
        contextmenu.addAction(self.renameAction)
        contextmenu.addAction(self.copyNameAction)
        contextmenu.addSeparator()
        contextmenu.addAction(self.deleteAction)
        if self.hasSelection():
            contextmenu.addSeparator()
            itemmenu = contextmenu.addMenu('Transform')
            itemmenu.addAction(self.resetAction)
            manager,object = self.objects[self.selection]
            manager.completeContextMenu(itemmenu,object,self)
            if self.panelmanager :
                panels = self.panelmanager.getObjectPanels()
                if len(panels) > 1:
                    contextmenu.addSeparator()
                    sendToMenu = contextmenu.addMenu('Send To')
                    for panel in panels:
                        if not panel is self.dock:
                            sendToAction = qt.QtGui.QAction(panel.name,contextmenu)
                            qt.QtCore.QObject.connect(sendToAction,qt.QtCore.SIGNAL('triggered(bool)'),TriggerParamFunc(self.sendSelectionTo,panel.name))
                            sendToMenu.addAction(sendToAction)
                    sendToNewAction = qt.QtGui.QAction('New Panel',contextmenu)
                    qt.QtCore.QObject.connect(sendToNewAction,qt.QtCore.SIGNAL('triggered(bool)'),self.sendSelectionToNewPanel)
                    sendToMenu.addSeparator()
                    sendToMenu.addAction(sendToNewAction)                
        contextmenu.addSeparator()
        if self.panelmanager:
            panelmenu = self.panelmanager.completeMenu(contextmenu,self.dock)
            panelmenu.addSeparator()
            panelmenu.addAction(self.savePanelImageAction)
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
        self.pasteAction.setEnabled((not self.panelmanager is None) and self.panelmanager.hasClipboard() and self.active)
        self.renameAction.setEnabled(selcond)
        self.copyNameAction.setEnabled(selcond)
        self.deleteAction.setEnabled(selcond)
        self.resetAction.setEnabled(selcond)
        contextmenu.exec_(event.globalPos())


    def enterEvent(self,event):
        """ when entering the objectList, reactivate the mouseTracking"""
        self.setMouseTracking(True)
        QtOpenGL.QGLWidget.enterEvent(self,event)


    def leaveEvent(self,event):
        """ when the mouse is leaving the objectList,  disable the mouseTracking and set the cursorselection to None"""
        self.setMouseTracking(False)
        self.setCursorSelection(None)
        QtOpenGL.QGLWidget.leaveEvent(self,event)

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
        self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),nbelem)

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
        self.emit(qt.QtCore.SIGNAL('valueChanged(int)'),pos)
        self.setSelection(pos)
    
    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)

    def saveImage(self):
        fname = qt.QtGui.QFileDialog.getSaveFileName(self,'Save Image','.',';;'.join([str(i)+' (*.'+str(i)+')' for i in qt.QtGui.QImageWriter.supportedImageFormats()]))
        if fname:
            self.grabFrameBuffer(True).save(fname)
            self.showMessage('Save '+repr(fname),3000)

class LpyObjectPanelDock (qt.QtGui.QDockWidget):
    def __init__(self,parent,name,panelmanager = None):    
        qt.QtGui.QDockWidget.__init__(self,parent)
        self.panelmanager = panelmanager
        self.setObjectName(name.replace(' ','_'))
        self.setName(name)
        self.dockWidgetContents = qt.QtGui.QWidget()
        self.dockWidgetContents.setObjectName(name+"DockWidgetContents")
        self.verticalLayout = qt.QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName(name+"verticalLayout")
        
        self.objectpanel = qt.QtGui.QScrollArea(self.dockWidgetContents)
        self.view = ObjectListDisplay(self,panelmanager)
        self.view.dock = self
        self.objectpanel.setWidget(self.view)
        self.objectpanel.setWidgetResizable(True)
        self.objectpanel.setObjectName(name+"panelarea")
        
        self.verticalLayout.addWidget(self.objectpanel)
        self.objectNameEdit = qt.QtGui.QLineEdit(self.dockWidgetContents)
        self.objectNameEdit.setObjectName(name+"NameEdit")
        self.verticalLayout.addWidget(self.objectNameEdit)        
        self.objectNameEdit.hide()
        self.setWidget(self.dockWidgetContents)
        
        qt.QtCore.QObject.connect(self.view,qt.QtCore.SIGNAL('valueChanged(int)'),self.__updateStatus)
        qt.QtCore.QObject.connect(self.view,qt.QtCore.SIGNAL('AutomaticUpdate()'),self.__transmit_autoupdate)
        qt.QtCore.QObject.connect(self.view,qt.QtCore.SIGNAL('selectionChanged(int)'),self.endNameEditing)
        qt.QtCore.QObject.connect(self.view,qt.QtCore.SIGNAL('renameRequest(int)'),self.displayName)
        qt.QtCore.QObject.connect(self.objectNameEdit,qt.QtCore.SIGNAL('editingFinished()'),self.updateName)
        self.dockNameEdition = False
        self.nameEditorAutoHide = True
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
            if self.nameEditorAutoHide : 
                self.objectNameEdit.hide()
        else:
            if self.nameEditorAutoHide : 
                self.objectNameEdit.show()
            self.objectNameEdit.setText(self.view.getSelectedObjectName())
            self.objectNameEdit.setFocus()

    def updateName(self):
        if not self.dockNameEdition :
            if self.view.hasSelection():
                self.view.setSelectedObjectName(str(self.objectNameEdit.text()))
                self.view.updateGL()
                if self.nameEditorAutoHide : 
                    self.objectNameEdit.hide()
        else :
            self.setName(self.objectNameEdit.text())
            if self.nameEditorAutoHide : 
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
        if not i is None and i >= 0 and self.view.objects[i][0].managePrimitive():
            self.emit(qt.QtCore.SIGNAL('valueChanged(bool)'),True)
        else:
            self.emit(qt.QtCore.SIGNAL('valueChanged(bool)'),False)

    def __transmit_autoupdate(self):
        self.emit(qt.QtCore.SIGNAL('AutomaticUpdate()'))
        
    def setName(self,name):
        self.name = name
        self.setWindowTitle(name)
        
    def rename(self):
        self.dockNameEdition = True
        if self.nameEditorAutoHide : 
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

class ObjectPanelManager(qt.QtCore.QObject):
    def __init__(self,parent):
        qt.QtCore.QObject.__init__(self,parent)
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
                    qt.QtCore.QObject.connect(npanel, qt.QtCore.SIGNAL('valueChanged(bool)'),self.parent.projectEdited)
                    qt.QtCore.QObject.connect(npanel, qt.QtCore.SIGNAL('valueChanged(bool)'),self.parent.projectParameterEdited)
                    qt.QtCore.QObject.connect(npanel, qt.QtCore.SIGNAL('AutomaticUpdate()'),self.parent.projectAutoRun)
                    self.panels.append(npanel)
                    self.parent.addDockWidget(qt.QtCore.Qt.LeftDockWidgetArea,npanel)
                    self.vparameterView.addAction(npanel.toggleViewAction())
                    if new_visible:
                        npanel.show()
                    #self.restoreDockWidget(npanel)
    def completeMenu(self,menu,panel):
        panelmenu = qt.QtGui.QMenu("Panel",menu)
        menu.addSeparator()
        menu.addMenu(panelmenu)
        panelAction = qt.QtGui.QAction('Rename',panelmenu)
        qt.QtCore.QObject.connect(panelAction,qt.QtCore.SIGNAL('triggered(bool)'),panel.rename)
        panelmenu.addAction(panelAction)
        panelAction = qt.QtGui.QAction('Delete',panelmenu)
        qt.QtCore.QObject.connect(panelAction,qt.QtCore.SIGNAL('triggered(bool)'),TriggerParamFunc(self.deletePanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = qt.QtGui.QAction('New',panelmenu)
        qt.QtCore.QObject.connect(panelAction,qt.QtCore.SIGNAL('triggered(bool)'),TriggerParamFunc(self.createNewPanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = qt.QtGui.QAction('Duplicate',panelmenu)
        qt.QtCore.QObject.connect(panelAction,qt.QtCore.SIGNAL('triggered(bool)'),TriggerParamFunc(self.duplicatePanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = qt.QtGui.QAction('Disable' if panel.view.isActive() else 'Enable',panelmenu)
        qt.QtCore.QObject.connect(panelAction,qt.QtCore.SIGNAL('triggered(bool)'),TriggerParamFunc(panel.view.setActive,not panel.view.isActive()))
        panelmenu.addAction(panelAction)
        subpanelmenu = qt.QtGui.QMenu("Theme",menu)
        panelmenu.addSeparator()
        panelmenu.addMenu(subpanelmenu)
        for themename,value in ObjectListDisplay.THEMES.iteritems():
            panelAction = qt.QtGui.QAction(themename,subpanelmenu)
            
            qt.QtCore.QObject.connect(panelAction,qt.QtCore.SIGNAL('triggered(bool)'),TriggerParamFunc(panel.view.applyTheme,value))
            subpanelmenu.addAction(panelAction)
            
        return panelmenu
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

if __name__ == '__main__':
    qapp = qt.QtGui.QApplication([])
    m = LpyObjectPanelDock(None,'TestPanel')
    m.show()
    qapp.exec_()
