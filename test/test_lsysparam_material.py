from openalea.lpy import *
from openalea.lpy.lsysparameters import *
from openalea.plantgl.all import NurbsCurve2D, BezierCurve2D, Polyline2D, NurbsPatch, Material, Texture2D, ImageTexture

code = """
###### INITIALISATION ######

__lpy_code_version__ = 1.1

def __initialiseContext__(context):
    import openalea.plantgl.all as pgl
    Color_0 = pgl.Material("Color_0" , ambient = (30,60,10) , diffuse = 3 , )
    Color_0.name = "Color_0"
    context.turtle.setMaterial(0,Color_0)
    Color_1 = pgl.Material("Color_1" , ambient = (60,0,0) , diffuse = 3 , )
    Color_1.name = "Color_1"
    context.turtle.setMaterial(1,Color_1)
    Color_2 = pgl.Material("Color_2" , ambient = (60,60,15) , diffuse = 3 , )
    Color_2.name = "Color_2"
    context.turtle.setMaterial(2,Color_2)
    Color_3 = pgl.Material("Color_3" , ambient = (0,0,60) , diffuse = 3 , )
    Color_3.name = "Color_3"
    context.turtle.setMaterial(3,Color_3)
    Color_5 = pgl.Material("Color_5" , ambient = (51,50,16) , diffuse = 1.76471 , specular = (0,0,0) , )
    Color_5.name = "Color_5"
    context.turtle.setMaterial(5,Color_5)
    Color_6 = pgl.Material("Color_6" , ambient = (71,91,46) , diffuse = 1.67033 , specular = (0,0,0) , )
    Color_6.name = "Color_6"
    context.turtle.setMaterial(6,Color_6)
    PGL_140318980955920 = pgl.ImageTexture("PGL_140318980955920" , "/Users/fboudon/Develop/oagit/mangosim/share/textures/leaf.png" , )
    PGL_140318980955920.name = "PGL_140318980955920"
    Color_10 = pgl.Texture2D(image = PGL_140318980955920 , )
    Color_10.name = "Color_10"
    context.turtle.setMaterial(10,Color_10)
    PGL_140675898004864 = pgl.ImageTexture("PGL_140675898004864" , "/Users/fboudon/Develop/oagit/mangosim/share/textures/leaf-bw.png" , )
    PGL_140675898004864.name = "PGL_140675898004864"
    Color_11 = pgl.Texture2D(image = PGL_140675898004864 , )
    Color_11.name = "Color_11"
    context.turtle.setMaterial(11,Color_11)
    PGL_140608034579296 = pgl.ImageTexture("PGL_140608034579296" , "/Users/fboudon/Develop/oagit/mangosim/share/textures/bark2.png" , )
    PGL_140608034579296.name = "PGL_140608034579296"
    Color_12 = pgl.Texture2D(image = PGL_140608034579296 , )
    Color_12.name = "Color_12"
    context.turtle.setMaterial(12,Color_12)
    Color_14 = pgl.Material("Color_14" , ambient = (62,125,20) , diffuse = 1.032 , specular = (0,0,0) , )
    Color_14.name = "Color_14"
    context.turtle.setMaterial(14,Color_14)
    Color_15 = pgl.Material("Color_15" , ambient = (59,49,16) , diffuse = 1.76271 , specular = (0,0,0) , )
    Color_15.name = "Color_15"
    context.turtle.setMaterial(15,Color_15)
    Color_16 = pgl.Material("Color_16" , ambient = (137,52,37) , diffuse = 1.64964 , specular = (0,0,0) , )
    Color_16.name = "Color_16"
    context.turtle.setMaterial(16,Color_16)
    Color_17 = pgl.Material("Color_17" , ambient = (55,60,14) , diffuse = 3 , specular = (0,0,0) , )
    Color_17.name = "Color_17"
    context.turtle.setMaterial(17,Color_17)
    Color_18 = pgl.Material("Color_18" , ambient = (30,60,10) , diffuse = 1.66667 , specular = (0,0,0) , )
    Color_18.name = "Color_18"
    context.turtle.setMaterial(18,Color_18)
    Color_20 = pgl.Material("Color_20" , ambient = (30,60,10) , diffuse = 1.66667 , specular = (0,0,0) , )
    Color_20.name = "Color_20"
    context.turtle.setMaterial(20,Color_20)
    Color_21 = pgl.Material("Color_21" , ambient = (162,52,68) , diffuse = 1.04321 , specular = (0,0,0) , )
    Color_21.name = "Color_21"
    context.turtle.setMaterial(21,Color_21)
    Color_22 = pgl.Material("Color_22" , ambient = (101,94,11) , diffuse = 1.65347 , specular = (0,0,0) , )
    Color_22.name = "Color_22"
    context.turtle.setMaterial(22,Color_22)
    Color_23 = pgl.Material("Color_23" , ambient = (2,2,2) , diffuse = 0 , specular = (0,0,0) , )
    Color_23.name = "Color_23"
    context.turtle.setMaterial(23,Color_23)
    Color_25 = pgl.Material("Color_25" , ambient = (30,60,10) , diffuse = 1.66667 , specular = (0,0,0) , )
    Color_25.name = "Color_25"
    context.turtle.setMaterial(25,Color_25)
    Color_26 = pgl.Material("Color_26" , ambient = (30,60,10) , diffuse = 1.96667 , specular = (0,0,0) , )
    Color_26.name = "Color_26"
    context.turtle.setMaterial(26,Color_26)
    Color_27 = pgl.Material("Color_27" , ambient = (39,80,12) , diffuse = 1.2625 , specular = (0,0,0) , )
    Color_27.name = "Color_27"
    context.turtle.setMaterial(27,Color_27)
    Color_28 = pgl.Material("Color_28" , ambient = (148,148,0) , diffuse = 0.810811 , specular = (0,0,0) , )
    Color_28.name = "Color_28"
    context.turtle.setMaterial(28,Color_28)
    Color_29 = pgl.Material("Color_29" , ambient = (59,49,16) , diffuse = 1.74576 , specular = (0,0,0) , )
    Color_29.name = "Color_29"
    context.turtle.setMaterial(29,Color_29)
    Color_30 = pgl.Material("Color_30" , ambient = (2,2,2) , diffuse = 0 , specular = (0,0,0) , )
    Color_30.name = "Color_30"
    context.turtle.setMaterial(30,Color_30)
    Color_32 = pgl.Material("Color_32" , ambient = (41,82,13) , diffuse = 1.10976 , specular = (0,0,0) , )
    Color_32.name = "Color_32"
    context.turtle.setMaterial(32,Color_32)
    Color_33 = pgl.Material("Color_33" , ambient = (107,0,107) , diffuse = 1.6729 , specular = (0,0,0) , )
    Color_33.name = "Color_33"
    context.turtle.setMaterial(33,Color_33)
    context.animation_timestep = 0.1
    context.options.setSelection('Module declaration',1)
    context.options.setSelection('Warning with sharp module',0)
    scalars = [('Geometry', 'Category'), ('RESOLUTION', 'Integer', 1, 0, 2), ('LEAFY', 'Bool', True), ('WITH_INFLO', 'Bool', True), ('GENERALIZEDCYLINDER', 'Bool', True), ('TEXTURE', 'Bool', True), ('DISTINCT_MI', 'Bool', False), ('ELASTICITY', 'Float', 0.02, 0.0, 1.0, 2), ('Data Export', 'Category'), ('EXPORT_TO_MTG', 'Bool', True), ('SAVE_MTG', 'Bool', False), ('FRUITMODEL_OUTPUT', 'Bool', False), ('Simulation', 'Category'), ('TREE', 'Integer', 0, 0, 2), ('SEED', 'Integer', 9, 0, 100), ('TIMESTEP', 'Integer', 90, 1, 180), ('REPEAT_LASTPROBAS', 'Bool', False), ('GLM', 'Category'), ('WITH_GLM', 'Bool', True), ('_GLM_TYPE', 'Integer', 3, 1, 3), ('_GLM_RESTRICTION', 'Integer', 0, 0, 6), ('Fruit Model', 'Category'), ('FRUIT_MODEL', 'Bool', True), ('FRUITBRANCHSIZE', 'Integer', 1, 1, 10), ('PARALLELFRUITMODEL', 'Bool', False), ('Visuals', 'Category'), ('TIMEBAR', 'Bool', False)]
    context["__scalars__"] = scalars
    for s in scalars:
      if not s[1] == "Category" : context[s[0]] = s[2]
    import openalea.plantgl.all as pgl
    axis1 = pgl.BezierCurve2D(
        pgl.Point3Array([(-0.5, 0, 1),(-0.166667, 0, 1),(0.214286, 0.00865801, 1),(0.45671, 0.138528, 1)]) ,
        )
    axis1.name = "axis1"
    axis2 = pgl.BezierCurve2D(
        pgl.Point3Array([(-0.5, 0, 1),(-0.166667, 0, 1),(0.209957, -0.017316, 1),(0.491342, -0.0952381, 1)]) ,
        )
    axis2.name = "axis2"
    axis3 = pgl.BezierCurve2D(
        pgl.Point3Array([(-0.5, 0, 1),(-0.166667, 0, 1),(0.166667, 0, 1),(0.5, 0, 1)]) ,
        )
    axis3.name = "axis3"
    axis4 = pgl.BezierCurve2D(
        pgl.Point3Array([(-0.5, 0, 1),(-0.158009, -0.047619, 1),(0.166667, 0.004329, 1),(0.443723, -0.393939, 1)]) ,
        )
    axis4.name = "axis4"
    axis5 = pgl.BezierCurve2D(
        pgl.Point3Array([(-0.416733, 0.0128104, 1),(0.2343, -0.076231, 1),(0.507411, -0.330906, 1),(0.662132, -0.814102, 1)]) ,
        )
    axis5.name = "axis5"
    leafsection = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(-0.508209, 0.16873, 1),(-0.515031, 0.138195, 1),(-0.198373, -0.0924227, 1),(-0.00298323, 0.188761, 1),(0.0897461, -0.106293, 1),(0.555704, 0.0979703, 1),(0.545047, 0.12817, 1)]) ,
        )
    leafsection.name = "leafsection"
    import openalea.plantgl.all as pgl
    leafwidthgrowth = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.0123039, 1),(0.289062, 0.0763736, 1),(0.289062, 0.454469, 1),(0.331839, 0.989763, 1),(1, 1, 1)]) ,
        )
    leafwidthgrowth.name = "leafwidthgrowth"
    leafwidth = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.0846264, 1),(0.239002, 1.00091, 1),(0.485529, 0.991241, 1),(0.718616, 1.00718, 1),(0.877539, 0.231273, 1),(1, 0.00332359, 1)]) ,
        )
    leafwidth.name = "leafwidth"
    petioleCurve = pgl.BezierCurve2D(
        pgl.Point3Array([(-0.543785, 0.192006, 1),(-0.178289, 0.176044, 1),(-0.0656355, 0.0728558, 1),(0.548469, 0.597983, 1),(0.397151, 0.581459, 1),(0.543571, 0.599108, 1)]) ,
        )
    petioleCurve.name = "petioleCurve"
    panel_0 = ({'active': True, 'visible': True, 'name': 'Panel GU'},[('Curve2D',axis1),('Curve2D',axis2),('Curve2D',axis3),('Curve2D',axis4),('Curve2D',axis5),('Curve2D',leafsection),('Function',leafwidthgrowth),('Function',leafwidth),('Curve2D',petioleCurve)])
    radius_base03 = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.131507, 1),(0.00454138, 0.0251277, 1),(0.578848, 0.00694723, 1),(1, 0.00524218, 1)]) ,
        )
    radius_base03.name = "radius_base03"
    inflo_radius = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.160173, 1),(0.393939, 0.151515, 1),(0.735931, 0.0649351, 1),(1, 0.025974, 1)]) ,
        )
    inflo_radius.name = "inflo_radius"
    radius = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.107326, 1),(0.192635, 0.0975655, 1),(0.457142, 0.000244746, 1),(1, 0.026087, 1)]) ,
        )
    radius.name = "radius"
    leafsize = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.431282, 1),(0.324129, 0.436757, 1),(0.408886, 0.416427, 1),(0.412274, 0.708684, 1),(0.844357, 0.703533, 1),(1, 0.246499, 1)]) ,
        )
    leafsize.name = "leafsize"
    leaflength = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 1, 1),(0.00149779, 1.00072, 1),(1, 0.995671, 1),(1, 0.400121, 1)]) ,
        )
    leaflength.name = "leaflength"
    leafpath = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(-0.5, 0, 1),(-0.145022, -0.0735931, 1),(0.0844156, -0.212121, 1),(0.123377, -0.497835, 1)]) ,
        )
    leafpath.name = "leafpath"
    panel_1 = ({'active': False, 'visible': True, 'name': 'Panel Unused'},[('Function',radius_base03),('Function',inflo_radius),('Function',radius),('Function',leafsize),('Function',leaflength),('Curve2D',leafpath)])
    fruitprofile = pgl.BezierCurve2D(
        pgl.Point3Array([(0.00563514, 0.994106, 1),(0.223956, 0.97883, 1),(0.425561, 0.771672, 1),(0.680945, 0.239898, 1),(0.509633, 0.00914747, 1),(0.0130125, 0.0011832, 1)]) ,
        )
    fruitprofile.name = "fruitprofile"
    bract_axis = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(-0.5, 0, 1),(-0.166667, -0.168182, 1),(0.143939, -0.140909, 1),(0.504545, -0.113636, 1)]) ,
        )
    bract_axis.name = "bract_axis"
    bract_section = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(-0.5, 0, 1),(-0.189394, -0.159091, 1),(0.221212, -0.186364, 1),(0.5, 0, 1)]) ,
        )
    bract_section.name = "bract_section"
    bract_width = pgl.NurbsCurve2D(
        ctrlPointList = pgl.Point3Array([(0, 0.141879, 1),(0.0485817, 1.44717, 1),(1, 1.11933, 1),(1, 0.0236465, 1)]) ,
        )
    bract_width.name = "bract_width"
    panel_2 = ({'active': True, 'visible': True, 'name': 'Panel Inflo'},[('Curve2D',fruitprofile),('Curve2D',bract_axis),('Curve2D',bract_section),('Function',bract_width)])
    parameterset = [panel_0,panel_1,panel_2,]
    context["__functions__"] = [('leafwidthgrowth',leafwidthgrowth),('leafwidth',leafwidth),('bract_width',bract_width),]
    context["__curves__"] = [('axis1',axis1),('axis2',axis2),('axis3',axis3),('axis4',axis4),('axis5',axis5),('leafsection',leafsection),('petioleCurve',petioleCurve),('fruitprofile',fruitprofile),('bract_axis',bract_axis),('bract_section',bract_section),]
    context["__parameterset__"] = parameterset
    context["axis1"] = axis1
    context["axis2"] = axis2
    context["axis3"] = axis3
    context["axis4"] = axis4
    context["axis5"] = axis5
    context["leafsection"] = leafsection
    context["leafwidthgrowth"] = pgl.QuantisedFunction(leafwidthgrowth)
    context["leafwidth"] = pgl.QuantisedFunction(leafwidth)
    context["petioleCurve"] = petioleCurve
    context["fruitprofile"] = fruitprofile
    context["bract_axis"] = bract_axis
    context["bract_section"] = bract_section
    context["bract_width"] = pgl.QuantisedFunction(bract_width)

"""


def test_complex11():

    lsys = Lsystem()
    lsys.setCode(code)

    lp = LsystemParameters(lsys)
    lp.is_valid()

    LsystemParameters.is_valid_schema(lp.dumps())

    lp1 = LsystemParameters()
    lp1.loads(lp.dumps())

    #lp1.dumps()

   

if __name__ == '__main__':
    test_complex11()
