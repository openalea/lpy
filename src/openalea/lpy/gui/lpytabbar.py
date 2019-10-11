from openalea.plantgl.gui.qt import qt
from openalea.plantgl.gui.qt.QtCore import QObject, Qt, pyqtSignal
from openalea.plantgl.gui.qt.QtWidgets import QApplication, QMenu, QMessageBox, QTabBar, QWidget

import openalea.lpy.gui.svnmanip as svnmanip
import os

class LpyTabBar(QTabBar):
    
    switchDocument = pyqtSignal(int,int)
    newDocumentRequest = pyqtSignal()

    def __init__(self,parent):
        QTabBar.__init__(self,parent)
        #self.setDrawBase(False)
        self.selection = None
        self.lpystudio = None
        self.initialtab = None
        self.inserted = set()
        
    def connectTo(self,lpystudio):
        self.lpystudio = lpystudio
        self.switchDocument.connect(lpystudio.switchDocuments)
        self.currentChanged.connect(lpystudio.changeDocument)
        self.newDocumentRequest.connect(lpystudio.newfile)
        
    def mousePressEvent(self,event):
        if event.button() == Qt.LeftButton:
            self.initialtab = self.tabAt(event.pos())
        QTabBar.mousePressEvent(self,event)
    
    def mouseReleaseEvent(self,event):
        if event.button() == Qt.LeftButton:
            tabselect = self.tabAt(event.pos())
            if tabselect != -1 and not self.initialtab is None:
                if tabselect != self.initialtab:
                    self.switchDocument.emit(tabselect,self.initialtab)
            self.initialtab = None
        QTabBar.mousePressEvent(self,event)

    # def mouseMoveEvent(self,event):
    #     tabselect = self.tabAt(event.pos())
    #     if tabselect != -1 and self.initialtab != None:
    #         if tabselect != originaltab:
    #             pass
    #             #self.emit(SIGNAL("switchDocument"),tabselect,originaltab)
    #     QTabBar.mouseMoveEvent(self,event)
    # def mouseDoubleClickEvent(self,event):
    #     tabselect = self.tabAt(event.pos())
    #     if tabselect != -1 :
    #         self.emit(SIGNAL("newDocumentRequest"))
    #     QTabBar.mouseDoubleClickEvent(self,event)

    def contextMenuEvent(self,event):
        self.selection = self.tabAt(event.pos())
        if self.selection != -1:
            menu = QMenu(self)
            action = menu.addAction('Close')
            action.triggered.connect(self.close)
            action = menu.addAction('Close all except this ')
            action.triggered.connect(self.closeAllExcept)
            menu.addSeparator()
            if self.lpystudio.simulations[self.selection].readonly:
                action = menu.addAction('Remove Readonly ')
                action.triggered.connect(self.removeReadOnly)
            else:
                action = menu.addAction('Set Readonly ')
                action.triggered.connect(self.setReadOnly)
            menu.addSeparator()
            action = menu.addAction('Copy filename ')
            action.triggered.connect(self.copyFilename)
            action = menu.addAction('Open folder')
            action.triggered.connect(self.openFolder)
            action = menu.addAction('Open terminal')
            action.triggered.connect(self.openTerminalAtFolder)
            fname = self.lpystudio.simulations[self.selection].fname
            if fname and svnmanip.hasSvnSupport() :
                if svnmanip.isSvnFile(fname):
                    menu.addSeparator()
                    status = svnmanip.svnFileTextStatus(fname)
                    if status != svnmanip.added:
                        action = menu.addAction('SVN Update')
                        action.triggered.connect(self.svnUpdate)
                    if status in  [svnmanip.added,svnmanip.modified]:
                        action = menu.addAction('SVN Commit')
                        action.triggered.connect(self.svnCommit)
                    if status != svnmanip.normal:
                        action = menu.addAction('SVN Revert')
                        action.triggered.connect(self.svnRevert)
                    if status != svnmanip.added:
                        menu.addSeparator()
                        action = menu.addAction('Is Up-to-date ?')
                        action.triggered.connect(self.svnIsUpToDate)
                elif svnmanip.isSvnFile(os.path.dirname(fname)):
                    menu.addSeparator()
                    action = menu.addAction('SVN Add')
                    action.triggered.connect(self.svnAdd)
            menu.exec_(event.globalPos())
    def openFolder(self):
        import os, sys
        fname = os.path.abspath(self.lpystudio.simulations[self.selection].fname)
        mdir = os.path.dirname(fname)
        if sys.platform == 'win32':
                import subprocess
                subprocess.call('explorer /select,"'+fname+'"')
        elif sys.platform == 'linux2':
                os.system('xdg-open "'+mdir+'"')
        else:
                os.system('open "'+mdir+'"')
    def openTerminalAtFolder(self):
        import os, sys
        fname = os.path.abspath(self.lpystudio.simulations[self.selection].fname)
        mdir = os.path.dirname(fname)
        if sys.platform == 'win32':
                import subprocess
                subprocess.call('CMD /K CD "'+mdir+'"')
        elif sys.platform == 'linux2':
                os.system('gnome-terminal --working-directory "'+mdir+'"')
        elif sys.platform == 'darwin':
                os.system('open -a"Terminal" "'+mdir+'"')

    def close(self):
        self.lpystudio.closeDocument(self.selection)

    def closeAllExcept(self):
        self.lpystudio.closeAllExcept(self.selection)

    def copyFilename(self):
        QApplication.clipboard().setText(self.lpystudio.simulations[self.selection].fname)

    def removeReadOnly(self):
        self.lpystudio.simulations[self.selection].removeReadOnly()

    def setReadOnly(self):
        self.lpystudio.simulations[self.selection].setReadOnly()
        
    def svnUpdate(self):
        self.lpystudio.simulations[self.selection].svnUpdate()
        
    def svnIsUpToDate(self):
        self.lpystudio.simulations[self.selection].svnIsUpToDate()
        
    def svnAdd(self):
        self.lpystudio.simulations[self.selection].svnAdd()
        
    def svnRevert(self):
        self.lpystudio.simulations[self.selection].svnRevert()
        
    def svnCommit(self):
        self.lpystudio.simulations[self.selection].svnCommit()
    
    def insertTab(self, index, val1, val2 = None):
        self.inserted.add(index)
        if val2 : QTabBar.insertTab(self, index, val1, val2)
        else : QTabBar.insertTab(self, index, val1)

        
class LpyTabBarNeighbor(QWidget):
    
    newDocumentRequest = pyqtSignal()

    def __init__(self,parent):
        QWidget.__init__(self,parent)
        
    def mouseDoubleClickEvent(self,event):
        self.newDocumentRequest.emit()
        QWidget.mouseDoubleClickEvent(self,event)
