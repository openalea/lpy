class ComposedLsystem:

    def __init__(self,lsystemlist,interpretationlist):
        self.lsystemlist = lsystemlist
        self.interpretationlist = interpretationlist
    
    def get_axiom(self):
        return self.lsystemlist[0].axiom
        
    def set_axiom(self,value):
        self.lsystemlist[0].axiom = value
    
    axiom = property(get_axiom,set_axiom)
    
    def derive(self,lstring = None, nbsteps = 1):
        cstring = lstring
        if cstring is None: cstring = self.get_axiom()
        for it in range(0,nbsteps):
            for lsystem in self.lsystemlist:
                cstring = lsystem.derive(cstring)
        return cstring
    
    def interpret(self,lstring):
        cstring = lstring
        for lsystem in self.interpretationlist[:-1]:
            cstring = lsystem.derive(cstring)
        cstring = self.interpretationlist[-1].interpret(cstring)
        return cstring
    
    def plot(self,lstring):
        cstring = lstring
        for lsystem in self.interpretationlist[:-1]:
            cstring = lsystem.derive(cstring)
        self.interpretationlist[-1].plot(cstring)

    def animate(self,lstring = None, nbsteps = 1, dt = 0.1):
        from openalea.plantgl.all import Sequencer
        s = Sequencer(dt)
        clstring = lstring
        for i in xrange(nbsteps):
            clstring = self.derive(clstring)
            self.plot(clstring)
            s.touch()
        