from openalea.vpltk.qt import qt

import traceback as tb
import sys

class ThreadTransferException (Exception):
    def __init__(self,exc_type,exc_value,exc_traceback):
        self.exc_type  = exc_type
        self.exc_value = exc_value
        self.exc_traceback = exc_traceback

class ComputationTask(qt.QtCore.QThread):
    def __init__(self, process = None, 
                       postprocess = None, 
                       preprocess = None, 
                       cleanupprocess = None):
        qt.QtCore.QThread.__init__(self)
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
                    self.exception = ThreadTransferException(sys.exc_type,sys.exc_value,sys.exc_traceback)
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
        self.emit(qt.QtCore.SIGNAL("killed()"))
    def __call__(self):
        self.start()


class ComputationTaskManager:
    def __init__(self):
        self.computationThread = None
        self.computationMutex = qt.QtCore.QMutex()
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
            except ThreadTransferException, e:
                self.graberror((e.exc_type,e.exc_value,e.exc_traceback))
            except:
                self.graberror()
            self.releaseCR()
            self.computationThread = None
        else:
            self.releaseCR()
        self.emit(qt.QtCore.SIGNAL('endTask(PyQt_PyObject)'),ct)
    def abortTask(self):
        ct = self.computationThread
        self.computationThread = None
        self.releaseCR()
        self.emit(qt.QtCore.SIGNAL('endTask(PyQt_PyObject)'),ct)
        self.clear()
    def killTask(self):
        ct = self.computationThread
        ct.kill()
        self.computationThread = None
        self.releaseCR()
        self.emit(qt.QtCore.SIGNAL('killedTask(PyQt_PyObject)'),ct)
        self.clear()
    def registerTask(self,task):
        if self.computationThread is None:
            if self.with_thread:
                qt.QtCore.QObject.connect(task,qt.QtCore.SIGNAL('finished()'),self.finalizeTask)
                qt.QtCore.QObject.connect(task,qt.QtCore.SIGNAL('terminated()'),self.abortTask)
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
              self.emit(qt.QtCore.SIGNAL('endTask(PyQt_PyObject)'),task)
    def acquireCR(self):
        """ acquire computation ressources """
        if not self.computationMutex.tryLock():
            self.taskRunningEvent()
        else:
            self.acquireEvent()
        self.isRunning()
    def taskRunningEvent(self):
        raise Exception('A task is already running')
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
        self.errorEvent(exc_info)
        errmsg = self.getErrorMessage(exc_info)
        if displayDialog:
            self.endErrorEvent(self.errorMessage(errmsg))
        else:
            self.endErrorEvent(None)
    def getErrorMessage(self,exc_info):
        print type(exc_info[1]), exc_info[1]
        msg = str(exc_info[1])
        if exc_info[0] == SyntaxError and len(msg) == 0:
            msg = exc_info[1].msg
        return 'An error occured:"'+str(exc_info[0].__name__)+':'+str(msg)+'"'
    def errorMessage(self,msg):
        return qt.QtGui.QMessageBox.warning(self,"Exception",msg,qt.QtGui.QMessageBox.Ok)
    def errorEvent(self,exc_info):
        pass
    def endErrorEvent(self,answer):
        pass
