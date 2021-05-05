from openalea.plantgl.gui.qt import qt
from openalea.plantgl.gui.qt.QtCore import QMimeData, QObject, QPoint, QRegExp, QTimer, Qt, pyqtSignal
from openalea.plantgl.gui.qt.QtGui import QColor, QFont, QPainter, QPalette, QPen, QPixmap, QSyntaxHighlighter, QTextCharFormat, QTextCursor, QTextDocument, QTextOption
from openalea.plantgl.gui.qt.QtWidgets import QLabel, QTextEdit, QWidget

class LineData:
    def __init__(self,i = None,p = None):
        self.imbricatedParanthesis = i
        self.previousProductionState = p

class IdGenerator:
    def __init__(self):
        self.id = 0
        self.stack = []
    def __call__(self):
        if len(self.stack) > 1 :
            return self.stack.pop(0)
        else :
            i = self.id
            self.id += 1
            return i
    def release(self,i):
        self.stack.append(i)
        
class LpySyntaxHighlighter(QSyntaxHighlighter):
    def __init__(self,editor):
        QSyntaxHighlighter.__init__(self,editor)
        self.rules = []
        self.lpykeywordFormat = QTextCharFormat()
        self.lpykeywordFormat.setForeground(Qt.darkMagenta)
        self.lpykeywordFormat.setFontWeight(QFont.Bold)
        self.lpykeywords = ['Axiom:','production','homomorphism','interpretation',
                            'decomposition','endlsystem','group','endgroup',
                            'derivation length','maximum depth','produce','nproduce','nsproduce','makestring','-->',
                            'consider:','ignore:','forward','backward','isForward','extern',
                            'Start','End','StartEach','EndEach','getGroup','useGroup','getIterationNb',
                            'module','-static->','@static','lpyimport','\%pastefile']
        for pattern in self.lpykeywords:
            self.rules.append((QRegExp(pattern),self.lpykeywordFormat))
        self.keywordFormat = QTextCharFormat()
        self.keywordFormat.setForeground(Qt.blue)
        self.keywordFormat.setFontWeight(QFont.Bold)
        import keyword
        self.pykeywords = keyword.kwlist + ['None','range','xrange', 'True','False','int','float','str','tuple','list']
        for pattern in self.pykeywords:
            self.rules.append((QRegExp(pattern),self.keywordFormat))
        self.delimiterFormat = QTextCharFormat()
        self.delimiterFormat.setForeground(Qt.darkBlue)
        self.delimiterFormat.setFontWeight(QFont.Bold)
        self.delimiterkeywords = '[](){}+-*/:<>='
        self.exprules = []
        self.prodFormat = QTextCharFormat()
        self.prodFormat.setForeground(Qt.black)
        self.prodFormat.setFontWeight(QFont.Bold)
        self.prodkeywords = ['Axiom:','module','produce','nproduce','nsproduce','makestring','-->','-static->','ignore:','consider:']
        for pattern in self.prodkeywords:
            self.exprules.append((QRegExp(pattern+'.*$'),len(pattern),self.prodFormat,0))
        self.funcFormat = QTextCharFormat()
        self.funcFormat.setForeground(Qt.magenta)
        self.exprules.append((QRegExp('def[ \t]+.*\('),3,self.funcFormat,1))
        self.stringFormat = QTextCharFormat()
        self.stringFormat.setForeground(Qt.darkGray)
        self.exprules.append((QRegExp('\"[^\"]*\"'),0,self.stringFormat,0))
        self.exprules.append((QRegExp("\'[^\']*\'"),0,self.stringFormat,0))
        self.tabFormat = QTextCharFormat()
        self.tabFormat.setBackground(QColor(220,220,220))
        self.spaceFormat = QTextCharFormat()
        self.spaceFormat.setBackground(QColor(240,240,240))
        self.tabRule = QRegExp("^[ \t]+")
        self.numberFormat = QTextCharFormat()
        self.numberFormat.setForeground(Qt.red)
        self.exprules.append((QRegExp('\d+(\.\d+)?(e[\+\-]?\d+)?'),0,self.numberFormat,0))        
        self.commentFormat = QTextCharFormat()
        self.commentFormat.setForeground(Qt.darkGreen)
        self.lsysruleExp = [QRegExp('.+:'),QRegExp('.+\-\->'), QRegExp('.+\-static\->')]
        self.commentExp = QRegExp('#.+$')
        self.ruleCommentExp = QRegExp('[ \t]+#.+$')
        self.prodbegExp =  QRegExp('[n]produce[ \t]*.')
        self.setCurrentBlockState(0)
        self.activated = True
        self.tabviewactivated = True
        self.lineid = IdGenerator()
        self.linedata = {}
    def setDocument(self,doc):
        QSyntaxHighlighter.setDocument(self,doc)
    def genlineid(self):
        return (self.lineid() << 2) +2
    def releaselinedata(self,lid):
        del self.linedata[lid]
        i =  ((lid-2) >> 2) 
        self.lineid.release(i)    
    def setActivation(self,value):
        self.activated = value
        self.rehighlight()
    def setTabViewActivation(self,value):
        self.tabviewactivated = value
        self.rehighlight()
    def highlightBlock(self,text):
      text = str(text)
      if self.activated:
        lentxt = len(text)
        prevst = self.currentBlockState() 
        if text.find('production:') >= 0:
            self.setCurrentBlockState(1)
        elif text.find('endlsystem') >= 0:
            self.setCurrentBlockState(0)
        elif self.previousBlockState() == -1:
            self.setCurrentBlockState(0)
        elif self.previousBlockState() & 2:
            st = self.linedata.get(self.previousBlockState(),None)
            if not st is None:
               imbricatedParanthesis = st.imbricatedParanthesis
            for i,c in enumerate(text):
                if c == '(': imbricatedParanthesis += 1
                if c == ')': 
                    imbricatedParanthesis -= 1
                    if imbricatedParanthesis <= 0:
                        break
            if imbricatedParanthesis <= 0:
                self.setFormat(0,i,self.prodFormat)
                lid = self.currentBlockState()
                self.setCurrentBlockState(st.previousProductionState)
            else:
                self.setFormat(0,text.size(),self.prodFormat)
                lid = self.currentBlockState()
                if lid < 0 or (lid & 2) == 0 :
                    lid = self.genlineid()
                else :
                    if self.linedata[lid].imbricatedParanthesis != imbricatedParanthesis :
                       self.releaselinedata(lid)
                       lid = self.genlineid()
                self.linedata[lid] = LineData(imbricatedParanthesis,st.previousProductionState)
                self.setCurrentBlockState(lid)
        else:
            self.setCurrentBlockState(self.previousBlockState())
        if prevst > 0 and (prevst & 2) and self.currentBlockState() < 2:
            self.releaselinedata(prevst)
        for i,c in enumerate(text):
            if c in self.delimiterkeywords:
                self.setFormat(i, 1, self.delimiterFormat)
        if self.currentBlockState() == 1:
            if lentxt > 0 and not text[0] in " \t":
                for ruleExp in self.lsysruleExp:
                    index = ruleExp.indexIn(text)
                    if index >= 0:
                        length = ruleExp.matchedLength()
                        self.setFormat(index, length, self.prodFormat)
                        break
        for rule in self.rules:
            expression = rule[0]
            index = expression.indexIn(text)
            while index >= 0:
                length = expression.matchedLength()
                if ((index == 0 or not text[index-1].isalnum()) and 
                   (index+length == lentxt or not text[index+length].isalnum())):
                    self.setFormat(index, length, rule[1])
                index = expression.indexIn(text, index + length)
        for rule in self.exprules:
            expression = rule[0]
            index = expression.indexIn(text)
            while index >= 0:
                length = expression.matchedLength()
                if index == 0 or not text[index-1].isalnum():
                    self.setFormat(index+rule[1], length-rule[1]-rule[3], rule[2])
                    mt = expression.cap(0)
                    ei = self.prodbegExp.indexIn(mt)
                    if ei >= 0 and str(self.prodbegExp.cap(0))[-1] == '(':
                        previousProductionState = self.previousBlockState()
                        imbricatedParanthesis = 1
                        for c in mt[ei+len(self.prodbegExp.cap(0))+1:]:
                          if c == '(': imbricatedParanthesis += 1
                          if c == ')': 
                            imbricatedParanthesis -= 1
                            if imbricatedParanthesis <= 0:
                               self.setCurrentBlockState(previousProductionState)
                               break
                        if imbricatedParanthesis > 0:
                            lid = self.genlineid()
                            self.setCurrentBlockState(lid)
                            self.linedata[lid] = LineData(imbricatedParanthesis,previousProductionState)                        
                index = expression.indexIn(text, index + length)
        if self.tabviewactivated:
            index = self.tabRule.indexIn(text)
            if index >= 0:
                length = self.tabRule.matchedLength()
                for i in range(index,index+length):
                    if text[i] == '\t':
                        self.setFormat(i, 1 , self.tabFormat)
                    else:
                        self.setFormat(i, 1 , self.spaceFormat)
        commentExp = self.commentExp #if self.currentBlockState() == 0 else self.ruleCommentExp
        index = commentExp.indexIn(text)
        while index >= 0:
            length = commentExp.matchedLength()
            self.setFormat(index, length, self.commentFormat)
            index = commentExp.indexIn(text,index+length+2)
    
class Margin(QWidget):
    
    lineClicked = pyqtSignal(int)

    def __init__(self,parent,editor):
        QWidget.__init__(self,parent)
        self.editor = editor
        self.showLines = True
        self.markers = {}
        self.markerStack = {}
        self.markerType = {}

    def paintEvent( self, paintEvent ):
        if self.showLines:
            maxheight = self.editor.viewport().height()
            maxline = self.editor.document().blockCount()
            painter = QPainter(self)
            painter.setPen(QPen(QColor(100,100,100)))
            h = 0
            line = -1
            while h < maxheight and line < maxline:
                cursor = self.editor.cursorForPosition(QPoint(1,h))
                nline = cursor.blockNumber()+1
                rect = self.editor.cursorRect(cursor)
                if nline > line:
                    line = nline
                    painter.drawText(0,rect.top()+2,40,rect.height()+2, Qt.AlignHCenter|Qt.AlignTop,str(line))
                    m = self.markers.get(line,None)
                    if not m is None:
                        lm = self.markerStack.get(line,None)
                        if not lm is None:
                            for slm in lm:
                                painter.drawPixmap(32,rect.top()+2,self.markerType[slm])
                        painter.drawPixmap(32,rect.top()+2,self.markerType[m])
                h = rect.top()+rect.height()+1
            painter.end()
    
    def mousePressEvent( self, event ):
        line = self.editor.cursorForPosition(event.pos()).blockNumber() 
        self.lineClicked.emit(line+1)
        #self.emit(SIGNAL("lineClicked(int)"),)
    def clear( self ):
        self.removeAllMarkers()
        self.markerType = {}
    def hasMarker(self):
        return len(self.markers) != 0
    def setMarkerAt(self,line,id):
        self.markers[line] = id
        if line in self.markerStack:
            del self.markerStack[line]
        self.update()
    def hasMarkerAt(self,line):
        return line in self.markers
    def hasMarkerTypeAt(self,line,id):
        if line in self.markers :
            if self.markers[line] == id: return True
            if line in self.markerStack:
                if id in self.markerStack[line]:
                    return True
        return False
    def getCurrentMarkerAt(self,line):
        return self.markers[line]
    def removeCurrentMarkerAt(self,line):
        del self.markers[line]
        if line in self.markerStack:
            self.markers[line] = self.markerStack[line].pop()
            if len(self.markerStack[line]) == 0:
                del self.markerStack[line]
        self.update()
    def removeMarkerTypeAt(self,line,id):
        if self.markers[line] == id:
            self.removeCurrentMarkerAt(line)
        else:
            self.markerStack[line].remove(id)
            if len(self.markerStack[line]) == 0:
                del self.markerStack[line]
        self.update()
    def removeAllMarkersAt(self,line):
        if line in self.marker:
            del self.markers[line]
        if line in self.markerStack:
            del self.markerStack[line]        
        self.update()
    def removeAllMarkers(self):
        self.markers = {}
        self.markerStack = {}        
        self.update()
    def addMarkerAt(self,line,id):
        val = self.markers.get(line,None)
        if not val is None:
            if line not in self.markerStack:
                self.markerStack[line] = []
            self.markerStack[line].append(val)
        self.markers[line] = id
        self.update()    
    def appendMarkerAt(self,line,id):
        val = self.markers.get(line,None)
        if not val is None:
            if line not in self.markerStack:
                self.markerStack[line] = []
            self.markerStack[line].append(id)
        else:
            self.markers[line] = id
        self.update()    
    def defineMarker(self,id,pixmap):
        self.markerType[id] = pixmap
    def getAllMarkers(self,id):
        return set([l for l,lid in self.markers.items() if id == lid]).union(set([l for l,lids in self.markerStack.items() if id in lids]))
    def decalMarkers(self,line,decal = 1):
        markers = {}
        markerStack = {}
        if decal < 0:
          for l,v in self.markers.items():
            if l <= line+decal:
                markers[l] = v
            elif l > line:
                markers[l+decal] = v
          for l,v in self.markerStack.items():
            if l <= line+decal:
                markerStack[l] = v
            elif l > line:
                markerStack[l+decal] = v        
        if decal > 0:
          for l,v in self.markers.items():
            if l < line:
                markers[l] = v
            else:
                markers[l+decal] = v
          for l,v in self.markerStack.items():
            if l < line:
                markerStack[l] = v
            else:
                markerStack[l+decal] = v
        if decal != 0:
            self.markers = markers
            self.markerStack = markerStack
            self.update()
    def saveState(self,obj):
        obj.markersState = (self.markers,self.markerStack)
    def restoreState(self,obj):
        if hasattr(obj,'markersState'):
            self.markers,self.markerStack = obj.markersState
        else:
            self.removeAllMarkers()
        
ErrorMarker,BreakPointMarker,CodePointMarker = list(range(3))

class LpyCodeEditor(QTextEdit):
    def __init__(self,parent):
        QTextEdit.__init__(self,parent)
        self.editor = None
        self.setAcceptDrops(True)
        self.setWordWrapMode(QTextOption.WrapAnywhere)
        self.findEdit = None
        self.gotoEdit = None
        self.matchCaseButton = None
        self.wholeWordButton = None
        self.nextButton = None
        self.previousButton = None
        self.replaceEdit = None
        self.replaceButton = None
        self.replaceAllButton = None
        self.replaceTab = True
        self.indentation = '  '
        self.hasError = False
        self.defaultdoc = self.document().clone()
        self.setDocument(self.defaultdoc)
        self.syntaxhighlighter = LpySyntaxHighlighter(self)
        self.zoomFactor = 0
        self.editionFont = None
        #self.syntaxhighlighter.setDocument(self.defaultdoc)
    def initWithEditor(self,lpyeditor):
        self.editor = lpyeditor
        self.findEdit = lpyeditor.findEdit
        self.frameFind = lpyeditor.frameFind
        self.gotoEdit = lpyeditor.gotoEdit
        self.matchCaseButton = lpyeditor.matchCaseButton
        self.wholeWordButton = lpyeditor.wholeWordButton
        self.nextButton = lpyeditor.findNextButton
        self.previousButton = lpyeditor.findPreviousButton
        self.replaceEdit = lpyeditor.replaceEdit
        self.replaceButton = lpyeditor.replaceButton
        self.replaceAllButton = lpyeditor.replaceAllButton
        self.statusBar = lpyeditor.statusBar()
        self.positionLabel = QLabel(self.statusBar)
        self.statusBar.addPermanentWidget(self.positionLabel)
        self.findEdit.textEdited.connect(self.findText)

        lpyeditor.actionFind.triggered.connect(self.focusFind)
        self.findEdit.returnPressed.connect(self.setFocus)
        self.gotoEdit.returnPressed.connect(self.gotoLineFromEdit)
        self.gotoEdit.returnPressed.connect(self.setFocus)
        self.previousButton.pressed.connect(self.findPreviousText)
        self.nextButton.pressed.connect(self.findNextText)
        self.replaceButton.pressed.connect(self.replaceText)
        self.replaceAllButton.pressed.connect(self.replaceAllText)
        self.cursorPositionChanged.connect(self.printCursorPosition)
        lpyeditor.actionZoomIn.triggered.connect(self.zoomInEvent)
        lpyeditor.actionZoomOut.triggered.connect(self.zoomOutEvent)
        lpyeditor.actionNoZoom.triggered.connect(self.resetZoom)
        lpyeditor.actionGoto.triggered.connect(self.setLineInEdit)

        self.defaultEditionFont = self.currentFont()
        self.defaultPointSize = self.currentFont().pointSize()
        self.setViewportMargins(50,0,0,0)
        self.sidebar = Margin(self,self)
        self.sidebar.setGeometry(0,0,50,100)
        self.sidebar.defineMarker(ErrorMarker,QPixmap(':/images/icons/warningsErrors16.png'))
        self.sidebar.defineMarker(BreakPointMarker,QPixmap(':/images/icons/BreakPoint.png'))
        self.sidebar.defineMarker(CodePointMarker,QPixmap(':/images/icons/greenarrow16.png'))
        self.sidebar.show() 
        self.sidebar.lineClicked.connect(self.checkLine)
    def checkLine(self,line):
        self.statusBar.showMessage("Line "+str(line)+" clicked",2000)
        if self.sidebar.hasMarkerAt(line):
            if self.hasError and self.errorLine == line:
                self.clearErrorHightlight()
            elif self.sidebar.hasMarkerTypeAt(line,BreakPointMarker):
                self.sidebar.removeMarkerTypeAt(line,BreakPointMarker)
            else:
                self.sidebar.appendMarkerAt(line,BreakPointMarker)
        else:
            self.sidebar.setMarkerAt(line,BreakPointMarker)
    def resizeEvent(self,event):
        self.sidebar.setGeometry(0,0,48,self.height())
        QTextEdit.resizeEvent(self,event)
    def scrollContentsBy(self,dx,dy):
        self.sidebar.update()
        self.sidebar.setFont(QFont(self.currentFont()))
        QTextEdit.scrollContentsBy(self,dx,dy)
    def focusInEvent ( self, event ):
        if self.editor : self.editor.currentSimulation().monitorfile()
        return QTextEdit.focusInEvent ( self, event )
    def setReplaceTab(self,value):
        self.replaceTab = value
    def tabSize(self):
        return len(self.indentation)
    def setTabSize(self, value):
        assert value > 0
        self.indentation = ' '*value
        self.setTabStopWidth(value*self.currentFont().pointSize())
    def setLpyDocument(self,document):
        self.syntaxhighlighter.setDocument(document)
        LpyCodeEditor.setDocument(self,document)
        self.syntaxhighlighter.rehighlight()
        self.applyZoom()
        if not self.editionFont is None and self.editionFont!= document.defaultFont() :
            document.setDefaultFont(self.editionFont)
    def zoomInEvent(self):
        self.zoomFactor += 1
        self.zoomIn()
    def zoomOutEvent(self):
        self.zoomFactor -= 1
        self.zoomOut()
    def resetZoom(self):
        if self.zoomFactor > 0:
            self.zoomOut(self.zoomFactor)
        elif self.zoomFactor < 0:
            self.zoomIn(-self.zoomFactor)
        self.zoomFactor = 0
    def applyZoom(self):
        self.zoomIn()
        self.zoomOut()
    def printCursorPosition(self):
        cursor = self.textCursor()
        self.positionLabel.setText('Line '+str(cursor.blockNumber()+1)+', Column '+str(cursor.columnNumber())+' ('+str(cursor.position())+')')
    def keyPressEvent(self,event):
        if self.hasError:
            self.clearErrorHightlight()
        lcursor = self.textCursor()
        bbn = lcursor.blockNumber()
        if lcursor.selectionStart() == lcursor.selectionEnd() and (event.key() == Qt.Key_Delete or event.key() == Qt.Key_Backspace):
            if event.key() == Qt.Key_Backspace:
                lcursor.movePosition(QTextCursor.PreviousCharacter,QTextCursor.KeepAnchor)
            else:
                lcursor.movePosition(QTextCursor.NextCharacter,QTextCursor.KeepAnchor)
        if event.key() == Qt.Key_Tab and lcursor.hasSelection():
            if event.modifiers() == Qt.NoModifier:
                self.tab()
            else:
                self.untab()
        else:
            seltxt = lcursor.selection().toPlainText()
            sbn = seltxt.count('\n')
            rev = self.document().revision()
            QTextEdit.keyPressEvent(self,event)
            if event.key() == Qt.Key_Return or event.key() == Qt.Key_Enter:
                self.returnEvent()
                sbn -=1
            elif event.key() == Qt.Key_Tab :
                self.tabEvent()
            if rev != self.document().revision():
                self.sidebar.decalMarkers(bbn+sbn,-sbn)        
    def returnEvent(self):
        cursor = self.textCursor()
        beg = cursor.selectionStart()
        end = cursor.selectionEnd()
        if beg == end:
            pos = cursor.position()
            ok = cursor.movePosition(QTextCursor.PreviousBlock,QTextCursor.MoveAnchor)
            if not ok: return
            txtok = True
            txt = ''
            while txtok:
                ok = cursor.movePosition(QTextCursor.NextCharacter,QTextCursor.KeepAnchor)
                if not ok: break
                txt2 = str(cursor.selection().toPlainText())
                txtok = (txt2[-1] in ' \t')
                if txtok:
                    txt = txt2
            cursor.setPosition(pos)
            ok = cursor.movePosition(QTextCursor.PreviousBlock,QTextCursor.MoveAnchor)
            if ok:
                ok = cursor.movePosition(QTextCursor.EndOfBlock,QTextCursor.MoveAnchor)
                if ok:
                    txtok = True
                    while txtok:
                        ok = cursor.movePosition(QTextCursor.PreviousCharacter,QTextCursor.KeepAnchor)
                        if not ok: break
                        txt2 = str(cursor.selection().toPlainText())
                        txtok = (txt2[0] in ' \t')
                        if not txtok:
                            if txt2[0] == ':':
                                txt += self.indentation
            cursor.setPosition(pos)
            cursor.joinPreviousEditBlock()
            cursor.insertText(txt)
            cursor.endEditBlock()
    def tabEvent(self):
        if self.replaceTab:
            cursor = self.textCursor()
            if cursor.hasSelection():
                cursor.joinPreviousEditBlock()
                cursor.deletePreviousChar() 
                self.tab(cursor)
                cursor.endEditBlock()        
            else:
                cursor.joinPreviousEditBlock()
                cursor.deletePreviousChar() 
                cursor.insertText(self.indentation)
                cursor.endEditBlock()        
    def getFindOptions(self):
        options = QTextDocument.FindFlags()
        if self.matchCaseButton.isChecked():
            options |= QTextDocument.FindCaseSensitively
        if self.wholeWordButton.isChecked():
            options |= QTextDocument.FindWholeWords
        return options
    def cursorAtStart(self):
        cursor = self.textCursor()
        cursor.setPosition(0,QTextCursor.MoveAnchor)
        self.setTextCursor(cursor)            
    def focusFind(self):
        if not self.frameFind.isVisible():
            self.setfindEditColor(QColor(255,255,255))
            self.frameFind.show()
            cursor = self.textCursor()
            if cursor.hasSelection() :
                self.findEdit.setText(cursor.selectedText())
            self.findEdit.selectAll()
            self.findEdit.setFocus()
        else:
            cursor = self.textCursor()
            if not cursor.hasSelection() or cursor.selectedText() == self.findEdit.text():
                self.findNextText()
            elif cursor.hasSelection():
                self.findEdit.setText(cursor.selectedText())
                self.findEdit.selectAll()
                self.findEdit.setFocus()
            
    def findNextText(self):        
        txt = self.findEdit.text()
        found = self.find(txt,self.getFindOptions())
        if found:
            self.setFocus()
            self.setfindEditColor(QColor(255,255,255))
        else:
            #self.statusBar.showMessage('Text not found !',2000)
            self.findEndOFFile()
            self.cursorAtStart()
            self.setfindEditColor(QColor(255,100,100))
    def setfindEditColor(self,color):
        palette = self.findEdit.palette()
        palette.setColor(QPalette.Base,color)
        self.findEdit.setPalette(palette)
    def findEndOFFile(self):
            q = QLabel('Text not found !')
            q.setPixmap(QPixmap(':/images/icons/wrap.png'))
            self.statusBar.addWidget(q)
            self.statusBar.showMessage('     End of page found, restart from top !',2000)
            QTimer.singleShot(2000,lambda : self.statusBar.removeWidget(q))            
    def findPreviousText(self):
        txt = self.findEdit.text()
        found = self.find(txt,QTextDocument.FindBackward|self.getFindOptions())
        if found:
            self.setFocus()
        else:
            
            self.findEndOFFile()
            self.cursorAtStart()
    def findText(self,txt):
        cursor = self.textCursor()
        cursor.setPosition(0,QTextCursor.MoveAnchor)
        self.setTextCursor(cursor)
        self.find(txt,self.getFindOptions())
    def replaceText(self):
        txt = self.findEdit.text()
        cursor = self.textCursor()
        if cursor.selectedText() == txt:
            cursor.beginEditBlock()
            cursor.removeSelectedText() 
            cursor.insertText(self.replaceEdit.text())
            cursor.endEditBlock()
            self.find(txt,self.getFindOptions())
        else:
            self.findNextText()
    def replaceAllText(self):
        txt = self.findEdit.text()
        cursor = self.textCursor()
        if cursor.selectedText() == txt:        
            nboccurrence = 1
            cursor.beginEditBlock()
            cursor.removeSelectedText()
            cursor.insertText(self.replaceEdit.text())
            found = self.find(txt,self.getFindOptions())
            while found :
                nboccurrence += 1
                cursor = self.textCursor()
                cursor.removeSelectedText()
                cursor.insertText(self.replaceEdit.text())
                found = self.find(txt,self.getFindOptions())            
            cursor.endEditBlock()
            self.statusBar.showMessage('Replace '+str(nboccurrence)+' occurrences.',5000)
            self.cursorAtStart()
        else:
            self.findNextText()
    def setSyntaxHighLightActivation(self,value):
        self.syntaxhighlighter.setActivation(value)
    def isSyntaxHighLightActivated(self):
        return self.syntaxhighlighter.activated
    def setTabHighLightActivation(self,value):
        self.syntaxhighlighter.setTabViewActivation(value)
    def isTabHighLightActivated(self):
        return self.syntaxhighlighter.tabviewactivated
    def canInsertFromMimeData(self,source):
        if source.hasUrls():
            return True
        else: return source.hasText()
            # return qt.QtGui.QTextEdit.canInsertFromMimeData(self,source)
    def insertFromMimeData(self,source):
        if source.hasUrls():
            if not self.editor is None:
                url = source.urls()[0]
                if len(url.host()) == 0 and url.path().startswith("/.file/id="):
                    import os
                    cmd = """osascript -e 'get posix path of posix file "{}" -- kthxbai'""".format(url.path())
                    path = os.popen(cmd).read().strip()
                else:
                    path = url.toLocalFile()
                self.editor.openfile(path)
        else : 
            nsource = QMimeData()
            nsource.setText(source.text())
            return QTextEdit.insertFromMimeData(self,nsource)
    def comment(self):
        cursor = self.textCursor()
        beg = cursor.selectionStart()
        end = cursor.selectionEnd()
        pos = cursor.position()
        cursor.beginEditBlock() 
        cursor.setPosition(beg,QTextCursor.MoveAnchor)
        cursor.movePosition(QTextCursor.StartOfBlock,QTextCursor.MoveAnchor)
        while cursor.position() <= end:
            cursor.insertText('#')
            oldpos = cursor.position()
            cursor.movePosition(QTextCursor.NextBlock,QTextCursor.MoveAnchor)
            if cursor.position() == oldpos:
                break
            end+=1
        cursor.endEditBlock()
        cursor.setPosition(pos,QTextCursor.MoveAnchor)
    def uncomment(self):
        cursor = self.textCursor()
        beg = cursor.selectionStart()
        end = cursor.selectionEnd()
        pos = cursor.position()
        cursor.beginEditBlock()
        cursor.setPosition(beg,QTextCursor.MoveAnchor)
        cursor.movePosition(QTextCursor.StartOfBlock,QTextCursor.MoveAnchor)
        while cursor.position() <= end:
            m = cursor.movePosition(QTextCursor.NextCharacter,QTextCursor.KeepAnchor)
            if True:
                if cursor.selectedText() == '#':
                        cursor.deleteChar()
                end-=1
            cursor.movePosition(QTextCursor.Down,QTextCursor.MoveAnchor)
            cursor.movePosition(QTextCursor.Left,QTextCursor.MoveAnchor)
        cursor.endEditBlock()
        cursor.setPosition(pos,QTextCursor.MoveAnchor)
    def tab(self, initcursor = None):
        if initcursor == False:
            initcursor = None
        cursor = self.textCursor() if initcursor is None else initcursor
        beg = cursor.selectionStart()
        end = cursor.selectionEnd()
        pos = cursor.position()
        if not initcursor : cursor.beginEditBlock()
        cursor.setPosition(beg,QTextCursor.MoveAnchor)
        cursor.movePosition(QTextCursor.StartOfBlock,QTextCursor.MoveAnchor)
        while cursor.position() <= end :
            if self.replaceTab:
                cursor.insertText(self.indentation)
                end+=len(self.indentation)
            else:
                cursor.insertText('\t')
                end+=1
            oldpos = cursor.position()
            cursor.movePosition(QTextCursor.NextBlock,QTextCursor.MoveAnchor)
            if cursor.position() == oldpos:
                break
        if not initcursor : cursor.endEditBlock()
        cursor.setPosition(pos,QTextCursor.MoveAnchor)
    def untab(self):
        cursor = self.textCursor()
        beg = cursor.selectionStart()
        end = cursor.selectionEnd()
        pos = cursor.position()
        cursor.beginEditBlock()
        cursor.setPosition(beg,QTextCursor.MoveAnchor)
        cursor.movePosition(QTextCursor.StartOfBlock,QTextCursor.MoveAnchor)
        while cursor.position() <= end:
            m = cursor.movePosition(QTextCursor.NextCharacter,QTextCursor.KeepAnchor)
            if cursor.selectedText() == '\t':
                cursor.deleteChar()
            else:
                for i in range(len(self.indentation)-1):
                    b = cursor.movePosition(QTextCursor.NextCharacter,QTextCursor.KeepAnchor)
                    if not b : break
                if cursor.selectedText() == self.indentation:
                    cursor.removeSelectedText()                    
            end-=1
            cursor.movePosition(QTextCursor.Down,QTextCursor.MoveAnchor)
            cursor.movePosition(QTextCursor.StartOfBlock,QTextCursor.MoveAnchor)
        cursor.endEditBlock()
        cursor.setPosition(pos,QTextCursor.MoveAnchor)
    def hightlightError(self,lineno):
        if self.editor : self.editor.textEditionWatch = False
        if self.hasError:
            self.clearErrorHightlight()
        self.sidebar.addMarkerAt(lineno,ErrorMarker)
        self.errorLine = lineno
        cursor = self.textCursor()
        cursor.setPosition(0)
        cursor.movePosition(QTextCursor.NextBlock,QTextCursor.MoveAnchor,lineno-1)
        cursor.movePosition(QTextCursor.EndOfBlock,QTextCursor.KeepAnchor)
        errorformat = QTextCharFormat() 
        errorformat.setBackground(Qt.yellow)
        cursor.setCharFormat(errorformat)
        self.gotoLine(lineno)
        self.hasError = True
        if self.editor : self.editor.textEditionWatch = True
    def clearErrorHightlight(self):
        cursor = self.textCursor()
        self.undo()
        self.setTextCursor(cursor)
        self.hasError = False  
        self.sidebar.removeCurrentMarkerAt(self.errorLine)
    def setEditionFontFamily(self,font):
        font.setPointSize( self.currentFont().pointSize() )
        self.setEditionFont(font)
    def setEditionFontSize(self,p):
        f = self.editionFont
        if self.editionFont is None:
            f = QFont(self.defaultEditionFont)
        f.setPointSize( p )
        self.setEditionFont(f)
    def setEditionFont(self,font):
        self.editionFont = font
        self.document().setDefaultFont(font)
    def isFontToDefault(self):
        if self.editionFont is None : return True
        return str(self.editionFont.family()) == str(self.defaultEditionFont.family()) and self.editionFont.pointSize() == self.defaultEditionFont.pointSize()
    def gotoLine(self,lineno):
        cursor = self.textCursor()
        cursor.setPosition(0)
        cursor.movePosition(QTextCursor.NextBlock,QTextCursor.MoveAnchor,lineno-1)
        self.setTextCursor(cursor)
        self.ensureCursorVisible()
    def gotoLineFromEdit(self):
        self.gotoLine(int(self.gotoEdit.text()))
    def setLineInEdit(self):
        self.gotoEdit.setText(str(self.textCursor().blockNumber()+1))
        self.gotoEdit.selectAll()
    def restoreSimuState(self,simu):
        if self.hasError:
            self.clearErrorHightlight()
        firstinit = simu.textdocument is None
        if firstinit:            
            simu.textdocument = self.document().clone()
        self.setLpyDocument(simu.textdocument)
        if firstinit:
            self.clear()
            self.setText(simu.code)
        if not simu.cursor is None:
            self.setTextCursor(simu.cursor)
            self.horizontalScrollBar().setValue(simu.hvalue)
            self.verticalScrollBar().setValue(simu.vvalue)
        self.sidebar.restoreState(simu)
    def saveSimuState(self,simu):
        simu.code = self.getCode()
        if simu.textdocument is None:
            print('custom document clone')
            simu.textdocument = self.document().clone()
        simu.cursor = self.textCursor()
        simu.hvalue = self.horizontalScrollBar().value()
        simu.vvalue = self.verticalScrollBar().value()
        self.sidebar.saveState(simu)

    def getCode(self):
        return str(self.toPlainText()).encode('iso-8859-1','replace').decode('iso-8859-1')

    def codeToExecute(self):
        cursor = self.textCursor()
        curpos = cursor.position()
        selbegin = cursor.selectionStart()
        selend   = cursor.selectionEnd()
        cursor.setPosition(selbegin)
        cursor.movePosition(QTextCursor.StartOfLine)
        cursor.setPosition(selend, QTextCursor.KeepAnchor)
        cursor.movePosition(QTextCursor.EndOfLine, QTextCursor.KeepAnchor)
        cmd = cursor.selectedText()
        if False : #len(cmd) > 0:
            lines = cmd.splitlines()
            fline = lines[0]
            nfline = fline.lstrip()
            torem = len(fline) - len(nfline)
            nlines = [l[torem:] for l in lines]
            cmd = '\n'.join(nlines)
        return cmd
