from openalea.lpy import *
import os, glob

def get_share_examples():
    d = os.path.os.path.dirname(__file__)
    sharepath = os.path.join(d,os.path.pardir,'share','examples')

    lpyfiles = glob.glob(os.path.join(sharepath,'*.lpy'))
    return lpyfiles
    
def exec_share_example(lfile):
        try:
            l = Lsystem(lfile)
            l.iterate()
        except Exception,e :
            print 'Example file :',lfile
            raise e
    
    
def test_share_examples():
    """ Test all lpy examples from share/ repository """
    for lfile in get_share_examples():
        yield exec_share_example,lfile

if __name__ == '__main__':
    for e,f in test_share_examples():
        e(f)