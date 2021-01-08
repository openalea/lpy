from openalea.lpy import *
from openalea.lpy.lsysparameters import *
from openalea.plantgl.all import NurbsCurve2D, BezierCurve2D, Polyline2D, NurbsPatch, NurbsPatch3D, Material, Texture2D, ImageTexture

def test_param_creation():
    lp = LsystemParameters()
    lp.set_color(0,Material((255,20,1)))
    texture = Texture2D(ImageTexture('image/bjunipc.png'),baseColor=(255,20,1,0))
    lp.set_color(1,texture)
    lp.set_option('Module declaration',1)
    lp.set_option('Warning with sharp module',0)
    lp.add('test', 1, category='test')
    lp.add('testFloat', 1.2, category='test')
    lp.add('testbool',True,category='test2')
    lp.add('testcurve1',NurbsCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), category='test')
    lp.add('testcurve2',BezierCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), category='test')
    lp.add('testcurve3',Polyline2D([(0,0),(0,1),(1,1),(1,0)]), category='test2')
    lp.add_function('testfunc',NurbsCurve2D([(0,0,1),(0,1,1),(1,1,1),(1,0,1)]), category='test2')
    lp.add('testpatch',NurbsPatch([[(0,-0.5+j/3.,i/3.,1) for j in range(4)] for i in range(4)]), category='test2')
    lp.get_category_info('test')['visible'] = True
    lp.get_category_info('test2')['visible'] = True
    lp.check_validity()
    return lp

def test_json_validate():
    lp = test_param_creation()
    p = lp.generate_json_parameter_dict()
    assert LsystemParameters.is_valid_schema(p)

def test_json_retrieve():
    lp = test_param_creation()
    p = lp.generate_json_parameter_dict()
    lp2 = LsystemParameters()
    lp2.retrieve_from_json_dict(p)
    lp2.check_validity()

    lp.check_similarity(lp2)

def test_json_dump():
    lp = test_param_creation()
    p = lp.dumps()
    lp2 = LsystemParameters()
    lp2.loads(p)
    lp2.check_validity()

    lp.check_similarity(lp2)

def test_param_get():
    lp = test_param_creation()
    assert 'test' in lp.get_category_names()
    assert not 'does-not-exists' in lp.get_category_names()
    assert len(lp.get_category_parameters('test')) == 4
    assert len(lp.get_category_parameters('test2')) == 4
    assert len(lp.get_category_parameters('does-not-exists')) == 0
    assert len(lp.get_options()) == 2

def test_param_py_code():
    lp = test_param_creation()
    code = lp.generate_py_code()
    print(code)
    l = Lsystem()
    l.setCode(code)
    lp2 = LsystemParameters(l)
    lp2.retrieve_from(l)
    lp.check_similarity(lp2)

def test_param_py_code_11():
    lp = test_param_creation()
    code = lp.generate_py_code(version=1.1)
    print(code)
    l = Lsystem()
    l.setCode(code)
    lp2 = LsystemParameters()
    lp2.retrieve_from(l)
    lp.check_similarity(lp2)

def test_param_apply():
    lp = test_param_creation()
    l = Lsystem()
    lp.apply_to(l)
    lp2 = LsystemParameters()
    lp2.retrieve_from(l)
    lp.check_similarity(lp2)

lpy_code_11 = """
###### INITIALISATION ######

__lpy_code_version__ = 1.1

def __initialiseContext__(context):
    context.options.setSelection('Module declaration',1)
    context.options.setSelection('Warning with sharp module',0)
    import openalea.plantgl.all as pgl
    Color_0 = pgl.Material("Color_0" , ambient = (255,20,1) , diffuse = 1 )
    Color_0.name = "Color_0"
    context.turtle.setMaterial(0,Color_0)
    PGL_140717355924736 = pgl.ImageTexture("PGL_140717355924736" , "image/bjunipc.png" )
    Color_1 = pgl.Texture2D(image = PGL_140717355924736 )
    Color_1.name = "Color_1"
    context.turtle.setMaterial(1,Color_1)

    scalars = [('test', 'Category'), 
               ('test', 'Integer', 1, 0, 100), 
               ('testFloat', 'Float', 1.2, 0, 100, 2), 
               ('test2', 'Category'), 
               ('testbool', 'Bool', True)]
    context["__scalars__"] = scalars
    for s in scalars:
        if not s[1] == "Category" : context[s[0]] = s[2]

    testcurve1 = pgl.NurbsCurve2D( [(0, 0, 1),(0, 1, 1),(1, 1, 1),(1, 0, 1)])
    testcurve1.name = "testcurve1"
    testcurve2 = pgl.BezierCurve2D([(0, 0, 1),(0, 1, 1),(1, 1, 1),(1, 0, 1)])
    testcurve2.name = "testcurve2"
    panel_0 = ({'name': 'test', 'active': True, 'visible': True },
               [('Curve2D',testcurve1),('Curve2D',testcurve2)])

    testcurve3 = pgl.Polyline2D([(0, 0),(0, 1),(1, 1),(1, 0)])
    testcurve3.name = "testcurve3"
    testfunc = pgl.NurbsCurve2D([(0, 0, 1),(0, 1, 1),(1, 1, 1),(1, 0, 1)])
    testfunc.name = "testfunc"
    testpatch = pgl.NurbsPatch([[(0, -0.5, 0, 1), (0, -0.166667, 0, 1), (0, 0.166667, 0, 1), (0, 0.5, 0, 1)], [(0, -0.5, 0.333333, 1), (0, -0.166667, 0.333333, 1), (0, 0.166667, 0.333333, 1), (0, 0.5, 0.333333, 1)], [(0, -0.5, 0.666667, 1), (0, -0.166667, 0.666667, 1), (0, 0.166667, 0.666667, 1), (0, 0.5, 0.666667, 1)], [(0, -0.5, 1, 1), (0, -0.166667, 1, 1), (0, 0.166667, 1, 1), (0, 0.5, 1, 1)]])
    testpatch.name = "testpatch"
    panel_1 = ({'name': 'test2', 'active': True, 'visible': True },
               [('Curve2D',testcurve3),('Function',testfunc),('NurbsPatch',testpatch)])

    parameterset = [panel_0,panel_1]
    context["__parameterset__"] = parameterset
    context["testcurve1"] = testcurve1
    context["testcurve2"] = testcurve2
    context["test"] = 1
    context["testFloat"] = 1.2
    context["testbool"] = True
    context["testcurve3"] = testcurve3
    context["testfunc"] = pgl.QuantisedFunction(testfunc)
    context["testpatch"] = testpatch
"""

def test_param_code_11():
    lp = test_param_creation()
    l = Lsystem()
    l.setCode(lpy_code_11)
    lp2 = LsystemParameters()
    lp2.retrieve_from(l)
    lp.check_similarity(lp2)

def test_duplicate():
    lp = LsystemParameters()
    lp.set_color(0,Material((255,20,1)))
    lp.set_color(0,Material((20,255,1)))
    assert len(lp.get_colors()) == 1
    lp.add('test', 1, category='test')
    lp.add('test', 1., category='test')
    assert len(lp.get_category_parameters('test')) == 1


def test_param_patch3d():
    lp = LsystemParameters()
    lp.add('testpatch3d',NurbsPatch3D.default(), category='test')
    lp.check_validity()
    code = lp.generate_py_code()
    print(code)
    l = Lsystem()
    l.setCode(code)
    lp2 = LsystemParameters(l)
    lp2.retrieve_from(l)
    lp.check_similarity(lp2)
    p = lp.generate_json_parameter_dict()
    print(p)
    p = lp.dumps()
    lp3 = LsystemParameters()
    lp3.loads(p)
    lp3.check_validity()

    lp.check_similarity(lp3)
  
if __name__ == '__main__':
    test_param_patch3d()

