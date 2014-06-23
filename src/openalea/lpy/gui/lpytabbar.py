from openalea.vpltk.qt import qt
import svnmanip
import os

QObject = qt.QtCore.QObject
SIGNAL = qt.QtCore.SIGNAL
QMessageBox = qt.QtGui.QMessageBox


class LpyTabBar(qt.QtGui.QTabBar):
    def __init__(self,parent):
        qt.QtGui.QTabBar.__init__(self,parent)
        self.setDrawBase(False)
        self.selection = None
        self.lpystudio = None
        self.initialtab = None
        
    def connectTo(self,lpystudio):
        self.lpystudio = lpystudio
        QObject.connect(self,SIGNAL('switchDocument'),lpystudio.switchDocuments)
        QObject.connect(self,SIGNAL('currentChanged(int)'),lpystudio.changeDocument)
        QObject.connect(self,SIGNAL('newDocumentRequest'),lpystudio.newfile)
        
    def mousePressEvent(self,event):
        if event.button() == qt.QtCore.Qt.LeftButton:
            self.initialtab = self.tabAt(event.pos())
        qt.QtGui.QTabBar.mousePressEvent(self,event)
    
    def mouseReleaseEvent(self,event):
        if event.button() == qt.QtCore.Qt.LeftButton:
            tabselect = self.tabAt(event.pos())
            if tabselect != -1 and not self.initialtab is None:
                if tabselect != self.initialtab:
                    self.emit(SIGNAL("switchDocument"),tabselect,self.initialtab)
            self.initialtab = None
        qt.QtGui.QTabBar.mousePressEvent(self,event)

    # def mouseMoveEvent(self,event):
    #     tabselect = self.tabAt(event.pos())
    #     if tabselect != -1 and self.initialtab != None:
    #         if tabselect != originaltab:
    #             pass
    #             #self.emit(SIGNAL("switchDocument"),tabselect,originaltab)
    #     qt.QtGui.QTabBar.mouseMoveEvent(self,event)
    # def mouseDoubleClickEvent(self,event):
    #     tabselect = self.tabAt(event.pos())
    #     if tabselect != -1 :
    #         self.emit(SIGNAL("newDocumentRequest"))
    #     qt.QtGui.QTabBar.mouseDoubleClickEvent(self,event)

    def contextMenuEvent(self,event):
        self.selection = self.tabAt(event.pos())
        if self.selection != -1:
            menu = qt.QtGui.QMenu(self)
            action = menu.addAction('Close')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.close)
            action = menu.addAction('Close all except this ')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.closeAllExcept)
            menu.addSeparator()
            if self.lpystudio.simulations[self.selection].readonly:
                action = menu.addAction('Remove Readonly ')
                QObject.connect(action,SIGNAL('triggered(bool)'),self.removeReadOnly)
            else:
                action = menu.addAction('Set Readonly ')
                QObject.connect(action,SIGNAL('triggered(bool)'),self.setReadOnly)
            menu.addSeparator()
            action = menu.addAction('Copy filename ')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.copyFilename)
            action = menu.addAction('Open folder')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.openFolder)
            action = menu.addAction('Open terminal')
            QObject.connect(action,SIGNAL('triggered(bool)'),self.openTerminalAtFolder)
            fname = self.lpystudio.simulations[self.selection].fname
            if fname and svnmanip.hasSvnSupport() :
                if svnmanip.isSvnFile(fname):
                    menu.addSeparator()
                    status = svnmanip.svnFileTextStatus(fname)
                    if status != svnmanip.added:
                        action = menu.addAction('SVN Update')
                        QObject.connect(action,SIGNAL('triggered(bool)'),self.svnUpdate)
                    if status in  [svnmanip.added,svnmanip.modified]:
                        action = menu.addAction('SVN Commit')
                        QObject.connect(action,SIGNAL('triggered(bool)'),self.svnCommit)
                    if status != svnmanip.normal:
                        action = menu.addAction('SVN Revert')
                        QObject.connect(action,SIGNAL('triggered(bool)'),self.svnRevert)
                    if status != svnmanip.added:
                        menu.addSeparator()
                        action = menu.addAction('Is Up-to-date ?')
                        QObject.connect(action,SIGNAL('triggered(bool)'),self.svnIsUpToDate)
                elif svnmanip.isSvnFile(os.path.dirname(fname)):
                    menu.addSeparator()
                    action = menu.addAction('SVN Add')
                    QObject.connect(action,SIGNAL('triggered(bool)'),self.svnAdd)
            menu.exec_(event.globalPos())
    def openFolder(self):
        import os, sys
        fname = os.path.abspath(self.lpystudio.simulations[self.selection].fname)
        mdir = os.path.dirname(fname)
        if sys.platform == 'win32':
                import subprocess
                #os.startfile(mdir)
                #os.system('explorer /select,"'+fname+'"')
                subprocess.call('CMD /K CD "'+mdir+'"')
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
                #os.startfile(mdir)
                #os.system('explorer /select,"'+fname+'"')
                subprocess.call('explorer /select,"'+fname+'"')
        elif sys.platform == 'linux2':
                os.system('gnome-terminal --working-directory "'+mdir+'"')
        elif sys.platform == 'darwin':
                os.system('open -a"Terminal" "'+mdir+'"')

    def close(self):
        self.lpystudio.closeDocument(self.selection)

    def closeAllExcept(self):
        self.lpystudio.closeAllExcept(self.selection)

    def copyFilename(self):
        qt.QtGui.QApplication.clipboard().setText(self.lpystudio.simulations[self.selection].fname)

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
        
        
class LpyTabBarNeighbor(qt.QtGui.QWidget):
    def __init__(self,parent):
        qt.QtGui.QWidget.__init__(self,parent)
        
    def mouseDoubleClickEvent(self,event):
        self.emit(SIGNAL("newDocumentRequest"))
        qt.QtGui.QWidget.mouseDoubleClickEvent(self,event)
