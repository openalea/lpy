from PyQt4.QtGui import QTabBar, QWidget, QMenu
from PyQt4.QtCore import SIGNAL, QObject


class LpyTabBar(QTabBar):
    def __init__(self,parent):
        QTabBar.__init__(self,parent)
        self.setDrawBase(False)
        self.selection = None
        self.lpystudio = None
    
    def connectTo(self,lpystudio):
        self.lpystudio = lpystudio
        QObject.connect(self,SIGNAL('switchDocument'),lpystudio.switchDocuments)
        QObject.connect(self,SIGNAL('currentChanged(int)'),lpystudio.changeDocument)
        QObject.connect(self,SIGNAL('newDocumentRequest'),lpystudio.newfile)
        
    def mouseMoveEvent(self,event):
        tabselect = self.tabAt(event.pos())
        if tabselect != -1 :
            originaltab = self.currentIndex()
            if tabselect != originaltab:
                self.emit(SIGNAL("switchDocument"),tabselect,originaltab)
        QTabBar.mouseMoveEvent(self,event)
    def mouseDoubleClickEvent(self,event):
        tabselect = self.tabAt(event.pos())
        if tabselect != -1 :
            self.emit(SIGNAL("newDocumentRequest"))
        QTabBar.mouseDoubleClickEvent(self,event)
    def contextMenuEvent(self,event):
        self.selection = self.tabAt(event.pos())
        if self.selection != -1:
            menu = QMenu(self)
            action = menu.addAction('Close')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.close)
            action = menu.addAction('Open folder')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.openFolder)
            menu.exec_(event.globalPos())
    def openFolder(self):
        import os
        os.startfile(os.path.dirname(self.lpystudio.simulations[self.selection].fname))
    def close(self):
        self.lpystudio.closeDocument(self.selection)

class LpyTabBarNeighbor(QWidget):
    def __init__(self,parent):
        QWidget.__init__(self,parent)
        
    def mouseDoubleClickEvent(self,event):
        self.emit(SIGNAL("newDocumentRequest"))
        QWidget.mouseDoubleClickEvent(self,event)