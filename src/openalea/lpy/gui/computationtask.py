from PyQt4.QtCore import *
from PyQt4.QtGui import *

import traceback as tb
import sys

class ThreadTransferException (Exception):
    def __init__(self,exc_type,exc_value,exc_traceback):
        self.exc_type  = exc_type
        self.exc_value = exc_value
        self.exc_traceback = exc_traceback

class ComputationTask(QThread):
    def __init__(self, process = None, postprocess = None):
        QThread.__init__(self)
        self.process = process
        self.postprocess = postprocess
        self.threaded = True
        self.exception = None
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
        self.run()
        self.finalize()
    def __call__(self):
        self.start()


class ComputationTaskManager:
    def __init__(self):
        self.computationThread = None
        self.computationMutex = QMutex()
        self.with_thread = False
    def toggleUseThread(self):
        self.with_thread = not self.with_thread
        if not self.with_thread :
            self.computationMutex.unlock()
    def finalizeTask(self):
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
    def abortTask(self):
        self.computationThread = None
        self.releaseCR()
        self.clear()
    def registerTask(self,task):
        if self.computationThread is None:
            if self.with_thread:
                QObject.connect(task,SIGNAL('finished()'),self.finalizeTask)
                QObject.connect(task,SIGNAL('terminated()'),self.abortTask)
                self.computationThread = task
                task.start()
            else:
              try:
                self.computationThread = task
                task.apply()
              except :
                self.graberror()
              self.computationThread = None
              self.releaseCR()
    def acquireCR(self):
        """ acquire computation ressources """
        if not self.computationMutex.tryLock():
            self.taskRunningEvent()
        else:
            self.acquireEvent()
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
    def graberror(self, exc_info = None):
        """ grab error """
        if exc_info is None:
            exc_info = sys.exc_info()
        tb.print_exception(*exc_info)
        self.errorEvent(exc_info)
        QMessageBox.warning(self,"Exception",'An error occured:"'+exc_info[1].message+'"',QMessageBox.Ok)
                