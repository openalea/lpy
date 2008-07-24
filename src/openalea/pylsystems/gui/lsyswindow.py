from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
import os, sys
import shutil
import traceback as tb
import optioneditordelegate as oed

try:
	import shell # openalea.visualea.shell as shell
except:
	shell = None
from openalea.plantgl.all import *
from openalea.pylsystems import *
import openalea.pylsystems as pylsys
from code import InteractiveInterpreter as Interpreter

# Restore default signal handler for CTRL+C
import signal; signal.signal(signal.SIGINT, signal.SIG_DFL)

# Generate GUI if necessary
uidir    = os.path.dirname(__file__)
uiprefix = os.path.join(uidir, 'lsysmainwindow')
uifname  = uiprefix + '.ui'
pyfname  = uiprefix + '.py'

if (os.path.exists(uifname) and 
    os.access(pyfname,os.F_OK|os.W_OK) and
    os.stat(pyfname).st_mtime < os.stat(uifname).st_mtime ) :
    print 'Generate Ui'
    fstream = file(pyfname,'w')
    uic.compileUi(uifname,fstream)
    fstream.close()
    import lsysmainwindow as lsmw


logofilename = os.path.join(uidir,'biglogo.png')

del uidir
del uiprefix
del uifname
del pyfname

import lsysmainwindow as lsmw
from computationtask import *

aboutTxt = """<b>L-Py</b><br> A Python version of <b>Lindenmayer Systems</b>.<br>
Version :"""+pylsys.LPY_VERSION_STR+"""<br>
Based on P. Prusinkiewicz et al. Lstudio/cpfg-lpfg specifications.<br>
Implemented by F. Boudon for Virtual Plants.<br>See:http://www-sop.inria.fr/virtualplants/
"""

specificationheadertxt = """LPy is based on the specification of Lstudio/cpfg-lpfg defined by P. Prusinkiewicz et al. (http://algorithmicbotany.org/lstudio). 

Here is a recap of the turtle symbols used in LPy (more are defined in cpfg-lpfg): 

"""
specificationtxt = """
Commands that control the rule sequencing have a specific syntax in lpy (compared to cpfg): 

def Start()   : is called at the beginning of the simulation.
def End()     : is called at the end of the simulation.
def StartEach() : is called before each derivation step.
def EndEach()   : is called at the end of the simulation.

forward()     : Next iteration will be done in forward direction
backward()    : Next iteration will be done in backward direction
isForward()   : Test whether direction is forward
ignore        : symbol to ignore.
consider      : symbol to consider.

For More details, see:
- P. Prusinkiewicz et al., 89, The algorithmic Beauty of Plants, Springer-Verlag.
- P. Prusinkiewicz. Graphical applications of L-systems. Proceedings of Graphics Interface '86, pp. 247-253.
- P. Prusinkiewicz, R. Karwowski, and B. Lane. The L+C plant modelling language. In Functional-Structural Plant Modelling in Crop Production, J. Vos et al. (eds.), Springer, 2007.

These commands have been added to the original cpfg-lpfg specification:

context()     : Get context of execution of the l-system. To use with care.
F(length,topradius) : The second argument of this turtle command has been added to define trapezoidal line segment (bottom radius is defined by the top radius of the previous segment for instance)
ignore(str)   : symbol to ignore.
consider(str) : symbol to consider.

These functions are imported from openalea.pylsystems module. Other data structures and functionnalities are available in the module. You can check them with help(openalea.pylsystems).
"""
        
class LSysWindow(QMainWindow, lsmw.Ui_MainWindow,ComputationTaskManager) :
    def __init__(self, parent=None, withinterpreter = True):
        """
        @param parent : parent window
        """
        QMainWindow.__init__(self, parent)
        ComputationTaskManager.__init__(self)
        lsmw.Ui_MainWindow.__init__(self)
        self.setupUi(self)
        self.lsystem = None
        self.fname = None
        self.tree = None
        self.nbiterations = 0
        self.timestep = 0
        self.setTimeStep(50)
        self.textedition = False
        self._textfileedition = False
        self.desc_items = {'__authors__'   : self.authorsEdit,
                          '__institutes__': self.intitutesEdit,
                          '__copyright__' : self.copyrightEdit,
                          '__description__' : self.descriptionEdit,
                          '__references__' : self.referenceEdit }
        if True:
            #print "Use python inter-thread communication"
            self.com_mutex = QMutex()
            self.com_waitcondition = QWaitCondition()
            registerPglPlotFunction(self.plotScene)
        self.frameFind.hide() 
        self.frameReplace.hide() 
        self.codeeditor.initWithButtons(self.findEdit,self.matchCaseButton,self.wholeWordButton,
                                        self.findNextButton,self.findPreviousButton,
                                        self.replaceEdit,self.replaceButton,self.replaceAllButton,self.statusBar())
        QObject.connect(self.actionNew,SIGNAL('triggered(bool)'),self.newfile)
        QObject.connect(self.actionOpen,SIGNAL('triggered(bool)'),lambda : self.openfile())
        QObject.connect(self.actionSave,SIGNAL('triggered(bool)'),lambda : self.savefile())
        QObject.connect(self.actionSaveAs,SIGNAL('triggered(bool)'),self.saveas)
        QObject.connect(self.actionClose,SIGNAL('triggered(bool)'),self.newfile)
        QObject.connect(self.actionClear,SIGNAL('triggered(bool)'),self.clearHistory)
        QObject.connect(self.actionRun, SIGNAL('triggered(bool)'),self.run)
        QObject.connect(self.actionAnimate, SIGNAL('triggered(bool)'),self.animate)
        QObject.connect(self.actionStep, SIGNAL('triggered(bool)'),self.step)
        QObject.connect(self.actionRewind, SIGNAL('triggered(bool)'),self.rewind)
        QObject.connect(self.actionComment, SIGNAL('triggered(bool)'),self.codeeditor.comment)
        QObject.connect(self.actionUncomment, SIGNAL('triggered(bool)'),self.codeeditor.uncomment)
        QObject.connect(self.actionInsertTab, SIGNAL('triggered(bool)'),self.codeeditor.tab)
        QObject.connect(self.actionRemoveTab, SIGNAL('triggered(bool)'),self.codeeditor.untab)
        QObject.connect(self.actionSyntax, SIGNAL('triggered(bool)'),self.codeeditor.setSyntaxHighLightActivation)
        QObject.connect(self.animtimestep, SIGNAL('valueChanged(int)'),self.setTimeStep)
        QObject.connect(self.animtimeSpinBox, SIGNAL('valueChanged(double)'),self.setTimeStep)
        QObject.connect(self.runButton, SIGNAL('pressed()'),self.run)
        QObject.connect(self.animButton, SIGNAL('pressed()'),self.animate)
        QObject.connect(self.stepButton, SIGNAL('pressed()'),self.step)
        QObject.connect(self.rewindButton, SIGNAL('pressed()'),self.rewind)
        QObject.connect(self.clearButton, SIGNAL('pressed()'),self.clear)
        QObject.connect(self.cancelButton, SIGNAL('pressed()'),self.cancelTask)
        QObject.connect(self.codeeditor, SIGNAL('textChanged()'),self.textEdited)
        QObject.connect(self.actionAbout, SIGNAL('triggered(bool)'),self.about)
        QObject.connect(self.actionAboutQt, SIGNAL('triggered(bool)'),QApplication.aboutQt)
        QObject.connect(self.actionAboutVPlants, SIGNAL('triggered(bool)'),self.aboutVPlants)
        self.codeeditor.editor = self
        #self.materialed.turtle = self.lsystem.context().turtle
        self.materialed.statusBar = self.statusBar()
        self.helpDisplay.setText(specificationheadertxt+helpTurtle()+"\n\n"+specificationtxt)
        self.withinterpreter = withinterpreter
        if withinterpreter and not shell is None:
            shellclass = shell.get_shell_class() # shell.PyCutExt #
            self.interpreter = Interpreter()
            self.interpreter.locals['window'] = self
            self.interpreter.locals['lsystem'] = self.lsystem
            self.shell = shellclass(self.interpreter, parent=self.interpreterDock)    
            self.interpreterDock.setWidget(self.shell)
            action = self.interpreterDock.toggleViewAction()
            action.setShortcut(QApplication.translate("MainWindow", "Ctrl+P", None, QApplication.UnicodeUTF8))
            self.menuTools.addSeparator()
            self.menuTools.addAction(action)
            self.interpreter.locals['tree'] = self.tree
            self.interpreter.runcode('from openalea.plantgl.all import *')
            self.interpreter.runcode('from openalea.pylsystems import *')
        else:
            self.interpreter = None
        settings = self.getSettings()
        settings.beginGroup('history')
        self.history = [ str(i) for i in settings.value('RecentFiles').toStringList() if not i is None and len(i) > 0]
        settings.endGroup()
        settings.beginGroup('threading')
        self.with_thread = settings.value('activated',QVariant(False)).toBool() 
        settings.endGroup()
        #self.cancelButton.setEnabled(self.with_thread)
        self.actionUseThread.setChecked(self.with_thread)
        QObject.connect(self.actionUseThread,SIGNAL('triggered()'),self.toggleUseThread)
        settings.beginGroup('animation')
        self.fitAnimationView = settings.value('fitview',QVariant(True)).toBool() 
        settings.endGroup()
        self.actionFitAnimationView.setChecked(self.fitAnimationView)
        QObject.connect(self.actionFitAnimationView,SIGNAL('triggered()'),self.toggleFitAnimationView)
        settings.beginGroup('pythonshell')
        pyshellvisibility = settings.value('visible',QVariant(False)).toBool() 
        settings.endGroup()
        settings.beginGroup('syntax')
        syntaxhlght = settings.value('highlighted',QVariant(True)).toBool()
        self.codeeditor.setSyntaxHighLightActivation(syntaxhlght)
        self.actionSyntax.setChecked(syntaxhlght)
        settings.endGroup()
        if not pyshellvisibility or not withinterpreter:
            self.interpreterDock.hide()
        if settings.status() != QSettings.NoError:
            raise 'settings error'
        del settings
        self.createnewfile()
        QObject.connect(self.menuRecents,SIGNAL("triggered(QAction *)"),self.recentMenuAction)
        self.createRecentMenu()
        self.printTitle()
    def taskRunningEvent(self):
        self.statusBar().showMessage('A task is already running !',5000)
        raise Exception('A task is already running')
    def acquireEvent(self):
        self.enableButtons(False)
    def releaseEvent(self):
        self.enableButtons(True)
    def enableButtons(self,enabled):
        self.runButton.setEnabled(enabled)
        self.animButton.setEnabled(enabled)
        self.stepButton.setEnabled(enabled)
        self.rewindButton.setEnabled(enabled)
        self.clearButton.setEnabled(enabled)
        self.cancelButton.setEnabled(not enabled)    
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
            self.lsystem.early_return = True
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
        if st[0] == '<string>':
            self.codeeditor.hightlightError(st[1])
    def toggleUseThread(self):
        ComputationTaskManager.toggleUseThread(self)
    def toggleFitAnimationView(self):
        self.fitAnimationView = not self.fitAnimationView
    def textEdited(self):
        self.textedition = True
        self._textfileedition = True
        self.printTitle()
    def isTextEdited(self):
        r = self.textedition
        self.textedition = False
        self.printTitle()
        return r
    def getFileTextEdition(self):
        return self._textfileedition 
    def setFileTextEdition(self,value):
        self._textfileedition = value
        self.printTitle()
    textfileedition = property(getFileTextEdition,setFileTextEdition)
    def printTitle(self):
        t = 'PyLsystems - '
        if self.textedition:
            t += '*'
        if self.fname is None:
            t += 'New'
        else:
            t += os.path.splitext(os.path.basename(self.fname))[0]
        if self._textfileedition:
            t += '*'
        self.setWindowTitle(t)
    def setNewLsystem(self):
        self.lsystem = Lsystem()
        if not self.interpreter is None:
            self.interpreter.locals['lsystem'] = self.lsystem
        self.setTree(self.lsystem.axiom,0)
        self.materialed.turtle = self.lsystem.context().turtle
        self.initializeParametersTable()
        self.firstView = True
        if not self.fname is None:
            self.lsystem.filename = self.fname
    def updateLsystemCode(self):
        self.lsystem.clear()
        if self.fname:
            self.lsystem.filename = self.fname
        self.lsystem.set(str(self.codeeditor.toPlainText()),self.actionParseDebug.isChecked())
    def refreshCodeEdition(self):
        lcode = str(self.lsystem)
        lcode = lcode.replace('Lsystem:\n','',1)
        self.codeeditor.setText(lcode)
    def setTree(self,tree,nbiterations):
        self.tree = tree
        self.nbiterations = nbiterations
        self.statusBar().showMessage('Nb Iterations : '+str(self.nbiterations),5000)
        if not self.interpreter is None:
            self.interpreter.locals['tree'] = tree
    def setTimeStep(self,val):
        if isinstance(val,int):
            self.timestep = val
        else:
            self.timestep = val*1000
        if self.timestep != self.animtimestep:
            self.animtimestep.setValue(self.timestep)
        if self.timestep*0.001 != self.animtimeSpinBox:
            self.animtimeSpinBox.setValue(self.timestep*0.001)
        if self.lsystem:
            self.lsystem.context().animation_timestep = self.timestep*0.001
    def getTimeStep(self):
        return self.timestep*0.001
    def creditsCode(self):
        txt = ''
        for key,editor in self.desc_items.iteritems():
            if type(editor) == QLineEdit:
                ltxt = editor.text()
            else:
                ltxt = editor.toPlainText()
            if len(ltxt) > 0:
                txt += key+' = """'+str(ltxt)+'"""\n'
        return txt
    def newfile(self):
        answer = QMessageBox.warning(self,"Discard Changes","Do you want to save previous document ?",
                                     QMessageBox.Save,QMessageBox.Discard,QMessageBox.Cancel)
        if answer == QMessageBox.Save:
            self.savefile()
        self.createnewfile()
    def createnewfile(self):
        self.acquireCR()
        self.setNewLsystem()
        self.releaseCR()
        self.codeeditor.clear()
        self.codeeditor.setText("Axiom: \n\nderivation length: 1\nproduction:\n\n\nhomomorphism:\n\n\nendlsystem")
        for editor in self.desc_items.itervalues():
            editor.clear()
        self.setTimeStep(self.lsystem.context().animation_timestep)
        self.fname = None
        self.textfileedition = False
    def openfile(self,fname = None):
        if fname is None:
            self.fname = str(QFileDialog.getOpenFileName(self,"Open Py Lsystems file",self.fname if self.fname else '.',
                                                      "PyLsystems Files (*.lpy);;All Files (*.*)"))
            self.appendInHistory(self.fname)
        else :
         if not os.path.exists(fname):
            self.removeInHistory(fname)
            QMessageBox.warning(self,"Inexisting file","File '"+fname+"' does not exist anymore.",QMessageBox.Ok)
            fname = None
         else:
            self.fname = fname            
            self.appendInHistory(self.fname)
        if self.fname:
          self.statusBar().showMessage("Load file '"+self.fname+"'",2000)
          self.acquireCR()
          try:
            self.setNewLsystem()
            f = file(self.fname,'r')
            txt = f.read()
            txts = txt.split('###### INITIALISATION ######')
            self.codeeditor.setText(txts[0])
            self.textfileedition = False
            if len(txts) == 2:
                context = self.lsystem.context()
                context.execute(txts[1])
                context.initialise()
                self.setTimeStep(context.animation_timestep)
                if context.has_key(context.InitialisationFunctionName):
                    del context[context.InitialisationFunctionName]
                for key,editor in self.desc_items.iteritems():
                  if context.has_key(key):
                    editor.setText(context[key])
                  else:
                    editor.clear()
                self.initializeParametersTable()
            else:
                for editor in self.desc_items.itervalues():
                  editor.clear()
          except:
            self.graberror()
          self.releaseCR()
    def savefile(self):
        if self.fname:
            if os.path.exists(self.fname):
                shutil.copy(self.fname,self.fname+'~')
            f = file(self.fname,'w')
            lsyscode = self.codeeditor.toPlainText()
            f.write(lsyscode)
            matinitcode = self.initialisationCode()
            creditsinitcode = self.creditsCode()
            if len(matinitcode) > 0 or len(creditsinitcode) > 0:
                if lsyscode[-1] != '\n':
                    f.write('\n')
                f.write('###### INITIALISATION ######\n\n')
                f.write(matinitcode)
                f.write(creditsinitcode)
            f.close()
            self.textfileedition = False
            self.statusBar().showMessage("Save file '"+self.fname+"'",2000)
            self.appendInHistory(self.fname)
            self.lsystem.filename = self.fname
        else:
            self.saveas()
    def initialisationCode(self):
        header = "def "+LsysContext.InitialisationFunctionName+"(context):\n\tfrom openalea.plantgl.all import Material,Color3\n"
        defaultlist = PglTurtle().getColorList()
        currentlist = self.lsystem.context().turtle.getColorList()
        nbdefault = len(defaultlist)
        nbcurrent = len(currentlist)
        init_txt = ''
        for i in xrange(nbcurrent):
            if ( (i >= nbdefault) or 
                 (not currentlist[i].isSimilar(defaultlist[i])) or 
                 (currentlist[i].name != defaultlist[i].name)):
                init_txt += '\tcontext.turtle.setMaterial('+str(i)+','+str(currentlist[i])+')\n'
        if not self.lsystem.context().is_animation_timestep_to_default():
                init_txt += '\tcontext.animation_timestep = '+str(self.getTimeStep())+'\n'
        options = self.lsystem.context().options
        for i in xrange(len(options)):
            if not options[i].isToDefault():
                init_txt += '\tcontext.options.setSelection("'+options[i].name+'",'+str(options[i].selection)+')\n'
        if len(init_txt) > 0:
            return header+init_txt
        else:
            return ''
    def saveas(self):
        self.fname = str(QFileDialog.getSaveFileName(self,"Open Py Lsystems file",self.fname if self.fname else '.',"PyLsystems Files (*.lpy);;All Files (*.*)"))
        if self.fname:
            self.savefile()
    def run(self):
      self.acquireCR()
      try:
        Viewer.start()
        Viewer.animation(False)
        self.updateLsystemCode()
        self.isTextEdited()
        lsys = self.lsystem
        dl = lsys.derivationLength
        def runprocess(task):
            task.result = lsys.iterate(dl)
            task.dl = lsys.getLastIterationNb()+1
        def runpostprocess(task):
          if hasattr(task,'result'):
            self.setTree(task.result,task.dl)
            self.firstView = False
            lsys.plot(task.result)
        task = ComputationTask(runprocess,runpostprocess)
        self.registerTask(task)
      except:
        self.graberror()
        self.releaseCR()
    def step(self):
      self.acquireCR()
      try:
        if self.isTextEdited() or self.lsystem.empty() or not self.tree:
            self.updateLsystemCode()
            self.setTree(self.lsystem.axiom,0)
        else:
            if self.nbiterations < self.lsystem.derivationLength:
              self.setTree(self.lsystem.iterate(self.nbiterations,1,self.tree),self.nbiterations+1)
            else:
              self.setTree(self.lsystem.axiom,0)
        self.lsystem.plot(self.tree)
        self.firstView = False
      except :
        self.graberror()
      self.releaseCR()
    def rewind(self):
        self.acquireCR()
        try:
            self.updateLsystemCode()
            self.isTextEdited()
            self.setTree(self.lsystem.axiom,0)
            self.lsystem.plot(self.tree)
        except:
            self.graberror()        
        self.releaseCR()
    def animate(self):
      self.acquireCR()
      try:
        Viewer.start()
        Viewer.animation(False if self.firstView else True)
        self.updateLsystemCode()
        edition = self.isTextEdited()
        lsys = self.lsystem
        tree = self.tree
        nbiter = self.nbiterations
        dt = self.getTimeStep()
        dl = self.lsystem.derivationLength
        def animateprocess(task):
            if self.firstView:
                lsys.plot(lsys.iterate())
                Viewer.animation(True)
                self.firstView = False
            if (not edition) and (not tree is None) and (0 < nbiter < dl):
              task.result = lsys.animate(tree,dt,nbiter,dl-nbiter)
            else:
              task.result = lsys.animate(dt,dl)
            task.dl = lsys.getLastIterationNb()+1
        def animatepostprocess(task):
          if hasattr(task,'result'):
            self.setTree(task.result,task.dl)
        task = ComputationTask(animateprocess,animatepostprocess)
        self.registerTask(task)
      except:
        self.graberror()
        self.acquireCR()
    def clear(self):
        self.acquireCR()
        try:
            self.lsystem.clear()
            self.setTree(None,0)
        except:
            self.graberror()        
        self.releaseCR()
    def getSettings(self):
        settings = QSettings(QSettings.IniFormat, QSettings.UserScope,'OpenAlea','PyLsystems')
        return settings
    def closeEvent(self,e):
        Viewer.stop()        
        settings = self.getSettings()
        settings.beginGroup('history')
        settings.setValue('RecentFiles',QVariant(QStringList(self.history)))
        settings.endGroup()
        settings.beginGroup('threading')
        settings.setValue('activated',QVariant(self.with_thread)) 
        settings.endGroup()
        if not self.interpreter is None:
            settings.beginGroup('pythonshell')
            settings.setValue('visible',QVariant(self.interpreterDock.isVisible()))
            settings.endGroup()
        settings.beginGroup('syntax')
        settings.setValue('highlighted',QVariant(self.codeeditor.isSyntaxHighLightActivated()))
        settings.endGroup()
        if settings.status() != QSettings.NoError:
            raise 'settings error'
        del settings
        if self.textfileedition:
            answer = QMessageBox.warning(self,"Discard Changes","Do you want to save previous document ?",
                                        QMessageBox.Save,QMessageBox.Discard,QMessageBox.Cancel)
            if answer == QMessageBox.Save:
                self.savefile()
            elif answer == QMessageBox.Cancel:
                e.ignore()
                return
        e.accept()
    def appendInHistory(self,fname):
        if fname is None:
            print 'Wrong added file in history'
        fname = str(fname)
        if not fname in self.history:
            self.history.insert(0,fname)
            self.createRecentMenu()
        elif fname == self.history[0]:
            self.history.remove(fname)
            self.history.insert(0,fname)
            self.createRecentMenu()
    def removeInHistory(self,fname):
        fname = str(fname)
        if fname in self.history:
            self.history.remove(fname)
            self.createRecentMenu()
    def createRecentMenu(self):
        self.menuRecents.clear()
        if len(self.history) > 0:
            for f in self.history:
                action = QAction(os.path.basename(str(f)),self.menuRecents)
                action.setData(QVariant(f))
                self.menuRecents.addAction(action)
            self.menuRecents.addSeparator()
        self.menuRecents.addAction(self.actionClear)
    def recentMenuAction(self,action):
        self.openfile(str(action.data().toString()))
    def clearHistory(self):
        self.history = []
        self.createRecentMenu()
    def about(self):
        QMessageBox.about(self,"PyLsystems",aboutTxt)
    def aboutVPlants(self):
      try:
        if hasattr(self,'splash'):
            self.splash.showMessage("Virtual Plants Team.\nCIRAD-INRIA-INRA\nSee:http://www-sop.inria.fr/virtualplants/",Qt.AlignBottom|Qt.AlignLeft)        
            self.splash.show()
        else:
            if not os.path.exists(logofilename):
                raise 'No image'                        
            pix = QPixmap(logofilename)
            self.splash = QSplashScreen(pix)
            self.splash.show()            
      except:        
        QMessageBox.about(self,"Virtual Plants","A Virtual Plants software.<br>See:http://www-sop.inria.fr/virtualplants/")
    def initializeParametersTable(self):
        #self.parametersTable.clear()
        self.optionModel = QStandardItemModel(0, 2)
        self.optionModel.setHorizontalHeaderLabels(["Parameters", "Value" ])
        self.parametersTable.setModel(self.optionModel)
        options = self.lsystem.context().options
        self.delegate = oed.OptionEditorDelegate()
        self.parametersTable.setItemDelegateForColumn(1,self.delegate)
        category = None
        categoryItem = None
        indexitem = 0
        for i in xrange(len(options)):
            option = options[i]
            if option.category != category:
                category = option.category
                #categoryItem = QStandardItem(option.category)
                #categoryItem.setEditable(False)
                #self.optionModel.setItem(indexitem, 0, categoryItem)
                #indexitem += 1
            si = QStandardItem(option.name)
            si.setToolTip(option.comment)
            si.setEditable(False)
            self.optionModel.setItem(indexitem, 0, si)
            si = QStandardItem(option.currentValue())
            si.option = option
            self.optionModel.setItem(indexitem, 1, si)
            indexitem += 1
        self.parametersTable.verticalHeader().hide()
        self.parametersTable.horizontalHeader().hide()
        self.parametersTable.resizeColumnsToContents()
        self.parametersTable.resizeRowsToContents()
        if QT_VERSION >= 0x040400:
            self.parametersTable.clearSpans()

def main():
    qapp = QApplication([])
    try:
        if not os.path.exists(logofilename):
            raise 'No image'                        
        pix = QPixmap(logofilename)
        splash = QSplashScreen(pix)
        splash.show()
        #splash.showMessage("PyLsystems",Qt.AlignBottom|Qt.AlignHCenter)
        splash.showMessage("<b>L-Py - "+pylsys.LPY_VERSION_STR+"</b>",Qt.AlignBottom|Qt.AlignHCenter)        
        qapp.processEvents()
    except:
        splash = None
    w = LSysWindow()
    w.show()
    if splash:
        splash.finish(w)
        w.splash = splash
    qapp.exec_()

if __name__ == '__main__':
    main()
