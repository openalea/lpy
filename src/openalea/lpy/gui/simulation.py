from PyQt4.QtCore import QObject, SIGNAL
from PyQt4.QtGui import *
from openalea.lpy import *
from openalea.plantgl.all import PglTurtle, Viewer
import optioneditordelegate as oed
import os, shutil
from time import clock
from lpystudiodebugger import AbortDebugger

defaultcode = "Axiom: \n\nderivation length: 1\nproduction:\n\n\nhomomorphism:\n\n\nendlsystem\n"

class LpySimulation:
    def __init__(self,lpywidget,index = 0, fname = None):
        self.lpywidget = lpywidget
        self.index = index
        self.lsystem = Lsystem()
        self._fname = fname
        self.tree = None
        self.nbiterations = 0
        self.timestep = 50
        self.textedition = False
        self._edited = False
        self._oldedited = False
        self.code = defaultcode
        self.textdocument = None
        self.cursor = None
        self.optionModel = None
        self.optionDelegate = None
        self.firstView = True
        self.iterateStep = None
        self.desc_items = {'__authors__'    : '' ,
                          '__institutes__'  : '' ,
                          '__copyright__'   : '' ,
                          '__description__' : '' ,
                          '__references__'  : '' }
        if not fname is None:
            self.open(fname)
    def getFname(self) : return self._fname
    def setFname(self,value) :
        self._fname = value
        self.lpywidget.documentNames.setTabText(self.index,self.getShortName())
    fname = property(getFname,setFname)
    def isEdited(self):
        return self._edited 
    def setEdited(self,value):
        self._edited = value
        self.lpywidget.printTitle()
        self.updateTabName()
    edited = property(isEdited,setEdited)
    def textEdited(self):
        self.textedition = True
        self._edited = True
        self.lpywidget.printTitle()
        self.updateTabName()
    def isTextEdited(self):
        r = self.textedition
        self.textedition = False
        self.lpywidget.printTitle()
        self.updateTabName()
        return r
    def isCurrent(self):
        return self.index == self.lpywidget.currentSimulationId
    def makeCurrent(self):
        self.lpywidget.changeDocument(self.index)
    def isDefault(self):
        if self.isCurrent():
            self.saveState()
        if not self.fname is None : return False
        if self.code != defaultcode : return False
        if self.textedition == True : return False
        if self._edited == True: return False
        for i in self.desc_items.itervalues():
            if len(i) > 0:
                return False
        ini = self.initialisationCode()
        if len(ini) > 0: return False
        return True            
    def getShortName(self):
        if self._fname is None : return 'New'
        else : return os.path.splitext(os.path.basename(self.fname))[0]
    def getTabName(self):
        t = ''
        #if self.textedition:
        #    t += '*'
        if self.fname is None:
            t += 'New'
        else:
            t += os.path.splitext(os.path.basename(self.fname))[0]
        #if self._edited:
        #    t += '*'
        return t
    def registerTab(self):
        icon = QIcon()
        if self._edited:
            icon.addPixmap(QPixmap(":/images/icons/codefile-red.png"),QIcon.Normal,QIcon.Off)
        else:
            icon.addPixmap(QPixmap(":/images/icons/codefile.png"),QIcon.Normal,QIcon.Off)
        self.lpywidget.documentNames.insertTab(self.index,icon,self.getTabName())
    def updateTabName(self):
        if self._oldedited != self._edited :
            icon = QIcon()
            if self._edited:
                icon.addPixmap(QPixmap(":/images/icons/codefile-red.png"),QIcon.Normal,QIcon.Off)
            else:
                icon.addPixmap(QPixmap(":/images/icons/codefile.png"),QIcon.Normal,QIcon.Off)
            self.lpywidget.documentNames.setTabIcon(self.index,icon)
            self._oldedited = self._edited
        self.lpywidget.documentNames.setTabText(self.index,self.getTabName())
    def getTimeStep(self):
        return self.timestep*0.001
    def getBackupName(self):
        if self.fname:
            return os.path.join(os.path.dirname(self.fname),'#'+os.path.basename(self.fname)+'#')
    def restoreState(self):        
        self.lpywidget.textEditionWatch = False
        te, tf = self.textedition, self._edited
        self.lpywidget.codeeditor.restoreSimuState(self)
        if self.optionModel is None:
            self.initializeParametersTable()
        self.lpywidget.parametersTable.setModel(self.optionModel)
        self.lpywidget.parametersTable.setItemDelegateForColumn(1,self.optionDelegate)
        self.textedition, self._edited = te, tf
        for key,editor in self.lpywidget.desc_items.iteritems():
            editor.setText(self.desc_items[key])
        self.lpywidget.setTimeStep(self.lsystem.context().animation_timestep)
        self.lpywidget.materialed.turtle = self.lsystem.context().turtle
        self.lpywidget.materialed.updateGL()
        if not self.lpywidget.interpreter is None:
            self.lpywidget.interpreter.locals['lstring'] = self.tree
            self.lpywidget.interpreter.locals['lsystem'] = self.lsystem
        self.lpywidget.printTitle()
        self.lpywidget.setTimeStep(self.lsystem.context().animation_timestep)
        if self.lpywidget.documentNames.currentIndex() != self.index:
            self.lpywidget.documentNames.setCurrentIndex(self.index)
        self.lpywidget.textEditionWatch = True
        if not self.fname is None:
            os.chdir(os.path.dirname(self.fname))
        #if not self.lsystem.isCurrent() : self.lsystem.makeCurrent()
    def saveState(self):
        #if self.lsystem.isCurrent() :self.lsystem.done()
        self.lpywidget.codeeditor.saveSimuState(self)
        for key,editor in self.lpywidget.desc_items.iteritems():
            if type(editor) == QLineEdit:
                self.desc_items[key] = editor.text()
            else:
                self.desc_items[key] = editor.toPlainText()
    def initializeParametersTable(self):
        self.optionModel = QStandardItemModel(0, 1)
        self.optionModel.setHorizontalHeaderLabels(["Parameter", "Value" ])
        options = self.lsystem.context().options
        self.optionDelegate = oed.OptionEditorDelegate()
        category = None
        categoryItem = None
        indexitem = 0
        for i in xrange(len(options)):
            option = options[i]
            if option.category != category:
                category = option.category
                sc = QStandardItem(category)                
                sc.setEditable(False)
                sc.setBackground(QBrush(QColor(172,168,153)))
                sc.setForeground(QBrush(QColor(255,255,255)))
                qf = QFont()
                qf.setBold(True)
                sc.setFont(qf)
                self.optionModel.setItem(indexitem, 0, sc)
                sc = QStandardItem()                
                sc.setEditable(False)
                sc.setBackground(QBrush(QColor(172,168,153)))
                sc.setForeground(QBrush(QColor(255,255,255)))
                self.optionModel.setItem(indexitem, 1, sc)
                indexitem += 1             
            si = QStandardItem(option.name)
            si.setToolTip(option.comment)
            si.setEditable(False)
            self.optionModel.setItem(indexitem, 0, si)
            si = QStandardItem(option.currentValue())
            si.option = option
            self.optionModel.setItem(indexitem, 1, si)
            indexitem += 1
        QObject.connect(self.optionModel,SIGNAL('itemChanged(QStandardItem*)'),self.textEdited)
    def setTree(self,tree,nbiterations,timing=None):
        self.tree = tree
        self.nbiterations = nbiterations
        msg = 'Nb Iterations : '+str(self.nbiterations)
        if not timing is None:
            msg += " in "+str(round(timing,3))+" sec."
        self.lpywidget.statusBar().showMessage(msg,10000)
        if not self.lpywidget.interpreter is None:
            self.lpywidget.interpreter.locals['lstring'] = self.tree
    def updateLsystemCode(self):
        if self.lpywidget.codeBackupEnabled:
            if self.fname and self._edited:
                bckupname = self.getBackupName()
                if self.isCurrent():
                    self.saveState()
                self.saveToFile(bckupname)
        self.lsystem.clear()
        if self.fname:
            self.lsystem.filename = self.fname
        self.code = str(self.lpywidget.codeeditor.toPlainText().toAscii())
        res = self.lsystem.set(self.code,self.lpywidget.showPyCode)
        if not res is None: print res
    def close(self):
        if self._edited:
            answer = QMessageBox.warning(self.lpywidget,self.getShortName(),"Do you want to save this document ?",
                                     QMessageBox.Save,QMessageBox.Discard,QMessageBox.Cancel)
            if answer == QMessageBox.Save: self.save()
            elif answer == QMessageBox.Cancel: return False
            elif answer == QMessageBox.Discard:
                bckupname = self.getBackupName()
                if bckupname and os.path.exists(bckupname): os.remove(bckupname)
        self.lpywidget.documentNames.removeTab(self.index)
        return True
    def save(self):
        if self.fname:
            if self.isCurrent():
                self.saveState()
            bckupname = self.getBackupName()
            if bckupname and os.path.exists(bckupname):
                os.remove(bckupname)
            if os.path.exists(self.fname) and self.lpywidget.fileBackupEnabled:
                shutil.copy(self.fname,self.fname+'~')
            self.saveToFile(self.fname)
            self.mtime = os.stat(self.fname).st_mtime
            self.setEdited(False)
            self.lpywidget.statusBar().showMessage("Save file '"+self.fname+"'",2000)
            self.lpywidget.appendInHistory(self.fname)
            self.lsystem.filename = self.fname
        else:
            self.saveas()
    def saveas(self):
        bckupname = self.getBackupName()
        fname = str(QFileDialog.getSaveFileName(self.lpywidget,"Open Py Lsystems file",self.fname if self.fname else '.',"Py Lsystems Files (*.lpy);;All Files (*.*)"))
        if fname:
            self.fname = fname
            os.chdir(os.path.dirname(fname))
            if bckupname and os.path.exists(bckupname):
                os.remove(bckupname)
            self.save()
    def saveToFile(self,fname):
        f = file(fname,'w')
        f.write(self.code)
        matinitcode = self.initialisationCode()
        creditsinitcode = self.creditsCode()
        if len(matinitcode) > 0 or len(creditsinitcode) > 0:
            if self.code[-1] != '\n':
                f.write('\n')
            f.write(LpyParsing.InitialisationBeginTag+'\n\n')
            f.write(matinitcode)
            f.write(creditsinitcode)
        f.close()        
    def initialisationCode(self):
        header = "def "+LsysContext.InitialisationFunctionName+"(context):\n"
        defaultlist = PglTurtle().getColorList()
        currentlist = self.lsystem.context().turtle.getColorList()
        nbdefault = len(defaultlist)
        nbcurrent = len(currentlist)
        init_txt = ''
        firstcol = True
        for i in xrange(nbcurrent):
            if ( (i >= nbdefault) or 
                 (not currentlist[i].isSimilar(defaultlist[i])) or 
                 (currentlist[i].name != defaultlist[i].name)):
                if firstcol :
                    init_txt += "\tfrom openalea.plantgl.all import Material,Color3\n"
                    firstcol = False
                init_txt += '\tcontext.turtle.setMaterial('+repr(i)+','+str(currentlist[i])+')\n'
        if not self.lsystem.context().is_animation_timestep_to_default():
                init_txt += '\tcontext.animation_timestep = '+str(self.getTimeStep())+'\n'
        options = self.lsystem.context().options
        for i in xrange(len(options)):
            if not options[i].isToDefault():
                init_txt += '\tcontext.options.setSelection('+repr(options[i].name)+','+str(options[i].selection)+')\n'
        if len(init_txt) > 0:
            return header+init_txt
        else:
            return '' 
    def creditsCode(self):
        txt = ''
        for key,value in self.desc_items.iteritems():             
            if len(value) > 0:
                txt += key+' = '+repr(str(value.toAscii()))+'\n'
        return txt
    def open(self,fname):
        self.setFname(fname)
        assert self._fname == fname
        recovery = False
        readname = self.fname
        bckupname = self.getBackupName()
        if bckupname and os.path.exists(bckupname):
            answer = QMessageBox.warning(self.lpywidget,"Recovery mode","A backup file '"+os.path.basename(bckupname)+"' exists. Do you want to recover ?",QMessageBox.Ok,QMessageBox.Discard)
            if answer == QMessageBox.Ok:
                recovery = True
                readname = bckupname
            elif answer == QMessageBox.Discard:
                os.remove(bckupname)     
        os.chdir(os.path.dirname(self.fname))
        f = file(readname,'rU')
        txt = f.read()
        txts = txt.split(LpyParsing.InitialisationBeginTag)            
        self.code = txts[0]
        self.textedition = recovery
        self.setEdited(recovery)
        if len(txts) == 2:
            context = self.lsystem.context()
            init = context.initialiseFrom(LpyParsing.InitialisationBeginTag+txts[1])
            if context.has_key(context.InitialisationFunctionName):
                del context[context.InitialisationFunctionName]
            for key in self.desc_items.iterkeys():
                if context.has_key(key):
                    self.desc_items[key] = context[key]
                    if init is None:
                        init = True
                else:
                    self.desc_items[key] = ''
            if init is None:
                import warnings
                warnings.warn('initialisation failed')
        else:
            for key in self.desc_items.iterkeys():
                self.desc_items[key] = ''
        self.mtime = os.stat(self.fname).st_mtime
        if self.textdocument:
            self.lpywidget.textEditionWatch = False
            self.textdocument.clear()
            self.textdocument.setPlainText(self.code)
            self.lpywidget.textEditionWatch = True
    def reload(self):
        if self.fname:
            self.open(self.fname)
    def run(self,task):
        dl = self.lsystem.derivationLength
        timing = clock()
        task.result = self.lsystem.iterate(dl)
        task.timing = clock() - timing
        task.dl = self.lsystem.getLastIterationNb()+1
    def post_run(self,task):
        if hasattr(task,'result'):
            self.setTree(task.result,task.dl,task.timing)
            self.firstView = False
            self.lsystem.plot(task.result)
    def animate(self,task):
        edition = self.isTextEdited()
        nbiter = self.nbiterations
        dt = self.getTimeStep()
        dl = self.lsystem.derivationLength
        if self.firstView and task.fitAnimationView:
            self.lsystem.plot(self.lsystem.iterate())
            self.firstView = False
            Viewer.animation(True)
        timing = clock()
        if (not edition) and (not self.tree is None) and (0 < nbiter < dl):
            task.result = self.lsystem.animate(self.tree,dt,nbiter,dl-nbiter)
        else:
            task.result = self.lsystem.animate(dt,dl)
        task.timing = clock() - timing
        task.dl = self.lsystem.getLastIterationNb()+1
    def pre_animate(self,task):
        if self.isTextEdited() or self.lsystem.empty() :
            self.updateLsystemCode()
        Viewer.start()
        Viewer.animation(False if self.firstView and task.fitAnimationView else True)
    def post_animate(self,task):
        if hasattr(task,'result'):
            self.setTree(task.result,task.dl,task.timing)
    def step(self):
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
    def iterate(self,n = None):
        if n is None:
            n = self.iterateStep
        if self.isTextEdited() or self.lsystem.empty() or not self.tree or self.nbiterations >= self.lsystem.derivationLength:
            self.updateLsystemCode()
            self.setTree(self.lsystem.axiom,0)
        self.setTree(self.lsystem.iterate(self.nbiterations,n,self.tree),self.nbiterations+n)
        self.lsystem.plot(self.tree)
        self.firstView = False
    def debug(self):
        self.lsystem.setDebugger(self.lpywidget.debugger)
        try:
            if self.isTextEdited() or self.lsystem.empty() or not self.tree:
                self.updateLsystemCode()
                self.setTree(self.lsystem.iterate(0,1,self.lsystem.axiom),1)
            else:
                if self.nbiterations < self.lsystem.derivationLength:
                    self.setTree(self.lsystem.iterate(self.nbiterations,1,self.tree),self.nbiterations+1)
                else:
                    self.setTree(self.lsystem.iterate(0,1,self.lsystem.axiom),1)
        except AbortDebugger,e :
            self.lsystem.clearDebugger()
            return
        self.lsystem.clearDebugger()
        self.lsystem.plot(self.tree)
        self.firstView = False
    def rewind(self):
        self.updateLsystemCode()
        self.isTextEdited()
        self.setTree(self.lsystem.axiom,0)
        self.lsystem.plot(self.tree)
    def clear(self):
        self.lsystem.clear()
        self.setTree(None,0)
    def cancel(self):
        self.lsystem.early_return = True
    def monitorfile(self):
        if not hasattr(self,'monitoring'):
          self.monitoring = True
          if not self.fname is None:
            if not os.path.exists(self.fname):
                answer = QMessageBox.warning(self.lpywidget,"Removed file","File '"+os.path.basename(self.fname)+"' do not exists anymore. Do you want to keep it in editor ?",QMessageBox.Yes,QMessageBox.No)
                if answer == QMessageBox.No:
                    self.lpywidget.closeDocument(self.index)
            elif os.stat(self.fname).st_mtime > self.mtime :
                answer = QMessageBox.warning(self.lpywidget,"File has changed","File '"+os.path.basename(self.fname)+"' has changed on disk. Do you want to reload it ?",QMessageBox.Yes,QMessageBox.No)
                if answer == QMessageBox.Yes:
                    self.reload()
          del self.monitoring
