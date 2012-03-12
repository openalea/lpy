import sys
import os
import stat
import shutil

# for py2exe
try:
    import openalea.lpy.gui.py2exe_release
    import os
    #os.chdir(__path__)
    #sys.path.insert(0, os.path.join(sys.prefix, "setuptools-0.6c9-py2.5.egg"))
    sys.path.insert(0, os.path.join(sys.prefix))
    py2exe_release = True
except:
    py2exe_release = False

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQGLViewer import *
import traceback as tb
import documentation as doc
import settings
import lpypreferences
from simulation import LpySimulation
from killsimulationdialog import KillSimulationDialog
from openalea.plantgl.all import *
from objectpanel import ObjectPanelManager


from openalea.lpy import *

# Restore default signal handler for CTRL+C
import signal; signal.signal(signal.SIGINT, signal.SIG_DFL)

# Add local dir as import dir
sys.path = ['']+sys.path

# Generate GUI if necessary
if not py2exe_release:
    import compile_ui as ui
    ldir    = os.path.dirname(__file__)
    ui.check_ui_generation(os.path.join(ldir, 'lpymainwindow.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'debugger_ui.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'debugger_right_ui.ui'))
    ui.check_rc_generation(os.path.join(ldir, 'lpyresources.qrc'))
    del ldir
    pass


import lpydock
import lpymainwindow as lsmw
from computationtask import *
from lpystudiodebugger import LpyVisualDebugger
from lpyprofiling import AnimatedProfiling, ProfilingWithFinalPlot, ProfilingWithNoPlot

class LpyPlotter:
    def __init__(self,parent):
        self.parent = parent
    def plot(self,scene):
        self.parent.plotScene(scene)
    def selection(self):
        return Viewer.selection
    def waitSelection(self,txt):
        return Viewer.waitSelection(txt)
    def save(self,fname,format):
        if self.parent.use_own_view3D:
            #self.parent.view3D.setSnapshotFormat(format)
            self.parent.view3D.saveTSnapshot(fname)
        else:
            Viewer.frameGL.saveImage(fname,format)
        
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
        self.editToolBar.hide()
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
        self.displayMetaInfo = False
        self.reloadAtStartup = True
        self.fileMonitoring = True
        self.exitWithoutPrompt = False
        self.cCompilerPath = ''
        self.profilingMode = ProfilingWithFinalPlot
        self.desc_items = {'__authors__'   : self.authorsEdit,
                          '__institutes__': self.intitutesEdit,
                          '__copyright__' : self.copyrightEdit,
                          '__description__' : self.descriptionEdit,
                          '__references__' : self.referenceEdit }
        self.com_mutex = QMutex()
        self.com_waitcondition = QWaitCondition()
        self.killsimudialog = KillSimulationDialog(self)
        self.plotter = LpyPlotter(self)
        self.use_own_view3D = False
        self.viewer = Viewer
        registerPlotter(self.plotter)
        class ViewerFuncAborter:
            def __init__(self):
                self.__shouldAbort = False
                self.__registered = False
            def shouldAbort(self):
                self.__shouldAbort = True
            def reset(self):
                self.__shouldAbort = False
                if not self.__registered:
                    self.__registered = True
                    Viewer.setDialogAbortFunc(self)
            def __call__(self):
                if self.__shouldAbort:
                    self.__shouldAbort = False
                    return True
                else:
                    return False
        self.viewAbortFunc =  ViewerFuncAborter()        
        self.frameFind.hide() 
        self.frameReplace.hide() 
        self.frameGoto.hide() 
        self.codeeditor.initWithEditor(self)        
        self.debugMode = False
        self.debugger = LpyVisualDebugger(self)
        st = self.statusBar()
        self.materialed.statusBar = st
        self.panelmanager = ObjectPanelManager(self)
        self.newfile()
        self.textEditionWatch = False
        self.documentNames.connectTo(self)

        QObject.connect(self,SIGNAL('endTask(PyQt_PyObject)'),self.endTaskCheck)
        QObject.connect(self.documentNamesMore,SIGNAL('newDocumentRequest'),self.newfile)
        QObject.connect(self.documentNamesMore2,SIGNAL('newDocumentRequest'),self.newfile)
        QObject.connect(self.actionNew,SIGNAL('triggered(bool)'),self.newfile)
        QObject.connect(self.actionOpen,SIGNAL('triggered(bool)'),lambda : self.openfile())
        QObject.connect(self.actionSave,SIGNAL('triggered(bool)'),lambda : self.savefile())
        QObject.connect(self.actionSaveAll,SIGNAL('triggered(bool)'),lambda : self.saveallfiles())
        QObject.connect(self.actionSaveAs,SIGNAL('triggered(bool)'),self.saveas)
        QObject.connect(self.actionClose,SIGNAL('triggered(bool)'),self.closeDoc)
        QObject.connect(self.actionImportCpfgProject,SIGNAL('triggered(bool)'),lambda : self.importcpfgproject())
        QObject.connect(self.actionImportCpfgFile,SIGNAL('triggered(bool)'),lambda : self.importcpfgfile())
        QObject.connect(self.actionClear,SIGNAL('triggered(bool)'),self.clearHistory)
        QObject.connect(self.actionRun, SIGNAL('triggered(bool)'),self.run)
        QObject.connect(self.actionAnimate, SIGNAL('triggered(bool)'),self.animate)
        QObject.connect(self.actionStep, SIGNAL('triggered(bool)'),self.step)
        QObject.connect(self.actionRewind, SIGNAL('triggered(bool)'),self.rewind)
        QObject.connect(self.actionStepInterpretation, SIGNAL('triggered(bool)'),self.stepInterpretation)
        QObject.connect(self.actionIterateTo, SIGNAL('triggered(bool)'),self.iterateTo)
        QObject.connect(self.actionNextIterate, SIGNAL('triggered(bool)'),self.nextIterate)
        QObject.connect(self.actionAutoRun, SIGNAL('triggered(bool)'),self.projectAutoRun)
        QObject.connect(self.actionDebug, SIGNAL('triggered(bool)'),self.debug)
        QObject.connect(self.actionProfile, SIGNAL('triggered(bool)'),self.profile)
        QObject.connect(self.actionRecord, SIGNAL('triggered(bool)'),self.record)
        QObject.connect(self.actionStop, SIGNAL('triggered(bool)'),self.cancelTask)
        QObject.connect(self.actionStop, SIGNAL('triggered(bool)'),self.abortViewer)
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
        QObject.connect(self.scalarEditor, SIGNAL('valueChanged()'),self.projectEdited)
        QObject.connect(self.scalarEditor, SIGNAL('valueChanged()'),self.projectParameterEdited)
        QObject.connect(self.actionPrint, SIGNAL('triggered(bool)'),self.printCode)
        self.actionView3D.setEnabled(self.use_own_view3D)
        QObject.connect(self.actionView3D, SIGNAL('triggered(bool)'),self.switchCentralView)
        self.aboutLpy = lambda x : doc.aboutLpy(self)
        QObject.connect(self.actionAbout, SIGNAL('triggered(bool)'),self.aboutLpy)
        QObject.connect(self.actionAboutQt, SIGNAL('triggered(bool)'),QApplication.aboutQt)
        self.aboutVPlants = lambda x : doc.aboutVPlants(self)
        QObject.connect(self.actionAboutVPlants, SIGNAL('triggered(bool)'),self.aboutVPlants)
        self.helpDisplay.setText(doc.getSpecification())        
        QObject.connect(self.actionOnlineHelp, SIGNAL('triggered(bool)'),self.onlinehelp)
        QObject.connect(self.actionSubmitBug, SIGNAL('triggered(bool)'),self.submitBug)
        QObject.connect(self.actionUseThread,SIGNAL('triggered()'),self.toggleUseThread)
        QObject.connect(self.actionFitAnimationView,SIGNAL('triggered()'),self.toggleFitAnimationView)
        QObject.connect(self.menuRecents,SIGNAL("triggered(QAction *)"),self.recentMenuAction)
        self.printTitle()
        self.centralViewIsGL = False
        self.stackedWidget.setCurrentIndex(0)
        settings.restoreState(self)
        self.createRecentMenu()
        self.textEditionWatch = True
    def switchCentralView(self):
        if not self.centralViewIsGL:
            self.stackedWidget.setCurrentIndex(1)
        else:
            self.stackedWidget.setCurrentIndex(0)
        self.centralViewIsGL = not self.centralViewIsGL
        self.actionView3D.setChecked(self.centralViewIsGL)
    def setView3DCentral(self,enabled=True):
        if self.centralViewIsGL != enabled:
            self.switchCentralView()
    def setIntegratedView3D(self,enabled):
        if self.use_own_view3D != enabled:
            self.use_own_view3D = enabled
            if not enabled and self.centralViewIsGL:
                self.switchCentralView()
            if not enabled:
                self.viewer = Viewer
            else:
                self.viewer = self.view3D
            self.actionView3D.setEnabled(enabled)
    def getObjectPanels(self):
        return self.panelmanager.getObjectPanels()
    def getMaxObjectPanelNb(self):
        return self.panelmanager.getMaxObjectPanelNb()
    def setObjectPanelNb(self,nb, new_visible = True):
        self.panelmanager.setObjectPanelNb(nb, new_visible)
    #def createNewPanel(self,above):
    #    self.panelmanager.createNewPanel(above)
    def abortViewer(self):
        self.viewAbortFunc.shouldAbort()
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
    def showDocumentAt(self,fname,line):
        if self.currentSimulation().fname == fname:
            self.codeeditor.gotoLine(line)
        else :
            id = None
            for i,s in enumerate(self.simulations):
                if s.fname == fname:
                    id = s.index
                    break
            if not id is None:
                self.changeDocument(id)
                self.codeeditor.gotoLine(line)
    def switchDocuments(self,id1,id2):
        self.simulations[id1],self.simulations[id2] = self.simulations[id2],self.simulations[id1]
        self.simulations[id1].index = id1
        self.simulations[id2].index = id2
        self.simulations[id1].updateTabName()
        self.simulations[id2].updateTabName()
        QObject.disconnect(self.documentNames,SIGNAL('currentChanged(int)'),self.changeDocument)
        self.documentNames.setCurrentIndex(id1)
        QObject.connect(self.documentNames,SIGNAL('currentChanged(int)'),self.changeDocument)
    def focusInEvent ( self, event ):
        self.currentSimulation().monitorfile()
        return QMainWindow.focusInEvent ( self, event )
    def closeDoc(self):
        self.closeDocument()
    def closeDocument(self,id = None):
        if id is None:
            id = self.currentSimulationId
        if self.simulations[id].close():
            self.documentNames.removeTab(id)
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
    def end(self,force = False):
        if force:
            self.exitWithoutPrompt = force
        self.close()
    def closeEvent(self,e):
        self.debugger.endDebug()
        self.viewer.stop()    
        settings.saveState(self)
        prompt = False
        if not self.exitWithoutPrompt:
            for simu in self.simulations:
               prompt = (prompt or simu.isEdited())
               if prompt: break
            if not prompt :
                answer = QMessageBox.warning(self,"Quitting","Are you sure ?", QMessageBox.Ok,QMessageBox.Cancel)
                if answer == QMessageBox.Cancel: e.ignore()
                else:  e.accept()
            else:    
                for simu in reversed(self.simulations):
                    if not simu.close(): 
                        e.ignore()
                        return
                e.accept()
        else:
            e.accept()            
        if e.isAccepted():
            self.interpreter.locals.clear()
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
        self.actionProfile.setEnabled(enabled)
        if self.debugMode == True:
            self.actionDebug.setEnabled(True)
        else:
            self.actionDebug.setEnabled(enabled)
        self.actionStop.setEnabled(not enabled)  
        self.documentNames.setEnabled(enabled)  
    def projectAutoRun(self,value = True):
        self.currentSimulation().autorun = value
    def viewer_plot(self,scene):
        if self.use_own_view3D:
            self.setView3DCentral()
        self.viewer.display(scene)            
    def plotScene(self,scene):
      if self.thread() != QThread.currentThread():
        #Viewer.display(scene)
        self.com_mutex.lock()
        e = QEvent(QEvent.Type(QEvent.User+1))
        e.scene = scene
        QApplication.postEvent(self,e)
        self.com_waitcondition.wait(self.com_mutex)
        self.com_mutex.unlock()
      else:
        self.viewer_plot(scene)
        QCoreApplication.instance().processEvents()
    def cancelTask(self):
        if self.debugMode:
            self.debugger.stop()
            if not self.debugger.running:
                self.debugMode = False
                self.releaseCR()
        else:
            if not self.computationThread is None:
                self.currentSimulation().cancel()
                self.killsimudialog.run(self.isRunning,self.killTask)
            else:
                if self.isRunning():
                    print "Force release"
                self.releaseCR()
    def customEvent(self,event):
        self.viewer_plot(event.scene)
        self.com_mutex.lock()
        self.com_mutex.unlock()
        self.com_waitcondition.wakeAll()
    def errorEvent(self,exc_info):
        if self.withinterpreter:
            self.interpreterDock.show()    
        t,v,trb = exc_info
        st = tb.extract_tb(trb)[-1]
        self.lastexception = v
        fnames = ['<string>',self.currentSimulation().getBaseName()]
        if st[0] in fnames :
            self.codeeditor.hightlightError(st[1])            
        elif t == SyntaxError:            
            lst = v.message.split(':')
            if len(lst) >= 3 and lst[0] in fnames:
                self.codeeditor.hightlightError(int(lst[1]))
            elif v.filename in fnames:
                self.codeeditor.hightlightError(v.lineno)
    def endErrorEvent(self,answer):
        if self.debugger.running:
            self.debugger.stopDebugger()
            self.debugMode = False
    def setToolBarApp(self,value):
        for bar in [self.FileBar,self.LsytemBar,self.editToolBar]:
            bar.setToolButtonStyle({'Icons' : Qt.ToolButtonIconOnly, 'Texts' : Qt.ToolButtonTextOnly , 'Icons and texts' : Qt.ToolButtonTextBesideIcon, 'Texts below icons' : Qt.ToolButtonTextUnderIcon }[str(value)])
    def getToolBarApp(self):
        return { Qt.ToolButtonIconOnly : (0,'Icons') , Qt.ToolButtonTextOnly : (1,'Texts') , Qt.ToolButtonTextBesideIcon : (2,'Icons and texts'), Qt.ToolButtonTextUnderIcon : (3,'Texts below icons')  }[self.FileBar.toolButtonStyle()]
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
    def projectParameterEdited(self,primitiveChanged=False):
        if self.currentSimulation().autorun :
            if not self.isRunning():
                self.run(True,primitiveChanged)
            else:
                self.shouldrerun = True
                if hasattr(self,'primitiveChanged'):
                    self.primitiveChanged |= primitiveChanged
                else:
                    self.primitiveChanged = primitiveChanged
    def endTaskCheck(self,task):
        if hasattr(task,'checkRerun'):
            if hasattr(self,'shouldrerun'):
                del self.shouldrerun
                primitiveChanged = self.primitiveChanged
                del self.primitiveChanged
                self.run(True,primitiveChanged)
    def printTitle(self):
        t = 'L-Py - '
        t += self.currentSimulation().getTabName()
        self.setWindowTitle(t)
    def printCode(self):
        printer = QPrinter()
        dialog =  QPrintDialog(printer, self);
        dialog.setWindowTitle("Print Document");
        if dialog.exec_() != QDialog.Accepted: return
        self.codeeditor.print_(printer)
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
            initialname = os.path.dirname(self.currentSimulation().fname) if self.currentSimulation().fname else '.'
            fname = QFileDialog.getOpenFileName(self, "Open  L-Py file",
                                                    initialname,
                                                    "L-Py Files (*.lpy);;All Files (*.*)")
            if not fname: return
            fname = str(fname)
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
    def saveallfiles(self):
        nbsaving = 0
        for sim in self.simulations:
            if sim.isEdited():
                sim.save()
                nbsaving += 1
        self.statusBar().showMessage("No file to save." if nbsaving == 0 else "%i file(s) saved." % nbsaving)
    def importcpfgfile(self,fname = None):
        if fname is None:
            initialname = os.path.dirname(self.currentSimulation().fname) if self.currentSimulation().fname else '.'
            fname = QFileDialog.getOpenFileName(self, "Open  Cpfg File",
                                                    initialname,
                                                    "Cpfg Files (*.l);;All Files (*.*)")
            if not fname: return
            fname = str(fname)
        elif not os.path.exists(fname):
            QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",QMessageBox.Ok)
            fname = None
        if fname:
            self.importcpfgproject(fname)
    def importcpfgproject(self,fname = None):
        if fname is None:
            initialname = os.path.dirname(self.currentSimulation().fname) if self.currentSimulation().fname else '.'
            fname = QFileDialog.getExistingDirectory(self, "Open  Cpfg Project",
                                                    initialname)
            if not fname: return
            fname = str(fname)
        elif not os.path.exists(fname):
            QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",QMessageBox.Ok)
            fname = None
        if fname:
            ind = self.getSimuIndex(fname)
            if not ind is None:
                self.simulations[ind].makeCurrent()
            else:
                self.acquireCR()
                try:
                    self.currentSimulation().saveState()
                    self.createNewLsystem()
                    self.currentSimulation().importcpfgproject(fname)
                    self.currentSimulation().restoreState()
                    self.statusBar().showMessage("Load file '"+fname+"'",2000)
                    if len(self.simulations) == 2 and self.simulations[0].isDefault():
                        self.closeDocument(0)
                except:
                    self.graberror()
                self.releaseCR()
    def run(self,rerun=False,primitiveChanged=False):
      self.acquireCR()
      try:
        if not self.use_own_view3D:
            self.viewAbortFunc.reset()
        simu = self.currentSimulation()
        self.viewer.start()
        if not rerun :
            self.viewer.setAnimation(eStatic)
        else:
            if primitiveChanged or simu.getOptimisationLevel() < 2:
                self.viewer.setAnimation(eAnimatedPrimitives)
            else:
                self.viewer.setAnimation(eAnimatedScene)
        simu.updateLsystemCode()
        simu.isTextEdited()
        task = ComputationTask(simu.run,simu.post_run,cleanupprocess=simu.cleanup)
        task.checkRerun = True
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()
    def step(self):
      self.acquireCR()
      simu = self.currentSimulation()
      if not self.use_own_view3D:
        self.viewAbortFunc.reset()
      try:
        task = ComputationTask(simu.step,simu.post_step,simu.pre_step,cleanupprocess=simu.cleanup)
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()      
    def stepInterpretation(self):
      self.acquireCR()
      simu = self.currentSimulation()
      if not self.use_own_view3D:
        self.viewAbortFunc.reset()
      try:
        task = ComputationTask(simu.stepInterpretation,simu.post_stepInterpretation,simu.pre_stepInterpretation,cleanupprocess=simu.cleanup)
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()      
    def iterateTo(self):
      simu = self.currentSimulation()
      initval = simu.iterateStep
      if initval is None:  initval = 1
      val,ok = QInputDialog.getInteger(self,"Number of Iterations","Choose number of iterations",initval,1)
      if ok:        
        simu.iterateStep = val
      self.nextIterate()
    def nextIterate(self):
      simu = self.currentSimulation()
      if simu.iterateStep is None:
        self.iterateTo()
      else:
        self.acquireCR()
        if not self.use_own_view3D:
            self.viewAbortFunc.reset()
        simu = self.currentSimulation()
        try:
          task = ComputationTask(simu.iterate,simu.post_step,simu.pre_step,cleanupprocess=simu.cleanup)
          self.registerTask(task)
        except:
          self.graberror()
          self.releaseCR()      
    def debug(self):
      if self.debugMode == True:
        self.debugger.next()
      else:
        self.debugMode = True
        self.acquireCR()
        if not self.use_own_view3D:
            self.viewAbortFunc.reset()
        simu = self.currentSimulation()
        try:
            simu.debug()
        except :
            self.graberror(displayDialog = False)
        self.releaseCR()
        self.debugMode = False
    def profile(self):
      self.profilerDock.show()
      self.acquireCR()
      simu = self.currentSimulation()
      if not self.use_own_view3D:
        self.viewAbortFunc.reset()
      try:
        task = ComputationTask(simu.profile,simu.post_profile,simu.pre_profile,cleanupprocess=simu.cleanup)
        task.mode = self.profilingMode
        task.profileView = self.profileView
        self.registerTask(task)        
      except:
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
      if not self.use_own_view3D:
        self.viewAbortFunc.reset()
      try:
        task = ComputationTask(simu.animate,simu.post_animate,simu.pre_animate,cleanupprocess=simu.cleanup)
        task.fitAnimationView = self.fitAnimationView
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()
    def record(self):
        fname = '.'
        if len(self.currentSimulation().fname) > 0:
            fname = os.path.splitext(self.currentSimulation().fname)[0]+'.png'
        fname = QFileDialog.getSaveFileName(self,'Choose template image file name',fname,'Images (*.png,*.bmp,*.jpg);;All Files (*)')
        if fname:
              fname = str(fname)
              self.acquireCR()
              simu = self.currentSimulation()
              if not self.use_own_view3D:
                self.viewAbortFunc.reset()
              try:
                task = ComputationTask(simu.animate,simu.post_animate,simu.pre_animate,cleanupprocess=simu.cleanup)
                task.fitAnimationView = self.fitAnimationView
                task.recording = os.path.splitext(fname)[0]+'-'
                self.registerTask(task)
              except:
                self.graberror()
                self.releaseCR()
    def clear(self):
        self.acquireCR()
        try:
            self.currentSimulation().clear()
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
    def setCCompilerPath(self,p):
        self.cCompilerPath = str(p)
        if len(self.cCompilerPath) != 0 and not self.cCompilerPath in os.environ['PATH']:
            os.environ['PATH']+=';'+self.cCompilerPath
    def submitBug(self):
        import webbrowser
        webbrowser.open("https://gforge.inria.fr/tracker/?func=add&group_id=79&atid=13767")
    def onlinehelp(self):
        import webbrowser
        webbrowser.open("http://openalea.gforge.inria.fr/dokuwiki/doku.php?id=packages:vplants:lpy:main")

def main():
    import sys, os
    args = sys.argv
    qapp = QApplication([])
    splash = doc.splashLPy()
    qapp.processEvents()
    w = LPyWindow()
    w.show()
    if len(args) > 1:
        for f in args[1:]:            
            w.openfile(f)
    if splash:
        splash.finish(w)
        w.splash = splash
    qapp.exec_()

if __name__ == '__main__':
    main()
