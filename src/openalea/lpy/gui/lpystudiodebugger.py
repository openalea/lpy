import openalea.lpy as lpy
from openalea.vpltk.qt import qt
from time import clock
from lpycodeeditor import CodePointMarker, BreakPointMarker

import sys
import traceback as tb


class AbortDebugger(Exception):
    def __init__(self,txt=''):
        Exception.__init__(self,txt)

def toHtml(txt):
    return txt.replace('<','&lt;').replace('<','&gt;')
    
        
class LpyVisualDebugger (lpy.LpyDebugger):
    def __init__(self,lpywidget):
        lpy.LpyDebugger.__init__(self)
        self.lpywidget = lpywidget
        self.debugWidget = self.lpywidget.debugWidget
        self.waitcond = qt.QtCore.QMutex()
        self.showHidden = False
        self.abort = False
        self.animation = False
        self.running = False
        self.animationTiming = 0.5
        self.breakPointMonitor = True
        self.markerLine = None
        self.srcView = self.debugWidget.right.srcView
        self.destView = self.debugWidget.right.destView
        self.ruleView = self.debugWidget.right.ruleView
        qt.QtCore.QObject.connect(self.debugWidget.right.nextDebugButton,qt.QtCore.SIGNAL('clicked()'),self.next)
        qt.QtCore.QObject.connect(self.debugWidget.right.animateDebugButton,qt.QtCore.SIGNAL('clicked()'),self.animate)
        qt.QtCore.QObject.connect(self.debugWidget.right.animationDebugSlider,qt.QtCore.SIGNAL('valueChanged(int)'),self.setAnimationTiming)
        qt.QtCore.QObject.connect(self.debugWidget.right.endDebugButton,qt.QtCore.SIGNAL('clicked()'),self.continueDebug)
    def setAnimationTiming(self, value):
        self.animationTiming = value /1000.
    def startDebugger(self):
        self.lpywidget.debugDock.show()
        if not self.lpywidget.debugDock.isWindow():
          try:
            docks = self.lpywidget.tabifiedDockWidget(self.lpywidget.debugDock)
          except AttributeError, e:          
            docks = []
            id = self.lpywidget.interpreterDock
            if id.isVisible and not id.isWindow():
              docks.append(id)
          for d in docks:
                self.lpywidget.tabifyDockWidget(d,self.lpywidget.debugDock)
        self.simu = self.lpywidget.currentSimulation()
        self.debugWidget.setEnabled(True)
        self.running = True
    def stopDebugger(self):
        self.running = False
        self.animation = False
        self.debugWidget.setEnabled(False)
        self.lpywidget.debugDock.hide()
        self.breakPointMonitor = True
    def retrieveBreakPoints(self):
        self.clearBreakPoints()
        for bp in self.lpywidget.codeeditor.sidebar.getAllMarkers(BreakPointMarker):
            self.insertCodeBreakPointAt(bp)
    def breakPointChanged(self):
        self.breakPointMonitor = True
    def begin(self,src,direction):
        self.startDebugger()
        self.direction = direction
        txtDirection = "Forward" if self.direction == lpy.eForward else "Backward"
        self.debugWidget.right.directionLabel.setText(txtDirection)
        self.src = src
        self.lensrc = len(src)
        self.debugWidget.right.progressBar.setRange(0,self.lensrc)
        self.srcView.setText(str(self.src))
        qt.QtCore.QObject.connect(self.lpywidget.codeeditor.sidebar,qt.QtCore.SIGNAL('lineClicked(int)'),self.breakPointChanged)
    def end(self,result):
        self.srcView.setText(str(self.src))
        self.destView.setText(str(result))
        self.ruleView.setText('')
        self.alwaysStop = True
        self.stopDebugger()
        qt.QtCore.QObject.disconnect(self.lpywidget.codeeditor.sidebar,qt.QtCore.SIGNAL('lineClicked(int)'),self.breakPointChanged)
    def print_src(self,pos_beg,pos_end):
        txt = ''
        nbChar = 0
        if pos_beg > 0:        
            txt += self.src.str_slice(0,pos_beg)
            nbChar = len(txt)
            txt = toHtml(txt)
        beforeMarker = '<BIG><B>'
        afterMarker  = '</B></BIG>'
        pattern = self.src.str_slice(pos_beg,pos_end)
        nbChar += len(pattern)
        txt += beforeMarker+toHtml(pattern)+afterMarker
        if pos_end < self.lensrc:
            txt2 = self.src.str_slice(pos_end,self.lensrc)            
            nbChar2 = len(txt2)
            if nbChar2 > 50:
                nbChar2 = 50
            txt += toHtml(txt2)
        else: nbChar2 = 0
        view = self.srcView
        view.setText(txt)
        cursor = view.textCursor()
        cursor.setPosition(nbChar+nbChar2)
        view.setTextCursor(cursor)
        view.ensureCursorVisible()
    def print_dest(self,dest,length = 0):
        txt = ''
        if self.direction == lpy.eForward:
            l = len(dest)
            pos_beg = l-length
            if pos_beg > 0:        
                txt += toHtml(dest.str_slice(0,pos_beg))
            if length > 0:
                txt += '<BIG><B>'+toHtml(dest.str_slice(pos_beg,l))+'</B></BIG>'
            self.destView.setText(txt)
            sl = self.destView.horizontalScrollBar()
            sl.setValue(sl.maximum())
        else:
            l = len(dest)
            pos_beg = length
            if pos_beg > 0:        
                txt += '<BIG><B>'+toHtml(dest.str_slice(0,pos_beg))+'</B></BIG>'
            if l-pos_beg > 0:
                txt += toHtml(dest.str_slice(pos_beg,l))
            self.destView.setText(txt)
            sl = self.destView.horizontalScrollBar()
            sl.setValue(sl.minimum())
    def total_match(self,pos_beg,pos_end,dest,prod_length,rule,args):
        self.print_src(pos_beg,pos_end)
        self.print_dest(dest,prod_length)
        self.ruleView.setText(str(rule.lineno)+': '+rule.name())
        self.addMarker(rule.lineno)
        self.setProgress(pos_end if self.direction == lpy.eForward else pos_beg)
        self.updateArgs(dict(zip(rule.parameterNames(),args)))
        self.wait()
        self.delMarker()
    def partial_match(self,pos_beg,pos_end,dest,rule,args):
        self.print_src(pos_beg,pos_end)        
        self.print_dest(dest)
        self.ruleView.setText(str(rule.lineno)+': '+rule.name()+' --> nothing produce!')
        self.addMarker(rule.lineno)
        self.updateArgs(dict(zip(rule.parameterNames(),args)))
        self.wait()
        self.delMarker()
    def error_match(self,pos_beg,pos_end,dest,rule,args,exc_info):
        self.print_src(pos_beg,pos_end)        
        self.print_dest(dest)
        self.ruleView.setText(str(rule.lineno)+': '+rule.name()+' --> raise exception!')
        self.addMarker(rule.lineno)
        self.updateArgs(dict(zip(rule.parameterNames(),args)))        
        tb.print_exception(*exc_info)
        self.lpywidget.errorEvent(exc_info)
        errmsg = self.lpywidget.getErrorMessage(exc_info)
        res = qt.QtGui.QMessageBox.warning(self.lpywidget,"Exception",errmsg,qt.QtGui.QMessageBox.Abort,qt.QtGui.QMessageBox.Ignore)
        self.delMarker()
        if res == qt.QtGui.QMessageBox.Ignore:
            return True
        else : 
            return False
    def identity(self,pos,dest):
        self.print_src(pos,pos+1)        
        self.print_dest(dest,1)
        self.ruleView.setText('ID')
        self.setProgress(pos+1 if self.direction == lpy.eForward else pos)
        self.updateArgs()
        self.wait()
    def addMarker(self,lineno):
        self.lpywidget.codeeditor.gotoLine(lineno)
        self.lpywidget.codeeditor.sidebar.addMarkerAt(lineno,CodePointMarker)
        self.markerLine = lineno
    def delMarker(self):
        if self.lpywidget.codeeditor.sidebar.hasMarkerTypeAt(self.markerLine,CodePointMarker):
            self.lpywidget.codeeditor.sidebar.removeMarkerTypeAt(self.markerLine,CodePointMarker)
        self.markerLine = None
    def wait(self):
        self.waitcond.lock()
        if self.animation :
            t = clock()
            while (clock()-t) < self.animationTiming and not self.waitcond.tryLock():
                qt.QtCore.QCoreApplication.instance().processEvents()
        else:
            while not self.waitcond.tryLock():            
                qt.QtCore.QCoreApplication.instance().processEvents()           
        self.waitcond.unlock()
        if self.abort == True:
            self.abort = False
            if not self.markerLine is None:
                self.delMarker()
            raise AbortDebugger()
        if self.breakPointMonitor:
            self.retrieveBreakPoints()
    def next(self):
        self.waitcond.unlock()
    def animate(self):
        self.animation = True
        self.waitcond.unlock()        
    def continueDebug(self):
        self.alwaysStop = False
        self.waitcond.unlock()        
    def stop(self):
        if self.animation:
            self.animation = False
        else:
            self.endDebug()
    def endDebug(self):
        if not self.waitcond.tryLock():
            self.abort = True
        self.waitcond.unlock()
        self.stopDebugger()
    def setProgress(self,val):
        if self.direction == lpy.eForward:
            self.debugWidget.right.progressBar.setValue(val)
        else:
            self.debugWidget.right.progressBar.setValue(self.lensrc-val)
    def updateArgs(self,args=None):
        if not args is None:
            self.updateTable(self.debugWidget.left.argTable,args)
        else:
            self.clearTable(self.debugWidget.left.argTable)
        d = {}
        self.simu.lsystem.context().getNamespace(d)
        if not self.showHidden:
            lpyobjects = dir(lpy)
            d = dict([ (n,v) for n,v in d.iteritems() if not n in lpyobjects and (len(n) < 2 or n[0:2] != '__')])
        self.updateTable(self.debugWidget.left.globalTable,d)
    def updateTable(self,table,args):
        model = qt.QtGui.QStandardItemModel(len(args), 2)
        model.setHorizontalHeaderLabels(["Name", "Value", "Type" ])
        indexitem = 0
        for name,val in args.iteritems():
            si = qt.QtGui.QStandardItem(name)
            si.setEditable(False)
            model.setItem(indexitem, 0, si)
            si = qt.QtGui.QStandardItem(repr(val))
            si.setEditable(False)
            model.setItem(indexitem, 1, si)
            try:
              si = qt.QtGui.QStandardItem(str(val.__class__.__name__))
            except:
              si = qt.QtGui.QStandardItem(str(type(val).__name__))
            si.setEditable(False)
            model.setItem(indexitem, 2, si)
            indexitem += 1
        table.setModel(model)
        table.model = model
    def clearTable(self,table):
        model = qt.QtGui.QStandardItemModel(0, 2)
        model.setHorizontalHeaderLabels(["Name", "Value", "Type" ])
        table.setModel(model)
        table.model = model

