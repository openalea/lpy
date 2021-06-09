from openalea.plantgl.gui.qt import qt
from .qt_check import QT_VERSION
import traceback as tb
import sys


from openalea.plantgl.gui.qt.QtCore import QMutex, QObject, QThread, pyqtSignal
from openalea.plantgl.gui.qt.QtWidgets import QMessageBox


class ThreadTransferException (Exception):
    def __init__(self,exc_type,exc_value,exc_traceback):
        self.exc_type  = exc_type
        self.exc_value = exc_value
        self.exc_traceback = exc_traceback

class ComputationTask(QThread):

    killed = pyqtSignal()

    def __init__(self, process = None, 
                       postprocess = None, 
                       preprocess = None, 
                       cleanupprocess = None):
        QThread.__init__(self)
        self.process = process
        self.postprocess = postprocess
        self.preprocess = preprocess
        self.cleanupprocess = cleanupprocess
        self.threaded = True
        self.exception = None
    def initialize(self):
        if self.preprocess:
          self.preprocess(self)
    def run(self):
        if self.process:
            if self.threaded:
                try:
                    self.process(self)
                except :
                    self.exception = ThreadTransferException(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2])
            else:
                self.process(self)
    def finalize(self):
        if self.threaded and not self.exception is None:
             raise self.exception
        if self.postprocess:
          self.postprocess(self)
    def apply(self):
        self.threaded = False
        self.initialize()
        self.run()
        self.finalize()
    def kill(self):
        self.quit()
        if self.cleanupprocess:
            self.cleanupprocess()
        self.killed.emit() 
    def __call__(self):
        self.start()


class ComputationTaskManager(QObject):
    endTask = pyqtSignal('PyQt_PyObject')
    killedTask = pyqtSignal('PyQt_PyObject')

    def __init__(self):
        self.computationThread = None
        self.computationMutex = QMutex()
        self.with_thread = False
    def toggleUseThread(self):
        self.with_thread = not self.with_thread
        if not self.with_thread :
            self.computationMutex.unlock()
    def finalizeTask(self):
        ct = self.computationThread
        if not self.computationThread is None:
            try:
                self.computationThread.finalize()
            except ThreadTransferException as e:
                self.graberror((e.exc_type,e.exc_value,e.exc_traceback))
            except:
                self.graberror()
            self.releaseCR()
            self.computationThread = None
        else:
            self.releaseCR()
        self.endTask.emit(ct)
    def abortTask(self):
        ct = self.computationThread
        self.computationThread = None
        self.releaseCR()
        self.endTask.emit(ct) 
        self.clear()
    def killTask(self):
        ct = self.computationThread
        ct.kill()
        self.computationThread = None
        self.releaseCR()
        self.killedTask.emit(ct) 
        self.clear()
    def registerTask(self,task):
        if self.computationThread is None:
            if self.with_thread:
                task.finished.connect(self.finalizeTask)
                if QT_VERSION <= 4:
                    task.terminated.connect(self.abortTask) 
                self.computationThread = task
                task.initialize()
                task.start()
            else:
              try:
                self.computationThread = task
                task.apply()
              except :
                self.graberror()
              self.computationThread = None
              self.releaseCR()
              self.endTask.emit(task) 
    def acquireCR(self):
        """ acquire computation ressources """
        if not self.computationMutex.tryLock():
            self.taskRunningEvent()
        else:
            self.acquireEvent()
        self.isRunning()
    def taskRunningEvent(self):
        raise Exception('A task is already running')
        pass
    def isRunning(self):
        if self.computationMutex.tryLock():
            self.computationMutex.unlock()
            return False
        else: return True
    def releaseCR(self):
        """ release computation ressources """
        self.computationMutex.tryLock()
        self.computationMutex.unlock()
        self.releaseEvent()
    def acquireEvent(self):
        pass
    def releaseEvent(self):
        pass
    def graberror(self, exc_info = None, displayDialog = True):
        """ grab error """
        if exc_info is None:
            exc_info = sys.exc_info()
        tb.print_exception(*exc_info)
        self.lastexception = exc_info[1]
        errmsg = self.getErrorMessage(exc_info)
        self.errorEvent(exc_info, errmsg, displayDialog)
        self.endErrorEvent()

        #if displayDialog:
        #    self.endErrorEvent(self.errorMessage(errmsg))
        #else:
        #    self.endErrorEvent(None)
    def getErrorMessage(self,exc_info):
        exception = exc_info[1] 
        msg = str(exc_info[1])
        if exc_info[0] == SyntaxError and len(msg) == 0:
            msg = exc_info[1].msg
        return str(exc_info[0].__name__)+':'+str(msg)
    def errorMessage(self,msg):
        return QMessageBox.warning(self,"Exception",msg,QMessageBox.Ok)
    def errorEvent(self, exc_info, errmsg,  displayDialog):
        pass
    def endErrorEvent(self):
        pass
