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

PYLSYS_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

#define DeclaredModule(modname) modname##ModuleClass
#define DeclareModuleEnd };

#define DeclareModuleBegin(modname) \
class DeclaredModule(modname) : public ModuleClass  { \
public: \
	DeclaredModule(modname) (const std::string& name): ModuleClass(name) {} \
	DeclaredModule(modname) (const std::string& name, const std::string& alias) :  ModuleClass(name,alias) {} \
	~DeclaredModule(modname)() {} \
	void interpret(ParamModule& m, PGL::Turtle& t)

#define DeclareSimpleModule(modname) DeclareModuleBegin(modname) { t.modname(); } DeclareModuleEnd
#define DeclareModuleReal1(modname) \
	DeclareModuleBegin(modname) {  \
		switch (m.argSize()) { \
			case 0:  t.modname(); break; \
			default:  t.modname(m._getReal(0)); break; } \
     } \
	DeclareModuleEnd \

DeclareSimpleModule(push)
DeclareSimpleModule(pop)

DeclareModuleBegin(F)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  t.F(); break;
		case 1:  t.F(m._getReal(0)); break;
		default: t.F(m._getReal(0),m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(f)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  t.f(); break;
		default:  t.f(m._getReal(0)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(GetPos)
{ m._setValues(t.getPosition()); }
DeclareModuleEnd

DeclareModuleBegin(GetHead)
{ m._setValues(t.getHeading()); }
DeclareModuleEnd

DeclareModuleBegin(GetUp)
{ m._setValues(t.getUp()); }
DeclareModuleEnd

DeclareModuleBegin(GetLeft)
{ m._setValues(t.getLeft()); }
DeclareModuleEnd

DeclareModuleBegin(GetRight)
{ m._setValues(-t.getLeft()); }
DeclareModuleEnd

DeclareSimpleModule(startGC)
DeclareSimpleModule(stopGC)
DeclareSimpleModule(startPolygon)
DeclareSimpleModule(stopPolygon)

DeclareModuleBegin(MoveTo)
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

DeclareModuleBegin(SetHead)
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

DeclareModuleReal1(left)
DeclareModuleReal1(right)
DeclareModuleReal1(up)
DeclareModuleReal1(down)
DeclareModuleReal1(rollL)
DeclareModuleReal1(rollR)

#if PGL_VERSION >= 0x020501
DeclareSimpleModule(turnAround)
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

DeclareSimpleModule(rollToVert)
DeclareModuleReal1(sphere)
DeclareModuleReal1(circle)

DeclareModuleBegin(label)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.label(m._getString(0));
}
DeclareModuleEnd

DeclareModuleBegin(incWidth)
{
	if (!m.hasArg())t.incWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(decWidth)
{
	if (!m.hasArg())t.decWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(setWidth)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(incColor)
{
	if (!m.hasArg())t.incColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(decColor)
{
	if (!m.hasArg())t.decColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(setColor)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(divScale)
{
	if (!m.hasArg())t.divScale();
	else t.divScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(multScale)
{
	if (!m.hasArg())t.multScale();
	else t.multScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(scale)
{
	if (!m.hasArg())t.scale();
	else t.scale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(surface)
{
	size_t nbargs = m.argSize();
	switch (nbargs) {
		case 0:  LsysWarning("Argument missing for module "+m.name()); break;
		case 1:  t.surface(m._getString(0),1); break;
		default: t.surface(m._getString(0), m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(pglshape)
{
	if(!m.hasArg())LsysWarning("Argument missing for module "+m.name());
	else {
		PGL::PglTurtle * pg = dynamic_cast<PGL::PglTurtle *>(&t);
		if (pg) pg->customGeometry(boost::python::extract<PGL::GeometryPtr>(m.getAt(0))());
	}
}
DeclareModuleEnd


/*---------------------------------------------------------------------------*/

ModuleClassPtr ModuleClass::None(new ModuleClass(""));
ModuleClassPtr ModuleClass::LeftBracket(new DeclaredModule(push)("[","SB"));
ModuleClassPtr ModuleClass::RightBracket(new DeclaredModule(pop)("]","EB"));
ModuleClassPtr ModuleClass::ExactRightBracket(new DeclaredModule(pop)("=]"));
ModuleClassPtr ModuleClass::F(new DeclaredModule(F)("F"));
ModuleClassPtr ModuleClass::f(new DeclaredModule(f)("f"));
ModuleClassPtr ModuleClass::G(new DeclaredModule(F)("G"));
ModuleClassPtr ModuleClass::g(new DeclaredModule(f)("g"));
ModuleClassPtr ModuleClass::X(new ModuleClass("X","MouseIns"));
ModuleClassPtr ModuleClass::Cut(new ModuleClass("%","Cut"));
ModuleClassPtr ModuleClass::Star(new ModuleClass("*"));
ModuleClassPtr ModuleClass::QueryPosition(new DeclaredModule(GetPos)("?P","GetPos"));
ModuleClassPtr ModuleClass::QueryHeading(new DeclaredModule(GetHead)("?H","GetHead"));
ModuleClassPtr ModuleClass::QueryUp(new DeclaredModule(GetUp)("?U","GetUp"));
ModuleClassPtr ModuleClass::QueryLeft(new DeclaredModule(GetLeft)("?L","GetLeft"));
ModuleClassPtr ModuleClass::QueryRigth(new DeclaredModule(GetRight)("?R","GetRight"));
ModuleClassPtr ModuleClass::StartGC(new DeclaredModule(startGC)("@Gc","StartGC"));
ModuleClassPtr ModuleClass::EndGC(new DeclaredModule(stopGC)("@Ge","EndGC"));
ModuleClassPtr ModuleClass::StartPolygon(new DeclaredModule(startPolygon)("{","BP"));
ModuleClassPtr ModuleClass::EndPolygon(new DeclaredModule(stopPolygon)("}","EP"));
ModuleClassPtr ModuleClass::SetPosition(new DeclaredModule(MoveTo)("@M","MoveTo"));
ModuleClassPtr ModuleClass::SetHeading(new DeclaredModule(SetHead)("@R","SetHead"));
ModuleClassPtr ModuleClass::Left(new DeclaredModule(left)("+","Left"));
ModuleClassPtr ModuleClass::Right(new DeclaredModule(right)("-","Right"));
ModuleClassPtr ModuleClass::Up(new DeclaredModule(up)("^","Up"));
ModuleClassPtr ModuleClass::Down(new DeclaredModule(down)("&","Down"));
ModuleClassPtr ModuleClass::RollL(new DeclaredModule(rollL)("/","RollL"));
ModuleClassPtr ModuleClass::RollR(new DeclaredModule(rollR)("\\","RollR"));
ModuleClassPtr ModuleClass::TurnAround(new DeclaredModule(turnAround)("|","TurnAround"));
ModuleClassPtr ModuleClass::RollToVert(new DeclaredModule(rollToVert)("@v","RollToVert"));
ModuleClassPtr ModuleClass::Sphere(new DeclaredModule(sphere)("@O","Sphere"));
ModuleClassPtr ModuleClass::Circle(new DeclaredModule(circle)("@o","Circle"));
ModuleClassPtr ModuleClass::Label(new DeclaredModule(label)("@L","Label"));
ModuleClassPtr ModuleClass::IncWidth(new DeclaredModule(incWidth)("_","IncWidth"));
ModuleClassPtr ModuleClass::DecWidth(new DeclaredModule(decWidth)("!","DecWidth"));
ModuleClassPtr ModuleClass::SetWidth(new DeclaredModule(setWidth)("SetWidth"));
ModuleClassPtr ModuleClass::IncColor(new DeclaredModule(incColor)(";","IncColor"));
ModuleClassPtr ModuleClass::DecColor(new DeclaredModule(decColor)(",","DecColor"));
ModuleClassPtr ModuleClass::SetColor(new DeclaredModule(setColor)("SetColor"));
ModuleClassPtr ModuleClass::DivScale(new DeclaredModule(divScale)("@Dd","DivScale"));
ModuleClassPtr ModuleClass::MultScale(new DeclaredModule(multScale)("@Di","MultScale"));
ModuleClassPtr ModuleClass::SetScale(new DeclaredModule(scale)("@D","SetScale"));
ModuleClassPtr ModuleClass::Surface(new DeclaredModule(surface)("surface"));
ModuleClassPtr ModuleClass::CpfgSurface(new DeclaredModule(surface)("~"));
ModuleClassPtr ModuleClass::PglShape(new DeclaredModule(pglshape)("@g","PglShape"));

void ModuleClass::clearPredefinedModules()
{
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

/*---------------------------------------------------------------------------*/

PYLSYS_END_NAMESPACE
