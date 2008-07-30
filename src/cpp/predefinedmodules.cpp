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

/*---------------------------------------------------------------------------*/

PredefinedModuleClass::PredefinedModuleClass(const std::string& name, const std::string& doc):
ModuleClass(name), documentation(doc) { PredefinedClasses->push_back(this); }

PredefinedModuleClass::PredefinedModuleClass(const std::string& name, const std::string& alias, const std::string& doc):
ModuleClass(name,alias), documentation(doc) { PredefinedClasses->push_back(this); }

PredefinedModuleClass::~PredefinedModuleClass() {}


#define DeclaredModule(modname) modname##ModuleClass
#define DeclareModuleEnd };

#define DeclareModuleBegin(modname, doc) \
class DeclaredModule(modname) : public PredefinedModuleClass  { \
public: \
	DeclaredModule(modname) (const std::string& name) :  PredefinedModuleClass(name,doc) { } \
	DeclaredModule(modname) (const std::string& name, const std::string& alias) :  PredefinedModuleClass(name,alias,doc) { } \
	~DeclaredModule(modname)() {} \
	void interpret(ParamModule& m, PGL::Turtle& t)

#define DeclareSimpleModule(modname,doc) DeclareModuleBegin(modname,doc) { t.modname(); } DeclareModuleEnd
#define DeclareModuleReal1(modname,doc) \
	DeclareModuleBegin(modname,doc) {  \
		switch (m.argSize()) { \
			case 0:  t.modname(); break; \
			default:  t.modname(m._getReal(0)); break; } \
     } \
	DeclareModuleEnd \

DeclareSimpleModule(push,"Push the state in the stack.")
DeclareSimpleModule(pop,"Pop last state from turtle stack and make it the its current state.")


DeclareModuleBegin(F, "Move forward and draw. Params: length , topradius.")
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  t.F(); break;
		case 1:  t.F(m._getReal(0)); break;
		default: t.F(m._getReal(0),m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(f, "Move forward and without draw. Params: length.")
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  t.f(); break;
		default:  t.f(m._getReal(0)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(GetPos,"Request position vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).")
{ m._setValues(t.getPosition()); }
DeclareModuleEnd

DeclareModuleBegin(GetHead,"Request heading vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).")
{ m._setValues(t.getHeading()); }
DeclareModuleEnd

DeclareModuleBegin(GetUp,"Request up vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).")
{ m._setValues(t.getUp()); }
DeclareModuleEnd

DeclareModuleBegin(GetLeft,"Request left vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).")
{ m._setValues(t.getLeft()); }
DeclareModuleEnd

DeclareModuleBegin(GetRight,"Request right vector information. Params : x,y,z or v (optional, default=Vector3, filled by Turtle).")
{ m._setValues(-t.getLeft()); }
DeclareModuleEnd

DeclareSimpleModule(startGC, "Start a new generalized cylinder.")
DeclareSimpleModule(stopGC,  "Pop generalized cylinder from the stack and render it.")
DeclareSimpleModule(startPolygon,"Start a new polygon.")
DeclareSimpleModule(stopPolygon,"Pop a polygon from the stack and render it.")

DeclareModuleBegin(MoveTo,"Set the turtle position. Params : x, y, z (optionals, default = 0).")
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

DeclareModuleBegin(SetHead,"Set the turtle Heading and Up vector. Params: hx, hy, hz, ux, uy, uz (optionals, default=0,0,1, 1,0,0).")
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

DeclareModuleReal1(left,  "Turn left  around Up vector. Params : angle (optional, in degrees).")
DeclareModuleReal1(right, "Turn right around Up vector. Params : angle (optional, in degrees).")
DeclareModuleReal1(up,    "Pitch up around Left vector. Params : angle (optional, in degrees).")
DeclareModuleReal1(down,  "Pitch down around Left vector. Params : angle (optional, in degrees).")
DeclareModuleReal1(rollL, "Roll left  around Heading vector. Params : angle (optional, in degrees).")
DeclareModuleReal1(rollR, "Roll right  around Heading vector. Params : angle (optional, in degrees).")

#if PGL_VERSION >= 0x020501
DeclareSimpleModule(turnAround, "Turn around 180deg the Up vector.")
#else
DeclareModuleBegin(turnAround)
{ t.left(180); }
DeclareModuleEnd

#ifdef _MSC_VER
#pragma message("You should upgrade your version of PlantGL.")
#else
#warning You should upgrade your version of PlantGL.
#endif
#endif

DeclareSimpleModule(rollToVert, "Roll to Vertical : Roll the turtle around the H axis so that H and U lie in a common vertical plane with U closest to up")
DeclareModuleReal1(sphere,"Draw a sphere. Params : radius (optional, should be positive, default = line width).")
DeclareModuleReal1(circle,"Draw a circle. Params : radius (optional, should be positive, default = line width).")

DeclareModuleBegin(label,"Draw a text label.")
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.label(m._getString(0));
}
DeclareModuleEnd

DeclareModuleBegin(incWidth,"Increase the current line width or set it if a parameter is given. Params : line width (optional).")
{
	if (!m.hasArg())t.incWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(decWidth,"Decrease the current line width or set it if a parameter is given. Params : line width (optional).")
{
	if (!m.hasArg())t.decWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(setWidth,"Set current line width. Params : line width.")
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(incColor,"Increase the current material index or set it if a parameter is given. Params : color index (optional, positive int).")
{
	if (!m.hasArg())t.incColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(decColor,"Decrease the current material index or set it if a parameter is given. Params : color index (optional, positive int).")
{
	if (!m.hasArg())t.decColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(setColor,"Set the current material index. Params : color index (positive int).")
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(divScale,"Divides the current turtle scale by a scale factor, Params : scale factor (optional, default = 1.0).")
{
	if (!m.hasArg())t.divScale();
	else t.divScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(multScale,"Multiplies the current turtle scale by a scale factor, Params : scale factor (optional, default = 1.0).")
{
	if (!m.hasArg())t.multScale();
	else t.multScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(scale,"Set the current turtle scale, Params : scale (optional, default = 1.0).")
{
	if (!m.hasArg())t.scale();
	else t.scale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(surface,"Draw the predefined surface at the turtle's current location and orientation. Params : surface name (by default, 'l' exists), scale factor (optional, default= 1.0, should be positive).")
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  LsysWarning("Argument missing for module "+m.name()); break;
		case 1:  t.surface(m._getString(0),1); break;
		default: t.surface(m._getString(0), m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(pglshape,"Draw a geometry at the turtle's current location and orientation. Params : surface name (by default, 'l' exists), a scale factor (optional, should be positive).")
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


/*---------------------------------------------------------------------------*/

std::vector<ModuleClassPtr> * ModuleClass::PredefinedClasses = NULL;

ModuleClassPtr ModuleClass::None;
ModuleClassPtr ModuleClass::LeftBracket;
ModuleClassPtr ModuleClass::RightBracket;
ModuleClassPtr ModuleClass::ExactRightBracket;
ModuleClassPtr ModuleClass::F;
ModuleClassPtr ModuleClass::f;
ModuleClassPtr ModuleClass::G;
ModuleClassPtr ModuleClass::g;
ModuleClassPtr ModuleClass::X;
ModuleClassPtr ModuleClass::Cut;
ModuleClassPtr ModuleClass::Star;
ModuleClassPtr ModuleClass::QueryPosition;
ModuleClassPtr ModuleClass::QueryHeading;
ModuleClassPtr ModuleClass::QueryUp;
ModuleClassPtr ModuleClass::QueryLeft;
ModuleClassPtr ModuleClass::QueryRigth;
ModuleClassPtr ModuleClass::StartGC;
ModuleClassPtr ModuleClass::EndGC;
ModuleClassPtr ModuleClass::StartPolygon;
ModuleClassPtr ModuleClass::EndPolygon;
ModuleClassPtr ModuleClass::SetPosition;
ModuleClassPtr ModuleClass::SetHeading;
ModuleClassPtr ModuleClass::Left;
ModuleClassPtr ModuleClass::Right;
ModuleClassPtr ModuleClass::Up;
ModuleClassPtr ModuleClass::Down;
ModuleClassPtr ModuleClass::RollL;
ModuleClassPtr ModuleClass::RollR;
ModuleClassPtr ModuleClass::TurnAround;
ModuleClassPtr ModuleClass::RollToVert;
ModuleClassPtr ModuleClass::Sphere;
ModuleClassPtr ModuleClass::Circle;
ModuleClassPtr ModuleClass::Label;
ModuleClassPtr ModuleClass::IncWidth;
ModuleClassPtr ModuleClass::DecWidth;
ModuleClassPtr ModuleClass::SetWidth;
ModuleClassPtr ModuleClass::IncColor;
ModuleClassPtr ModuleClass::DecColor;
ModuleClassPtr ModuleClass::SetColor;
ModuleClassPtr ModuleClass::DivScale;
ModuleClassPtr ModuleClass::MultScale;
ModuleClassPtr ModuleClass::SetScale;
ModuleClassPtr ModuleClass::Surface;
ModuleClassPtr ModuleClass::CpfgSurface;
ModuleClassPtr ModuleClass::PglShape;

std::vector<ModuleClassPtr>& ModuleClass::getPredefinedClasses()
{
	if(!ModuleClass::PredefinedClasses) 
		createPredefinedClasses();
	return *ModuleClass::PredefinedClasses; 
}

void ModuleClass::createPredefinedClasses() {
	if(!ModuleClass::PredefinedClasses)
		ModuleClass::PredefinedClasses = new ModuleClassVector();
	None = new PredefinedModuleClass("","None Module.");
	LeftBracket = new DeclaredModule(push)("[","SB");
	RightBracket = new DeclaredModule(pop)("]","EB");
	ExactRightBracket = new DeclaredModule(pop)("=]");
	F = new DeclaredModule(F)("F");
	f = new DeclaredModule(f)("f");
	G = new DeclaredModule(F)("G");
	g = new DeclaredModule(f)("g");
	X = new PredefinedModuleClass("X","MouseIns","Module inserted just before module selected by user in visualisation."); 
	Cut = new PredefinedModuleClass("%","Cut","Cut the remainder of the current branch in the string.");
	Star = new PredefinedModuleClass("*","Used to specify Null production (produce *) or matching of any module in rules predecessor.");
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
}

void ModuleClass::clearPredefinedClasses()
{
	if (ModuleClass::PredefinedClasses) {
		ModuleClass::PredefinedClasses->clear();
		delete ModuleClass::PredefinedClasses;
		ModuleClass::PredefinedClasses = NULL;
		ModuleClass::PglShape = NULL;
		ModuleClass::CpfgSurface = NULL;
		ModuleClass::Surface = NULL;
		ModuleClass::SetScale = NULL;
		ModuleClass::MultScale = NULL;
		ModuleClass::DivScale = NULL;
		ModuleClass::SetColor = NULL;
		ModuleClass::DecColor = NULL;
		ModuleClass::IncColor = NULL;
		ModuleClass::SetWidth = NULL;
		ModuleClass::DecWidth = NULL;
		ModuleClass::IncWidth = NULL;
		ModuleClass::Label = NULL;
		ModuleClass::Circle = NULL;
		ModuleClass::Sphere = NULL;
		ModuleClass::RollToVert = NULL;
		ModuleClass::TurnAround = NULL;
		ModuleClass::RollR = NULL;
		ModuleClass::RollL = NULL;
		ModuleClass::Down = NULL;
		ModuleClass::Up = NULL;
		ModuleClass::Right = NULL;
		ModuleClass::Left = NULL;
		ModuleClass::SetHeading = NULL;
		ModuleClass::SetPosition = NULL;
		ModuleClass::EndPolygon = NULL;
		ModuleClass::StartPolygon = NULL;
		ModuleClass::EndGC = NULL;
		ModuleClass::StartGC = NULL;
		ModuleClass::QueryRigth = NULL;
		ModuleClass::QueryLeft = NULL;
		ModuleClass::QueryUp = NULL;
		ModuleClass::QueryHeading = NULL;
		ModuleClass::QueryPosition = NULL;
		ModuleClass::Star = NULL;
		ModuleClass::Cut = NULL;
		ModuleClass::X = NULL;
		ModuleClass::g = NULL;
		ModuleClass::G = NULL;
		ModuleClass::f = NULL;
		ModuleClass::F = NULL;
		ModuleClass::ExactRightBracket = NULL;
		ModuleClass::RightBracket = NULL;
		ModuleClass::LeftBracket = NULL;
		ModuleClass::None = NULL;
	}
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE
