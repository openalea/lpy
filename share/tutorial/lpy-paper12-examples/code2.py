from openalea.plantgl.all import *
from openalea.lpy import *

def main():
    lsys = Lsystem("lsystem1.lpy")
    print lsys.dr  # This would print 0.02
    lsys.axiom = Lstring("Apex(2)",lsys)
    lstring = lsys.axiom
    timer = Sequencer(0.1)
    for i in range(10):
        lsys.dr += 0.02
        lstring = lsys.derive(lstring,1)
        interpretedstring = lsys.interpret(lstring)
        scene = lsys.sceneInterpretation(interpretedstring)
        Viewer.display(scene)
        timer.touch()

if __name__ == '__main__':
    main()