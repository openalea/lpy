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


#from openalea.plantgl.gui import qt
# RedirectionEventId = qt.QtCore.QEvent.User + 100

import sys
sys_stderr = None
sys_stdout = None
sys_stdin = None

try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False


class MultipleRedirection(object):

    """ Dummy file which redirects stream to multiple file """

    def __init__(self, *streams):
        """ The stream is redirect to the file list 'files' """

        self.streams = streams

    def write(self, str):
        """ Emulate write function """

        for stream in self.streams:
            stream.write(str)

    def flush(self):
        pass


class NoneOutput(object):

    """ Dummy file which redirects stream to nothing """

    def __init__(self):
        """ The stream is redirect to nothing """
        pass

    def write(self, str):
        """ Emulate write function """
        pass


class GraphicalStreamRedirection(object):

    """ Redirection of a stream as graphic output """

    def __init__(self, threadedstdout, threadedstderr):
        """  capture all interactive input/output """
        self.sys_stdout = sys.stdout
        self.sys_stderr = sys.stderr
        self.sys_stdin = sys.stdin

        self.stdout = threadedstdout
        self.stderr = threadedstderr

        # We suppose that 2 members self.stderr and self.stdout 
        # are defined by ipython cland represent threaded output stream.
        # This class makes it possible to activate them even 
        # if python execute code outside the ipython interpreter

        if not py2exe_release:
            sys.stderr = MultipleRedirection(self.sys_stderr, self.stderr)
        else:
            sys.stderr = self.stderr

        sys.stdout = self.stdout

    def __del__(self):
        sys.stdout = self.sys_stdout
        sys.stderr = self.sys_stderr
        sys.stdin  = self.sys_stdin

    def multipleStdOutRedirection(self, enabled=True):
        """ make multiple (sys.stdout/pyconsole) or single (pyconsole) redirection of stdout """
        if enabled:
            sys.stdout = MultipleRedirection(self.sys_stdout, self.stdout)
        else:
            sys.stdout = self.stdout

    def selfAsStdOutRedirection(self):
        sys.stdout = self.stdout

    def sysAsStdOutRedirection(self):
        sys.stdout = self.sys_stdout

    def noneAsStdOutRedirection(self):
        sys.stdout = NoneOutput()

    def hasMultipleStdOutRedirection(self):
        return isinstance(sys.stdout, MultipleRedirection)

    def isSelfStdOutRedirection(self):
        return sys.stdout == self.stdout

    def isSysStdOutRedirection(self):
        return sys.stdout == self.sys_stdout

    def isNoneAsStdOutRedirection(self):
        return isinstance(sys.stdout, NoneOutput)

    def multipleStdErrRedirection(self, enabled=True):
        """ make multiple (sys.stderr/pyconsole) or single (pyconsole) redirection of stderr """
        if enabled:
            sys.stderr = MultipleRedirection(self.sys_stderr, self.stderr)
        else:
            sys.stderr = self.stderr

    def selfAsStdErrRedirection(self):
        sys.stderr = self.stderr

    def sysAsStdErrRedirection(self):
        sys.stderr = self.sys_stderr

    def noneAsStdErrRedirection(self):
        sys.stderr = NoneOutput()

    def hasMultipleStdErrRedirection(self):
        return isinstance(sys.stderr, MultipleRedirection)

    def isSelfStdErrRedirection(self):
        return sys.stderr == self.stderr

    def isSysStdErrRedirection(self):
        return sys.stderr == self.sys_stderr

    def isNoneAsStdErrRedirection(self):
        return isinstance(sys.stderr, NoneOutput)

    def setOutputRedirection(self, selfoutput=True, sysoutput=True, outanderr=3):
        if selfoutput:
            if sysoutput:
                if outanderr & 1:
                    self.multipleStdOutRedirection(True)
                if outanderr & 2:
                    self.multipleStdErrRedirection(True)
            else:
                if outanderr & 1:
                    self.selfAsStdOutRedirection()
                if outanderr & 2:
                    self.selfAsStdErrRedirection()
        elif sysoutput:
            if outanderr & 1:
                self.sysAsStdOutRedirection()
            if outanderr & 2:
                self.sysAsStdErrRedirection()
        else:
            if outanderr & 1:
                self.noneAsStdOutRedirection()
            if outanderr & 2:
                self.noneAsStdErrRedirection()

    def flush(self):
        pass
