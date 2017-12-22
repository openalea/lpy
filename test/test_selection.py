from openalea.lpy import *
from testresources import get_filename

class Plotter:
    def __init__(self):
        self.selectionAsked = False
    def display(self,sc):
        pass
    def selection(self):
        if not self.selectionAsked:
            print 'selection'
            self.selectionAsked = True
            return [3]

def test_selection():
    """ Test customisation of the plot and selection procedure """
    plot = Plotter()
    registerPlotter(plot)
    l = Lsystem(get_filename('test_selection.lpy'))
    ln = len(l.axiom)
    l.context().makeCurrent()
    assert l.axiom == AxialTree('N[+NN][-N]N') and 'Invalid axiom parsing'
    print l.axiom
    res = l.iterate(1)
    print res
    assert len(res) == ln+1
    assert res[3].name == '%'
    res = l.derive(res,1,1)
    print res
    assert len(res) == ln-2
    assert plot.selectionAsked and "Selection has not been asked"
    l.done()
    cleanPlotter()

if __name__ == '__main__':
    test_selection()