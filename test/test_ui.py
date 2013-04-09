from openalea.lpy.gui.lpystudio import *
from openalea.vpltk.qt import qt
import sys,os

class TLPyWindow (LPyWindow):
    def __init__(self,*args):
        LPyWindow.__init__(self,*args)
        self.exitWithoutPrompt = True
        qt.QtCore.QTimer.singleShot(5000,qt.QtCore.QCoreApplication.quit)

if sys.platform == 'win32':
    qapp = None
    widget = None
    testfile = os.path.join(os.getcwd(),'fibonacci.lpy')

    def init_app():
        global qapp, widget
        if qapp is None:
            qapp = qt.QtGui.QApplication([])
        if widget is None:
            widget = TLPyWindow()

    def test_show():
        """ Test show of Lpy widget """
        init_app()
        widget.show()
        qapp.processEvents()
    
    def test_open():
        """ Test open of document in Lpy widget """
        widget.openfile(testfile)
        qapp.processEvents()
        
    def test_run():
        """ Test run of document in Lpy widget """
        widget.actionRun.trigger()
        qapp.processEvents()

    def test_close():
        """ Test close of document in Lpy widget """
        widget.closeDocument()
        qapp.processEvents()
        
    def test_exit():
        """ Test exit of Lpy widget """
        widget.end(force=True)
        qapp.processEvents()
        #QCoreApplication.quit()
        
    if __name__ == '__main__': 
        import traceback as tb
        test_func = [ (n,v) for n,v in globals().items() if 'test' in n]
        test_func.sort(lambda x,y : cmp(x[1].func_code.co_firstlineno,y[1].func_code.co_firstlineno))
        for tfn,tf in test_func:
            print tfn
            try:
                tf()
            except:
                tb.print_exc()
