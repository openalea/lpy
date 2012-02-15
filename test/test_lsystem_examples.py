from openalea.lpy import *

def run_lsystem(fname):
    l = Lsystem(fname)
    l.iterate()

def test_lstring_change() : run_lsystem('test_lstring_change.lpy')

def test_lstring_change2() : run_lsystem('test_lstring_change2.lpy')

def test_lstring_change3() : run_lsystem('test_lstring_change3.lpy')

def test_functionalaxiom() : run_lsystem('test_functionalaxiom.lpy')
