from openalea.plantgl.all import *
from openalea.plantgl.gui import qt
from OpenGL.GL import *
from OpenGL.GLU import *
import sys, traceback, os
from math import sin, pi

from .objectmanagers import get_managers

from openalea.plantgl.gui.qt.QtCore import QObject, QPoint, Qt, pyqtSignal, QT_VERSION_STR
from openalea.plantgl.gui.qt.QtGui import QFont, QFontMetrics, QImageWriter, QColor, QPainter
from openalea.plantgl.gui.qt.QtWidgets import QAction, QApplication, QDockWidget, QFileDialog, QLineEdit, QMenu, QMessageBox, QScrollArea, QVBoxLayout, QWidget


def renderText(self, x, y, text, font = QFont(), color = None):

    # Retrieve last OpenGL color to use as a font color
    if color is None:
        glColor = glGetDoublev(GL_CURRENT_COLOR)
        fontColor = QColor(glColor[0], glColor[1], glColor[2], glColor[3])
    else:
        fontColor = QColor(*color)

    # Render text
    self.painter.setPen(fontColor)
    self.painter.setFont(font)
    self.painter.drawText(x, y, text)

    pass

try:
    assert False
    from openalea.plantgl.gui.qt.QtGui import QOpenGLWidget  
    QGLParentClass = QOpenGLWidget 
    print('Use QOpenGLWidget')
    NewOpenGLClass = True

    QGLParentClass.mRenderText = renderText


    pass
except:
    from openalea.plantgl.gui.qt.QtOpenGL import QGLWidget 
    QGLParentClass = QGLWidget 
    NewOpenGLClass = False

    def mRenderText(self, x, y, text, font = QFont(), color = None):
        if not color is None: glColor4fv(color)
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
        self.renderText( x,y,text,font)

    QGLWidget.mRenderText = mRenderText


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
    previousids = [ retrieveidinname(name,prefix) for name in names if name.startswith(prefix)]
    previousids = [v for v in previousids if not v is None]
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

        
from .objectdialog import ObjectDialog

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
            if not self.editor: return
            self.editorDialog.setupUi(self.editor)
            self.editorDialog.setWindowTitle(self.manager.typename+' Editor')
            self.manager.fillEditorMenu(self.editorDialog.menu(),self.editor)
            self.editorDialog.valueChanged.connect(self.__transmit_valueChanged__)
            self.editorDialog.hidden.connect(self.endEditionEvent)
            self.editorDialog.AutomaticUpdate.connect(self.__transmit_autoUpdate__)
            
    def startObjectEdition(self,obj,id):
        """ used by panel. ask for object edition to start. Use getEditor and  setObjectToEditor """
        self.editedobjectid = id
        if not self.editor:
            self.init()
            if not self.editor:
                QMessageBox.warning(self.panel,"Cannot edit","Cannot edit object ! Python module (PyQGLViewer) is certainly missing!")
                return
        self.manager.setObjectToEditor(self.editor,obj)
        self.editorDialog.setWindowTitle(self.manager.typename+' Editor - '+self.manager.getName(obj))
        self.editorDialog.hasChanged = False
        self.editorDialog.show()
        self.editorDialog.activateWindow()
        self.editorDialog.raise_()


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


class ObjectListDisplay(QGLParentClass): 
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

    valueChanged = pyqtSignal(int)
    selectionChanged = pyqtSignal(int)
    AutomaticUpdate = pyqtSignal()
    renameRequest = pyqtSignal(int)
    
    def __init__(self,parent, panelmanager = None):
        QGLParentClass.__init__(self,parent)
        
        # global manager of all the panels
        self.panelmanager = panelmanager 
        
        # objects and their managers
        self.objects = [] # list of pair (manager,obj) 
        self.managers = {} # dict of managers of object
        self.managerDialogs = {} # dialog for editor corresponding to manager
        
        # loading managers
        for typename, manager in list(get_managers().items()):
            try:
                md = ManagerDialogContainer(self,manager)
                md.init()
                self.managers[typename] = manager
                self.managerDialogs[manager] = md
            except Exception as e:
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
        self.bgObject = None
        
        # OpenGL object
        self.discretizer = Discretizer()
        self.renderer = GLRenderer(self.discretizer)
        self.borderList = None
        self.selectedBorderList = None
        self.backGroundList = None

        self.with_translation = (int(QT_VERSION_STR.split('.')[1]) < 14)

        self.createContextMenuActions()
        self.theme = self.Theme()
        self.setTheme(self.BLACK_THEME)
        self._width, self._heigth = self.width(), self.height()
        self._scalingfactor = 1

        self._font = QFont(self.font())

    def doUpdate(self):
        if NewOpenGLClass:
            self.update()
        else:
            self.updateGL()

    def setTheme(self,theme):
        self.theme.values.update(theme)
        
        for name,value in list(self.theme.values.items()):
            setattr(self.theme,name,[i/255. for i in value]+[0.5 if 'humbnailBackGround' in name else 1.0])
        
        for m in list(self.managers.values()):
            m.setTheme(theme)
    
    def getTheme(self):
        from copy import deepcopy
        theme = deepcopy(self.theme.values)
        
        for m in list(self.managers.values()):
            theme.update(m.getTheme())
    
    def applyTheme(self,theme):
        self.setTheme(theme)
        self.generateDisplayList()
        self.doUpdate()
        
    def isActive(self):
        return self.active
        
    def setActive(self,enabled):
        if self.active != enabled:
            self.active = enabled
            self.generateDisplayList()
            if not self.active:
                self.setSelection(None)
            self.valueChanged.emit(-1)

    def transmit_autoUpdate(self,enabled):
        if enabled:
            self.AutomaticUpdate.emit()
    

    def setOrientation(self,orientation):
        self.orientation  = orientation
        self.updateFrameView()


    def setSelection(self,selection):
        if self.selection != selection:
            self.selection = selection
            self.cursorselection = selection
            self.selectionChanged.emit(selection if not selection is None else -1)
        self.doUpdate()


    def setCursorSelection(self,selection):
        """function setCursorSelection: update the cursorselection parameter of the objectpanel, if the mouse cursor is not placed over an object, it will be None"""
        self.cursorselection = selection
        self.setToolTip('' if selection is None else self.getCursorSelectionObjectName())
        self.doUpdate()     

   
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
        self.valueChanged.emit(self.selection)
        
        

    def getBorderSize(self):
        if self.orientation == Qt.Vertical:
            decal = max( 0, (self._width - self.thumbwidth) / 2)
        else:
            decal = max( 0, (self._height - self.thumbwidth) / 2 )      
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
        panels = self.panelmanager.getObjectPanels() if self.panelmanager else [self]
        for panel in panels:
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
        self.valueChanged.emit(i)

    def renameSelection(self):
        """ rename an object in the list """
        if self.hasSelection() :
            self.renameRequest.emit(self.selection)

    def resetSelection(self):
        """ reset an object in the list """
        if self.hasSelection() :
            manager,obj = self.getSelectedObject()
            name = manager.getName(obj)
            obj = manager.reset(obj)
            manager.setName(obj,name)
            self.objects[self.selection] = (manager,obj)
            self.valueChanged.emit(self.selection)
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
            self.doUpdate()
            self.valueChanged.emit(objectid)

    def sendSelectionTo(self,panelname):
        targetpanel = self.panelmanager.getPanel(panelname)
        if targetpanel and self.hasSelection():
            object = self.objects[self.selection]
            self.deleteSelection()
            targetpanel.appendObjects([object])        

    def sendSelectionToNewPanel(self):
        self.panelmanager.createNewPanel()
        self.sendSelectionTo(self.panelmanager.getObjectPanels()[-1].name)
    
    def resizeGL(self, w, h):
        """ resizing events"""
        ow, oh = w,h
        pw, ph = self.parent().width(),self.parent().height()
        self._width, self._height = w, h
        dpr = self.window().devicePixelRatio()
        self._scalingfactor = dpr # w/float(pw), h/float(ph)

        if w == 0 or h == 0: return
        if pw > ph+50 :
            scalingfactor = dpr # h/float(ph)
            self.thumbwidth = max(self.minthumbwidth*scalingfactor, min(self.maxthumbwidth*scalingfactor, h*0.95))
            self.objectthumbwidth = self.thumbwidth*0.7
            if self.orientation == Qt.Vertical:
                self.setOrientation(Qt.Horizontal)
            else:
                self.updateFrameView()
        else:
            scalingfactor = dpr # w/float(pw)
            self.thumbwidth = max(self.minthumbwidth*scalingfactor, min(self.maxthumbwidth*scalingfactor, w*0.95))
            self.objectthumbwidth = self.thumbwidth*0.7
            if self.orientation == Qt.Horizontal:
                self.setOrientation(Qt.Vertical)
            else:
                self.updateFrameView()
        self.computeBackGround(w,h)
        self.generateDisplayList()
    
    def generateDisplayList(self):
        try:
            fakeList = glGenLists(1)
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
            if self.isVisible():
                self.drawBackGround(self._width,self._height)
            glEndList()
        except:
            self.borderList = None
            self.selectedBorderList = None
            self.backGroundList = None
        
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
                
        points = ([ (self.bgwidth *i / float(nbpoint-1), heigth(i,nbpoint), 0) for i in range(nbpoint)]+
                  [ (self.bgwidth *i / float(nbpoint-1), 0, 0) for i in range(nbpoint)])
        indices = [ (i,i+1,nbpoint+i+1,nbpoint+i) for i in range(nbpoint-1)]
        self.bgObject = QuadSet(points,indices)
            
    def drawBackGround(self,w,h):
        if self.bgObject is None: self.computeBackGround(w,h)
        glPushMatrix()
        if self.active:
            c = self.theme.waveColor
        else:
            c = self.theme.inactiveWaveColor
        if self.orientation == Qt.Vertical:
            glTranslatef(0,0,-10)
            glScalef(-1,1,1)
            glRotatef(90,0,0,1)
            nb = h/(self.bgwidth)
        else:
            glTranslatef(0,h,-10)
            glScalef(1,-1,1)
            nb = w/(self.bgwidth)
        for i in range(int(nb)+1):
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
        w = self._width
        h = self._height
        if w == 0 or h == 0: return
        if NewOpenGLClass:
            self.painter = QPainter(self)
            self.painter.setRenderHints(QPainter.Antialiasing | QPainter.TextAntialiasing)

        if self.active:
            bgcol = self.theme.backGroundColor
        else:
            bgcol = self.theme.inactiveBackGroundColor
        try:
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
            #glTranslatef(w*(self._scalingfactor-1)*0.5,-h*(self._scalingfactor-1),0)
            #glScalef(self._scalingfactor,self._scalingfactor,1)

            vscroll, hscroll = 0, 0
            if self.with_translation:
                hscroll = self.scroll.horizontalScrollBar().value()
                vscroll = self.scroll.verticalScrollBar().value()
                glTranslatef(-hscroll,-vscroll,0)

            if not self.backGroundList is None:
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

                if not self.borderList is None:
                    if self.selection == i:
                        glCallList(self.selectedBorderList)
                    else:
                        glCallList(self.borderList)
               
                glTranslatef(self.thumbwidth/2,self.thumbwidth/2,0)                  
                manager.displayThumbnail(obj,i,self.cursorselection==i,self.objectthumbwidth)
                
                glPopMatrix()

                if not self.active:
                    txtColor = self.theme.inactiveText
                else:
                    if self.cursorselection == i:
                        txtColor =  self.theme.selectedBottomText
                    else:
                        txtColor = self.theme.bottomText
                if self.orientation == Qt.Vertical:
                    tx,ty, ty2 = b1,(i*self.thumbwidth)+b2,((i-1)*self.thumbwidth)+b2+3
                else:
                    tx,ty, ty2 = (i*self.thumbwidth)+b2,b1, b1-self.thumbwidth+3
                self.drawTextIn(manager.getName(obj),tx+decal.x()-hscroll,ty+decal.y()-vscroll,self.thumbwidth, color = txtColor)

                if self.active:
                    if self.cursorselection == i:
                        txtColor = self.theme.selectedTopText
                    else:
                        txtColor = self.theme.topText
                        pass
                self.drawTextIn(manager.typename,tx+decal.x()-hscroll,ty2+decal.y()-vscroll,self.thumbwidth, below = True, color = txtColor)
                i+=1 
        except Exception as e:
            exc_info = sys.exc_info()
            traceback.print_exception(*exc_info)
        if NewOpenGLClass:
            self.painter.end()

    def drawTextIn(self, text, x, y, width, below = False, color = None):
            fm = QFontMetrics(self._font)
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
                text = text[0:int(nbchar/2)]+'...'+text[int(lt-nbchar/2):]
                tw = fm.width(text)
            px =  (mtw-tw) / 2
            py = width-1-fm.descent()
            if mth > th:
                py -= (mth-th)/2
            #glPolygonMode(GL_FRONT_AND_BACK,GL_FILL)
            #if not color is None: glColor4fv(color)
            self.mRenderText((x+px)/self._scalingfactor, (y+py)/self._scalingfactor, str(text), self._font, color = color)
            return 
            
    def itemUnderPos(self,pos):
        """function that will return the object under mouseCursor, if no object is present, this wil return None"""
        w = self.width() if self.orientation == Qt.Vertical else self.height()
        posx, posy = pos.x()*self._scalingfactor, pos.y()*self._scalingfactor
        #if self.with_translation:
        #    hscroll = self.scroll.horizontalScrollBar().value()
        #    vscroll = self.scroll.verticalScrollBar().value()
        #    posx -= hscroll
        #    posy -= vscroll

        b1, b2 = self.getBorderSize()
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
            QGLParentClass.mousePressEvent(self,event)
        else:
            QGLParentClass.mousePressEvent(self,event)


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
                    else:
                        self.selectionPositionBegin -= QPoint(self.thumbwidth*(self.selection-item),0)
                    self.doUpdate()
                    self.selection = item 
            if not item is None:
                self.showMessage("Mouse on item "+str(item)+ " : '"+self.objects[item][0].getName(self.objects[item][1])+"'",2000)
                
    def mouseReleaseEvent(self,event):
        self.selectionPositionBegin = None
        self.selectionPositionCurrent = None
        QGLParentClass.mouseReleaseEvent(self,event)
        self.doUpdate()

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
        self.editAction.triggered.connect(self.editSelection)
        self.newItemMenu = QMenu("New item",self)
        for mname, manager in sorted(list(self.managers.items())):
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
        self.copyAction.triggered.connect(self.copySelection)
        self.cutAction = QAction('Cut',self)
        self.cutAction.triggered.connect(self.cutSelection)
        self.pasteAction = QAction('Paste',self)
        self.pasteAction.triggered.connect(self.paste)
        self.deleteAction = QAction('Delete',self)
        self.deleteAction.triggered.connect(self.deleteSelection)
        self.copyNameAction = QAction('Copy Name',self)
        self.copyNameAction.triggered.connect(self.copySelectionName)
        self.renameAction = QAction('Rename',self)
        self.renameAction.triggered.connect(self.renameSelection)
        self.resetAction = QAction('Reset',self)
        self.resetAction.triggered.connect(self.resetSelection)
        self.savePanelImageAction = QAction('Save Image',self)
        self.savePanelImageAction.triggered.connect(self.saveImage)
        

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
                            sendToAction = QAction(panel.name,contextmenu)
                            sendToAction.triggered.connect(TriggerParamFunc(self.sendSelectionTo,panel.name))
                            sendToMenu.addAction(sendToAction)
                    sendToNewAction = QAction('New Panel',contextmenu)
                    sendToNewAction.triggered.connect(self.sendSelectionToNewPanel)
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
        QGLParentClass.enterEvent(self,event)


    def leaveEvent(self,event):
        """ when the mouse is leaving the objectList,  disable the mouseTracking and set the cursorselection to None"""
        self.setMouseTracking(False)
        self.setCursorSelection(None)
        QGLParentClass.leaveEvent(self,event)

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
        self.valueChanged.emit(nbelem)

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
        self.valueChanged.emit(pos)
        self.setSelection(pos)
    
    def showMessage(self,msg,timeout):
        if hasattr(self,'statusBar'):
            self.statusBar.showMessage(msg,timeout)
        else:
            print(msg)

    def saveImage(self):
        fname = QFileDialog.getSaveFileName(self,'Save Image','.',';;'.join([str(i.data(), encoding='utf-8').upper()+' files (*.'+str(i.data(), encoding='utf-8')+')' for i in QImageWriter.supportedImageFormats()]))
        if fname:
            self.grabFrameBuffer(True).save(fname[0])
            self.showMessage('Save '+repr(fname[0]),3000)

class LpyObjectPanelDock (QDockWidget):
    valueChanged = pyqtSignal(bool)
    AutomaticUpdate = pyqtSignal()

    def __init__(self,parent,name,panelmanager = None):    
        QDockWidget.__init__(self,parent)
        self.panelmanager = panelmanager
        self.setObjectName(name.replace(' ','_'))
        self.setName(name)
        self.dockWidgetContents = QWidget()
        self.dockWidgetContents.setObjectName(name+"DockWidgetContents")
        self.verticalLayout = QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName(name+"verticalLayout")
        
        self.objectpanel = QScrollArea(self.dockWidgetContents)
        self.view = ObjectListDisplay(self,panelmanager)
        self.view.dock = self
        self.view.scroll = self.objectpanel
        self.objectpanel.setWidget(self.view)
        self.objectpanel.setWidgetResizable(True)
        self.objectpanel.setObjectName(name+"panelarea")
        
        self.verticalLayout.addWidget(self.objectpanel)
        self.objectNameEdit = QLineEdit(self.dockWidgetContents)
        self.objectNameEdit.setObjectName(name+"NameEdit")
        self.verticalLayout.addWidget(self.objectNameEdit)        
        self.objectNameEdit.hide()
        self.setWidget(self.dockWidgetContents)
        
        self.view.valueChanged.connect(self.__updateStatus)
        self.view.AutomaticUpdate.connect(self.__transmit_autoupdate)
        self.view.selectionChanged.connect(self.endNameEditing)
        self.view.renameRequest.connect(self.displayName)
        self.objectNameEdit.editingFinished.connect(self.updateName)
        self.dockNameEdition = False
        self.nameEditorAutoHide = True
        self.setAcceptDrops(True)
    
    def dragEnterEvent(self,event):
        event.acceptProposedAction()

    def dropEvent(self,event):
        if event.mimeData().hasUrls() :
            self.fileDropEvent(str(event.mimeData().urls()[0].toLocalFile()))

    def fileDropEvent(self,fname):
        for manager in self.view.managers.values():
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
                self.view.doUpdate()
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
        if not i is None and 0 <= i < len(self.view.objects) and self.view.objects[i][0].managePrimitive():
            self.valueChanged.emit(True)
        else:
            self.valueChanged.emit(False)

    def __transmit_autoupdate(self):
        self.AutomaticUpdate.emit()
        
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
        if 'active' in info:
            self.view.setActive(info['active'])        
        if 'visible' in info:
            self.previousVisibility = info['visible']
            self.setVisible(info['visible'])


class DockerMover:
    def __init__(self, mainwindow, position, panel):
        self.mainwindow = mainwindow
        self.panel = panel
        self.position = position
    def __call__(self):
        if self.position == 'Floating':
            if not self.panel.isFloating():
                self.panel.setFeatures(QDockWidget.DockWidgetClosable | QDockWidget.DockWidgetFloatable)
                self.panel.setFloating(True)
        else:
            self.panel.setFeatures(QDockWidget.DockWidgetClosable)
            self.panel.setFloating(False)
            if self.mainwindow.dockWidgetArea(self.panel) != self.position:
                visibilitycheck = self.mainwindow.isVisible()
                parentdock = [dock for dock in self.mainwindow.findChildren(QDockWidget) if dock != self.panel and (not visibilitycheck or not dock.isHidden()) and self.mainwindow.dockWidgetArea(dock) == self.position]
                self.mainwindow.addDockWidget(self.position, self.panel)
                #print([p.windowTitle() for p in parentdock])
                if len(parentdock) > 0:
                    self.mainwindow.tabifyDockWidget(parentdock[0], self.panel)

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
                for i in range(nbreused):
                    npanel,visible = self.unusedpanels.pop(0)
                    self.panels.append(npanel)
                    if visible:
                        npanel.show()
                    self.vparameterView.addAction(npanel.toggleViewAction())
            if nbtoadd-nbunusedpanels > 0:
                for i in range(nbtoadd-nbunusedpanels):
                    npanel = LpyObjectPanelDock(self.parent,"Panel "+str(i+nbpanel+nbunusedpanels),self)
                    npanel.setStatusBar(self.parent.statusBar())
                    npanel.valueChanged.connect(self.parent.projectEdited)
                    npanel.valueChanged.connect(self.parent.projectParameterEdited)
                    npanel.AutomaticUpdate.connect(self.parent.projectAutoRun)
                    npanel.setFeatures(QDockWidget.DockWidgetClosable)
                    self.panels.append(npanel)
                    DockerMover(self.parent, Qt.BottomDockWidgetArea, npanel)()
                    #self.parent.addDockWidget(Qt.BottomDockWidgetArea,npanel)
                    self.vparameterView.addAction(npanel.toggleViewAction())
                    if new_visible:
                        npanel.show()
                    #self.restoreDockWidget(npanel)
    def completeMenu(self,menu,panel):
        panelmenu = QMenu("Panel",menu)
        menu.addSeparator()
        menu.addMenu(panelmenu)
        panelAction = QAction('Rename',panelmenu)
        panelAction.triggered.connect(panel.rename)
        panelmenu.addAction(panelAction)
        panelAction = QAction('Delete',panelmenu)
        panelAction.triggered.connect(TriggerParamFunc(self.deletePanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = QAction('New',panelmenu)
        panelAction.triggered.connect(TriggerParamFunc(self.createNewPanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = QAction('Duplicate',panelmenu)
        panelAction.triggered.connect(TriggerParamFunc(self.duplicatePanel,panel))
        panelmenu.addAction(panelAction)
        panelAction = QAction('Disable' if panel.view.isActive() else 'Enable',panelmenu)
        panelAction.triggered.connect(TriggerParamFunc(panel.view.setActive,not panel.view.isActive()))
        panelmenu.addAction(panelAction)

        subpanelmenu = QMenu("Theme",menu)
        panelmenu.addSeparator()
        panelmenu.addMenu(subpanelmenu)
        for themename,value in ObjectListDisplay.THEMES.items():
            panelAction = QAction(themename,subpanelmenu)
            panelAction.triggered.connect(TriggerParamFunc(panel.view.applyTheme,value))
            subpanelmenu.addAction(panelAction)

        subpanelmenu = QMenu("Move To",menu)
        panelmenu.addSeparator()
        panelmenu.addMenu(subpanelmenu)

        for position, flag in [('Bottom', Qt.BottomDockWidgetArea), ('Left', Qt.LeftDockWidgetArea), ('Right', Qt.RightDockWidgetArea), ('Top', Qt.TopDockWidgetArea), ('Floating', 'Floating')]:
            panelAction = QAction(position,subpanelmenu)
            panelAction.triggered.connect(DockerMover(self.parent.window(),flag, panel))
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

def main():
    qapp = QApplication([])
    m = LpyObjectPanelDock(None,'TestPanel')
    m.show()
    qapp.exec_()

if __name__ == '__main__':
    main()
