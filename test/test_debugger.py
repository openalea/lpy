from openalea.lpy import *

class MDebugger(LpyDebugger):
    def __init__(self):
        LpyDebugger.__init__(self)
    def begin(self,src,direction):
        self.direction = direction
        print 'Axiom:',src
    def end(self,result):
        print 'Result:',result
    def total_match(self,src,pos_beg,pos_end,dest,prod_length,rule,args):
        assert src[pos_beg].name == 'B'
        if self.direction == eForward:
            print src[pos_beg:pos_end],'--',rule.lineno-1,'-->',dest[-prod_length:]
        else:
            print src[pos_beg:pos_end],'--',rule.lineno-1,'-->',dest[:prod_length]
    def partial_match(self,src,pos_beg,pos_end,dest,rule,args):
        assert src[pos_beg].name == 'C'
        print src[pos_beg:pos_end],'--',rule.lineno-1,'--> failed'
    def identity(self,src,pos,dest):
        assert src[pos].name in 'AC'
        if self.direction == eForward:
            print src[pos],'-- ID ->',dest[-1]
        else:
            print src[pos],'-- ID ->',dest[0]

lcode = """
Axiom: BAABAC
production:
B --> D
C :  pass
endlsystem
"""
            
def test_debugger():
    l = Lsystem()
    l.set(lcode)
    d = MDebugger()
    l.setDebugger(d)
    l.iterate()
    
def test_debugger_backward():
    l = Lsystem()
    l.set("backward()\n"+lcode)
    d = MDebugger()
    l.setDebugger(d)
    l.iterate()

if __name__ == '__main__':
    test_debugger()
    test_debugger_backward()