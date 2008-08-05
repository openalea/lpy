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

class MultipleRedirection:
    """ Dummy file which redirects stream to multiple file """

    def __init__(self, stream, guistream):
        """ The stream is redirect to the file list 'files' """

        self.stream = stream
        self.guistream = guistream

    def write(self, str):
        """ Emulate write function """

        self.stream.write(str)
        if self.guistream.thread() != QThread.currentThread():
            e = QEvent(QEvent.Type(RedirectionEventId))
            e.txt = str
            QApplication.postEvent(self.guistream,e)
            pass
        else:
            self.guistream.write(str)

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
        #sys.stdout   = ThreadedRedirection(self)
        sys.stdout   = MultipleRedirection(sys.stdout, self)
        sys.stderr   = MultipleRedirection(sys.stderr, self)
        sys.stdin    = self
    
    def customEvent(self,event):
        """ custom event processing. Redirection to write """
        if event.type() == RedirectionEventId:
            self.write(event.txt)
        else:
            self.__class__.__bases__[0].customEvent(self)
            
