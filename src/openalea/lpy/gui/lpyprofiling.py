from openalea.vpltk.qt import qt
import os

AnimatedProfiling, ProfilingWithFinalPlot, ProfilingWithNoPlot = range(3)


class MyItem(qt.QtGui.QStandardItem):
    def __init__(self,value):
        qt.QtGui.QStandardItem.__init__(self,str(value))
        self.value = value
    def __lt__(self,other):
        return self.value < other.value

def profileItem(st,rule_table=None,timing= None, gfname = None):
    if not gfname is None:
        gfname = os.path.basename(gfname)
    strtype = (type(st.code) == str)
    sis = []
    if strtype:
        si = MyItem(st.code)
    else:
        n = st.code.co_name
        p = '('+','.join([st.code.co_varnames[i] for i in xrange(st.code.co_argcount)])+')'
        if rule_table.has_key(n):
            n = rule_table[n]
            n += ':'+p
        else:
            n+=p
        si = MyItem(n)
    sis.append(si)
    if timing:
        si = MyItem("%2.4f" % float(100*st.totaltime/timing))
    else:
        si = MyItem('')       
    sis.append(si)
    sis += [MyItem(st.totaltime),MyItem(st.callcount),MyItem(st.inlinetime)]
    if not strtype:
        fname = st.code.co_filename
        if fname == '<string>':
            fname =  gfname
        elif os.path.exists(fname):
            fname = os.path.basename(fname)
        sis += [MyItem(fname),MyItem(st.code.co_firstlineno)]
    for si in sis:
        si.setEditable(False)
    return sis

    
def sort_stats(stats):
    class StEntry:
        def __init__(self,entry):
            self.entry = entry
            if hasattr(entry,'calls') and not entry.calls is None:
                self.calls = map(StEntry,list(entry.calls))
            else:
                self.calls = []
        def __getattr__(self,name):
            if hasattr(self.entry,name):
                return getattr(self.entry,name)
            else: raise AttributeError(name)
    statdict = {}
    for s in stats:
        statdict[s.code] = StEntry(s)
    v = statdict.values()
    for s in v:
        for subs in s.calls:
            if statdict.has_key(subs.code) and subs.entry.callcount == statdict[subs.code].entry.callcount:
                subs.calls = statdict[subs.code].calls
                del statdict[subs.code]
    return statdict.values()

class ProfileItemModel (qt.QtGui.QStandardItemModel):
    def __init__(self,a,b,table,lpywidget,fname):
        qt.QtGui.QStandardItemModel.__init__(self,a, b)
        self.lpywidget = lpywidget
        self.fname = fname
        qt.QtCore.QObject.connect(table,qt.QtCore.SIGNAL('doubleClicked(const QModelIndex&)'),self.selectionEvent)
    def selectionEvent(self,element):
        it = self.itemFromIndex(element)
        r = it.row()
        item = self.itemFromIndex(element.sibling(r,5))
        if item:
            fname = item.value
            if fname and fname == os.path.basename(self.fname):
                line = self.itemFromIndex(element.sibling(r,6)).value
                self.lpywidget.showDocumentAt(self.fname,line)
    
def drawProfileTable(table,stats,rule_table = None,timing = None, fname = None, lpywidget = None):
        nstats = sort_stats(stats)
        optionModel = ProfileItemModel(0, 1,table,lpywidget,fname)
        optionModel.setHorizontalHeaderLabels(["Name","% Time","Total Time","Call","Inline Time","Module", "Line" ])
        optionModel.setSortRole(1)
        indexitem = 0
        if rule_table is None:
            rule_table = {}
        st_stack = []
        for st in nstats:
            items = profileItem(st,rule_table,timing,fname)
            optionModel.appendRow(items)
            if st.calls :
              st_stack.append((st,items))
              while len(st_stack) > 0:
                refst,reftotitem = st_stack.pop(-1)
                refitem = reftotitem[0]
                for sst in refst.calls:
                    subitems = profileItem(sst,rule_table,timing,fname)
                    refitem.appendRow(subitems)
                    if hasattr(sst,'calls') and len(sst.calls) > 0:
                        st_stack.append((sst,subitems))
        table.setModel(optionModel)
        optionModel.sort(1,qt.QtCore.Qt.DescendingOrder)
