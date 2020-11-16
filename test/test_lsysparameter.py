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
    lp.add('testcurve1',NurbsCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), category='test')
    lp.add('testcurve2',BezierCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), category='test')
    lp.add('testcurve3',Polyline2D([(0,0),(0,1),(1,1),(1,0)]), category='test2')
    lp.add_function('testfunc',NurbsCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), category='test2')
    lp.add('testpatch',NurbsPatch([[(0,-0.5+j/3.,i/3.,1) for j in range(4)] for i in range(4)]), category='test2')
    lp.check_validity()
    return lp

def test_json_validate():
    lp = test_param_creation()
    p = lp.generate_json_parameter_dict()
    assert LsystemParameters.validate_schema(p)

def test_json_retrieve():
    lp = test_param_creation()
    p = lp.generate_json_parameter_dict()
    lp2 = LsystemParameters()
    lp2.retrieve_from_json_dict(p)
    lp2.check_validity()

    lp.check_similarity(lp2)

def test_json_get():
    lp = test_param_creation()
    assert 'test' in lp.categories()
    assert 'test2' in lp.categories()
    assert len(lp.category_parameters('test')) == 5
    assert len(lp.category_parameters('test2')) == 3

if __name__ == '__main__':
    test_json_validate()