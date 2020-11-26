import sys
import os
import stat
import shutil
import asyncio

# for py2exe
try:
    import openalea.lpy.gui.py2exe_release
    import os
    sys.path.insert(0, os.path.join(sys.prefix))
    py2exe_release = True
except:
    py2exe_release = False

from . import qt_check 
import openalea.plantgl.gui.qt.QtCore
try:
   import PyQGLViewer
except ImportError as e:
    PyQGLViewer = None


import traceback as tb
from . import documentation as doc
from . import settings
from . import lpypreferences
from .simulation import LpySimulation
from .killsimulationdialog import KillSimulationDialog
from .objectpanel import ObjectPanelManager

try:
    import matplotlib
    matplotlib.use('Qt'+str(QT_VERSION)+'Agg')
except:
    pass

from openalea.plantgl.all import Viewer, eStatic, eAnimatedPrimitives, eAnimatedScene
from openalea.lpy import *


from openalea.plantgl.gui.qt.compat import *
from openalea.plantgl.gui.qt.QtCore import QCoreApplication, QEvent, QMutex, QObject, QThread, QWaitCondition, QTimer, Qt, pyqtSignal, pyqtSlot
from openalea.plantgl.gui.qt.QtGui import QIcon, QPixmap, QTextCursor
from openalea.plantgl.gui.qt.QtWidgets import QApplication, QAction, QDialog, QFileDialog, QInputDialog, QMainWindow, QMessageBox, QTabBar
try:
    from openalea.plantgl.gui.qt.QtPrintSupport import QPrintDialog, QPrinter
except:
    from openalea.plantgl.gui.qt.QtGui import QPrintDialog, QPrinter


# Restore default signal handler for CTRL+C
#import signal; signal.signal(signal.SIGINT, signal.SIG_DFL)

# Add local dir as import dir
sys.path = ['']+sys.path

from . import generate_ui
from . import lpydock
from . import lpymainwindow as lsmw
from .computationtask import *
from .lpystudiodebugger import LpyVisualDebugger
from .lpyprofiling import AnimatedProfiling, ProfilingWithFinalPlot, ProfilingWithNoPlot


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
            self.parent.view3D.saveToSnapshot(fname)
        else:
            Viewer.frameGL.saveImage(fname,format)
        
class LPyWindow(QMainWindow, lsmw.Ui_MainWindow, ComputationTaskManager) :

    endTask = pyqtSignal('PyQt_PyObject')
    killedTask = pyqtSignal('PyQt_PyObject')

    instances = []

    def __init__(self, parent=None, withinterpreter = True):
        """
        :param parent : parent window
        """
        QMainWindow.__init__(self, parent)
        ComputationTaskManager.__init__(self)
        lsmw.Ui_MainWindow.__init__(self)

        self.setObjectName('LPYMainWindow')
        self.setWindowIcon(QIcon(":/images/icons/mango.png"))

        import weakref
        LPyWindow.instances.append(weakref.ref(self))

        self.withinterpreter = withinterpreter
        self.setupUi(self)
        self.editToolBar.hide()
        lpydock.initDocks(self)

        QTimer.singleShot(1000, lambda: lpydock.initShell(self))

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
        self.fitRunView = True
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
        #self.documentNames.setShape(QTabBar.TriangularNorth)
        #self.documentNames.setTabsClosable(True)
        self.newfile()
        self.textEditionWatch = False
        self.documentNames.connectTo(self)

        self.endTask.connect(self.endTaskCheck) 
        # self.documentNamesMore.newDocumentRequest = pyqtSignal() # AUTO SIGNAL TRANSLATION in class LPyWindow
        self.documentNamesMore.newDocumentRequest.connect(self.newfile) 
        # self.documentNamesMore2.newDocumentRequest = pyqtSignal() # AUTO SIGNAL TRANSLATION in class LPyWindow
        self.documentNamesMore2.newDocumentRequest.connect(self.newfile) 
        self.actionNew.triggered.connect(self.newfile) 
        self.actionOpen.triggered.connect(lambda : self.openfile()) 
        self.actionSave.triggered.connect(lambda : self.savefile()) 
        self.actionSaveAll.triggered.connect(lambda : self.saveallfiles()) 
        self.actionSaveAs.triggered.connect(self.saveas) 
        self.actionClose.triggered.connect(self.closeDoc) 
        self.actionImportCpfgProject.triggered.connect(lambda : self.importcpfgproject()) 
        self.actionImportCpfgFile.triggered.connect(lambda : self.importcpfgfile()) 
        self.actionClear.triggered.connect(self.clearHistory) 
        self.actionSaveSession.triggered.connect(self.saveSession) 
        self.actionRun.triggered.connect(self.run) 
        self.actionAnimate.triggered.connect(self.animate) 
        self.actionStep.triggered.connect(self.step) 
        self.actionRewind.triggered.connect(self.rewind) 
        self.actionStepInterpretation.triggered.connect(self.stepInterpretation) 
        self.actionIterateTo.triggered.connect(self.iterateTo) 
        self.actionNextIterate.triggered.connect(self.nextIterate) 
        self.actionAutoRun.triggered.connect(self.projectAutoRun) 
        self.actionDebug.triggered.connect(self.debug) 
        self.actionProfile.triggered.connect(self.profile) 
        self.actionRecord.triggered.connect(self.record) 
        self.actionStop.triggered.connect(self.cancelTask) 
        self.actionStop.triggered.connect(self.abortViewer) 
        self.actionExecute.triggered.connect(self.executeCode) 
        self.actionComment.triggered.connect(self.codeeditor.comment) 
        self.actionUncomment.triggered.connect(self.codeeditor.uncomment) 
        self.actionInsertTab.triggered.connect(self.codeeditor.tab) 
        self.actionRemoveTab.triggered.connect(self.codeeditor.untab) 
        self.actionSyntax.triggered.connect(self.setSyntaxHighLightActivation) 
        self.actionTabHightlight.triggered.connect(self.setTabHighLightActivation) 
        self.actionPreferences.triggered.connect(self.preferences.show) 
        self.animtimestep.valueChanged.connect(self.setTimeStep) 
        self.animtimeSpinBox.valueChanged.connect(self.setTimeStep) 
        self.codeeditor.textChanged.connect(self.textEdited) 
        self.descriptionEdit.textChanged.connect(self.projectEdited) 
        self.referenceEdit.textChanged.connect(self.projectEdited) 
        self.authorsEdit.textChanged.connect(self.projectEdited) 
        self.intitutesEdit.textChanged.connect(self.projectEdited) 
        self.copyrightEdit.textChanged.connect(self.projectEdited) 
        self.materialed.valueChanged.connect(self.projectEdited) 
        self.scalarEditor.valueChanged.connect(self.projectEdited) 
        self.scalarEditor.valueChanged.connect(self.projectParameterEdited) 
        self.actionPrint.triggered.connect(self.printCode) 
        self.actionView3D.setEnabled(self.use_own_view3D)
        self.actionView3D.triggered.connect(self.switchCentralView) 
        self.aboutLpy = lambda x : doc.aboutLpy(self)
        self.actionAbout.triggered.connect(self.aboutLpy) 
        self.actionAboutQt.triggered.connect(QApplication.aboutQt) 
        self.aboutVPlants = lambda x : doc.aboutVPlants(self)
        self.helpDisplay.setText(doc.getSpecification())        
        self.actionOnlineHelp.triggered.connect(self.onlinehelp) 
        self.actionSubmitBug.triggered.connect(self.submitBug) 
        self.actionCheckUpdate.triggered.connect(self.check_lpy_update) 
        self.actionUseThread.triggered.connect(self.toggleUseThread) 
        self.actionFitAnimationView.triggered.connect(self.toggleFitAnimationView) 
        self.menuRecents.triggered.connect(self.recentMenuAction) 
        self.initSVNMenu()
        self.printTitle()
        self.centralViewIsGL = False
        self.svnLastRevisionChecked = 0
        self.svnLastDateChecked = 0.0
        self.stackedWidget.setCurrentIndex(0)
        self.setAnimated(False)
        settings.restoreState(self)
        self.createRecentMenu()
        #if not py2exe_release:
        try:
            self.createTutorialMenu()
        except:
            pass
        self.textEditionWatch = True
        self._initialized = False        
        try:
            self.lpy_update_enabled = self.check_lpy_update_available()
        except:
            pass

    def init(self):
        self.textEditionWatch = False
        self.recoverPreviousFiles()
        self.textEditionWatch = True
        if True : #self.lpy_update_enabled: 
            self.check_lpy_update(True)
        self.documentNames.show()
        self.currentSimulation().updateTabName()

    def check_lpy_update_available(self):
        available = True
        if not available:
            self.actionCheckUpdate.setEnabled(False)
        return available
    
    def retrieve_official_lpy_version(self, channel = 'openalea'):
        import urllib.request, urllib.error, urllib.parse
        versionurl = 'https://raw.githubusercontent.com/'+channel+'/lpy/master/src/openalea/lpy/__version__.py'
        try:
            response = urllib.request.urlopen(versionurl)
        except urllib.error.URLError as ue:
            import openalea.lpy.__version__ as lv
            return get_version_majorminor(lv.__version_number__), lv.LPY_VERSION_STR
        else:
            pyversioncode = response.read()
            lvofficial = {}
            exec(pyversioncode, lvofficial)
            return get_version_majorminor(lvofficial['__version_number__']),lvofficial['LPY_VERSION_STR']

    def check_lpy_update(self, silent = False):
        import openalea.lpy.__version__ as lv
        import os, time
        if not silent or ((self.svnLastDateChecked + 7*24*60*60) < time.time()):
            self.svnLastDateChecked = time.time()
            officialversion, offverstring = self.retrieve_official_lpy_version()
            officialdevversion, offverdevstring = self.retrieve_official_lpy_version('fredboudon')
            if get_version_majorminor(lv.__version_number__) < officialversion:
                QMessageBox.information(self,"Lpy Update","Your version is "+lv.LPY_VERSION_STR+".\nA new release version of lpy seems available on github :"+offverstring+"\n.")
            elif get_version_majorminor(lv.__version_number__) < officialdevversion:
                QMessageBox.information(self,"Lpy Update","Your version is "+lv.LPY_VERSION_STR+".\nA new develop version of lpy seems available on github :"+offverdevstring+"\n.")
            elif not silent:
                QMessageBox.information(self,"Lpy Update","Your version is "+lv.LPY_VERSION_STR+".\nYou are up-to-date!")
            else:
                self.statusBar().showMessage("L-Py "+lv.LPY_VERSION_STR+" is up-to-date.")                        

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
            if PyQGLViewer:
                self.use_own_view3D = enabled
                if not enabled and self.centralViewIsGL:
                    self.switchCentralView()
                if not enabled:
                    self.viewer = Viewer                    
                    Viewer.display = self.previousplotfunction
                else:
                    if not hasattr(self, 'previousplotfunction'):
                        self.previousplotfunction = Viewer.display
                    self.viewer = self.view3D
                    def myplot(sc): self.plotScene(sc)
                    Viewer.display = staticmethod(myplot)
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
    def findDocumentId(self, fname):
        for i,s in enumerate(self.simulations):
            if s.fname == fname:
                return s.index

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
        self.documentNames.currentChanged.disconnect(self.changeDocument)
        self.documentNames.setCurrentIndex(id1)
        self.documentNames.currentChanged.connect(self.changeDocument)
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
            for i in range(id+1,len(self.simulations)):
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
    def saveSession(self):
        settings.saveState(self)        
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
        if not self.currentSimulation().autorun:
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
                    print("Force release")
                self.releaseCR()
    def customEvent(self,event):
        self.viewer_plot(event.scene)
        self.com_mutex.lock()
        self.com_mutex.unlock()
        self.com_waitcondition.wakeAll()
    def errorEvent(self, exc_info, errmsg,  displayDialog):
        if self.withinterpreter:
            self.interpreterDock.show()    

        t,v,trb = exc_info
        stacksummary = list(reversed(tb.extract_tb(trb)))
        print(len(stacksummary))
        for fid,frame in enumerate(stacksummary):
            print(frame.filename)
            if 'openalea/lpy' in frame.filename:
                stacksummary = stacksummary[:fid]
                break
        print(len(stacksummary))
        self.lastexception = v
        if t == SyntaxError:
            errorfile = v.filename
            lineno = v.lineno
        else:
            if len(stacksummary) > 0:
                st = stacksummary[0]
                errorfile = st.filename
                lineno = st.lineno
            else:
                errorfile = None
                lineno = None
        fnames = ['<string>',self.currentSimulation().getBaseName()]

        if errorfile in fnames :
            self.codeeditor.hightlightError(lineno)
        def showErrorOnFile():
            docid = self.findDocumentId(errorfile)
            if docid:
                self.showDocumentAt(errorfile, lineno)
            else:
                self.showfilecontent(errorfile)
            self.codeeditor.hightlightError(lineno)

        if displayDialog:
            dialog = QMessageBox(QMessageBox.Warning, "Exception", (os.path.basename(errorfile)+':' if errorfile else '')+ (str(lineno)+':' if lineno else '')+errmsg, QMessageBox.Ok, self)
            if errorfile and (not errorfile in fnames) :
                showbutton = dialog.addButton("Show file", QMessageBox.ApplyRole)
                showbutton.clicked.connect(showErrorOnFile)
            if len(stacksummary) > 0:
                dialog.setDetailedText(errmsg+'\n\n'+'\n'.join(tb.format_list(stacksummary)))
            dialog.exec_()
            #showError = QMessageBox.warning(self,"Exception", msg, QMessageBox.Ok)



    def endErrorEvent(self):
        if self.debugger.running:
            self.debugger.stopDebugger()
            self.debugMode = False

    def setToolBarApp(self,value):
        if type(value) == int: # hack since pyqtSlot does not seems to work
            value = ['Icons', 'Texts', 'Icons and texts', 'Texts below icons'][value]
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
        fname = self.currentSimulation().getFileName()
        self.setWindowFilePath(fname)
        t = 'L-Py - '
        t += self.currentSimulation().getTabName()
        self.setWindowTitle(t)
        self.setWindowIcon(self.currentSimulation().generateIcon())
        
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
    def recoverPreviousFiles(self):
        from . import lpytmpfile as tf
        import os
        torecover = tf.getPreviousTmpLpyFiles()
        nbrecoverfile = len(torecover)
        if  nbrecoverfile > 0:
            answer = QMessageBox.warning(self,"Recovery mode","Backup files exist (%s). Do you want to recover ?" % nbrecoverfile ,QMessageBox.Ok,QMessageBox.Discard)
            recover = (answer == QMessageBox.Ok)
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
            fname = QFileDialog.getOpenFileName(self, "Open  L-Py file",
                                                    initialname,
                                                    "L-Py Files (*.lpy);;All Files (*.*)")
            if not fname: return
            fname = fname[0]
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
                self.showfilecontent(fname)
                self.releaseCR()
    def showfilecontent(self,fname):
        try:
            self.currentSimulation().saveState()
            self.createNewLsystem(fname)
            self.currentSimulation().restoreState()
            self.statusBar().showMessage("Load file '"+fname+"'",2000)
            if len(self.simulations) == 2 and self.simulations[0].isDefault():
                self.closeDocument(0)
        except:
            self.graberror()

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
        self.saveSession()
        self.statusBar().showMessage("No file to save." if nbsaving == 0 else "%i file(s) saved." % nbsaving)
    def importcpfgfile(self,fname = None):
        if fname is None:
            initialname = os.path.dirname(self.currentSimulation().fname) if self.currentSimulation().fname else '.'
            fname = QFileDialog.getOpenFileName(self, "Open  Cpfg File",
                                                    initialname,
                                                    "Cpfg Files (*.l);;All Files (*.*)")
            if not fname: return
            fname = str(fname[0])
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
        task = ComputationTask(simu.run,simu.post_run,simu.pre_run,cleanupprocess=simu.cleanup)
        task.checkRerun = True
        task.fitRunView = self.fitRunView
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
        task.fitRunView = self.fitRunView
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
        task.fitRunView = self.fitRunView
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
          task.fitRunView = self.fitRunView
          self.registerTask(task)
        except:
          self.graberror()
          self.releaseCR()      
    def debug(self):
      if self.debugMode == True:
        next(self.debugger)
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
        task.fitRunView = self.fitRunView
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
              fname = str(fname[0])
              self.acquireCR()
              simu = self.currentSimulation()
              if not self.use_own_view3D:
                self.viewAbortFunc.reset()
              try:
                task = ComputationTask(simu.animate,simu.post_animate,simu.pre_animate,cleanupprocess=simu.cleanup)
                task.fitAnimationView = self.fitAnimationView
                task.recording = os.path.splitext(fname)[0]+'-'
                task.recording_suffix = os.path.splitext(fname)[1][1:]
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
            print('Wrong added file in history')
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
                action = QAction(os.path.basename(f),self.menuRecents)
                action.setData(to_qvariant(f))
                action.setIcon(icon)
                self.menuRecents.addAction(action)
            self.menuRecents.addSeparator()
        self.menuRecents.addAction(self.actionClear)
    def createTutorialMenu(self):
        self.menuTutorials.clear()
        iconfile = QIcon()
        iconfile.addPixmap(QPixmap(":/images/icons/codefile.png"),QIcon.Normal,QIcon.Off)
        iconfolder = QIcon()
        iconfolder.addPixmap(QPixmap(":/images/icons/fileopen.png"),QIcon.Normal,QIcon.Off)
        from openalea.lpy.gui.shared_data import shared_data
        import openalea.lpy
        import os
        if 'CONDA_PREFIX' in os.environ:
            if sys.platform == 'win32':
                shared_data_path = os.path.join(os.environ['CONDA_PREFIX'], 'Library','share', 'lpy', 'tutorial')
            else:
                shared_data_path = os.path.join(os.environ['CONDA_PREFIX'], 'share', 'lpy', 'tutorial')                
        else:
            shared_data_path = shared_data(openalea.lpy, share_path='share/tutorial')
        if not shared_data_path is None and os.path.exists(shared_data_path):
            import os
            cpath = os.path.abspath(shared_data_path)
            cmenu = self.menuTutorials
            toprocess = [(cpath,cmenu)]
            while len(toprocess) > 0:
               cpath,cmenu = toprocess.pop(0)
               csubpath = os.listdir(cpath)
               csubpath.sort()
               for fname in csubpath:
                    absfname =  os.path.join(cpath,fname)
                    if os.path.isdir(absfname):
                        childmenu = cmenu.addMenu(iconfolder,os.path.basename(str(fname)))
                        toprocess.append( (absfname,childmenu) )
                        childmenu.triggered.connect(self.recentMenuAction) 
                    elif fname[-4:] == '.lpy':
                        action = QAction(os.path.basename(str(fname)),cmenu)
                        action.setData(to_qvariant(absfname))
                        action.setIcon(iconfile)
                        cmenu.addAction(action)
        else:
            self.menuFile.removeAction(self.menuTutorials.menuAction())
    def recentMenuAction(self,action):
        self.openfile(str(action.data()))
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
        cmd = self.codeeditor.codeToExecute()
        #print '... '+'\n... '.join(cmd.splitlines())
        #self.interpreter.runcode(cmd)
        self.shellwidget.execute(cmd)
        cursor = self.codeeditor.textCursor()
        cursor.movePosition(QTextCursor.Down)
        self.codeeditor.setTextCursor(cursor)
    def submitBug(self):
        import webbrowser
        webbrowser.open("https://github.com/openalea/lpy/issues")
    def onlinehelp(self):
        import webbrowser
        webbrowser.open("https://lpy-fb.readthedocs.io/")
    def initSVNMenu(self):
        from . import svnmanip
        if not svnmanip.hasSvnSupport() :
            self.menuSVN.setEnabled(False)
        else:
            self.menuSVN.aboutToShow.connect(self.updateSVNMenu) 

            self.actionSVNUpdate.triggered.connect(self.svnUpdate) 
            self.actionSVNCommit.triggered.connect(self.svnCommit) 
            self.actionSVNRevert.triggered.connect(self.svnRevert) 
            self.actionSVNAdd.triggered.connect(self.svnAdd) 
            self.actionSVNIsUpToDate.triggered.connect(self.svnIsUpToDate) 

    def updateSVNMenu(self):
        from . import svnmanip
        import os
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
    print('L-Py, version '+lpyversion.LPY_VERSION_STR)

def help():
    versionmessage()
    print('Frederic Boudon et al., Virtual Plants, CIRAD/INRIA/INRA')
    print() 
    print('lpy [OPTIONS] [FILES]')
    print('OPTIONS:')
    print('--help    : print this help')
    print('--version : print version of the software.')
    print('--safe | --no-safe: load settings in a safe or no safe mode')
    print('--run lpyfile: run an lpymodel')
    print()
    print('See http://openalea.gforge.inria.fr/wiki/doku.php?id=packages:vplants:lpy:main for more documentation') 

def runmodel(fname):
    from openalea.lpy import Lsystem
    l = Lsystem(fname)
    lstring = l.iterate()
    l.plot(lstring)

def animatemodel(fname):
    from openalea.lpy import Lsystem
    l = Lsystem(fname)
    l.animate()

def main():
    import sys, os
    args = sys.argv
    if '--help' in args or '-h' in args:
        help()
        return
    elif '--version' in args or '-v' in args:
        versiomessage()
        return

    elif '--run' in args or '-r' in args:
        fname = args[args.index('-r' if '-r' in args else '--run')+1]
        runmodel(fname)
        return

    elif '--animate' in args or '-a' in args:
        fname = args[args.index('-a' if '-a' in args else '--animate')+1]
        animatemodel(fname)
        return

    toopen = []
    if len(args) > 1: toopen = list(map(os.path.abspath,[a for a in args[1:] if not a.startswith('-')]))

    import sys
    from openalea.lpy.__version__ import LPY_VERSION_STR
    qapp = QApplication(sys.argv+['-qwindowtitle','L-Py'])
    #qapp.setAttribute(Qt.AA_DontCreateNativeWidgetSiblings)
    qapp.setApplicationName('L-Py')
    qapp.setApplicationDisplayName('L-Py')
    qapp.setDesktopFileName('L-Py')
    qapp.setApplicationVersion(LPY_VERSION_STR)
    qapp.setWindowIcon(QIcon(":/images/icons/mango.png"))
    splash = doc.splashLPy()
    qapp.processEvents()
    w = LPyWindow()
    w.show()    
    for f in toopen:
        w.openfile(f)
    if splash:
        splash.finish(w)
        w.splash = splash

    qapp.exec_()

if __name__ == '__main__':
    main()
