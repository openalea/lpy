import os

from openalea.lpy import Lsystem,AxialTree,generateScene
from openalea.plantgl.all import Viewer,PglTurtle
from openalea.vpltk.qt import QtCore

def is_file(str):
    return os.path.isfile(str)

def axialtree(lstring):
    """ Build an axial tree object """
    return AxialTree(lstring)

def writeLstring(lstring,fname):
    """ Write an axial tree into a file as Lsystem string """
    f = file(fname,'w')
    f.write(str(lstring))
    f.close()    

def lsystem(file_name, axiom = '', derivationlength = -1, parameters = {}):
    """ Build a lsystem object from file_name """
    l = Lsystem(file_name, parameters)

    if len(axiom):
        l.makeCurrent()
        if type(axiom) != AxialTree:
            axiom = AxialTree(axiom)
        l.axiom = axiom
        #l.done()
    if derivationlength >= 0:
        l.derivationLength = derivationlength

    return l    


def run_lpy(lpy_filename, axiom = '', derivationlength = -1, parameters = {}):
    """ Run a LSystem file and return the resulting AxialTree.

    :Parameter:
        - LSystem source: (IFileStr) a lpy filename
        - axiom: (str) axiom for lpy
        - derivationlength (int)
        - parameters: (dict) global parameters that can be changed outside lpy.

    :Returns:
        - axial tree
        - the modified lsystem object
    """

    l = lsystem(lpy_filename, axiom, derivationlength,parameters=parameters)
    return run(l)


    
def animate(lsystem, timestep):
    """ Animate a lsystem """

    if(lsystem):
        Viewer.animation(True)
        lsystem.animate(timestep * 0.01)

    
def run(lsystem, axiom = '', nbstep = -1, parameters = {}):
    """ Run a lsystem """
    c_iter = lsystem.getLastIterationNb()
    if nbstep < 0:
        nbstep = lsystem.derivationLength - c_iter
    if len(axiom) == 0:
        axiom = lsystem.axiom
    elif type(axiom) == str:
        lsystem.makeCurrent()
        axiom = AxialTree(axiom)
        lsystem.done()
    if len(parameters) > 0:
        lsystem.context().updateNamespace(parameters)
    return lsystem.iterate(axiom,c_iter,nbstep), lsystem

def iterate(lsystem, axiom = '', nbstep = -1, parameters = {}, local = {}):
    if parameters and (not isinstance(parameters, dict)) and is_file(parameters):
        with open(parameters) as f:
            py_code = f.read()
            parameters = {}
            exec(py_code, globals(), parameters)

    local_settings = dict(parameters)
    if local:
        local_settings.update(local)

    axial, l = run(lsystem, axiom, nbstep, local_settings)
    params = dict(parameters)
    d = dict()
    l.context().getNamespace(d)
    for k in parameters:
        params[k] = d[k]
    return axial, l, params

def plot(axiom = '', lsystem = None):
    """ Plot a string """
    if len(axiom) == 0:
        axiom = lsystem.axiom
    elif type(axiom) != AxialTree:
        if lsystem: lsystem.makeCurrent()
        axiom = AxialTree(axiom)
        if lsystem: lsystem.done()
    if (lsystem):
        Viewer.animation(False)
        lsystem.plot(axiom)
    else:
        plot(axiom)
    return (axiom,)
        

def Tree2Scene(axialtree, lsystem = None):
    """ Convert AxialTree to PGL Scene """

    if type(axialtree) != AxialTree:
        axialtree = AxialTree(axialtree)
    if lsystem:
        return lsystem.sceneInterpretation(axialtree)
    else:
        return (generateScene(axialtree),)


WithLpyGui = False
LSysWidget = None

if WithLpyGui:
  try:
    from openalea.lpy.gui.lpystudio import LPyWindow
    from openalea.visualea.node_widget import NodeWidget

    class LSysWidget(NodeWidget,  LPyWindow):
        def __init__(self, node, parent):

            LPyWindow.__init__(self, parent)
            NodeWidget.__init__(self, node)
            QtCore.QObject.connect(self.codeeditor,QtCore.SIGNAL('textChanged()'),self.updateNode)
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
    LSysWidget = None

