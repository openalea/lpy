from openalea.lpy import *
from sys import getrefcount

def test_context():
    """ Creation and simple actions on an execution context  """
    l = LsysContext()
    #print getrefcount(l)
    l.makeCurrent()
    assert l.isCurrent()
    l.done()

def test_autodelcontext():
    """ Test whether an execution context is automatically deleted """
    def ctx():
        l = LsysContext()
        l.makeCurrent()
        assert l.isCurrent()
        #l.done()  : should be automatically done
    ctx()
    l = LsysContext()
    l.makeCurrent()
    assert l.isCurrent()
    l = Lsystem()
    l.makeCurrent()
    l = Lsystem()
    l.makeCurrent()
    
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


    