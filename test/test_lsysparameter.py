from openalea.lpy import *
from openalea.lpy.lsysparameters import *
from openalea.plantgl.all import NurbsCurve2D, BezierCurve2D, Polyline2D, NurbsPatch

def test_param_creation():
    l = Lsystem()
    l.execContext().options.setSelection('Module declaration',1)
    l.execContext().options.setSelection('Warning with sharp module',0)
    lp = LsystemParameters(l)
    lp.add('test', 1, category='test')
    lp.add('testFloat', 1.2, category='test')
    lp.add('testbool',True,category='test')
    lp.add('testcurve1',NurbsCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), 'Curve2D', category='test')
    lp.add('testcurve2',BezierCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), 'Curve2D', category='test')
    lp.add('testcurve3',Polyline2D([(0,0),(0,1),(1,1),(1,0)]), 'Curve2D', category='test2')
    lp.add('testfunc',NurbsCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), 'Function', category='test2')
    lp.add('testpatch',NurbsPatch([[(0,-0.5+j/3.,i/3.,1) for j in range(4)] for i in range(4)]),'NurbsPatch', category='test2')
    assert lp.is_valid()
    return lp

def test_json_validate():
    lp = test_param_creation()
    p = lp.generate_json_parameter_dict()
    print(p)
    assert LsystemParameters.validate_schema(p)


if __name__ == '__main__':
    test_json_validate()