# -*- python -*-
#
#       OpenAlea.Visualea: OpenAlea graphical user interface
#
#       Copyright 2006 - 2007 - 2008 INRIA - CIRAD - INRA  
#
#       File author(s): Samuel Dufour-Kowalski <samuel.dufour@sophia.inria.fr>
#                       Christophe Pradal <christophe.prada@cirad.fr>
#
#       Distributed under the CeCILL v2 License.
#       See accompanying file LICENSE.txt or copy at
#           http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
# 
#       OpenAlea WebSite : http://openalea.gforge.inria.fr
#
################################################################################


import sys
from PyQt4 import QtCore, QtGui
from PyQt4.QtGui import QApplication
from PyQt4.QtCore import QThread, QEvent

RedirectionEventId = QEvent.User+100
sys_stderr = None
sys_stdout = None
sys_stdin = None

try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False
    
class MultipleRedirection:
    """ Dummy file which redirects stream to multiple file """

    def __init__(self, *streams):
        """ The stream is redirect to the file list 'files' """

        self.streams = streams

    def write(self, str):
        """ Emulate write function """

        for stream in self.streams:
            stream.write(str)

class NoneOutput:
    """ Dummy file which redirects stream to nothing """

    def __init__(self):
        """ The stream is redirect to nothing """
        pass
    def write(self, str):
        """ Emulate write function """
        pass

class ThreadedRedirection:
    """ Dummy file which redirects stream to threaded gui output """

    def __init__(self,  guistream):
        """ The stream is redirect to the file list 'files' """

        self.guistream = guistream

    def write(self, str):
        """ Emulate write function """

        if self.guistream.thread() != QThread.currentThread():
            e = QEvent(QEvent.Type(RedirectionEventId))
            e.txt = str
            QApplication.postEvent(self.guistream,e)
            pass
        else:
            self.guistream.write(str)

            
class GraphicalStreamRedirection:
    """ Redirection of a stream as graphic output """
    
    def __init__(self):
        """  capture all interactive input/output """
        global sys_stdout, sys_stderr, sys_stdin
        if sys_stdout is None:  sys_stdout = sys.stdout
        if sys_stderr is None:  sys_stderr = sys.stderr
        if sys_stdin is None:   sys_stdin = sys.stdin
        sys.stdout   = ThreadedRedirection(self)
        sys.stderr   = ThreadedRedirection(self)
        sys.stdin    = self
        #self.multipleStdOutRedirection()

    def __del__(self):
        sys.stdout   = sys_stdout
        sys.stderr   = sys_stderr
        sys.stdin    = sys_stdin
        
    def customEvent(self,event):
        """ custom event processing. Redirection to write """
        if event.type() == RedirectionEventId:
            self.write(event.txt)
            
    def multipleStdOutRedirection(self,enabled = True):
        """ make multiple (sys.stdout/pyconsole) or single (pyconsole) redirection of stdout """
        if enabled:
            sys.stdout   = MultipleRedirection(sys_stdout, ThreadedRedirection(self))
        else:
            sys.stdout   = ThreadedRedirection(self)

    def selfAsStdOutRedirection(self):
        sys.stdout   = ThreadedRedirection(self)
        
    def sysAsStdOutRedirection(self):
        sys.stdout   = sys_stdout
        
    def noneAsStdOutRedirection(self):
        sys.stdout   = NoneOutput()
        
    def hasMultipleStdOutRedirection(self):
        return isinstance(sys.stdout, MultipleRedirection)
        
    def isSelfStdOutRedirection(self):
        return isinstance(sys.stdout, ThreadedRedirection)
        
    def isSysAsStdOutRedirection(self):
        return sys.stdout   == sys_stdout
        
    def isNoneAsStdOutRedirection(self):
        return isinstance(sys.stdout, NoneOutput)
        
        
        
    def multipleStdErrRedirection(self,enabled = True):
        """ make multiple (sys.stderr/pyconsole) or single (pyconsole) redirection of stderr """
        if enabled:
            sys.stderr   = MultipleRedirection(sys_stderr, ThreadedRedirection(self))
        else:
            sys.stderr   = ThreadedRedirection(self)
    def selfAsStdErrRedirection(self):
        sys.stderr   = ThreadedRedirection(self)
                
    def sysAsStdErrRedirection(self):
        sys.stderr   = sys_stderr
        
    def noneAsStdErrRedirection(self):
        sys.stderr   = NoneOutput()
        
    def hasMultipleStdErrRedirection(self):
        return isinstance(sys.stderr, MultipleRedirection)
        
    def isSelfStdErrRedirection(self):
        return isinstance(sys.stderr, ThreadedRedirection)
        
    def isSysAsStdErrRedirection(self):
        return sys.stderr   == sys_stderr
        
    def isNoneAsStdErrRedirection(self):
        return isinstance(sys.stderr, NoneOutput)

    def setOutputRedirection(self, selfoutput = True, sysoutput = True, outanderr = 3):
        if selfoutput:
            if sysoutput:
                if outanderr & 1 : self.multipleStdOutRedirection()
                if outanderr & 2 : self.multipleStdErrRedirection()                
            else:
                if outanderr & 1 :self.selfAsStdOutRedirection()
                if outanderr & 2 : self.selfAsStdErrRedirection() 
        elif sysoutput:
            if outanderr & 1 :self.sysAsStdOutRedirection()
            if outanderr & 2 : self.sysAsStdErrRedirection()
        else:
            if outanderr & 1 :self.noneAsStdOutRedirection()
            if outanderr & 2 : self.noneAsStdErrRedirection()
        