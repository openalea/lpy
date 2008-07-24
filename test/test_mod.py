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

if __name__ == '__main__':
	test_lstring2py()