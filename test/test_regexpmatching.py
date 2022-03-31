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

def runmatch(code, lcodebeg = lcodebeg,optionvalues = list(range(3))): 
    if type(optionvalues) == int:
        optionvalues = [optionvalues]
    for i in range(3):        
        l = Lsystem()
        print(i)
        l.context().options.setSelection('Module matching',i)
        if i in optionvalues:
            l.set(lcodebeg+code)
            l.iterate()
        else:
            try:
                l.set(lcodebeg+code)
                l.iterate()
                print(("Test do not fail for unsupported module matching mode : %i." % i))
                warnings.warn("Test do not fail for unsupported module matching mode : %i." % i)
            except:
                pass

########################################################

def test_match_left_none():
    """ Test matching with None in left context """
    lcode_left_none = """
Axiom : A
production:
None < A :
    global matched
    matched = True
"""
    runmatch(lcode_left_none)

########################################################


def test_match_right_none():
    """ Test matching with None in right context """
    lcode_right_none = """
Axiom : A
production:
A > None :
    global matched
    matched = True
"""
    runmatch(lcode_right_none)

########################################################

def test_match_nones():
    """ Test matching with None in both contexts """
    lcode_nones = """
Axiom : A
production:
None < A > None :
    global matched
    matched = True
"""
    runmatch(lcode_nones)

########################################################



def test_match_or_none():
    """ Test matching with or and None on rigth"""
    lcode = """
Axiom : A B
production:
B > or(C,None) :
    global matched
    matched = True
"""
    print(""" Test matching with or and None on rigth""")
    runmatch(lcode)

########################################################


def test_match_or_none2():
    """ Test matching with or and None on left"""
    lcode = """
Axiom : A B
production:
or(C,None) < A  :
    global matched
    matched = True
"""
    runmatch(lcode)

########################################################

def test_match_bracket_right_context():
    """ Test matching with bracket in left contexts """
    lcode_bracket_right_context = """
Axiom : I [ A ] A
production:

I [ < A  :
    global matched
    matched = not matched
"""
    runmatch(lcode_bracket_right_context)

########################################################


def test_match_multiple_brackets_right_context():
    """ Test matching with multiple brackets in left contexts """
    lcode = """
Axiom : I(0) [ I(1) [ A ] ]
production:

I(x) [ I(y) [ < A  :
    assert x == 0 and y == 1
    global matched
    matched = True
"""
    runmatch(lcode)

########################################################

def test_match_left_regexp_to_none():
    """ Test matching of x regexp with no left contexts """
    lcode = """
Axiom : A 
production:

x(I) < A  :
    global matched
    matched = True
"""
    runmatch(lcode)

########################################################


def test_match_right_regexp_to_none():
    """ Test matching of x regexp with no right contexts """
    lcode = """
Axiom : A 
production:

A > x(I) :
    global matched
    matched = True
"""
    runmatch(lcode)

########################################################


def test_match_regexp_including_start():
    """ Test matching of a regexp wich takes into account the start """
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
    runmatch(lcode)

if __name__ == '__main__':
    test_match_or_none()