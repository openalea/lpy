from PyQt4.QtCore import QRegExp,QObject,Qt,SIGNAL
from PyQt4.QtGui import QTextEdit,QSyntaxHighlighter,QTextDocument,QTextCharFormat,QFont,QTextCursor,QTextOption,QLabel, QColor


class LpySyntaxHighlighter(QSyntaxHighlighter):
    def __init__(self,editor):
        QSyntaxHighlighter.__init__(self,editor)
        self.rules = []
        keywordFormat = QTextCharFormat()
        keywordFormat.setForeground(Qt.darkMagenta)
        keywordFormat.setFontWeight(QFont.Bold)
        self.lpykeywords = ['Axiom:','production','homomorphism','interpretation',
                            'decomposition','endlsystem','group','endgroup',
                            'derivation length','maximum depth','produce','nproduce','-->',
                            'consider:','ignore:','forward','backward','isForward',
                            'Start','End','StartEach','EndEach','getGroup','useGroup','module']
        for pattern in self.lpykeywords:
            self.rules.append((QRegExp(pattern),keywordFormat))
        keywordFormat = QTextCharFormat()
        keywordFormat.setForeground(Qt.blue)
        keywordFormat.setFontWeight(QFont.Bold)
        self.pykeywords = ['class','if','else','elif','while','None','not','is', 'def',
                            'for','range','xrange', 'True','False','from','import',
                            'lambda','or','and','print','pass','in','return','global',
                            'as','int','float','str','tuple','list','assert']
        for pattern in self.pykeywords:
            self.rules.append((QRegExp(pattern),keywordFormat))
        self.delimiterFormat = QTextCharFormat()
        self.delimiterFormat.setForeground(Qt.darkBlue)
        self.delimiterFormat.setFontWeight(QFont.Bold)
        self.delimiterkeywords = '[](){}+-*/:<>='
        self.exprules = []
        self.prodFormat = QTextCharFormat()
        self.prodFormat.setForeground(Qt.black)
        self.prodFormat.setFontWeight(QFont.Bold)
        self.prodkeywords = ['Axiom:','produce','nproduce','-->','module']
        for pattern in self.prodkeywords:
            self.exprules.append((QRegExp(pattern+'.*$'),len(pattern),self.prodFormat,1))
        self.funcFormat = QTextCharFormat()
        self.funcFormat.setForeground(Qt.magenta)
        self.exprules.append((QRegExp('def.*\('),3,self.funcFormat,1))
        self.stringFormat = QTextCharFormat()
        self.stringFormat.setForeground(Qt.darkGray)
        self.exprules.append((QRegExp('\".*\"'),0,self.stringFormat,0))
        self.exprules.append((QRegExp("\'.*\'"),0,self.stringFormat,0))
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
        self.lsysruleExp = QRegExp('.+:')
        self.lsysruleExp2 = QRegExp('.+\-\->')
        self.commentExp = QRegExp('#.+$')
        self.ruleCommentExp = QRegExp('[ \t]+#.+$')
        self.setCurrentBlockState(0)
        self.activated = True
        self.tabviewactivated = True
    def setActivation(self,value):
        self.activated = value
        self.rehighlight()
    def setTabViewActivation(self,value):
        self.tabviewactivated = value
        self.rehighlight()
    def highlightBlock(self,text):
      if self.activated:
        lentxt = len(text)
        if text.indexOf('production:') >= 0:
            self.setCurrentBlockState(1)
        elif text.indexOf('endlsystem') >= 0:
            self.setCurrentBlockState(0)
        elif self.previousBlockState() == -1:
            self.setCurrentBlockState(0)
        else:
            self.setCurrentBlockState(self.previousBlockState())
        for i,c in enumerate(text):
            if str(c) in self.delimiterkeywords:
                self.setFormat(i, 1, self.delimiterFormat)
        if self.currentBlockState() == 1:
            if lentxt > 0 and not str(text[0]) in " \t":
                index = text.indexOf(self.lsysruleExp)
                if index >= 0:
                    length = self.lsysruleExp.matchedLength()
                    self.setFormat(index, length, self.prodFormat)
                else:
                    index = text.indexOf(self.lsysruleExp2)
                    if index >= 0:
                        length = self.lsysruleExp2.matchedLength()
                        self.setFormat(index, length, self.prodFormat)
        for rule in self.rules:
            expression = rule[0]
            index = text.indexOf(expression);
            while index >= 0:
                length = expression.matchedLength()
                if ((index == 0 or not text.at(index-1).isLetterOrNumber()) and 
                   (index == lentxt or not text.at(index+length).isLetterOrNumber())):
                    self.setFormat(index, length, rule[1])
                index = text.indexOf(expression, index + length)
        for rule in self.exprules:
            expression = rule[0]
            index = text.indexOf(expression);
            while index >= 0:
                length = expression.matchedLength()
                if index == 0 or not text.at(index-1).isLetterOrNumber():
                    self.setFormat(index+rule[1], length-rule[1]-rule[3], rule[2])
                index = text.indexOf(expression, index + length)
        if self.tabviewactivated:
            index = text.indexOf(self.tabRule);
            if index >= 0:
                length = self.tabRule.matchedLength()
                for i in xrange(index,index+length):
                    if text.at(i).toAscii() == '\t':
                        self.setFormat(i, 1 , self.tabFormat)
                    else:
                        self.setFormat(i, 1 , self.spaceFormat)
        commentExp = self.commentExp #if self.currentBlockState() == 0 else self.ruleCommentExp
        index = text.indexOf(commentExp)
        while index >= 0:
            length = commentExp.matchedLength()
            self.setFormat(index, length, self.commentFormat)
            index = text.indexOf(commentExp,index+length+2)
    

class LpyCodeEditor(QTextEdit):
    def __init__(self,parent):
        QTextEdit.__init__(self,parent)
        self.editor = None
        self.setAcceptDrops(True)
        self.setWordWrapMode(QTextOption.WrapAnywhere)
        self.findEdit = None
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
        QObject.connect(self.findEdit, SIGNAL('textEdited(const QString&)'),self.findText)
        QObject.connect(self.findEdit, SIGNAL('returnPressed()'),self.setFocus)
        QObject.connect(self.previousButton, SIGNAL('pressed()'),self.findPreviousText)
        QObject.connect(self.nextButton, SIGNAL('pressed()'),self.findNextText)
        QObject.connect(self.replaceButton, SIGNAL('pressed()'),self.replaceText)
        QObject.connect(self.replaceAllButton, SIGNAL('pressed()'),self.replaceAllText)
        QObject.connect(self, SIGNAL('cursorPositionChanged()'),self.printCursorPosition)
        QObject.connect(lpyeditor.actionZoomIn, SIGNAL('triggered()'),self.zoomInEvent)
        QObject.connect(lpyeditor.actionZoomOut, SIGNAL('triggered()'),self.zoomOutEvent)
        QObject.connect(lpyeditor.actionNoZoom, SIGNAL('triggered()'),self.resetZoom)
        self.defaultEditionFont = self.currentFont()
        self.defaultPointSize = self.currentFont().pointSize()
    def focusInEvent ( self, event ):
        self.editor.currentSimulation().monitorfile()
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
        QTextEdit.keyPressEvent(self,event)
        if event.key() == Qt.Key_Return or event.key() == Qt.Key_Enter:
            self.returnEvent()
        elif event.key() == Qt.Key_Tab :
            self.tabEvent()
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
    def findNextText(self):
        txt = self.findEdit.text()
        found = self.find(txt,self.getFindOptions())
        if found:
            self.setFocus()
        else:
            self.statusBar.showMessage('Text not found !',2000)
            self.cursorAtStart()
    def findPreviousText(self):
        txt = self.findEdit.text()
        found = self.find(txt,QTextDocument.FindBackward|self.getFindOptions())
        if found:
            self.setFocus()
        else:
            self.statusBar.showMessage('Text not found !',2000)
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
        else:
            return QTextEdit.canInsertFromMimeData(self,source)
    def insertFromMimeData(self,source):
        if source.hasUrls():
            if not self.editor is None:
                self.editor.openfile(str(source.urls()[0].toLocalFile()))
        else:
            QTextEdit.insertFromMimeData(self,source)
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
            cursor.movePosition(QTextCursor.Down,QTextCursor.MoveAnchor)
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
    def tab(self):
        cursor = self.textCursor()
        beg = cursor.selectionStart()
        end = cursor.selectionEnd()
        pos = cursor.position()
        cursor.beginEditBlock()
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
            cursor.movePosition(QTextCursor.Down,QTextCursor.MoveAnchor)
            if cursor.position() == oldpos:
                break
        cursor.endEditBlock()
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
                for i in xrange(len(self.indentation)-1):
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
        self.editor.textEditionWatch = False
        if self.hasError:
            self.clearErrorHightlight()
        cursor = self.textCursor()
        cursor.setPosition(0)
        cursor.movePosition(QTextCursor.NextBlock,QTextCursor.MoveAnchor,lineno-1)
        cursor.movePosition(QTextCursor.EndOfBlock,QTextCursor.KeepAnchor)
        errorformat = QTextCharFormat() 
        errorformat.setBackground(Qt.yellow)
        cursor.setCharFormat(errorformat)
        prevcursor = self.textCursor()
        self.setTextCursor(cursor)
        self.ensureCursorVisible()
        self.setTextCursor(prevcursor)
        self.hasError = True
        self.editor.textEditionWatch = True
    def clearErrorHightlight(self):
        cursor = self.textCursor()
        self.undo()
        self.setTextCursor(cursor)
        self.hasError = False
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
