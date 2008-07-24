from openalea.lpy import *
import warnings

lcode_matchingmode = """
mode = ParamModule.eMatchingMethod.eWithStarNValueConstraint
def Start():
    assert ParamModule.matchingMethod == mode
"""

def test_matchingmode():
    """ Test setting of the module matching method """
    l = Lsystem()
    modes = { 0: ParamModule.eMatchingMethod.eSimple, 1: ParamModule.eMatchingMethod.eWithStar , 1: ParamModule.eMatchingMethod.eWithStarNValueConstraint }
    l.set(lcode_matchingmode)
    for key,val in modes.items():        
        l.context().options.setSelection('Module matching',key)
        l.context()['mode'] = val
        l.iterate()

lcodebeg = """
#print ParamModule.matchingMethod
matched = False

def StartEach():
    global matched
    matched = False

    
def EndEach():
  assert matched
"""


def runmatch(code,optionvalues = range(3)): 
    if type(optionvalues) == int:
        optionvalues = [optionvalues]
    for i in range(3):        
        l = Lsystem()
        l.context().options.setSelection('Module matching',i)
        if i in optionvalues:
            l.set(lcodebeg+code)
            l.iterate()
        else:
            try:
                l.set(lcodebeg+code)
                l.iterate()
                print "Test do not fail for unsupported module matching mode : %i." % i
                warnings.warn("Test do not fail for unsupported module matching mode : %i." % i)
            except:
                pass
                

########################################################

lcodemodwithoutarg = """
Axiom : A
production:
A(x) :
    assert False and "Wrong matching. Match mod with two args with pattern of mod with one arg"
A :
    global matched
    matched = True
"""

def match_mod_with_no_arg():
    """ Test matching of module without argument """
    runmatch(lcodemodwithoutarg)


########################################################

lcodemodwithonearg = """
Axiom : A(3)
production:
A :
    assert False and "Wrong matching. Match mod with one arg with pattern of mod without arg"
A(x) :
    global matched
    matched = True
    assert x == 3 and "Wrong argument values"
"""

def test_match_mod_with_onearg(modmatching = None):
    """ Test matching of module with one argument """
    runmatch(lcodemodwithonearg)

########################################################

lcodemodwithtwoargs = """

Axiom : A(3,4)

production:

A :
    raise Exception("Wrong matching. Match mod with two args with pattern of mod without arg")
A(x) :
    raise Exception("Wrong matching. Match mod with two args with pattern of mod with one arg")

A(x,y) :
    global matched
    matched = True
    assert x == 3 and y == 4 and "Wrong argument values"

"""

def test_match_mod_with_twoargs():
    """ Test matching of module with two arguments """
    runmatch(lcodemodwithtwoargs)

########################################################

lcodemodwithvaluearg = """
Axiom : A(3)
production:
A :
    raise Exception("Wrong matching. Match mod with one arg with pattern of mod without arg")
A(3) :
    global matched
    matched = True
    produce
A(x) :
    raise Exception("Wrong matching. Should have prefer pattern with value")
"""

def test_match_mod_with_valuearg():
    """ Test matching of module with pattern with a value as argument """
    runmatch(lcodemodwithvaluearg,2)


########################################################

lcodemodwithvalueargs = """
Axiom : A(3,4)
production:
A :
    raise Exception("Wrong matching. Match mod with one arg with pattern of mod without arg")
A(3,4) :
    global matched
    matched = True
    produce
A(x,y) :
    raise Exception("Wrong matching. Should have prefer pattern with value")
"""

def test_match_mod_with_valueargs():
    """ Test matching of module with pattern with values as arguments """
    runmatch(lcodemodwithvalueargs,2)

########################################################

lcodemodwithstararg = """
Axiom : A(3)
production:
A :
    raise Exception("Wrong matching. Match mod with one arg with pattern of mod without arg")
A(*args) :
    global matched
    matched = True
    assert len(args) == 1 and args[0] == 3 and "Bad value for *args. Got %s instead of [3]." %str(args)
"""

def test_match_mod_with_stararg():
    """ Test matching of module with pattern with a star arg as argument """
    runmatch(lcodemodwithstararg,range(1,3))

########################################################

lcodemodwithstararg_for_two = """
Axiom : A(3,4)
production:
A :
    assert False and "Wrong matching. Match mod with one arg with pattern of mod without arg"
A(*args) :
    global matched
    matched = True
    assert len(args) == 2 and args[0] == 3 and args[1] == 4
"""

def test_match_mod_with_stararg_for_two():
    """ Test matching of module with pattern with a star arg as argument """
    runmatch(lcodemodwithstararg_for_two,range(1,3))

########################################################

lcodemodwithstararg_for_zero = """
Axiom : A
production:
A(*args) :
    global matched
    matched = True
    assert len(args) == 0
"""

def test_match_mod_with_stararg_for_zero():
    """ Test matching of module with pattern with a star arg as argument """
    runmatch(lcodemodwithstararg_for_zero,range(1,3))

########################################################

lcodemodwithstarmod = """
Axiom : A
production:
* :
    global matched
    matched = True
*(name,x) :
    raise Exception('match star module pattern with wrong number (2) of arg')
"""

def test_match_mod_with_starmod():
    """ Test matching of module with pattern with a star module """
    runmatch(lcodemodwithstarmod,range(1,3))

########################################################

lcodemodwithstarmod_withnamearg = """
Axiom : A
production:
*(name) :
    global matched
    matched = True
    assert name == 'A'
*(name,x) :
    raise Exception('match star module pattern with wrong number (2) of arg')
"""

def test_match_mod_with_starmod_withnamearg():
    """ Test matching of module with pattern with a star module and name arg """
    runmatch(lcodemodwithstarmod_withnamearg,range(1,3))

########################################################

lcodemodwithstarmod_onearg = """
Axiom : A(2)
production:
*(name) :
    raise Exception('match star module pattern with wrong number (1) of arg')
*(name,x) :
    global matched
    matched = True
    assert x == 2
*(x,y,z) :
    raise Exception('match star module pattern with wrong number (3) of arg')
"""


def test_match_mod_with_starmod_onearg():
    """ Test matching of module with one arg with pattern with a star module """
    runmatch(lcodemodwithstarmod_onearg,range(1,3))
    
########################################################

lcodemodwithstarmod_onearg_staronly = """
Axiom : A(2)
production:
* :
    global matched
    matched = True
*(x) :
    raise Exception('match star module pattern with wrong number (1) of arg')
*(x,y,z) :
    raise Exception('match star module pattern with wrong number (3) of arg')
"""

def test_match_mod_with_starmod_onearg_staronly():
    """ Test matching of module with one arg with pattern with a star module only """
    runmatch(lcodemodwithstarmod_onearg_staronly,range(1,3))

########################################################

lcodemodwithstarmod_two = """
Axiom : A(2,2)
production:
*(x) :
    raise Exception('match star module pattern with wrong number (1) of arg')
*(x,y) :
    raise Exception('match star module pattern with wrong number (2) of arg')
*(x,y,z,w) :
    raise Exception('match star module pattern with wrong number (4) of arg')
*(name,x,y) :
    global matched
    matched = True
    assert name == 'A' and x == 2 and y == 2
"""

def test_match_mod_with_starmod_two():
    """ Test matching of module with two arg with pattern with a star module """
    runmatch(lcodemodwithstarmod_two,range(1,3))

########################################################

lcodemodwithstarmod_two_staronly = """
Axiom : A(2,2)
production:
*(x) :
    raise Exception('match star module pattern with wrong number (1) of arg')
*(x,y) :
    raise Exception('match star module pattern with wrong number (2) of arg')
*(x,y,z,w) :
    raise Exception('match star module pattern with wrong number (4) of arg')
* :
    global matched
    matched = True
"""

def test_match_mod_with_starmod_two_staronly():
    """ Test matching of module with two arg with pattern with a star module only """
    runmatch(lcodemodwithstarmod_two_staronly,range(1,3))

########################################################

lcodemodwithstarmod_stararg = """
Axiom : A(2,3)
production:
*(*args) :
    global matched
    matched = True
    assert len(args) == 3 and args[0] == 'A' and args[1] == 2 and args[2] == 3
"""

def test_match_mod_with_starmod_stararg():
    """ Test matching of module with two arg with pattern with a star module with star args"""
    runmatch(lcodemodwithstarmod_stararg,range(1,3))

########################################################

lcodemodwithstarmod_stararg_name = """
Axiom : A(2,3)
production:
*(name,*args) :
    global matched
    matched = True
    assert name == 'A' and len(args) == 2 and args[0] == 2 and args[1] == 3
"""

def test_match_mod_with_starmod_stararg_name():
    """ Test matching of module with two arg with pattern with a star module with args name and star args """
    runmatch(lcodemodwithstarmod_stararg_name,range(1,3))

########################################################

lcodemodwithstarmod_args_and_stararg = """
Axiom : A(2,3)
production:
*(name,x,*args) :
    global matched
    matched = True
    assert name == 'A' and x == 2 and len(args) == 1 and args[0] == 3
"""

def test_match_mod_with_starmod_args_and_stararg():
    """ Test matching of module with two arg with pattern with a star module with 2 args and star args """
    runmatch(lcodemodwithstarmod_args_and_stararg,range(1,3))

########################################################

lcodemodwithstarmod_enoughargs_and_stararg = """
Axiom : A(2,3)
production:
*(name,x,y,w,*args) :
    raise Exception('match star module pattern with wrong number (5) of arg')
*(name,x,y,*args) :
    global matched
    matched = True
    assert name == 'A' and x == 2 and y == 3 and len(args) == 0 
"""

def test_match_mod_with_starmod_enoughargs_and_stararg():
    """ Test matching of module with two arg with pattern with a star module with enough args and star args """
    runmatch(lcodemodwithstarmod_enoughargs_and_stararg,range(1,3))

########################################################

lcode_lc = """
Axiom : BA
production:
B < A :
    global matched
    matched = True
"""

def test_match_left_context():
    """ Test matching of module with left context """
    l = Lsystem()
    l.set(lcodebeg+lcode_lc)
    l.iterate()

########################################################

lcode_rc = """
Axiom : BA
production:
B > A :
    global matched
    matched = True
"""

def test_match_right_context():
    """ Test matching of module with right context """
    l = Lsystem()
    l.set(lcodebeg+lcode_rc)
    l.iterate()

########################################################

lcode_nlc = """
Axiom : BA
production:
B --> C
C << A :
    global matched
    matched = True
"""

def test_match_new_left_context():
    """ Test matching of module with new left context """
    l = Lsystem()
    l.set(lcodebeg+lcode_nlc)
    l.iterate()

########################################################

lcode_nrc = """
def Start(): backward()
Axiom : AB
production:
B --> C
A >> C :
    global matched
    matched = True
"""

def test_match_new_right_context():
    """ Test matching of module with new right context """
    l = Lsystem()
    l.set(lcodebeg+lcode_nrc)
    l.iterate()

########################################################

lcode_acf = """
Axiom : ABC
production:
A --> D
A < D << B >  C :
    global matched
    matched = True
"""

def test_match_forward_contexts():
    """ Test matching of module with all contexts in forward mode """
    l = Lsystem()
    l.set(lcodebeg+lcode_acf)
    l.iterate()

########################################################

lcode_bnc = """
def Start(): backward()
Axiom : ABC
production:
B :
    assert isForward() == False and 'backward matching not enabled'
    global matched
    matched = True
    produce
"""

def test_match_backward_nocontext():
    """ Test matching of module with no context in backward mode """
    l = Lsystem()
    l.set(lcodebeg+lcode_bnc)
    l.iterate()
    
########################################################

lcode_brc = """
def Start(): backward()
Axiom : ABC
production:
B > C :
    assert isForward() == False and 'backward matching not enabled'
    global matched
    matched = True
    produce
"""

def test_match_backward_right_context():
    """ Test matching of module with one right context in backward mode """
    l = Lsystem()
    l.set(lcodebeg+lcode_brc)
    l.iterate()
    
########################################################

lcode_blc = """
def Start(): backward()
Axiom : ABC
production:
A < B :
    assert isForward() == False and 'backward matching not enabled'
    global matched
    matched = True
    produce
"""

def test_match_backward_left_context():
    """ Test matching of module with one left context in backward mode """
    l = Lsystem()
    l.set(lcodebeg+lcode_blc)
    l.iterate()
    
########################################################

lcode_b2c = """
def Start(): backward()
Axiom : ABC
production:
A < B > C :
    assert isForward() == False and 'backward matching not enabled'
    global matched
    matched = True
    produce
"""

def test_match_backward_two_contexts():
    """ Test matching of module with both left and right context in backward mode """
    l = Lsystem()
    l.set(lcodebeg+lcode_b2c)
    l.iterate()
    
########################################################

lcode_acb = """
def Start(): backward()
Axiom : ABC
production:
C :
    assert isForward() == False and 'backward matching not enabled'
    produce D
A < B >> D > C :
    global matched
    matched = True
    produce
"""

def test_match_backward_allcontexts():
    """ Test matching of module with all contexts in backward mode """
    l = Lsystem()
    l.set(lcodebeg+lcode_acb)
    l.iterate()

########################################################    

lcodeignore = """
Axiom : A+B
production:
ignore: +
A < B:
    global matched
    matched = True
    produce
"""

def test_match_ignore():
    """ Test ignore in matching """
    l = Lsystem()
    l.set(lcodebeg+lcodeignore)
    l.iterate()
    
########################################################

lcodeconsider = """
Axiom : A+B
production:
consider: AB
A < B:
    global matched
    matched = True
    produce
"""

def test_match_consider():
    """ Test consider in matching """
    l = Lsystem()
    l.set(lcodebeg+lcodeconsider)
    l.iterate()
    
    
########################################################

def matching_run(fname):
    l = Lsystem(fname)
    l.iterate()
    
def test_simple_match() : 
    """ Simple matching """
    matching_run('test_simple_matching.lpy')
def test_axial_match() : 
    """ Test matching with axial tree context modification"""
    matching_run('test_axial_matching.lpy')
#def test_match_future() : matching_run('test_matching_future.lpy')
    
########################################################

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
            
    #test_simple_match()
    #test_match()
    #test_match_future()