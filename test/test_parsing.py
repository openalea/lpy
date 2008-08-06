from openalea.lpy import *

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
    pycode = l.set(lcode,True)
    # It work only because rules are simple and do not necessitate new lines of code.
    assert lcode.count('\n') == pycode.count('\n') and "lines correspondance is maybe not preserved." 

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
