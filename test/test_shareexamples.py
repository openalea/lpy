from openalea.lpy import *
import os, glob

def get_share_examples():
    d = os.path.os.path.dirname(__file__)
    sharepath = os.path.join(d,os.path.pardir,'share')

    lpyfiles = glob.glob(os.path.join(sharepath,'*.lpy'))
    return lpyfiles

def test_share_examples():
    """ Test all lpy examples from share/ repository """
    for lfile in get_share_examples():
        try:
            l = Lsystem(lfile)
            l.iterate()
        except Exception,e :
            print 'Example file :',lfile
            raise e

if __name__ == '__main__':
    test_share_examples()