import openalea.lpy as lpy
from PyQt4.QtCore import QObject, QMutex, QCoreApplication,  SIGNAL
from PyQt4.QtGui import QTableWidgetItem, QStandardItem, QStandardItemModel, QTextCursor, QPixmap
from time import clock

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
        self.waitcond = QMutex()
        self.showHidden = False
        self.abort = False
        self.animation = False
        self.running = False
        self.animationTiming = 0.5
        self.srcView = self.debugWidget.right.srcView
        self.destView = self.debugWidget.right.destView
        self.ruleView = self.debugWidget.right.ruleView
        QObject.connect(self.debugWidget.right.nextDebugButton,SIGNAL('clicked()'),self.next)
        QObject.connect(self.debugWidget.right.animateDebugButton,SIGNAL('clicked()'),self.animate)
        QObject.connect(self.debugWidget.right.animationDebugSlider,SIGNAL('valueChanged(int)'),self.setAnimationTiming)
        QObject.connect(self.debugWidget.right.endDebugButton,SIGNAL('clicked()'),self.toEndDebug)
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
    def begin(self,src,direction):
        self.startDebugger()
        self.direction = direction
        txtDirection = "Forward" if self.direction == lpy.eForward else "Backward"
        self.debugWidget.right.directionLabel.setText(txtDirection)
        self.src = src
        self.lensrc = len(src)
        self.debugWidget.right.progressBar.setRange(0,self.lensrc)
        self.srcView.setText(str(self.src))
    def end(self,result):
        self.srcView.setText(str(self.src))
        self.destView.setText(str(result))
        self.active = True
        self.stopDebugger()
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
        self.lpywidget.codeeditor.gotoLine(rule.lineno)
        self.lpywidget.codeeditor.sidebar.addMarker(rule.lineno,QPixmap(':/images/icons/BreakPointGreen.png'))
        self.setProgress(pos_end if self.direction == lpy.eForward else pos_beg)
        self.updateArgs(dict(zip(rule.parameterNames(),args)))
        self.wait()
        self.lpywidget.codeeditor.sidebar.removeCurrentMarker(rule.lineno)
    def partial_match(self,pos_beg,pos_end,dest,rule,args):
        self.print_src(pos_beg,pos_end)        
        self.print_dest(dest)
        self.ruleView.setText(str(rule.lineno)+': '+rule.name()+' --> nothing produce!')
        self.lpywidget.codeeditor.gotoLine(rule.lineno)
        self.lpywidget.codeeditor.sidebar.addMarker(rule.lineno,QPixmap(':/images/icons/BreakPointGreen.png'))
        self.updateArgs(dict(zip(rule.parameterNames(),args)))
        self.wait()
        self.lpywidget.codeeditor.sidebar.removeCurrentMarker(rule.lineno)
    def identity(self,pos,dest):
        self.print_src(pos,pos+1)        
        self.print_dest(dest,1)
        self.ruleView.setText('ID')
        self.setProgress(pos+1 if self.direction == lpy.eForward else pos)
        self.updateArgs()
        self.wait()
    def wait(self):
        self.waitcond.lock()
        if self.animation :
            t = clock()
            while (clock()-t) < self.animationTiming and not self.waitcond.tryLock():
                QCoreApplication.instance().processEvents()
        else:
            while not self.waitcond.tryLock():            
                QCoreApplication.instance().processEvents()           
        self.waitcond.unlock()
        if self.abort == True:
            self.abort = False
            raise AbortDebugger()
    def next(self):
        self.waitcond.unlock()
    def animate(self):
        self.animation = True
        self.waitcond.unlock()        
    def toEndDebug(self):
        self.active = False
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
        model = QStandardItemModel(len(args), 2)
        model.setHorizontalHeaderLabels(["Name", "Value", "Type" ])
        indexitem = 0
        for name,val in args.iteritems():
            si = QStandardItem(name)
            si.setEditable(False)
            model.setItem(indexitem, 0, si)
            si = QStandardItem(repr(val))
            si.setEditable(False)
            model.setItem(indexitem, 1, si)
            try:
              si = QStandardItem(str(val.__class__.__name__))
            except:
              si = QStandardItem(str(type(val).__name__))
            si.setEditable(False)
            model.setItem(indexitem, 2, si)
            indexitem += 1
        table.setModel(model)
        table.model = model
    def clearTable(self,table):
        model = QStandardItemModel(0, 2)
        model.setHorizontalHeaderLabels(["Name", "Value", "Type" ])
        table.setModel(model)
        table.model = model

