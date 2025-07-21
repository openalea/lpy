from openalea.lpy import *
import os, sys, glob

def get_lpy_tst():
    d = os.path.os.path.dirname(__file__)
    sharepath = os.path.join(d,'lpytest')

    lpyfiles = glob.glob(os.path.join(sharepath,'*.lpy'))
    return lpyfiles
    
def exec_lpy_tst(lfile):
        try:
            l = Lsystem(lfile)
            l.iterate()
        except Exception as e :
            print('Test file :',lfile)
            raise e
    
toavoid = []
if sys.platform == 'win32':
    toavoid += ['test_inheritance.lpy']
    
def test_lpy_tests():
    """ Test all lpy examples from lpytest/ repository """
    for lfile in get_lpy_tst():
        if os.path.basename(lfile) in toavoid:
            continue
        print(lfile)
        return exec_lpy_tst,lfile

        
def test_diese_bug():
    """ Test diese bug """
    try:
        exec_lpy_tst("diese_bug.lpy")
        ok = False
    except Exception as e:
        print(e)
        ok = True
    assert ok
        
        
if __name__ == '__main__':
    for e,f in test_lpy_tests():
        try:
            e(f)
        except Exception as e:
            print('****** FAILED:',f)
            print(e)
