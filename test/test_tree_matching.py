from openalea.lpy import *
from testresources import get_filename
import warnings

lcodebeg = """
matched = False

def StartEach():
    global matched
    matched = False

    
def EndEach():
  print matched
  assert matched
"""

def matching_run(code,optionvalues = range(4)): 
    if type(optionvalues) == int:
        optionvalues = [optionvalues]
    for i in range(4):
        l = Lsystem()
        print 'option =',i
        if i in optionvalues:
            l.set(code)
            l.context().options.setSelection('String matching',i)
            l.iterate()
        else:
            try:
                l.set(code)
                l.context().options.setSelection('String matching',i)
                l.iterate()
                print "Test do not fail for unsupported string matching mode : %i." % i
                warnings.warn("Test do not fail for unsupported string matching mode : %i." % i)
            except:
                pass

def test_axial_match() : 
    """ Test matching with axial tree context modification"""
    f = open(get_filename('test_axial_matching.lpy'))
    code = f.read()
    f.close()
    matching_run(code,range(1,4))

def test_ms_match() : 
    """ Test matching with multiscale axial tree context modification"""
    f = open(get_filename('test_msmatch.lpy'))
    code = f.read()
    f.close()
    matching_run(code,range(2,4))

def test_ms_match2() : 
    """ Test matching with multiscale axial tree context modification 2"""
    code = lcodebeg+"""
module L,I : scale = 2
module E,U : scale = 1
Axiom: ELU(0)I(1)I(2)U(3)I(4)I(5)I(6)EL
production:
U(x) > E:
    global matched
    matched = True
"""
    matching_run(code,range(2,4))

def test_axial_msmatch() : 
    """ Test matching with axial tree context modification"""
    f = open(get_filename('test_axial_msmatch.lpy'))
    code = f.read()
    f.close()
    matching_run(code,range(2,4))
    
#def test_match_future() : matching_run('test_matching_future.lpy')

########################################################

if __name__ == '__main__':
    import traceback as tb
    test_func = [ (n,v) for n,v in globals().items() if 'test' in n]
    test_func.sort(lambda x,y : cmp(x[1].func_code.co_firstlineno,y[1].func_code.co_firstlineno))
    for tfn,tf in test_func:
        print 'testing func:', tfn
        try:
            tf()
        except:
            tb.print_exc()
