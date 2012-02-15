from openalea.lpy import *
import traceback as tb

def test_lstring2py():
	""" Test use of lstring2py """
	l = LsysContext()
	l.makeCurrent()
	declare('tralala,toto')
	lstr = 'FF[+F]tralala(2)[toto]'
	l = eval(lstring2py(lstr))
	print l
	assert len(l) == 10
	ax = AxialTree(l)
	assert len(ax) == 10
	ax = AxialTree(lstr)
	assert len(ax) == 10

lcode = """
# produce A
Axiom : F # B
production:
# produce C
# D --> D

F --> F # E

# comments
f :
    # produce I
    produce f
+ :
    produce - # J
"""    
    
def test_comments():
    """ Test comments in Lsystem parsing """
    l = Lsystem()
    opt = l.context().options.setSelection('Module declaration','Mandatory')
    assert opt == True and "Option have disappear or change name"
    l.set(lcode) # an exception should be raised if new module are declared
    l = Lsystem()
    l.set(lcode)
    assert len(l.context().declaredModules()) == 0 and "Some comment were interpreted as code and declare new modules." 
    
def test_parsingcorrespondance():
    """ Test parsing correspondance """
    l = Lsystem()
    pycode = l.set(lcode,debug=True)
    # It work only because rules are simple and do not necessitate new lines of code.
    assert lcode.count('\n') == pycode.count('\n') and "lines correspondance is maybe not preserved." 

fakecode = """
Axiom: A
###### INITIALISATION ######
def """+LsysContext.InitialisationFunctionName+"""(context):
	context.options.setSelection("Module declaration",1)
"""

def test_options_init(verbose = False):
    l = Lsystem()
    try:
        l.set(fakecode)
        raised = False
    except:
        if verbose:
            tb.print_exc()
        raised = True
    if not raised:
        raise Exception('Parsing options were not respected')

def test_format_reading(verbose = False):
    version = 2.5
    s = LpyParsing.VersionTag % version 
    s+='\n'
    print s
    read_version = LpyParsing.getFormatVersion(s)
    print read_version
    assert read_version == version
    supported_versions = LpyParsing.formats
    for v in supported_versions:
        assert LpyParsing.isSupportedFormat(v)
    assert not LpyParsing.isSupportedFormat(max(supported_versions)+1)
    l = Lsystem()
    try :
        l.set(s)
        raised = False
    except:
        if verbose:
            tb.print_exc()
        raised = True
    if not raised:
        raise Exception('Unsupported lpy format do not raise exception!')

lmlcode = """
Axiom: A(0)
derivation length: 2
production:
A(x) :
    nproduce ( 
            B(1,x)
            C(2)
            )
    x /0
"""
def test_multi_line_production(verbose = False):
    """ multi line production """
    l = Lsystem()
    l.set(lmlcode)
    try:
        l.iterate()
    except Exception,e:
        import sys         
        lineno = tb.extract_tb(sys.exc_info()[2])[-1][1]
        assert lineno == 10
        
if __name__ == '__main__':    
    test_func = [ (n,v) for n,v in globals().items() if 'test' in n]
    test_func.sort(lambda x,y : cmp(x[1].func_code.co_firstlineno,y[1].func_code.co_firstlineno))
    for tfn,tf in test_func:
        print tfn
        try:
            tf()
        except:
            tb.print_exc()
