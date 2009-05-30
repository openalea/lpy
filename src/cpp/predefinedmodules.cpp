/* ---------------------------------------------------------------------------
 #
 #       L-Py: L-systems in Python
 #
 #       Copyright 2003-2008 UMR Cirad/Inria/Inra Dap - Virtual Plant Team
 #
 #       File author(s): F. Boudon (frederic.boudon@cirad.fr)
 #
 # ---------------------------------------------------------------------------
 #
 #                      GNU General Public Licence
 #
 #       This program is free software; you can redistribute it and/or
 #       modify it under the terms of the GNU General Public License as
 #       published by the Free Software Foundation; either version 2 of
 #       the License, or (at your option) any later version.
 #
 #       This program is distributed in the hope that it will be useful,
 #       but WITHOUT ANY WARRANTY; without even the implied warranty of
 #       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 #       GNU General Public License for more details.
 #
 #       You should have received a copy of the GNU General Public
 #       License along with this program; see the file COPYING. If not,
 #       write to the Free Software Foundation, Inc., 59
 #       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 #
 # ---------------------------------------------------------------------------
 */


#include "moduleclass.h"
#include "mod.h"
#include <plantgl/scenegraph/pgl_version.h>
#include <plantgl/algo/modelling/pglturtle.h>

LPY_BEGIN_NAMESPACE
TOOLS_USING_NAMESPACE
PGL_USING_NAMESPACE
#define bp boost::python

/*---------------------------------------------------------------------------*/

PredefinedModuleClass::PredefinedModuleClass(const std::string& name, const std::string& doc, eCategory _category):
ModuleClass(name), documentation(doc), category(_category) 
{
	if(category == ePatternMatching) onlyInPattern = true;
	PredefinedClasses->push_back(this); 
}

PredefinedModuleClass::PredefinedModuleClass(const std::string& name, const std::string& alias, const std::string& doc, eCategory _category):
ModuleClass(name,alias), documentation(doc), category(_category) 
{ 
	if(category == ePatternMatching) onlyInPattern = true;
	PredefinedClasses->push_back(this); 
}

PredefinedModuleClass::~PredefinedModuleClass() {}


#define DeclaredModule(modname) modname##ModuleClass
#define DeclareModuleEnd };

#define DeclareModuleBegin(modname, doc, category) \
class DeclaredModule(modname) : public PredefinedModuleClass  { \
public: \
	DeclaredModule(modname) (const std::string& name) :  PredefinedModuleClass(name,doc,category) { } \
	DeclaredModule(modname) (const std::string& name, const std::string& alias) :  PredefinedModuleClass(name,alias,doc,category) { } \
	~DeclaredModule(modname)() {} \
	void interpret(ParamModule& m, PGL::Turtle& t)

#define DeclareSimpleModule(modname,doc,category) DeclareModuleBegin(modname,doc,category) { t.modname(); } DeclareModuleEnd
#define DeclareModuleReal1(modname,doc,category) \
	DeclareModuleBegin(modname,doc,category) {  \
		switch (m.argSize()) { \
			case 0:  t.modname(); break; \
			default:  t.modname(m._getReal(0)); break; } \
     } \
	DeclareModuleEnd \

DeclareSimpleModule(push,"Push the state in the stack.",eStructure)
DeclareSimpleModule(pop,"Pop last state from turtle stack and make it the its current state.",eStructure)


DeclareModuleBegin(F, "Move forward and draw. Params: length , topradius.",ePrimitive)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  t.F(); break;
		case 1:  t.F(m._getReal(0)); break;
		default: t.F(m._getReal(0),m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(f, "Move forward and without draw. Params: length.",ePrimitive)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  t.f(); break;
		default:  t.f(m._getReal(0)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(GetPos,"Request position vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getPosition()); }
DeclareModuleEnd

DeclareModuleBegin(GetHead,"Request heading vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getHeading()); }
DeclareModuleEnd

DeclareModuleBegin(GetUp,"Request up vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getUp()); }
DeclareModuleEnd

DeclareModuleBegin(GetLeft,"Request left vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getLeft()); }
DeclareModuleEnd

DeclareModuleBegin(GetRight,"Request right vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(-t.getLeft()); }
DeclareModuleEnd

DeclareSimpleModule(startGC, "Start a new generalized cylinder.",ePrimitive)
DeclareSimpleModule(stopGC,  "Pop generalized cylinder from the stack and render it.",ePrimitive)
DeclareSimpleModule(startPolygon,"Start a new polygon.",ePrimitive)
DeclareSimpleModule(stopPolygon,"Pop a polygon from the stack and render it.",ePrimitive)

DeclareModuleBegin(MoveTo,"Set the turtle position. Params : x, y, z (optionals, default = 0).",ePosition)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
         case 0:  t.move(); break;
         case 1:  t.move(m._getReal(0)); break;
         case 2:  t.move(m._getReal(0),m._getReal(1)); break;
         default: t.move(m._getReal(0),m._getReal(1),m._getReal(2)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(SetHead,"Set the turtle Heading and Up vector. Params: hx, hy, hz, ux, uy, uz (optionals, default=0,0,1, 1,0,0).",eRotation)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
      case 0:  t.setHead(); break;
      case 1:  t.setHead(m._getReal(0)); break;
      case 2:  t.setHead(m._getReal(0),m._getReal(1)); break;
      case 3:  t.setHead(m._getReal(0),m._getReal(1),m._getReal(2)); break;
      case 4:  t.setHead(m._getReal(0),m._getReal(1),m._getReal(2),
                             m._getReal(3)); break;
      case 5:  t.setHead(m._getReal(0),m._getReal(1),m._getReal(2),
                             m._getReal(3),m._getReal(4)); break;
      default: t.setHead(m._getReal(0),m._getReal(1),m._getReal(2),
                              m._getReal(3),m._getReal(4),m._getReal(5)); break;
	}
}
DeclareModuleEnd

DeclareModuleReal1(left,  "Turn left  around Up vector. Params : angle (optional, in degrees).",eRotation)
DeclareModuleReal1(right, "Turn right around Up vector. Params : angle (optional, in degrees).",eRotation)
DeclareModuleReal1(up,    "Pitch up around Left vector. Params : angle (optional, in degrees).",eRotation)
DeclareModuleReal1(down,  "Pitch down around Left vector. Params : angle (optional, in degrees).",eRotation)
DeclareModuleReal1(rollL, "Roll left  around Heading vector. Params : angle (optional, in degrees).",eRotation)
DeclareModuleReal1(rollR, "Roll right  around Heading vector. Params : angle (optional, in degrees).",eRotation)

#if PGL_VERSION >= 0x020501
DeclareSimpleModule(turnAround, "Turn around 180deg the Up vector.",eRotation)
#else
DeclareModuleBegin(turnAround,eRotation)
{ t.left(180); }
DeclareModuleEnd

#ifdef _MSC_VER
#pragma message("You should upgrade your version of PlantGL.")
#else
#warning You should upgrade your version of PlantGL.
#endif
#endif

DeclareSimpleModule(rollToVert, "Roll to Vertical : Roll the turtle around the H axis so that H and U lie in a common vertical plane with U closest to up",eRotation)
DeclareModuleReal1(sphere,"Draw a sphere. Params : radius (optional, should be positive, default = line width).",ePrimitive)
DeclareModuleReal1(circle,"Draw a circle. Params : radius (optional, should be positive, default = line width).",ePrimitive)

DeclareModuleBegin(label,"Draw a text label.",ePrimitive)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.label(m._getString(0));
}
DeclareModuleEnd

DeclareModuleBegin(incWidth,"Increase the current line width or set it if a parameter is given. Params : line width (optional).",eWidth)
{
	if (!m.hasArg())t.incWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(decWidth,"Decrease the current line width or set it if a parameter is given. Params : line width (optional).",eWidth)
{
	if (!m.hasArg())t.decWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(setWidth,"Set current line width. Params : line width.",eWidth)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(incColor,"Increase the current material index or set it if a parameter is given. Params : color index (optional, positive int).",eColor)
{
	if (!m.hasArg())t.incColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(decColor,"Decrease the current material index or set it if a parameter is given. Params : color index (optional, positive int).",eColor)
{
	if (!m.hasArg())t.decColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(setColor,"Set the current material index. Params : color index (positive int).",eColor)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(divScale,"Divides the current turtle scale by a scale factor, Params : scale factor (optional, default = 1.0).",eScale)
{
	if (!m.hasArg())t.divScale();
	else t.divScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(multScale,"Multiplies the current turtle scale by a scale factor, Params : scale factor (optional, default = 1.0).",eScale)
{
	if (!m.hasArg())t.multScale();
	else t.multScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(scale,"Set the current turtle scale, Params : scale (optional, default = 1.0).",eScale)
{
	if (!m.hasArg())t.scale();
	else t.scale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(surface,"Draw the predefined surface at the turtle's current location and orientation. Params : surface name (by default, 'l' exists), scale factor (optional, default= 1.0, should be positive).",ePrimitive)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  LsysWarning("Argument missing for module "+m.name()); break;
		case 1:  t.surface(m._getString(0),1); break;
		default: t.surface(m._getString(0), m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(pglshape,"Draw a geometry at the turtle's current location and orientation. Params : a geometric model, a scale factor (optional, should be positive).",ePrimitive)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else {
		PGL::PglTurtle * pg = dynamic_cast<PGL::PglTurtle *>(&t);
		if (pg) {
#if PGL_VERSION >= 0x020701
			if (m.argSize() == 1)
				pg->customGeometry(boost::python::extract<PGL::GeometryPtr>(m.getAt(0))());
			else 
				pg->customGeometry(boost::python::extract<PGL::GeometryPtr>(m.getAt(0))(),m._getReal(1));
#else
#ifdef _MSC_VER
#pragma message("Second argument of @g will be disabled. Upgrade PlantGL.")
#else
#warning Second argument of @g will be disabled. Upgrade PlantGL.
#endif
			pg->customGeometry(boost::python::extract<PGL::GeometryPtr>(m.getAt(0))());
#endif
		}
	}
}
DeclareModuleEnd


DeclareModuleBegin(elasticity,"Set Branch Elasticity. Params : real value (optional, default= 0.0, should be between [0,1]).",eTropism)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 1:  {
				real_t val = m._getReal(0);
				if (val < 0 || val > 1) LsysWarning("Invalid elasticity value");
				else t.setElasticity(val); 
				 }
			break;
		default: t.setElasticity(); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(tropism,"Set Tropism. Params : Vector3 (optional, default= (1,0,0)).",eTropism)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0: t.setTropism(); break;
		case 1:{
			bp::extract<Vector3> ex (m.getAt(0));
			if (ex.check()) t.setTropism(ex()); 
			else t.setTropism(m._getReal(0));
			   }
			break;
		case 2:{
			bp::extract<Vector2> ex (m.getAt(0));
			if (ex.check()) t.setTropism(Vector3(ex(),0)); 
			else t.setTropism(m._getReal(0),m._getReal(1));	
			   }
			break;
		case 3:
		default:
			t.setTropism(m._getReal(0),m._getReal(1),m._getReal(2)); 
			break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(setcontour,"Set Cross Section of Generalized Cylinder. Params : Curve2D.",ePrimitive)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0: LsysWarning("missing argument to SetContour"); break;
		default:
			t.setCrossSection(bp::extract<Curve2DPtr>(m.getAt(0))()); 
			break;
	}
}
DeclareModuleEnd
/*---------------------------------------------------------------------------*/

std::vector<ModuleClassPtr> * ModuleClass::PredefinedClasses = NULL;

#define CORE_DEC_PM(MName) ModuleClassPtr ModuleClass::MName;

PREDEFINED_MODULE_APPLY(CORE_DEC_PM)


std::vector<ModuleClassPtr>& ModuleClass::getPredefinedClasses()
{
	if(!ModuleClass::PredefinedClasses) 
		createPredefinedClasses();
	return *ModuleClass::PredefinedClasses; 
}

void ModuleClass::createPredefinedClasses() {
	if(!ModuleClass::PredefinedClasses)
		ModuleClass::PredefinedClasses = new ModuleClassList();
	None = new PredefinedModuleClass("","None Module.");
	LeftBracket = new DeclaredModule(push)("[","SB");
	RightBracket = new DeclaredModule(pop)("]","EB");
	// ExactRightBracket = new DeclaredModule(pop)("=]");
	ExactRightBracket = new PredefinedModuleClass("=]","Match exactly a closing bracket",PredefinedModuleClass::ePatternMatching);
	F = new DeclaredModule(F)("F");
	f = new DeclaredModule(f)("f");
	X = new PredefinedModuleClass("X","MouseIns","Module inserted just before module selected by user in visualisation.",PredefinedModuleClass::eStringManipulation); 
	Cut = new PredefinedModuleClass("%","Cut","Cut the remainder of the current branch in the string.",PredefinedModuleClass::eStringManipulation);
	Star = new PredefinedModuleClass("*","Used to specify Null production (produce *) or matching of any module in rules predecessor.",PredefinedModuleClass::ePatternMatching);
	RepExp = new PredefinedModuleClass("x","repexp","Used to specify matching of a repetition of modules in rules right context.",PredefinedModuleClass::ePatternMatching);
	RepExp->aliases.push_back("all");
	QueryPosition = new DeclaredModule(GetPos)("?P","GetPos");
	QueryHeading = new DeclaredModule(GetHead)("?H","GetHead");
	QueryUp = new DeclaredModule(GetUp)("?U","GetUp");
	QueryLeft = new DeclaredModule(GetLeft)("?L","GetLeft");
	QueryRigth = new DeclaredModule(GetRight)("?R","GetRight");
	StartGC = new DeclaredModule(startGC)("@Gc","StartGC");
	EndGC = new DeclaredModule(stopGC)("@Ge","EndGC");
	StartPolygon = new DeclaredModule(startPolygon)("{","BP");
	EndPolygon = new DeclaredModule(stopPolygon)("}","EP");
	SetPosition = new DeclaredModule(MoveTo)("@M","MoveTo");
	SetHeading = new DeclaredModule(SetHead)("@R","SetHead");
	Left = new DeclaredModule(left)("+","Left");
	Right = new DeclaredModule(right)("-","Right");
	Up = new DeclaredModule(up)("^","Up");
	Down = new DeclaredModule(down)("&","Down");
	RollL = new DeclaredModule(rollL)("/","RollL");
	RollR = new DeclaredModule(rollR)("\\","RollR");
	TurnAround = new DeclaredModule(turnAround)("|","TurnAround");
	RollToVert = new DeclaredModule(rollToVert)("@v","RollToVert");
	Sphere = new DeclaredModule(sphere)("@O","Sphere");
	Circle = new DeclaredModule(circle)("@o","Circle");
	Label = new DeclaredModule(label)("@L","Label");
	IncWidth = new DeclaredModule(incWidth)("_","IncWidth");
	DecWidth = new DeclaredModule(decWidth)("!","DecWidth");
	SetWidth = new DeclaredModule(setWidth)("SetWidth");
	IncColor = new DeclaredModule(incColor)(";","IncColor");
	DecColor = new DeclaredModule(decColor)(",","DecColor");
	SetColor = new DeclaredModule(setColor)("SetColor");
	DivScale = new DeclaredModule(divScale)("@Dd","DivScale");
	MultScale = new DeclaredModule(multScale)("@Di","MultScale");
	SetScale = new DeclaredModule(scale)("@D","SetScale");
	Surface = new DeclaredModule(surface)("surface");
	CpfgSurface = new DeclaredModule(surface)("~");
	PglShape = new DeclaredModule(pglshape)("@g","PglShape");
	Elasticity = new DeclaredModule(elasticity)("@Ts","Elasticity");
	Tropism = new DeclaredModule(tropism)("@Tp","Tropism");
	SetContour = new DeclaredModule(setcontour)("SetContour");
	GetIterator = new PredefinedModuleClass("?I","GetIterator","Request an iterator over the current Lstring.",PredefinedModuleClass::ePatternMatching);
	New = new PredefinedModuleClass("new","newmodule","Create a new module whose class is given by first argument.",PredefinedModuleClass::eStringManipulation);
}

#define CLEAR_PM(MName) ModuleClass::MName = NULL;

void ModuleClass::clearPredefinedClasses()
{
	if (ModuleClass::PredefinedClasses) {
		ModuleClass::PredefinedClasses->clear();
		delete ModuleClass::PredefinedClasses;
		ModuleClass::PredefinedClasses = NULL;
		PREDEFINED_MODULE_APPLY(CLEAR_PM)
	}
}

/*---------------------------------------------------------------------------*/


const char * PredefinedModuleClass::CATEGORY_NAME[] = {
		"None",
		"Structure",
		"Rotation",
		"Position",
		"Scale",
		"Primitive",
		"Width",
		"Color",
	    "Tropism",
		"Request",
		"String Manipulation",
		"Pattern Matching",
		"User Defined"
};

std::string PredefinedModuleClass::getCategoryName(eCategory cat){
	assert(cat < eLastCategory);
	return CATEGORY_NAME[cat];
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE
