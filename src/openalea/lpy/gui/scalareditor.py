from openalea.vpltk.qt import qt
from scalar import *
try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

import sys
    
# Generate GUI if necessary
if not py2exe_release:
    import os
    import compile_ui as ui
    ldir    = os.path.dirname(__file__)
    ui.check_ui_generation(os.path.join(ldir, 'scalarmetaedit.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'scalarfloatmetaedit.ui'))
    del ldir

import scalarmetaedit as sme

class ScalarDialog(qt.QtGui.QDialog,sme.Ui_ScalarDialog):
    def __init__(self,*args):
        qt.QtGui.QDialog.__init__(self,*args)
        self.setupUi(self)
        qt.QtCore.QObject.connect(self.minValueEdit,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateRange)
        qt.QtCore.QObject.connect(self.maxValueEdit,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateRange)
    def setScalar(self,value):
        self.nameEdit.setText(value.name)
        self.valueEdit.setValue(value.value)
        self.minValueEdit.setValue(value.minvalue)
        self.maxValueEdit.setValue(value.maxvalue)
        self.minValueEdit.setEnabled(not value.isBool())
        self.maxValueEdit.setEnabled(not value.isBool())
        self.valueEdit.setRange(self.minValueEdit.value(),self.maxValueEdit.value())
    def getScalar(self):
        return IntegerScalar(str(self.nameEdit.text()),self.valueEdit.value(),self.minValueEdit.value(),self.maxValueEdit.value())
    def updateRange(self,v):
        if self.minValueEdit.value() >= self.maxValueEdit.value():
            self.maxValueEdit.setValue(self.minValueEdit.value()+1)
        self.valueEdit.setRange(self.minValueEdit.value(),self.maxValueEdit.value())

import scalarfloatmetaedit as sfme

class FloatScalarDialog(qt.QtGui.QDialog,sfme.Ui_FloatScalarDialog):
    def __init__(self,*args):
        qt.QtGui.QDialog.__init__(self,*args)
        self.setupUi(self)
        qt.QtCore.QObject.connect(self.minValueEdit,qt.QtCore.SIGNAL('valueChanged(double)'),self.updateRange)
        qt.QtCore.QObject.connect(self.maxValueEdit,qt.QtCore.SIGNAL('valueChanged(double)'),self.updateRange)
        qt.QtCore.QObject.connect(self.decimalEdit,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateDecimal)
    def setScalar(self,value):
        self.nameEdit.setText(value.name)
        self.valueEdit.setValue(value.value)
        self.minValueEdit.setValue(value.minvalue)
        self.maxValueEdit.setValue(value.maxvalue)
        self.minValueEdit.setEnabled(not value.isBool())
        self.maxValueEdit.setEnabled(not value.isBool())
        self.valueEdit.setRange(self.minValueEdit.value(),self.maxValueEdit.value())
    def getScalar(self):
        return FloatScalar(str(self.nameEdit.text()),self.valueEdit.value(),self.minValueEdit.value(),self.maxValueEdit.value(),self.decimalEdit.value())
    def updateRange(self,v):
        if self.minValueEdit.value() >= self.maxValueEdit.value():
            self.maxValueEdit.setValue(self.minValueEdit.value()+1)
        self.valueEdit.setRange(self.minValueEdit.value(),self.maxValueEdit.value())
    def updateDecimal(self, value):
        self.valueEdit.setDecimals(value)
        self.minValueEdit.setDecimals(value)
        self.maxValueEdit.setDecimals(value)

if True : #not sys.platform == 'darwin':        
    class ItemSlider(qt.QtGui.QWidget):
        def __init__(self,orientation,parent,item):
            qt.QtGui.QWidget.__init__(self,parent)
            self.setFocusPolicy(qt.QtCore.Qt.StrongFocus)            
            self.setMinimumHeight(20) 
            horizontalLayout = qt.QtGui.QHBoxLayout(self)
            horizontalLayout.setContentsMargins(0, 0, 0, 0)
            self.label = qt.QtGui.QLabel(self)
            horizontalLayout.addWidget(self.label)
            self.slider = qt.QtGui.QSlider(orientation,self)
            horizontalLayout.addWidget(self.slider)
            self.item = item
            scalar = item.scalar
            self.isfloat = scalar.isFloat()
            if item.scalar.isFloat():
                self.spinBox = qt.QtGui.QDoubleSpinBox(self)
                self.spinBox.setSingleStep(0.1**scalar.decimals)
            else:
                self.spinBox = qt.QtGui.QSpinBox(self)
            self.spinBox.setMinimumHeight(20)                
            horizontalLayout.addWidget(self.spinBox)
            self.spinBox.hide()
            #self.slider.hide()
            self.chgButton = qt.QtGui.QPushButton('O',self)
            self.chgButton.setMaximumWidth(15)
            self.chgButton.setMinimumWidth(15)
            horizontalLayout.addWidget(self.chgButton)
            self.setRange(scalar.minvalue,scalar.maxvalue)
            self.label.setMinimumWidth(self.labelwidth)
            self.setValue(scalar.value)
            self.locked = False

            if self.isfloat:
                qt.QtCore.QObject.connect(self.slider,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateInt2FloatItem)
                qt.QtCore.QObject.connect(self.spinBox,qt.QtCore.SIGNAL('valueChanged(double)'),self.updateItem)
            else:
                qt.QtCore.QObject.connect(self.slider,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateItem)
                qt.QtCore.QObject.connect(self.spinBox,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateItem)
            qt.QtCore.QObject.connect(self.chgButton,qt.QtCore.SIGNAL('pressed()'),self.changeEditor)

        def updateInt2FloatItem(self,value):
            a = 10.**self.item.scalar.decimals
            self.updateItem(value/a)
            
        def updateItem(self,value):
            if self.item.scalar.value != value and not self.locked:
                self.locked = True
                self.item.scalar.value = value
                self.setValue(value)
                self.item.setText(str(value))
                self.label.setMinimumWidth(self.labelwidth)
                self.emit(qt.QtCore.SIGNAL('valueChanged(Pyqt.Qt_PyObject)'),self.item.scalar)
                self.locked = False
            
        def setRange(self,minv,maxv):
            if self.isfloat:
                a = 10**self.item.scalar.decimals
                self.labelwidth = self.fontMetrics().width(' '+str(int(a*maxv))+'. ')
                self.slider.setRange(int(minv*a),int(maxv*a))
            else:
                self.slider.setRange(minv,maxv)
                self.labelwidth = self.fontMetrics().width(' '+str(maxv)+' ')
            self.spinBox.setRange(minv,maxv)
            self.label.setText(' '*(2+len(str(maxv))))
            
        def setValue(self,value):
            if self.isfloat:
                a = 10**self.item.scalar.decimals
                nv = int(value * a)
                if self.slider.value() != nv:
                    self.slider.setValue(value * a)
            else:
                if self.slider.value() != value:
                    self.slider.setValue(value)
            if self.spinBox.value() != value:
                self.spinBox.setValue(value)
            
        def changeEditor(self):
            if self.spinBox.isHidden():
                self.slider.hide()
                self.label.hide()
                self.spinBox.show()
                self.spinBox.move(0,0)
            else:
                self.slider.show()
                self.label.show()
                self.spinBox.hide()

else:
    class ItemSlider(qt.QtGui.QSpinBox):
        def __init__(self,orientation, parent, item):
            qt.QtGui.QSpinBox.__init__(self, parent)
            self.item = item
            scalar = item.scalar
            self.setRange(scalar.minvalue,scalar.maxvalue)
            self.setValue(scalar.value)
            qt.QtCore.QObject.connect(self,qt.QtCore.SIGNAL('valueChanged(int)'),self.updateItem)
        def updateItem(self,value):
            self.item.scalar.value = value
            self.item.setText(str(value))
            self.emit(qt.QtCore.SIGNAL('valueChanged(Pyqt.Qt_PyObject)'),self.item.scalar)

class ScalarEditorDelegate(qt.QtGui.QItemDelegate):
    """ 
    Tool class used in LsysWindow scalar editor 
    It allows to choose a float value from a slider in a QTable
    """
    def __init__(self,maineditor):
        qt.QtGui.QItemDelegate.__init__(self)
        self.maineditor = maineditor

    def createEditor(self, parent, option, index):
        """ Create the editor """
        item = index.model().itemFromIndex(index)
        if not item.scalar.isBool():
            editor = ItemSlider(qt.QtCore.Qt.Horizontal,parent,item)
            qt.QtCore.QObject.connect(editor,qt.QtCore.SIGNAL('valueChanged(PyQt_PyObject)'),self.maineditor.internalValueChanged)
            return editor
    
    def setEditorData(self, editor, index):
        """ Accessor """
        scalar = index.model().itemFromIndex(index).scalar
        if not scalar.isBool():
            editor.setRange(scalar.minvalue,scalar.maxvalue)
            editor.setValue(scalar.value)

    def setModelData(self, editor, model, index):
        """ Accessor """
        #value = editor.value()
        #model.itemFromIndex(index).scalar.value = value
        #model.itemFromIndex(index).setText(str(value))

        
class MyItemModel(qt.QtGui.QStandardItemModel):
    def __init__(self,a,b,scalarmap):
        qt.QtGui.QStandardItemModel.__init__(self,a,b)
        self.scalarmap = scalarmap
        
    def dropMimeData(self,data,action,row,column,parent):        
        encoded = data.data("application/x-qstandarditemmodeldatalist")
        stream = qt.QtCore.QDataStream(encoded, qt.QtCore.QIODevice.ReadOnly)
        r = stream.readInt()
        self.emit(qt.QtCore.SIGNAL("moveRequest(int,int)"),r,parent.row())
        return True
        
    def supportedDropActions(self):
        return qt.QtCore.Qt.MoveAction

#window.scalarEditor.scalarModel

class ScalarEditor (qt.QtGui.QTreeView):
    def __init__(self,parent):
        qt.QtGui.QTreeView.__init__(self,parent)
        self.scalars = []
        self.scalarmap = {}
        self.initTable()
        self.scalarDelegate = ScalarEditorDelegate(self)
        self.setItemDelegateForColumn(1,self.scalarDelegate)
        self.createContextMenu()
        self.metaIntEdit = ScalarDialog(self)
        self.metaFloatEdit = FloatScalarDialog(self)
        self.setItemsExpandable(False)
        self.setIndentation(0)
    # def setItemsExpandable(self, value) : pass
    # def setAllColumnsShowFocus(self, value) : pass
    def initTable(self):
        self.scalarModel = MyItemModel(0, 1, self.scalarmap)
        qt.QtCore.QObject.connect(self.scalarModel,qt.QtCore.SIGNAL('itemChanged(QStandardItem*)'),self.internalItemChanged)
        qt.QtCore.QObject.connect(self.scalarModel,qt.QtCore.SIGNAL('moveRequest(int,int)'),self.moveItem)
        self.scalarModel.setHorizontalHeaderLabels(["Parameter", "Value" ])
        self.setModel(self.scalarModel)
    def contextMenuEvent(self,event):
        items = self.selection()
        self.deleteAction.setEnabled(len(items) > 0)
        self.editAction.setEnabled(len(items) == 1 and not(self.scalars[items[0]].isCategory() or self.scalars[items[0]].isBool()))
        self.menu.exec_(event.globalPos())
    def createContextMenu(self):
        self.menu = qt.QtGui.QMenu("Scalar Edit",self)
        self.menu.addAction("New Integer",self.newScalar)
        self.menu.addAction("New Float",self.newFloatScalar)
        self.menu.addAction("New Boolean",self.newBoolScalar)
        self.menu.addSeparator()
        self.menu.addAction("New Category",self.newCategoryScalar)
        self.menu.addSeparator()
        self.deleteAction = self.menu.addAction("Delete",self.deleteScalars)
        self.editAction = self.menu.addAction("Edit",self.editMetaScalar)
    def selection(self):
        items = list(set([i.row() for i in self.selectedIndexes()]))
        items.sort(lambda x,y: -cmp(x,y))
        return items
    def deleteScalars(self):
        for i in self.selection():
            self.scalarModel.removeRow(i)
            del self.scalars[i]
        self.emit(qt.QtCore.SIGNAL('valueChanged()'))
    def editMetaScalar(self):
        item = self.selection()[0]
        v = self.scalars[item]
        sc = self.visualEditMetaScalar(v)
        if sc and v != sc:
            v.importValue(sc)
            v.si_name.setText(v.name)
            v.si_value.setText(str(v.value))
            self.emit(qt.QtCore.SIGNAL('itemValueChanged(PyQt_PyObject)'),v)
            self.emit(qt.QtCore.SIGNAL('valueChanged()'))
    def visualEditMetaScalar(self,scalar):
        metaEdit = self.metaIntEdit
        if scalar.isFloat():
            metaEdit = self.metaFloatEdit
        metaEdit.setScalar(scalar)
        res = metaEdit.exec_()
        if res: return metaEdit.getScalar()
    def getItems(self,scalar):
        si_name = qt.QtGui.QStandardItem(scalar.name)
        si_name.setEditable(True)
        #si_name.setData(scalar)
        si_name.scalar = scalar
        si_name.nameEditor = True
        if scalar.isCategory():
            b = qt.QtGui.QBrush(qt.QtGui.QColor(255,255,255))
            si_name.setForeground(b)
            b = qt.QtGui.QBrush(qt.QtGui.QColor(0,0,0))
            si_name.setBackground(b)
            return [si_name]
            si_value = qt.QtGui.QStandardItem()
            si_value.setEditable(False)
            si_value.setBackground(b)
        elif scalar.isBool():
            si_value = qt.QtGui.QStandardItem()
            si_value.setCheckable(True)
            si_value.setCheckState(qt.QtCore.Qt.Checked if scalar.value else qt.QtCore.Qt.Unchecked)
            si_value.stdEditor = True
        else:
            si_value = qt.QtGui.QStandardItem(str(scalar.value))
        si_value.scalar = scalar
        scalar.si_name = si_name
        scalar.si_value = si_value
        self.scalarmap[scalar.name] = (scalar, si_name, si_value)
        return [si_name,si_value]
    def newScalar(self):
        s = self.visualEditMetaScalar(IntegerScalar('default_scalar'))
        if s:
            self.scalars.append(s)        
            self.scalarModel.appendRow(self.getItems(s))
            self.internalValueChanged(s)
    def newFloatScalar(self):
        s = self.visualEditMetaScalar(FloatScalar('default_scalar'))
        if s:
            self.scalars.append(s)        
            self.scalarModel.appendRow(self.getItems(s))
            self.internalValueChanged(s)
    def newBoolScalar(self):
        s = BoolScalar('default_bool',True)
        self.scalars.append(s)        
        self.scalarModel.appendRow(self.getItems(s))
        self.internalValueChanged(s)
    def newCategoryScalar(self):
        s = CategoryScalar('new category')
        self.scalars.append(s)
        ri = self.scalarModel.indexFromItem(self.scalarModel.invisibleRootItem())        
        self.scalarModel.appendRow(self.getItems(s))
        self.setFirstColumnSpanned(len(self.scalars)-1,ri,True)
        self.internalValueChanged(s)
    def setScalars(self,values):
        self.scalars = values
        self.replotScalars()
    def getScalars(self):
        return self.scalars
    def replotScalars(self):
        self.initTable()
        ri = self.scalarModel.indexFromItem(self.scalarModel.invisibleRootItem())
        for i, sc in enumerate(self.scalars):
            self.scalarModel.appendRow(self.getItems(sc))
            if sc.isCategory():
                self.setFirstColumnSpanned(i,ri,True)
    def internalValueChanged(self,scalar):
        self.emit(qt.QtCore.SIGNAL('itemValueChanged(PyQt_PyObject)'),scalar)
        self.emit(qt.QtCore.SIGNAL('valueChanged()'))
    def internalItemChanged(self,item):
        if hasattr(item,'nameEditor'):
            item.scalar.name = str(item.text())
            self.emit(qt.QtCore.SIGNAL('valueChanged()'))
        elif hasattr(item,'stdEditor'):
            item.scalar.value = item.checkState() == qt.QtCore.Qt.Checked
            self.emit(qt.QtCore.SIGNAL('valueChanged()'))
    def moveItem(self, r0, r1):
        item = self.scalars.pop(r0)
        if r1 == -1:
            self.scalars.append(item)
        else:
            self.scalars.insert(r1,item)
        self.replotScalars()
        self.emit(qt.QtCore.SIGNAL('valueChanged()'))