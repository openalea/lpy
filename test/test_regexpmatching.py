from openalea.lpy import *
import warnings

lcodebeg = """
matched = False

def StartEach():
    global matched
    matched = False

    
def EndEach():
  assert matched
"""

def runmatch(code, lcodebeg = lcodebeg,optionvalues = range(3)): 
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

lcode_left_none = """
Axiom : A
production:
None < A :
    global matched
    matched = True
"""

def test_match_left_none():
    """ Test matching with None in left context """
    runmatch(lcode_left_none)

########################################################

lcode_right_none = """
Axiom : A
production:
A > None :
    global matched
    matched = True
"""

def test_match_right_none():
    """ Test matching with None in right context """
    runmatch(lcode_right_none)

########################################################

lcode_nones = """
Axiom : A
production:
None < A > None :
    global matched
    matched = True
"""

def test_match_nones():
    """ Test matching with None in both contexts """
    runmatch(lcode_nones)

########################################################

lcode = """
Axiom : A B
production:
B > or(C,None) :
    global matched
    matched = True
"""

def test_match_or_none():
    """ Test matching with or and None on rigth"""
    runmatch(lcode)

########################################################

lcode = """
Axiom : A B
production:
or(C,None) < A  :
    global matched
    matched = True
"""

def test_match_or_none2():
    """ Test matching with or and None on left"""
    runmatch(lcode)

########################################################

lcode_bracket_right_context = """
Axiom : I [ A ] A
production:

I [ < A  :
    global matched
    matched = not matched
"""

def test_match_bracket_right_context():
    """ Test matching with bracket in left contexts """
    runmatch(lcode_bracket_right_context)

########################################################

lcode = """
Axiom : I(0) [ I(1) [ A ] ]
production:

I(x) [ I(y) [ < A  :
    assert x == 0 and y == 1
    global matched
    matched = True
"""

def test_match_multiple_brackets_right_context():
    """ Test matching with multiple brackets in left contexts """
    runmatch(lcode)

########################################################

lcode = """
Axiom : A 
production:

x(I) < A  :
    global matched
    matched = True
"""

def test_match_left_regexp_to_none():
    """ Test matching of x regexp with no left contexts """
    runmatch(lcode)

########################################################

lcode = """
Axiom : A 
production:

A > x(I) :
    global matched
    matched = True
"""

def test_match_right_regexp_to_none():
    """ Test matching of x regexp with no right contexts """
    runmatch(lcode)

########################################################

lcode = """
result = {}
def StartEach():
    global result
    result = {}
def EndEach():
    assert result == { 0 : [], 1 : [0], 2 : [0,1], 3 : [0,1,2], 4 : [0,1] }
    
Axiom : I(0) [ I(1) [ I(2) I(3) ] I(4) ]
production:

x(I(x)) << I(y):
  global result
  result[y] = x

"""

def test_match_regexp_including_start():
    """ Test matching of a regexp wich takes into account the start """
    runmatch(lcode)

