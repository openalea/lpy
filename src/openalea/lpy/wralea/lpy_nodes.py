
from openalea.lpy import Lsystem,AxialTree,generateScene
from openalea.plantgl.all import Viewer,PglTurtle
import PyQt4.QtCore as qt


def axialtree(lstring):
    """ Build an axial tree object """
    return AxialTree(lstring)

def writeLstring(lstring,fname):
    """ Write an axial tree into a file as Lsystem string """
    f = file(fname,'w')
    f.write(str(lstring))
    f.close()    

def lsystem(code, axiom = '', derivationlength = -1, parameters = {}):
    """ Build a lsystem object from code """

    l = Lsystem()
    if len(axiom):
        if type(axiom) != AxialTree:
            axiom = AxialTree(axiom)
        l. axiom = axiom
    if derivationlength >= 0:
        l.derivationLength = derivationlength
    l.set(str(code))
    l.context().updateNamespace(parameters)

    return l


def animate(lsystem, timestep):
    """ Animate a lsystem """

    if(lsystem):
        Viewer.animation(True)
        lsystem.animate(timestep * 0.01)

    
def run(lsystem, axiom = '', nbstep = -1):
    """ Run a lsystem """
    if nbstep < 0:
        nbstep = lsystem.derivationLength
    if len(axiom) == 0:
        axiom = lsystem.axiom
    elif type(axiom) == str:
        axiom = AxialTree(axiom)
    return (lsystem.iterate(nbstep,axiom),)

def plot(axiom = '', lsystem = None):
    """ Plot a string """
    if len(axiom) == 0:
        axiom = lsystem.axiom
    elif type(axiom) != AxialTree:
        axiom = AxialTree(axiom)
    if (lsystem):
        Viewer.animation(False)
        lsystem.plot(axiom)
    else:
        plot(axiom)
        

def Tree2Scene(axialtree, lsystem = None):
    """ Convert AxialTree to PGL Scene """

    if type(axialtree) != AxialTree:
        axialtree = AxialTree(axialtree)
    if lsystem:
        return lsystem.sceneInterpretation(axialtree)
    else:
        return (generateScene(axialtree),)


WithLpyGui = True
try:
    from openalea.lpy.gui.lpystudio import LPyWindow
    from openalea.visualea.node_widget import NodeWidget

    class LSysWidget(NodeWidget,  LPyWindow):
        def __init__(self, node, parent):

             LPyWindow.__init__(self, parent)
            NodeWidget.__init__(self, node)
            qt.QObject.connect(self.codeeditor,qt.SIGNAL('textChanged()'),self.updateNode)
        def notify(self, sender, event):
            """ Function called by observed objects """
            lcode = self.node.get_input('Code')
            self.lsystem.set(lcode)
            axiom = self.node.get_input('Axiom')
            if len(axiom) > 0:
                self.lsystem.axiom = AxialTree(axiom)
            derivationLength = self.node.get_input('DerivationLength')
            if derivationLength >= 0:
                self.lsystem.derivationLength = derivationLength
            self.codeeditor.setText(lcode)            
        def updateNode(self):
            """ Function called when editor modify object """
            self.node.set_input('Code',str(self.codeeditor.toPlainText()),notify=False)
        def showEvent(self,e):
            self.notify(self,None)


except:
    print "Import lpy.gui has failed"
    WithLpyGui = False

