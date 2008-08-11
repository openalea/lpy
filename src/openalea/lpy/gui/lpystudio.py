from PyQt4.QtCore import *
from PyQt4.QtGui import *
import os
import shutil
import traceback as tb
import compile_ui as ui
import documentation as doc
import settings
import lpydock
import lpypreferences
from simulation import LpySimulation
from openalea.plantgl.all import *

from openalea.lpy import *

# Restore default signal handler for CTRL+C
import signal; signal.signal(signal.SIGINT, signal.SIG_DFL)

# Generate GUI if necessary
ldir    = os.path.dirname(__file__)
ui.check_ui_generation(os.path.join(ldir, 'lpymainwindow.ui'))
ui.check_rc_generation(os.path.join(ldir, 'lpyresources.qrc'))
del ldir


import lpymainwindow as lsmw
from computationtask import *

        
class LPyWindow(QMainWindow, lsmw.Ui_MainWindow,ComputationTaskManager) :
    def __init__(self, parent=None, withinterpreter = True):
        """
        @param parent : parent window
        """
        QMainWindow.__init__(self, parent)
        ComputationTaskManager.__init__(self)
        lsmw.Ui_MainWindow.__init__(self)
        self.withinterpreter = withinterpreter
        self.setupUi(self)        
        lpydock.initDocks(self)
        self.preferences = lpypreferences.LpyPreferences(self)
        icon = QIcon()
        icon.addPixmap(QPixmap(":/images/icons/history.png"),QIcon.Normal,QIcon.Off)
        self.menuRecents.setIcon(icon)
        self.simulations = []
        self.currentSimulationId = None
        self.history = []
        self.historymaxsize = 50 
        self.fileBackupEnabled = True
        self.codeBackupEnabled = True
        self.fitAnimationView = True
        self.with_thread = False
        self.showPyCode = False
        self.reloadAtStartup = True
        self.fileMonitoring = True
        self.desc_items = {'__authors__'   : self.authorsEdit,
                          '__institutes__': self.intitutesEdit,
                          '__copyright__' : self.copyrightEdit,
                          '__description__' : self.descriptionEdit,
                          '__references__' : self.referenceEdit }
        self.com_mutex = QMutex()
        self.com_waitcondition = QWaitCondition()
        registerPglPlotFunction(self.plotScene)
        
        self.frameFind.hide() 
        self.frameReplace.hide() 
        self.codeeditor.initWithEditor(self)        
        self.newfile()
        self.currentSimulation().restoreState()
        self.textEditionWatch = False
        self.documentNames.setDrawBase(False)
        QObject.connect(self.documentNames,SIGNAL('currentChanged(int)'),self.changeDocument)
        QObject.connect(self.actionNew,SIGNAL('triggered(bool)'),self.newfile)
        QObject.connect(self.actionOpen,SIGNAL('triggered(bool)'),lambda : self.openfile())
        QObject.connect(self.actionSave,SIGNAL('triggered(bool)'),lambda : self.savefile())
        QObject.connect(self.actionSaveAs,SIGNAL('triggered(bool)'),self.saveas)
        QObject.connect(self.actionClose,SIGNAL('triggered(bool)'),self.closeDoc)
        QObject.connect(self.actionClear,SIGNAL('triggered(bool)'),self.clearHistory)
        QObject.connect(self.actionRun, SIGNAL('triggered(bool)'),self.run)
        QObject.connect(self.actionAnimate, SIGNAL('triggered(bool)'),self.animate)
        QObject.connect(self.actionStep, SIGNAL('triggered(bool)'),self.step)
        QObject.connect(self.actionRewind, SIGNAL('triggered(bool)'),self.rewind)
        QObject.connect(self.actionStop, SIGNAL('triggered(bool)'),self.cancelTask)
        QObject.connect(self.actionComment, SIGNAL('triggered(bool)'),self.codeeditor.comment)
        QObject.connect(self.actionUncomment, SIGNAL('triggered(bool)'),self.codeeditor.uncomment)
        QObject.connect(self.actionInsertTab, SIGNAL('triggered(bool)'),self.codeeditor.tab)
        QObject.connect(self.actionRemoveTab, SIGNAL('triggered(bool)'),self.codeeditor.untab)
        QObject.connect(self.actionSyntax, SIGNAL('triggered(bool)'),self.setSyntaxHighLightActivation)
        QObject.connect(self.actionTabHightlight, SIGNAL('triggered(bool)'),self.setTabHighLightActivation)
        QObject.connect(self.actionPreferences, SIGNAL('triggered(bool)'),self.preferences.show)
        QObject.connect(self.animtimestep, SIGNAL('valueChanged(int)'),self.setTimeStep)
        QObject.connect(self.animtimeSpinBox, SIGNAL('valueChanged(double)'),self.setTimeStep)
        QObject.connect(self.codeeditor, SIGNAL('textChanged()'),self.textEdited)
        QObject.connect(self.descriptionEdit, SIGNAL('textChanged()'),self.projectEdited)
        QObject.connect(self.referenceEdit, SIGNAL('textChanged()'),self.projectEdited)
        QObject.connect(self.authorsEdit, SIGNAL('textChanged()'),self.projectEdited)
        QObject.connect(self.intitutesEdit, SIGNAL('textChanged()'),self.projectEdited)
        QObject.connect(self.copyrightEdit, SIGNAL('textChanged()'),self.projectEdited)
        QObject.connect(self.materialed, SIGNAL('valueChanged()'),self.projectEdited)
        self.aboutLpy = lambda x : doc.aboutLpy(self)
        QObject.connect(self.actionAbout, SIGNAL('triggered(bool)'),self.aboutLpy)
        QObject.connect(self.actionAboutQt, SIGNAL('triggered(bool)'),QApplication.aboutQt)
        self.aboutVPlants = lambda x : doc.aboutVPlants(self)
        QObject.connect(self.actionAboutVPlants, SIGNAL('triggered(bool)'),self.aboutVPlants)
        self.materialed.statusBar = self.statusBar()
        self.helpDisplay.setText(doc.getSpecification())
        QObject.connect(self.actionUseThread,SIGNAL('triggered()'),self.toggleUseThread)
        QObject.connect(self.actionFitAnimationView,SIGNAL('triggered()'),self.toggleFitAnimationView)
        QObject.connect(self.menuRecents,SIGNAL("triggered(QAction *)"),self.recentMenuAction)
        self.printTitle()
        settings.restoreState(self)
        self.createRecentMenu()
        self.textEditionWatch = True
    def currentSimulation(self):
        return self.simulations[self.currentSimulationId]
    def changeDocument(self,id):
        if self.currentSimulationId != id and id >= 0:
            if not self.currentSimulationId is None:
                self.currentSimulation().saveState()
            self.currentSimulationId = id        
            self.currentSimulation().monitorfile()
            self.currentSimulation().restoreState()
        if self.documentNames.currentIndex() != id:
            self.documentNames.setCurrentIndex(id)
    def focusInEvent ( self, event ):
        self.currentSimulation().monitorfile()
        return QMainWindow.focusInEvent ( self, event )
    def closeDoc(self):
        self.closeDocument()
    def closeDocument(self,id = None):
        if id is None:
            id = self.currentSimulationId
        if self.simulations[id].close():
            for i in xrange(id+1,len(self.simulations)):
                self.simulations[i].index = i-1
            self.textEditionWatch = False
            defaultdoc = self.codeeditor.defaultdoc
            self.codeeditor.setLpyDocument(defaultdoc)
            self.simulations.pop(id)            
            self.textEditionWatch = True
            if len(self.simulations) == 0:
                self.currentSimulationId = None
                self.newfile()
            else:
                self.currentSimulation().restoreState()
    def closeEvent(self,e):
        Viewer.stop()    
        settings.saveState(self)
        for simu in reversed(self.simulations):
            if not simu.close():
                e.ignore()
                return
        e.accept()
    def taskRunningEvent(self):
        self.statusBar().showMessage('A task is already running !',5000)
        raise Exception('A task is already running')
    def acquireEvent(self):
        self.enableButtons(False)
    def releaseEvent(self):
        self.enableButtons(True)
    def enableButtons(self,enabled):
        self.actionRun.setEnabled(enabled)
        self.actionAnimate.setEnabled(enabled)
        self.actionStep.setEnabled(enabled)
        self.actionRewind.setEnabled(enabled)
        self.actionClear.setEnabled(enabled)
        self.actionClose.setEnabled(enabled)
        self.actionStop.setEnabled(not enabled)  
        pass
    def plotScene(self,scene):
      if self.thread() != QThread.currentThread():
        #Viewer.display(scene)
        self.com_mutex.lock()
        e = QEvent(QEvent.Type(QEvent.User+1))
        e.scene = scene
        QApplication.postEvent(self,e)
        self.com_waitcondition.wait(self.com_mutex)
        self.com_mutex.unlock()
        pass
      else:
        Viewer.display(scene)
        QCoreApplication.instance().processEvents()
    def cancelTask(self):
        if not self.computationThread is None:
            self.currentSimulation().cancel()
        else:
          if self.isRunning():
            print "Force release"
            self.releaseCR()
    def customEvent(self,event):
        Viewer.display(event.scene)
        self.com_mutex.lock()
        self.com_mutex.unlock()
        self.com_waitcondition.wakeAll()
    def errorEvent(self,exc_info):
        if self.withinterpreter:
            self.interpreterDock.show()    
        t,v,trb = exc_info
        st = tb.extract_tb(trb)[-1]
        if st[0] == '<string>' :            
            self.codeeditor.hightlightError(st[1])            
        elif t == SyntaxError:            
            lst = v.message.split(':')
            if len(lst) == 3 and lst[0] == '<string>':
                self.codeeditor.hightlightError(int(lst[1]))
    def setToolBarApp(self,value):
        self.toolBar.setToolButtonStyle({'Icons' : Qt.ToolButtonIconOnly, 'Texts' : Qt.ToolButtonTextOnly , 'Icons and texts' : Qt.ToolButtonTextBesideIcon, 'Texts below icons' : Qt.ToolButtonTextUnderIcon }[str(value)])
    def getToolBarApp(self):
        return { Qt.ToolButtonIconOnly : (0,'Icons') , Qt.ToolButtonTextOnly : (1,'Texts') , Qt.ToolButtonTextBesideIcon : (2,'Icons and texts'), Qt.ToolButtonTextUnderIcon : (3,'Texts below icons')  }[self.toolBar.toolButtonStyle()]
    def toggleUseThread(self):
        ComputationTaskManager.toggleUseThread(self)
    def toggleFitAnimationView(self):
        self.fitAnimationView = not self.fitAnimationView
    def textEdited(self):
        if self.textEditionWatch :
            self.currentSimulation().textEdited()
    def projectEdited(self):
        if self.textEditionWatch :
            self.currentSimulation().setEdited(True)        
    def printTitle(self):
        t = 'L-Py - '
        t += self.currentSimulation().getTabName()
        self.setWindowTitle(t)
    def createNewLsystem(self, fname = None):
        i = len(self.simulations)
        self.simulations.append(LpySimulation(self,i,fname))
        self.currentSimulationId = i
        self.currentSimulation().registerTab()
    def setTimeStep(self,val):
        if isinstance(val,int):
            self.currentSimulation().timestep = val
        else:
            self.currentSimulation().timestep = val*1000
        t = self.currentSimulation().timestep
        if t != self.animtimestep:
            self.animtimestep.setValue(t)
        if t*0.001 != self.animtimeSpinBox:
            self.animtimeSpinBox.setValue(t*0.001)
        if self.currentSimulation().lsystem:
            self.currentSimulation().lsystem.context().animation_timestep = t*0.001
            self.projectEdited()
    def newfile(self):
        self.acquireCR()
        if not self.currentSimulationId is None and self.currentSimulationId > 0:
            self.currentSimulation().saveState()
        self.createNewLsystem()
        self.releaseCR()
        self.currentSimulation().restoreState()
    def getSimuIndex(self,fname):
        for sim in self.simulations:
            if sim.fname == fname:
                return sim.index
        return None
    def openfile(self,fname = None):
        if fname is None:
            fname = str(QFileDialog.getOpenFileName(self,"Open Py Lsystems file",self.currentSimulation().fname if self.currentSimulation().fname else '.',
                                                      "Py Lsystems Files (*.lpy);;All Files (*.*)"))
                                                     
            self.appendInHistory(fname)
        else :
         if not os.path.exists(fname):
            self.removeInHistory(fname)
            QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",QMessageBox.Ok)
            fname = None
         else:
            self.appendInHistory(fname)
        if fname:
            ind = self.getSimuIndex(fname)
            if not ind is None:
                self.simulations[ind].makeCurrent()
            else:
                self.acquireCR()
                try:
                    self.currentSimulation().saveState()
                    self.createNewLsystem(fname)
                    self.currentSimulation().restoreState()
                    self.statusBar().showMessage("Load file '"+fname+"'",2000)
                    if len(self.simulations) == 2 and self.simulations[0].isDefault():
                        self.closeDocument(0)
                except:
                    self.graberror()
                self.releaseCR()
    def savefile(self):
        self.currentSimulation().save()
    def saveas(self):
        self.currentSimulation().saveas()
    def run(self):
      self.acquireCR()
      try:
        Viewer.start()
        Viewer.animation(False)
        simu = self.currentSimulation()
        simu.updateLsystemCode()
        simu.isTextEdited()
        task = ComputationTask(simu.run,simu.post_run)
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()
    def step(self):
      self.acquireCR()
      simu = self.currentSimulation()
      try:
        simu.step()
      except :
        self.graberror()
      self.releaseCR()
    def rewind(self):
        self.acquireCR()
        try:
            self.currentSimulation().rewind()
        except:
            self.graberror()
        self.releaseCR()
    def animate(self):
      self.acquireCR()
      simu = self.currentSimulation()
      try:
        Viewer.start()
        Viewer.animation(False if simu.firstView and self.fitAnimationView else True)
        simu.updateLsystemCode()
        task = ComputationTask(simu.animate,simu.post_animate)
        task.fitAnimationView = self.fitAnimationView
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()
    def clear(self):
        self.acquireCR()
        try:
            self.currentSimulation.clear()
        except:
            self.graberror()        
        self.releaseCR()
    def appendInHistory(self,fname):
        if fname is None:
            print 'Wrong added file in history'
        fname = str(fname)
        if not fname in self.history:
            self.history.insert(0,fname)
        elif fname == self.history[0]:
            self.history.remove(fname)
            self.history.insert(0,fname)
        if len(self.history) > self.historymaxsize:
            del self.history[self.historymaxsize:]
        self.createRecentMenu()        
    def removeInHistory(self,fname):
        fname = str(fname)
        if fname in self.history:
            self.history.remove(fname)
            self.createRecentMenu()
    def createRecentMenu(self):
        self.menuRecents.clear()
        icon = QIcon()
        icon.addPixmap(QPixmap(":/images/icons/codefile.png"),QIcon.Normal,QIcon.Off)
        if len(self.history) > 0:
            for f in self.history:
                action = QAction(os.path.basename(str(f)),self.menuRecents)
                action.setData(QVariant(f))
                action.setIcon(icon)
                self.menuRecents.addAction(action)
            self.menuRecents.addSeparator()
        self.menuRecents.addAction(self.actionClear)
    def recentMenuAction(self,action):
        self.openfile(str(action.data().toString()))
    def clearHistory(self):
        self.history = []
        self.createRecentMenu()
    def setSyntaxHighLightActivation(self,value):
        self.textEditionWatch = False
        self.codeeditor.setSyntaxHighLightActivation(value)
        self.textEditionWatch = True
    def setTabHighLightActivation(self,value):
        self.textEditionWatch = False
        self.codeeditor.setTabHighLightActivation(value)
        self.textEditionWatch = True
        

def main():
    qapp = QApplication([])
    splash = doc.splashLPy()
    w = LPyWindow()
    w.show()
    if splash:
        splash.finish(w)
        w.splash = splash
    qapp.exec_()

if __name__ == '__main__':
    main()
