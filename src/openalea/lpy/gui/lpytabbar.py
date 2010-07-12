from PyQt4.QtGui import QTabBar, QWidget
from PyQt4.QtCore import SIGNAL


class LpyTabBar(QTabBar):
    def __init__(self,parent):
        QTabBar.__init__(self,parent)
        self.setDrawBase(False)
        
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

class LpyTabBarNeighbor(QWidget):
    def __init__(self,parent):
        QWidget.__init__(self,parent)
        
    def mouseDoubleClickEvent(self,event):
        self.emit(SIGNAL("newDocumentRequest"))
        QWidget.mouseDoubleClickEvent(self,event)