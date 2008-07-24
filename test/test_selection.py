from openalea.lpy import *

def display(sc):
    pass

selectionAsked = False
def selection():
    global selectionAsked
    if not selectionAsked:
        print 'selection'
        selectionAsked = True
        return [3]

def test_selection():
    """ Test customisation of the plot and selection procedure """
    global selectionAsked
    selectionAsked = False
    registerPglPlotFunction(display)
    registerGetSelectionFunction(selection)
    l = Lsystem('test_selection.lpy')
    ln = len(l.axiom)
    l.context().makeCurrent()
    assert l.axiom == AxialTree('N[+NN][-N]N') and 'Invalid axiom parsing'
    print l.axiom
    res = l.iterate(1)
    print res
    assert len(res) == ln+1
    assert res[3].name == '%'
    res = l.iterate(1,1,res)
    print res
    assert len(res) == ln-2
    assert selectionAsked and "Selection has not been asked"
    l.done()
    cleanPglPlotFunction()
    cleanGetSelectionFunction()

if __name__ == '__main__':
    test_selection()