from PyQt4.Qt import *
from scalar import *
try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

# Generate GUI if necessary
if not py2exe_release:
    import os
    import compile_ui as ui
    ldir    = os.path.dirname(__file__)
    ui.check_ui_generation(os.path.join(ldir, 'scalarmetaedit.ui'))
    del ldir

import scalarmetaedit as sme

class ScalarDialog(QDialog,sme.Ui_ScalarDialog):
    def __init__(self,*args):
        QDialog.__init__(self,*args)
        self.setupUi(self)
        QObject.connect(self.minValueEdit,SIGNAL('valueChanged(int)'),self.updateRange)
        QObject.connect(self.maxValueEdit,SIGNAL('valueChanged(int)'),self.updateRange)
    def setScalar(self,value):
        self.nameEdit.setText(value.name)
        self.valueEdit.setValue(value.value)
        self.minValueEdit.setValue(value.minvalue)
        self.maxValueEdit.setValue(value.maxvalue)
        self.valueEdit.setRange(self.minValueEdit.value(),self.maxValueEdit.value())
    def getScalar(self):
        return Scalar(str(self.nameEdit.text()),self.valueEdit.value(),self.minValueEdit.value(),self.maxValueEdit.value())
    def updateRange(self,v):
        if self.minValueEdit.value() >= self.maxValueEdit.value():
            self.maxValueEdit.setValue(self.minValueEdit.value()+1)
        self.valueEdit.setRange(self.minValueEdit.value(),self.maxValueEdit.value())


class ItemSlider(QSlider):
    def __init__(self,orientation,parent,item):
        QSlider.__init__(self,orientation,parent)
        self.item = item
        scalar = item.scalar
        self.setRange(scalar.minvalue,scalar.maxvalue)
        self.setValue(scalar.value)
        QObject.connect(self,SIGNAL('valueChanged(int)'),self.updateItem)
    def updateItem(self,value):
        self.item.scalar.value = value
        self.item.setText(str(value))        
        self.emit(SIGNAL('valueChanged(PyQt_PyObject)'),self.item.scalar)
        
class ScalarEditorDelegate(QItemDelegate):
    """ 
    Tool class used in LsysWindow scalar editor 
    It allows to choose a float value from a slider in a QTable
    """
    def __init__(self,maineditor):
        QItemDelegate.__init__(self)
        self.maineditor = maineditor

    def createEditor(self, parent, option, index):
        """ Create the editor """
        item = index.model().itemFromIndex(index)
        editor = ItemSlider(Qt.Horizontal,parent,item)
        QObject.connect(editor,SIGNAL('valueChanged(PyQt_PyObject)'),self.maineditor.internalValueChanged)
        return editor
    
    def setEditorData(self, editor, index):
        """ Accessor """
        scalar = index.model().itemFromIndex(index).scalar
        editor.setRange(scalar.minvalue,scalar.maxvalue)
        editor.setValue(scalar.value)

    def setModelData(self, editor, model, index):
        """ Accessor """
        #value = editor.value()
        #model.itemFromIndex(index).scalar.value = value
        #model.itemFromIndex(index).setText(str(value))

class ScalarEditor (QTreeView):
    def __init__(self,parent):
        QTableView.__init__(self,parent)
        self.initTable()
        self.scalars = []
        self.scalarDelegate = ScalarEditorDelegate(self)
        self.setItemDelegateForColumn(1,self.scalarDelegate)
        self.createContextMenu()
        self.metaEdit = ScalarDialog(self)
    def initTable(self):
        self.scalarModel = QStandardItemModel(0, 1)
        QObject.connect(self.scalarModel,SIGNAL('itemChanged(QStandardItem*)'),self.internalItemChanged)
        self.scalarModel.setHorizontalHeaderLabels(["Parameter", "Value" ])
        self.setModel(self.scalarModel)
    def contextMenuEvent(self,event):
        items = self.selection()
        self.deleteAction.setEnabled(len(items) > 0)
        self.editAction.setEnabled(len(items) == 1)
        self.menu.exec_(event.globalPos())
    def createContextMenu(self):
        self.menu = QMenu("Scalar Edit",self)
        self.menu.addAction("New",self.newScalar)
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
        self.emit(SIGNAL('valueChanged()'))
    def editMetaScalar(self):
        item = self.selection()[0]
        v = self.scalars[item]
        sc = self.visualEditMetaScalar(v)
        if sc and v != sc:
            v.importValue(sc)
            v.si_name.setText(v.name)
            v.si_value.setText(str(v.value))
            self.emit(SIGNAL('itemValueChanged(PyQt_PyObject)'),v)
            self.emit(SIGNAL('valueChanged()'))
    def visualEditMetaScalar(self,scalar):
        self.metaEdit.setScalar(scalar)
        res = self.metaEdit.exec_()
        if res: return self.metaEdit.getScalar()
    def getItems(self,scalar):
        si_name = QStandardItem(scalar.name)
        si_name.setEditable(True)
        si_name.scalar = scalar
        si_name.nameEditor = True
        si_value = QStandardItem(str(scalar.value))
        si_value.scalar = scalar
        scalar.si_name = si_name
        scalar.si_value = si_value
        return [si_name,si_value]
    def newScalar(self):
        s = self.visualEditMetaScalar(Scalar('default_scalar'))
        if s:
            self.scalars.append(s)        
            self.scalarModel.appendRow(self.getItems(s))
            self.internalValueChanged(s)
    def setScalars(self,values):
        self.scalars = values
        self.replotScalars()
    def getScalars(self):
        return self.scalars
    def replotScalars(self):
        self.initTable()
        for sc in self.scalars:
            self.scalarModel.appendRow(self.getItems(sc))
    def getState(self):
        return self.scalars,self.scalarModel
    def restoreState(self,scalars,state):
        self.scalars,self.scalarModel = scalars,state
        self.setModel(self.scalarModel)
    def internalValueChanged(self,scalar):
        self.emit(SIGNAL('itemValueChanged(PyQt_PyObject)'),scalar)
        self.emit(SIGNAL('valueChanged()'))
    def internalItemChanged(self,item):
        if hasattr(item,'nameEditor'):
            item.scalar.name = str(item.text())
            self.emit(SIGNAL('valueChanged()'))
        
        