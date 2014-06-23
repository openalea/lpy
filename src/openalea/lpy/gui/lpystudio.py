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
from openalea.vpltk import qt
from openalea.vpltk.qt.compat import *
from PyQGLViewer import *
import traceback as tb
import documentation as doc
import settings
import lpypreferences
from simulation import LpySimulation
from killsimulationdialog import KillSimulationDialog
from openalea.plantgl.all import *
from objectpanel import ObjectPanelManager

try:
    import matplotlib
    matplotlib.use('Qt4Agg')
except:
    pass

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
    ui.check_ui_generation(os.path.join(ldir, 'logindialog.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'logdialog.ui'))
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
        
class LPyWindow(qt.QtGui.QMainWindow, lsmw.Ui_MainWindow,ComputationTaskManager) :
    def __init__(self, parent=None, withinterpreter = True):
        """
        :param parent : parent window
        """
        qt.QtGui.QMainWindow.__init__(self, parent)
        ComputationTaskManager.__init__(self)
        lsmw.Ui_MainWindow.__init__(self)
        self.withinterpreter = withinterpreter
        self.setupUi(self)
        self.editToolBar.hide()
        lpydock.initDocks(self)
        self.preferences = lpypreferences.LpyPreferences(self)
        icon = qt.QtGui.QIcon()
        icon.addPixmap(qt.QtGui.QPixmap(":/images/icons/history.png"),qt.QtGui.QIcon.Normal,qt.QtGui.QIcon.Off)
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
        self.com_mutex = qt.QtCore.QMutex()
        self.com_waitcondition = qt.QtCore.QWaitCondition()
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
        self.documentNames.setShape(qt.QtGui.QTabBar.TriangularNorth)
        #self.documentNames.setTabsClosable(True)
        self.newfile()
        self.textEditionWatch = False
        self.documentNames.connectTo(self)

        qt.QtCore.QObject.connect(self,qt.QtCore.SIGNAL('endTask(PyQt_PyObject)'),self.endTaskCheck)
        qt.QtCore.QObject.connect(self.documentNamesMore,qt.QtCore.SIGNAL('newDocumentRequest'),self.newfile)
        qt.QtCore.QObject.connect(self.documentNamesMore2,qt.QtCore.SIGNAL('newDocumentRequest'),self.newfile)
        qt.QtCore.QObject.connect(self.actionNew,qt.QtCore.SIGNAL('triggered(bool)'),self.newfile)
        qt.QtCore.QObject.connect(self.actionOpen,qt.QtCore.SIGNAL('triggered(bool)'),lambda : self.openfile())
        qt.QtCore.QObject.connect(self.actionSave,qt.QtCore.SIGNAL('triggered(bool)'),lambda : self.savefile())
        qt.QtCore.QObject.connect(self.actionSaveAll,qt.QtCore.SIGNAL('triggered(bool)'),lambda : self.saveallfiles())
        qt.QtCore.QObject.connect(self.actionSaveAs,qt.QtCore.SIGNAL('triggered(bool)'),self.saveas)
        qt.QtCore.QObject.connect(self.actionClose,qt.QtCore.SIGNAL('triggered(bool)'),self.closeDoc)
        qt.QtCore.QObject.connect(self.actionImportCpfgProject,qt.QtCore.SIGNAL('triggered(bool)'),lambda : self.importcpfgproject())
        qt.QtCore.QObject.connect(self.actionImportCpfgFile,qt.QtCore.SIGNAL('triggered(bool)'),lambda : self.importcpfgfile())
        qt.QtCore.QObject.connect(self.actionClear,qt.QtCore.SIGNAL('triggered(bool)'),self.clearHistory)
        qt.QtCore.QObject.connect(self.actionRun, qt.QtCore.SIGNAL('triggered(bool)'),self.run)
        qt.QtCore.QObject.connect(self.actionAnimate, qt.QtCore.SIGNAL('triggered(bool)'),self.animate)
        qt.QtCore.QObject.connect(self.actionStep, qt.QtCore.SIGNAL('triggered(bool)'),self.step)
        qt.QtCore.QObject.connect(self.actionRewind, qt.QtCore.SIGNAL('triggered(bool)'),self.rewind)
        qt.QtCore.QObject.connect(self.actionStepInterpretation, qt.QtCore.SIGNAL('triggered(bool)'),self.stepInterpretation)
        qt.QtCore.QObject.connect(self.actionIterateTo, qt.QtCore.SIGNAL('triggered(bool)'),self.iterateTo)
        qt.QtCore.QObject.connect(self.actionNextIterate, qt.QtCore.SIGNAL('triggered(bool)'),self.nextIterate)
        qt.QtCore.QObject.connect(self.actionAutoRun, qt.QtCore.SIGNAL('triggered(bool)'),self.projectAutoRun)
        qt.QtCore.QObject.connect(self.actionDebug, qt.QtCore.SIGNAL('triggered(bool)'),self.debug)
        qt.QtCore.QObject.connect(self.actionProfile, qt.QtCore.SIGNAL('triggered(bool)'),self.profile)
        qt.QtCore.QObject.connect(self.actionRecord, qt.QtCore.SIGNAL('triggered(bool)'),self.record)
        qt.QtCore.QObject.connect(self.actionStop, qt.QtCore.SIGNAL('triggered(bool)'),self.cancelTask)
        qt.QtCore.QObject.connect(self.actionStop, qt.QtCore.SIGNAL('triggered(bool)'),self.abortViewer)
        qt.QtCore.QObject.connect(self.actionExecute, qt.QtCore.SIGNAL('triggered(bool)'),self.executeCode)
        qt.QtCore.QObject.connect(self.actionComment, qt.QtCore.SIGNAL('triggered(bool)'),self.codeeditor.comment)
        qt.QtCore.QObject.connect(self.actionUncomment, qt.QtCore.SIGNAL('triggered(bool)'),self.codeeditor.uncomment)
        qt.QtCore.QObject.connect(self.actionInsertTab, qt.QtCore.SIGNAL('triggered(bool)'),self.codeeditor.tab)
        qt.QtCore.QObject.connect(self.actionRemoveTab, qt.QtCore.SIGNAL('triggered(bool)'),self.codeeditor.untab)
        qt.QtCore.QObject.connect(self.actionSyntax, qt.QtCore.SIGNAL('triggered(bool)'),self.setSyntaxHighLightActivation)
        qt.QtCore.QObject.connect(self.actionTabHightlight, qt.QtCore.SIGNAL('triggered(bool)'),self.setTabHighLightActivation)
        qt.QtCore.QObject.connect(self.actionPreferences, qt.QtCore.SIGNAL('triggered(bool)'),self.preferences.show)
        qt.QtCore.QObject.connect(self.animtimestep, qt.QtCore.SIGNAL('valueChanged(int)'),self.setTimeStep)
        qt.QtCore.QObject.connect(self.animtimeSpinBox, qt.QtCore.SIGNAL('valueChanged(double)'),self.setTimeStep)
        qt.QtCore.QObject.connect(self.codeeditor, qt.QtCore.SIGNAL('textChanged()'),self.textEdited)
        qt.QtCore.QObject.connect(self.descriptionEdit, qt.QtCore.SIGNAL('textChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.referenceEdit, qt.QtCore.SIGNAL('textChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.authorsEdit, qt.QtCore.SIGNAL('textChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.intitutesEdit, qt.QtCore.SIGNAL('textChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.copyrightEdit, qt.QtCore.SIGNAL('textChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.materialed, qt.QtCore.SIGNAL('valueChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.scalarEditor, qt.QtCore.SIGNAL('valueChanged()'),self.projectEdited)
        qt.QtCore.QObject.connect(self.scalarEditor, qt.QtCore.SIGNAL('valueChanged()'),self.projectParameterEdited)
        qt.QtCore.QObject.connect(self.actionPrint, qt.QtCore.SIGNAL('triggered(bool)'),self.printCode)
        self.actionView3D.setEnabled(self.use_own_view3D)
        qt.QtCore.QObject.connect(self.actionView3D, qt.QtCore.SIGNAL('triggered(bool)'),self.switchCentralView)
        self.aboutLpy = lambda x : doc.aboutLpy(self)
        qt.QtCore.QObject.connect(self.actionAbout, qt.QtCore.SIGNAL('triggered(bool)'),self.aboutLpy)
        qt.QtCore.QObject.connect(self.actionAboutQt, qt.QtCore.SIGNAL('triggered(bool)'),qt.QtGui.QApplication.aboutQt)
        self.aboutVPlants = lambda x : doc.aboutVPlants(self)
        qt.QtCore.QObject.connect(self.actionAboutVPlants, qt.QtCore.SIGNAL('triggered(bool)'),self.aboutVPlants)
        self.helpDisplay.setText(doc.getSpecification())        
        qt.QtCore.QObject.connect(self.actionOnlineHelp, qt.QtCore.SIGNAL('triggered(bool)'),self.onlinehelp)
        qt.QtCore.QObject.connect(self.actionSubmitBug, qt.QtCore.SIGNAL('triggered(bool)'),self.submitBug)
        qt.QtCore.QObject.connect(self.actionCheckUpdate, qt.QtCore.SIGNAL('triggered(bool)'),self.check_lpy_update)        
        qt.QtCore.QObject.connect(self.actionUseThread,qt.QtCore.SIGNAL('triggered()'),self.toggleUseThread)
        qt.QtCore.QObject.connect(self.actionFitAnimationView,qt.QtCore.SIGNAL('triggered()'),self.toggleFitAnimationView)
        qt.QtCore.QObject.connect(self.menuRecents,qt.QtCore.SIGNAL("triggered(QAction *)"),self.recentMenuAction)
        self.initSVNMenu()
        self.printTitle()
        self.centralViewIsGL = False
        self.svnLastRevisionChecked = 0
        self.svnLastDateChecked = 0.0
        self.stackedWidget.setCurrentIndex(0)
        settings.restoreState(self)
        self.createRecentMenu()
        if not py2exe_release:
            self.createTutorialMenu()
        self.textEditionWatch = True
        self._initialized = False        
        self.lpy_update_enabled = self.check_lpy_update_available()
    def init(self):
        self.textEditionWatch = False
        self.recoverPreviousFiles()
        self.textEditionWatch = True
        if True : #self.lpy_update_enabled: 
            self.check_lpy_update(True)
    def check_lpy_update_available(self):
        import svnmanip, os
        available = False
        if svnmanip.hasSvnSupport() :
            import openalea.lpy.__version__ as lv
            testfile = os.path.dirname(lv.__file__)+'/__version__.py'
            #print testfile, svnmanip.isSvnFile(testfile)
            if svnmanip.isSvnFile(testfile):
                available = not svnmanip.isSSHRepository(testfile)
        if not available:
            self.actionCheckUpdate.setEnabled(False)
        return available
        
    def check_lpy_update(self, silent = False):
        import svnmanip, os, time
        if svnmanip.hasSvnSupport():
            import openalea.lpy.__version__ as lv
            testfile = os.path.dirname(lv.__file__)+'/__version__.py'            
            if svnmanip.isSvnFile(testfile):
                # we are dealing with a develop version of lpy
                current_rev = svnmanip.svnFileInfo(testfile).revision.number
                if not silent or ((current_rev > self.svnLastRevisionChecked) and ((self.svnLastDateChecked + 24*60*60) < time.time())):
                    self.svnLastDateChecked = time.time()
                    if svnmanip.isSSHRepository(testfile): # in case of svn+ssh protocol, we do not even try to not block the process.
                        self.svnLastRevisionChecked = current_rev
                        if not silent:
                            qt.QtGui.QMessageBox.information(self,"Lpy Update","You have a develop version of lpy.\nCannot check svn repository.\nProtocol 'SVN+SSH' not supported correctly by PySvn.")
                        else:
                            self.statusBar().showMessage("Cannot check version of svn repository of lpy. Protocol 'SVN+SSH' not supported correctly by PySvn.")                        
                    else:
                        try:
                            if not svnmanip.svnIsUpToDate(testfile,self if not silent else None,True):
                                qt.QtGui.QMessageBox.information(self,"Lpy Update","A new develop version of lpy seems available !\nPlease update sources of lpy, plantgl, vpltk and recompile.")
                                self.svnLastRevisionChecked = current_rev
                            elif not silent:
                                qt.QtGui.QMessageBox.information(self,"Lpy Update","You have a develop version of lpy.\nYou are up-to-date.")
                            else:
                                self.statusBar().showMessage("L-Py is up-to-date.")                        
                        except:
                            if not silent:
                                qt.QtGui.QMessageBox.information(self,"Lpy Update","You have a develop version of lpy.\nCannot check svn repository.")
                            else:
                                self.statusBar().showMessage('Cannot check version of svn repository of lpy.')
            else: # release version
                if silent:
                    self.statusBar().showMessage("Cannot check update with release version of lpy for now.")
                else:
                    qt.QtGui.QMessageBox.information(self,"Lpy Update","Cannot check update with release version of lpy for now.")

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
        self.simulations[id1].updateTabName(True)
        self.simulations[id2].updateTabName(True)
        qt.QtCore.QObject.disconnect(self.documentNames,qt.QtCore.SIGNAL('currentChanged(int)'),self.changeDocument)
        self.documentNames.setCurrentIndex(id1)
        qt.QtCore.QObject.connect(self.documentNames,qt.QtCore.SIGNAL('currentChanged(int)'),self.changeDocument)
    def focusInEvent ( self, event ):
        self.currentSimulation().monitorfile()
        return qt.QtGui.QMainWindow.focusInEvent ( self, event )
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
                answer = qt.QtGui.QMessageBox.warning(self,"Quitting","Are you sure ?", qt.QtGui.QMessageBox.Ok,qt.QtGui.QMessageBox.Cancel)
                if answer == qt.QtGui.QMessageBox.Cancel: e.ignore()
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
    def showEvent(self,event):
        if not self._initialized:
            self.init()
            self._initialized = True
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
      if self.thread() != qt.QtCore.QThread.currentThread():
        #Viewer.display(scene)
        self.com_mutex.lock()
        e = qt.QtCore.QEvent(qt.QtCore.QEvent.Type(qt.QtCore.QEvent.User+1))
        e.scene = scene
        qt.QtGui.QApplication.postEvent(self,e)
        self.com_waitcondition.wait(self.com_mutex)
        self.com_mutex.unlock()
      else:
        self.viewer_plot(scene)
        qt.QtCore.QCoreApplication.instance().processEvents()
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
            bar.setToolButtonStyle({'Icons' : qt.QtCore.Qt.ToolButtonIconOnly, 'Texts' : qt.QtCore.Qt.ToolButtonTextOnly , 'Icons and texts' : qt.QtCore.Qt.ToolButtonTextBesideIcon, 'Texts below icons' : qt.QtCore.Qt.ToolButtonTextUnderIcon }[str(value)])
    def getToolBarApp(self):
        return { qt.QtCore.Qt.ToolButtonIconOnly : (0,'Icons') , qt.QtCore.Qt.ToolButtonTextOnly : (1,'Texts') , qt.QtCore.Qt.ToolButtonTextBesideIcon : (2,'Icons and texts'), qt.QtCore.Qt.ToolButtonTextUnderIcon : (3,'Texts below icons')  }[self.FileBar.toolButtonStyle()]
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
        printer = qt.QtGui.QPrinter()
        dialog =  qt.QtGui.QPrintDialog(printer, self);
        dialog.setWindowTitle("Print Document");
        if dialog.exec_() != qt.QtGui.QDialog.Accepted: return
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
    def recoverPreviousFiles(self):
        import lpytmpfile as tf
        import os
        torecover = tf.getPreviousTmpLpyFiles()
        nbrecoverfile = len(torecover)
        if  nbrecoverfile > 0:
            answer = qt.QtGui.QMessageBox.warning(self,"Recovery mode","Backup files exist (%s). Do you want to recover ?" % nbrecoverfile ,qt.QtGui.QMessageBox.Ok,qt.QtGui.QMessageBox.Discard)
            recover = (answer == qt.QtGui.QMessageBox.Ok)
            for f in torecover:
                if recover:
                    self.acquireCR()
                    try:
                        self.currentSimulation().saveState()
                        self.createNewLsystem()
                        self.currentSimulation().importtmpfile(f)
                        self.currentSimulation().restoreState()
                        self.statusBar().showMessage("Load file '"+f+"'",2000)
                        if len(self.simulations) == 2 and self.simulations[0].isDefault():
                            self.closeDocument(0)
                    except:
                        self.graberror()
                    self.releaseCR()
                else:
                    os.remove(f)
    def getSimuIndex(self,fname):
        for sim in self.simulations:
            if sim.fname == fname:
                return sim.index
        return None
    def openfile(self,fname = None):
        if fname is None:
            initialname = os.path.dirname(self.currentSimulation().fname) if self.currentSimulation().fname else '.'
            fname = qt.QtGui.QFileDialog.getOpenFileName(self, "Open  L-Py file",
                                                    initialname,
                                                    "L-Py Files (*.lpy);;All Files (*.*)")
            if not fname: return
            # fname = str(fname)
            self.appendInHistory(fname)
        else :
         if not os.path.exists(fname):
            self.removeInHistory(fname)
            qt.QtGui.QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",qt.QtGui.QMessageBox.Ok)
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
            fname = qt.QtGui.QFileDialog.getOpenFileName(self, "Open  Cpfg File",
                                                    initialname,
                                                    "Cpfg Files (*.l);;All Files (*.*)")
            if not fname: return
            fname = str(fname)
        elif not os.path.exists(fname):
            qt.QtGui.QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",qt.QtGui.QMessageBox.Ok)
            fname = None
        if fname:
            self.importcpfgproject(fname)
    def importcpfgproject(self,fname = None):
        if fname is None:
            initialname = os.path.dirname(self.currentSimulation().fname) if self.currentSimulation().fname else '.'
            fname = qt.QtGui.QFileDialog.getExistingDirectory(self, "Open  Cpfg Project",
                                                    initialname)
            if not fname: return
            fname = str(fname)
        elif not os.path.exists(fname):
            qt.QtGui.QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",qt.QtGui.QMessageBox.Ok)
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
      val,ok = qt.QtGui.QInputDialog.getInteger(self,"Number of Iterations","Choose number of iterations",initval,1)
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
        fname = qt.QtGui.QFileDialog.getSaveFileName(self,'Choose template image file name',fname,'Images (*.png,*.bmp,*.jpg);;All Files (*)')
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
        fname = unicode(fname)
        if not fname in self.history:
            self.history.insert(0,fname)
        elif fname == self.history[0]:
            self.history.remove(fname)
            self.history.insert(0,fname)
        if len(self.history) > self.historymaxsize:
            del self.history[self.historymaxsize:]
        self.createRecentMenu()        
    def removeInHistory(self,fname):
        fname = unicode(fname)
        if fname in self.history:
            self.history.remove(fname)
            self.createRecentMenu()
    def createRecentMenu(self):
        self.menuRecents.clear()
        icon = qt.QtGui.QIcon()
        icon.addPixmap(qt.QtGui.QPixmap(":/images/icons/codefile.png"),qt.QtGui.QIcon.Normal,qt.QtGui.QIcon.Off)
        if len(self.history) > 0:
            for f in self.history:
                action = qt.QtGui.QAction(os.path.basename(f),self.menuRecents)
                action.setData(to_qvariant(f))
                action.setIcon(icon)
                self.menuRecents.addAction(action)
            self.menuRecents.addSeparator()
        self.menuRecents.addAction(self.actionClear)
    def createTutorialMenu(self):
        self.menuTutorials.clear()
        iconfile = qt.QtGui.QIcon()
        iconfile.addPixmap(qt.QtGui.QPixmap(":/images/icons/codefile.png"),qt.QtGui.QIcon.Normal,qt.QtGui.QIcon.Off)
        iconfolder = qt.QtGui.QIcon()
        iconfolder.addPixmap(qt.QtGui.QPixmap(":/images/icons/fileopen.png"),qt.QtGui.QIcon.Normal,qt.QtGui.QIcon.Off)
        from openalea.deploy.shared_data import shared_data
        import openalea.lpy
        shared_data_path = shared_data(openalea.lpy.__path__, share_path='share/tutorial')
        if not shared_data_path is None:
            import os
            cpath = os.path.abspath(shared_data_path)
            cmenu = self.menuTutorials
            toprocess = [(cpath,cmenu)]
            while len(toprocess) > 0:
               cpath,cmenu = toprocess.pop(0)
               for fname in os.listdir(cpath):
                    absfname =  os.path.join(cpath,fname)
                    if os.path.isdir(absfname):
                        childmenu = cmenu.addMenu(iconfolder,os.path.basename(str(fname)))
                        toprocess.append( (absfname,childmenu) )
                        qt.QtCore.QObject.connect(childmenu,qt.QtCore.SIGNAL("triggered(QAction *)"),self.recentMenuAction)
                    elif fname[-4:] == '.lpy':
                        action = qt.QtGui.QAction(os.path.basename(str(fname)),cmenu)
                        action.setData(to_qvariant(absfname))
                        action.setIcon(iconfile)
                        cmenu.addAction(action)
    def recentMenuAction(self,action):
        self.openfile(unicode(action.data()))
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
    def executeCode(self):
        self.interpreter.runcode(self.codeeditor.textCursor().selectedText())
    def submitBug(self):
        import webbrowser
        webbrowser.open("https://gforge.inria.fr/tracker/?func=add&group_id=79&atid=13767")
    def onlinehelp(self):
        import webbrowser
        webbrowser.open("http://openalea.gforge.inria.fr/dokuwiki/doku.php?id=packages:vplants:lpy:main")
    def initSVNMenu(self):
        import svnmanip
        if not svnmanip.hasSvnSupport() :
            self.menuSVN.setEnabled(False)
        else:
            qt.QtCore.QObject.connect(self.menuSVN, qt.QtCore.SIGNAL('aboutToShow()'),self.updateSVNMenu)            

            qt.QtCore.QObject.connect(self.actionSVNUpdate, qt.QtCore.SIGNAL('triggered(bool)'),self.svnUpdate)            
            qt.QtCore.QObject.connect(self.actionSVNCommit, qt.QtCore.SIGNAL('triggered(bool)'),self.svnCommit)            
            qt.QtCore.QObject.connect(self.actionSVNRevert, qt.QtCore.SIGNAL('triggered(bool)'),self.svnRevert)            
            qt.QtCore.QObject.connect(self.actionSVNAdd, qt.QtCore.SIGNAL('triggered(bool)'),self.svnAdd)            
            qt.QtCore.QObject.connect(self.actionSVNIsUpToDate, qt.QtCore.SIGNAL('triggered(bool)'),self.svnIsUpToDate)

    def updateSVNMenu(self):
        import svnmanip, os
        if svnmanip.hasSvnSupport() :
            fname = self.currentSimulation().fname

            if not fname or not svnmanip.isSvnFile(fname):
                for action in  [self.actionSVNUpdate, self.actionSVNCommit, self.actionSVNRevert, self.actionSVNIsUpToDate]:
                    action.setEnabled(False)

                self.actionSVNAdd.setEnabled(not fname is None and svnmanip.isSvnFile(os.path.dirname(fname)))
            else :
                    status = svnmanip.svnFileTextStatus(fname)
                    isadded = (status == svnmanip.added)
                    ismodified = (status == svnmanip.modified)
                    isnormal = (status == svnmanip.normal)

                    self.actionSVNUpdate.setEnabled(not isadded)
                    self.actionSVNIsUpToDate.setEnabled(not isadded)
                    self.actionSVNCommit.setEnabled(ismodified or isadded)
                    self.actionSVNRevert.setEnabled(not isnormal)
                    self.actionSVNAdd.setEnabled(False)


    def svnUpdate(self):
        self.currentSimulation().svnUpdate()
        
    def svnIsUpToDate(self):
        self.currentSimulation().svnIsUpToDate()
        
    def svnAdd(self):
        self.currentSimulation().svnAdd()
        
    def svnRevert(self):
        self.currentSimulation().svnRevert()
        
    def svnCommit(self):
        self.currentSimulation().svnCommit()
        
def versionmessage():
    import openalea.lpy.__version__ as lpyversion
    print 'L-Py, version '+lpyversion.LPY_VERSION_STR

def help():
    versionmessage()
    print 'Frederic Boudon et al., Virtual Plants, CIRAD/INRIA/INRA'
    print 
    print 'lpy [OPTIONS] [FILES]'
    print 'OPTIONS:'
    print '--help    : print this help'
    print '--version : print version of the software.'
    print '--safe | --no-safe: load settings in a safe or no safe mode'
    print
    print 'See http://openalea.gforge.inria.fr/wiki/doku.php?id=packages:vplants:lpy:main for more documentation' 


def main():
    import sys, os
    args = sys.argv
    if '--help' in args or '-h' in args:
        help()
        return
    elif '--version' in args or '-v' in args:
        versiomessage()
        return

    toopen = []
    if len(args) > 1: toopen = map(os.path.abspath,args[1:])

    qapp = qt.QtGui.QApplication([])
    splash = doc.splashLPy()
    qapp.processEvents()
    w = LPyWindow()
    w.show()    
    for f in toopen:
        if f[0] != '-':            
            w.openfile(f)
    if splash:
        splash.finish(w)
        w.splash = splash
    qapp.exec_()

if __name__ == '__main__':
    main()
