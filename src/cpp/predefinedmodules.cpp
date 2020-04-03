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
#include "module.h"
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
		switch (m.size()) { \
			case 0:  t.modname(); break; \
			default:  t.modname(m._getReal(0)); break; } \
     } \
	DeclareModuleEnd \

DeclareSimpleModule(push,"Push the state in the stack.",eStructure)
DeclareSimpleModule(pop,"Pop last state from turtle stack and make it the its current state.",eStructure)


DeclareModuleBegin(F, "Move forward and draw. Params: 'length , topradius'.",ePrimitive)
{
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0:  t.F(); break;
		case 1:  t.F(m._getReal(0)); break;
		default: t.F(m._getReal(0),m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(f, "Move forward and without draw. Params: 'length'.",ePrimitive)
{
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0:  t.f(); break;
		default:  t.f(m._getReal(0)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(nF,"Produce a n steps path of a given length and varying radius. Params : 'length, dlength [, radius = 1, radiusvariation = None]'.",ePrimitive)
{
#if PGL_VERSION >= 0x020F00
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0: 
		case 1: 
			LsysWarning("missing argument to nF"); break;
		case 2: 
            t.nF(m._getReal(0),m._getReal(1)); break;
		case 3:
            t.nF(m._getReal(0),m._getReal(1),m._getReal(2)); break;
		default:
            t.nF(m._getReal(0),m._getReal(1),m._getReal(2),bp::extract<QuantisedFunctionPtr>(m.getAt(3)));
			break;
	}
#endif
}
DeclareModuleEnd


DeclareModuleBegin(GetPos,"Request position vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getPosition()); }
DeclareModuleEnd

DeclareModuleBegin(GetHead,"Request heading vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getHeading()); }
DeclareModuleEnd

DeclareModuleBegin(GetUp,"Request up vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getUp()); }
DeclareModuleEnd

DeclareModuleBegin(GetLeft,"Request left vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(t.getLeft()); }
DeclareModuleEnd

DeclareModuleBegin(GetRight,"Request right vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).",eRequest)
{ m._setValues(-t.getLeft()); }
DeclareModuleEnd

DeclareModuleBegin(GetFrame,"Request turtle frame information. Params : 'p,h,u,l' (optional, filled by Turtle).",eRequest)
{ m._setFrameValues(t.getPosition(),t.getHeading(),t.getUp(),t.getLeft()); }
DeclareModuleEnd

DeclareSimpleModule(startGC, "Start a new generalized cylinder.",ePrimitive)
DeclareSimpleModule(stopGC,  "Pop generalized cylinder from the stack and render it.",ePrimitive)
DeclareSimpleModule(startPolygon,"Start a new polygon.",ePrimitive)
DeclareSimpleModule(polygonPoint,"Add a point for polygon.",ePrimitive)
DeclareModuleBegin(stopPolygon,"Pop a polygon from the stack and render it. Params : concavetest (default=False).",ePrimitive)
{
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0:  t.stopPolygon(); break;
		default:  t.stopPolygon(m._getBool(0)); break;
	}
}
DeclareModuleEnd


DeclareModuleBegin(MoveTo,"Set the turtle position. Params : 'x, y, z' or 'v' (optionals, default = None for not changing specific coordinates).",ePosition)
{
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  t.move(); break;
         case 1:  
           {
              bp::extract<Vector3> ex (m.getAt(0));
              if (ex.check()) { t.move(ex());}
              else {
                real_t y = t.getPosition().y(); 
                real_t z = t.getPosition().z();
                t.move(m._getReal(0),y,z); 
              }
              break;
           }
         case 2:  {
            real_t x = 0;
            if (m.getAt(0) == boost::python::object()){ x = t.getPosition().x(); }
            else x = m._getReal(0);
            real_t y = 0;
            if (m.getAt(1) == boost::python::object()){ y = t.getPosition().y(); }
            else y = m._getReal(1);
            real_t z = t.getPosition().z();
            t.move(x,y,z); break;
        }
         default: {
            real_t x = 0;
            if (m.getAt(0) == boost::python::object()){ x = t.getPosition().x(); }
            else x = m._getReal(0);
            real_t y = 0;
            if (m.getAt(1) == boost::python::object()){ y = t.getPosition().y(); }
            else y = m._getReal(1);
            real_t z = 0;
            if (m.getAt(2) == boost::python::object()){ z = t.getPosition().z(); }
            else z = m._getReal(2);
            t.move(x,y,z); break;
        }
	}
}
DeclareModuleEnd



DeclareModuleBegin(moveRel,"Move relatively from current the turtle position. Params : 'x, y, z' or 'v'(optionals, default = 0).",ePosition)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1: 
           {
              bp::extract<Vector3> ex (m.getAt(0));
              if (ex.check()) { t.shift(ex());}
              else t.shift(m._getReal(0)); 
              break;
           }
         case 2:  t.shift(m._getReal(0),m._getReal(1)); break;
         default: t.shift(m._getReal(0),m._getReal(1),m._getReal(2)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(lineTo,"Trace line to (x,y,z) without changing the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter' (optionals, default = 0).",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1:  
           {
              bp::extract<Vector3> ex (m.getAt(0));
              if (ex.check()) { t.lineTo(ex());}
              else t.lineTo(m._getReal(0),0,0); break;
           }
         case 2:  
           {
                bp::extract<Vector3> ex (m.getAt(0));
                if (ex.check()) { t.lineTo(ex(),m._getReal(1));}
                else t.lineTo(m._getReal(0),m._getReal(1)); break;
           }
         case 3:  t.lineTo(m._getReal(0),m._getReal(1),m._getReal(2)); break;
         default: t.lineTo(m._getReal(0),m._getReal(1),m._getReal(2),m._getReal(3)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(orientedLineTo,"Trace line toward (x,y,z) and change the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter' (optionals, default = 0).",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1:  
             {
                 bp::extract<Vector3> ex (m.getAt(0));
                if (ex.check()) { t.oLineTo(ex());}
                else t.oLineTo(m._getReal(0)); 
                break;
             }
         case 2: 
             {
                bp::extract<Vector3> ex (m.getAt(0));
                if (ex.check()) { t.oLineTo(ex(),m._getReal(1));}
                else t.oLineTo(m._getReal(0),m._getReal(1)); 
                break;
             }
         case 3:  t.oLineTo(m._getReal(0),m._getReal(1),m._getReal(2)); break;
         default: t.oLineTo(m._getReal(0),m._getReal(1),m._getReal(2),m._getReal(3)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(pinPoint,"Orient turtle toward (x,y,z) . Params : 'x, y, z' or 'v' (optionals, default = 0).",eRotation)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1:  
             {
                bp::extract<Vector3> ex (m.getAt(0));
                if (ex.check()) { t.pinpoint(ex());}
                else t.pinpoint(m._getReal(0)); 
                break;
             }
         case 2:  t.pinpoint(m._getReal(0),m._getReal(1)); break;
         default: t.pinpoint(m._getReal(0),m._getReal(1),m._getReal(2)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(lineRel,"Trace line to pos+(x,y,z) without changing the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter'(optionals, default = 0).",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1:  
             {
                bp::extract<Vector3> ex (m.getAt(0));
                if (ex.check()) { t.lineRel(ex());}
                else t.lineRel(m._getReal(0),0,0); 
                break;
             }
         case 2: 
             {
                bp::extract<Vector3> ex (m.getAt(0));
                if (ex.check()) { t.lineRel(ex(),m._getReal(1));}
                else t.lineRel(m._getReal(0),m._getReal(1)); break;
             }
         case 3:  t.lineRel(m._getReal(0),m._getReal(1),m._getReal(2)); break;
         default: t.lineRel(m._getReal(0),m._getReal(1),m._getReal(2),m._getReal(3)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(oLineRel,"Trace line toward pos+(x,y,z) and change the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter' (optionals, default = 0).",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1:
             {
                 bp::extract<Vector3> ex (m.getAt(0));
                 if (ex.check()) { t.oLineRel(ex());}
                 else t.oLineRel(m._getReal(0)); 
                 break;
             }
         case 2:  
             {
                 bp::extract<Vector3> ex (m.getAt(0));
                 if (ex.check()) { t.oLineRel(ex(),m._getReal(1));}
                 else t.oLineRel(m._getReal(0),m._getReal(1)); break;
             }
         case 3:  t.oLineRel(m._getReal(0),m._getReal(1),m._getReal(2)); break;
         default: t.oLineRel(m._getReal(0),m._getReal(1),m._getReal(2),m._getReal(3)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(pinPointRel,"Orient turtle toward pos+(x,y,z) . Params : 'x, y, z' or 'v' (optionals, default = 0).",eRotation)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  break;
         case 1:  
            {
                 bp::extract<Vector3> ex (m.getAt(0));
                 if (ex.check()) { t.pinpointRel(ex());}
                 else t.pinpointRel(m._getReal(0)); 
                 break;
            }
         case 2:  t.pinpointRel(m._getReal(0),m._getReal(1)); break;
         default: t.pinpointRel(m._getReal(0),m._getReal(1),m._getReal(2)); break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(SetHead,"Set the turtle Heading and Up vector. Params: 'hx, hy, hz, ux, uy, uz' or 'h,v' (optionals, default=0,0,1, 1,0,0).",eRotation)
{
	size_t nbargs = m.size();
    Vector3 h;
    Vector3 u;
    bool withu = false;
	switch (nbargs) {
      case 0:  t.setHead(); return;
      case 1:  
      {
          bp::extract<Vector3> ex (m.getAt(0));
          if (ex.check()) h = ex();
          else h = Vector3(m._getReal(0),0,0); 
          break;
      }
      case 2:  
       {
            bp::extract<Vector3> ex (m.getAt(0));
		    if (ex.check()) {
                  h = ex();
                  u = m._get<Vector3>(1);
                  withu = true;
            }
		    else h = Vector3(m._getReal(0),m._getReal(1)); 
            break;
       }
      case 3:
          {
            h = Vector3(m._getReal(0),m._getReal(1),m._getReal(2));
            break;
          }
      case 4: 
          {
            h = Vector3(m._getReal(0),m._getReal(1),m._getReal(2));
            u = Vector3(m._getReal(3));
            withu = true;
            break;
          }
      case 5:  
          {
            h = Vector3(m._getReal(0),m._getReal(1),m._getReal(2));
            u = Vector3(m._getReal(3),m._getReal(4));
            withu = true;
            break;
          }
      default: 
          {
            h = Vector3(m._getReal(0),m._getReal(1),m._getReal(2));
            u = Vector3(m._getReal(3),m._getReal(4),m._getReal(5));
            withu = true;
            break;
          }
    }
    real_t l = h.normalize();
    if (l < GEOM_EPSILON) 
        LsysError("SetHead should have a valid heading direction");
    else if (!withu) t.pinpointRel(h);
    else {
        l = u.normalize();
        if (l < GEOM_EPSILON) {
            LsysWarning("SetHead should have a valid up direction");
            t.pinpointRel(h);
        }
        else t.setHead(h,u); 
    }

}
DeclareModuleEnd

DeclareModuleBegin(eulerAngles,"Set the orientation of the turtle from the absolute euler angles. Params: 'azimuth, elevation, roll' (optionals, default=180,90,0).",eRotation)
{
    size_t nbargs = m.size();
    real_t azimuth = 180;
    real_t elevation = 90;
    real_t roll = 0;
    switch (nbargs) {
      case 0:  t.eulerAngles(); return;
      case 1:  
      {
        azimuth = m._getReal(0);
        break;
      }
      case 2:  
       {
        azimuth = m._getReal(0);
        elevation = m._getReal(1);
        break;
       }
      default: 
       {
        azimuth = m._getReal(0);
        elevation = m._getReal(1);
        roll = m._getReal(2);
        break;
       }
    }
#if PGL_VERSION >= 0x021400
    t.eulerAngles(azimuth,elevation,roll);
#else
    t.setHead(1,0,0,0,0,1);
    t.left(azimuth);
    t.up(elevation);
    t.rollL(roll);
#endif
}
DeclareModuleEnd

DeclareModuleReal1(left,  "Turn left  around Up vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(right, "Turn right around Up vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(up,    "Pitch up around Left vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(down,  "Pitch down around Left vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(rollL, "Roll left  around Heading vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(rollR, "Roll right  around Heading vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(iRollL, "Roll left intrinsically around Heading vector. Params : 'angle' (optional, in degrees).",eRotation)
DeclareModuleReal1(iRollR, "Roll right intrinsically around Heading vector. Params : 'angle' (optional, in degrees).",eRotation)

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

DeclareModuleBegin(rollToVert, "Roll to Vertical : Roll the turtle around the H axis so that H and U lie in a common vertical plane with U closest to up",eRotation)
{
 #if PGL_VERSION >= 0x021500
    switch(m.size()){
        case 0: t.rollToVert(); break;
        default: t.rollToVert(m._getReal(0)); break;
    }
#else
    t.rollToVert();
#ifdef _MSC_VER
#pragma message("RollToVert module with parameter will be disabled. Upgrade PlantGL.")
#else
#warning RollToVert module with parameter will be disabled. Upgrade PlantGL.
#endif

#endif
}
DeclareModuleEnd

DeclareModuleBegin(rollToHorizontal, "Roll to Horizontal : Roll the turtle so that H lie in the horizontal plane",eRotation)
{
#if PGL_VERSION >= 0x021500
    switch(m.size()){
        case 0: t.rollToHorizontal(); break;
        default: t.rollToHorizontal(m._getReal(0)); break;
    }
#else

#ifdef _MSC_VER
#pragma message("RollToHorizontal module will be disabled. Upgrade PlantGL.")
#else
#warning RollToHorizontal module will be disabled. Upgrade PlantGL.
#endif
#endif
}
DeclareModuleEnd

DeclareModuleReal1(sphere,"Draw a sphere. Params : 'radius' (optional, should be positive, default = line width).",ePrimitive)
DeclareModuleReal1(circle,"Draw a circle. Params : 'radius' (optional, should be positive, default = line width).",ePrimitive)

DeclareModuleBegin(box,"Draw a box. Params : 'length','topradius'.",ePrimitive)
{
 #if PGL_VERSION >= 0x021300
    switch(m.size()){
        case 0: t.box(); break;
        case 1: t.box(m._getReal(0)); break;
        default:
           t.box(m._getReal(0),m._getReal(1)); break;
    }
#endif
}
DeclareModuleEnd

DeclareModuleBegin(quad,"Draw a quad. Params : 'length','topradius'.",ePrimitive)
{
 #if PGL_VERSION >= 0x021300
    switch(m.size()){
        case 0: t.quad(); break;
        case 1: t.quad(m._getReal(0)); break;
        default:
           t.quad(m._getReal(0),m._getReal(1)); break;
    }
#endif
}
DeclareModuleEnd


DeclareModuleBegin(label,"Draw a text label. Params : 'text','size'.",ePrimitive)
{
	if(m.empty())LsysWarning("Argument missing for module "+m.name());
	else 
  {
 #if PGL_VERSION >= 0x021200
   int size = -1;
    if (m.size() >= 2) size = m._getInt(1);
    t.label(m._getString(0),size);
#else
    t.label(m._getString(0));
#endif
  }
}
DeclareModuleEnd

DeclareModuleBegin(incWidth,"Increase the current line width or set it if a parameter is given. Params : 'width' (optional).",eWidth)
{
	if (m.empty())t.incWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(decWidth,"Decrease the current line width or set it if a parameter is given. Params : 'width' (optional).",eWidth)
{
	if (m.empty())t.decWidth();
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(setWidth,"Set current line width. Params : 'width'.",eWidth)
{
	if(m.empty())LsysWarning("Argument missing for module "+m.name());
	else t.setWidth(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(incColor,"Increase the current material index or set it if a parameter is given. Params : 'index' (optional, positive int).",eColor)
{
	if (m.empty())t.incColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(decColor,"Decrease the current material index or set it if a parameter is given. Params : 'index' (optional, positive int).",eColor)
{
	if (m.empty())t.decColor();
	else t.setColor(m._getInt(0));
}
DeclareModuleEnd

DeclareModuleBegin(setColor,"Set the current material. Params : 'index' (positive int) or 'r,g,b[,a]' or 'material'.",eColor)
{
    if(m.empty()) t.setColor(t.getColor());
    else {
        int nbatt = m.size();
        if (nbatt == 1) {
            boost::python::extract<PGL::AppearancePtr>  appextractor(m.getAt(0));
            if (appextractor.check()) t.setCustomAppearance(appextractor());
            else t.setColor(m._getInt(0));
        }
        else if (nbatt >= 3) {
            Material * mat = new Material(Color3(m._get<uchar_t>(0),m._get<uchar_t>(1),m._get<uchar_t>(2)),1);
            if (nbatt >= 4) mat->getTransparency() = m._get<uchar_t>(3) / 255.f;
            t.setCustomAppearance(AppearancePtr(mat));
        }
    }
}
DeclareModuleEnd


DeclareModuleBegin(interpolateColors,"Set the current material. Params : 'index1', 'index2', 'alpha' .",eColor)
{
#if PGL_VERSION >= 0x021300
    size_t nbargs = m.size();
    switch (nbargs) {
         case 0: 
         case 1:  
           {
            LsysWarning("Argument missing for module "+m.name());
              break;
           }
         case 2:  
            t.interpolateColors(m._getInt(0),m._getInt(1)); break;
         default: 
            t.interpolateColors(m._getInt(0),m._getInt(1),m._getReal(2)); break;
    }
#else
#ifdef _MSC_VER
#pragma message("InterpolateColors module will be disabled. Upgrade PlantGL.")
#else
#warning InterpolateColors module will be disabled. Upgrade PlantGL.
#endif
#endif
}
DeclareModuleEnd


DeclareModuleBegin(textureBaseColor,"Set the base color of the texture. Params : 'index' (positive int) or 'r,g,b[,a]' or 'material'.",eTexture)
{
#if PGL_VERSION >= 0x021500
    if(!m.empty()) {
        int nbatt = m.size();
        if (nbatt == 1) {
            boost::python::extract<PGL::MaterialPtr>  appextractor(m.getAt(0));
            if (appextractor.check()) t.setTextureBaseColor(Color4(appextractor()->getDiffuseColor(), appextractor()->getTransparency()));
            else t.setTextureBaseColor(m._getInt(0));
        }
        else if (nbatt >= 3) {
            Color4 c(m._get<uchar_t>(0),m._get<uchar_t>(1),m._get<uchar_t>(2),0);
            if (nbatt >= 4) c.getAlpha() = m._get<uchar_t>(3) / 255.f;
            t.setTextureBaseColor(c);
        }
    } 
#else
#ifdef _MSC_VER
#pragma message("TextureBaseColor module will be disabled. Upgrade PlantGL.")
#else
#warning TextureBaseColor module will be disabled. Upgrade PlantGL.
#endif
#endif
}
DeclareModuleEnd

DeclareModuleBegin(interpolateTextureBaseColors,"Set the base color of the texture from interpolation of 2 predefined material. Params : 'index1', 'index2', 'alpha' .",eTexture)
{
#if PGL_VERSION >= 0x021500
    size_t nbargs = m.size();
    switch (nbargs) {
         case 0: 
         case 1:  
           {
            LsysWarning("Argument missing for module "+m.name());
              break;
           }
         case 2:  
            t.interpolateTextureBaseColors(m._getInt(0),m._getInt(1)); break;
         default: 
            t.interpolateTextureBaseColors(m._getInt(0),m._getInt(1),m._getReal(2)); break;
    }
#else
#ifdef _MSC_VER
#pragma message("InterpolateTextureBaseColors module will be disabled. Upgrade PlantGL.")
#else
#warning InterpolateColors module will be disabled. Upgrade PlantGL.
#endif
#endif
}

DeclareModuleEnd
DeclareModuleBegin(divScale,"Divides the current turtle scale by a scale factor, Params : 'scale_factor' (optional, default = 1.0).",eScale)
{
	if (m.empty())t.divScale();
	else t.divScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(multScale,"Multiplies the current turtle scale by a scale factor, Params : 'scale_factor' (optional, default = 1.0).",eScale)
{
	if (m.empty())t.multScale();
	else t.multScale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(scale,"Set the current turtle scale, Params : 'scale' (optional, default = 1.0).",eScale)
{
	if (m.empty())t.scale();
	else t.scale(m._getReal(0));
}
DeclareModuleEnd

DeclareModuleBegin(surface,"Draw the predefined surface at the turtle's current location and orientation. Params : 'surface_name' (by default, 'l' exists), 'scale_factor' (optional, default= 1.0, should be positive).",ePrimitive)
{
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0:  LsysWarning("Argument missing for module "+m.name()); break;
		case 1:  t.surface(m._getString(0),1); break;
		default: t.surface(m._getString(0), m._getReal(1)); break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(pglshape,"Draw a geometry at the turtle's current location and orientation. Params : 'geometric_model', 'scale_factor' (optional, should be positive) or 'shape' or 'scene' or 'material'.",ePrimitive)
{
	if(m.empty())LsysWarning("Argument missing for module "+m.name());
	else {
		PGL::PglTurtle * pg = dynamic_cast<PGL::PglTurtle *>(&t);
		if (pg) {
#if PGL_VERSION >= 0x020701
			if (m.size() == 1){
                boost::python::extract<PGL::GeometryPtr>  geomextractor(m.getAt(0));
				if(geomextractor.check())pg->customGeometry(geomextractor());
                else {
                    boost::python::extract<PGL::AppearancePtr>  appextractor(m.getAt(0));
                    if (appextractor.check()) pg->setCustomAppearance(appextractor());
                    else {
                        boost::python::extract<PGL::ShapePtr>  shextractor(m.getAt(0));
                        if (shextractor.check()) {
                            PGL::ShapePtr sh = shextractor();
                            pg->setCustomAppearance(sh->getAppearance());
                            pg->customGeometry(sh->getGeometry());
                            pg->removeCustomAppearance();
                        }
                        else {
                            boost::python::extract<PGL::ScenePtr>  scextractor(m.getAt(0));
                            if (scextractor.check()) {
                                PGL::ScenePtr sc = scextractor();
                                for(Scene::const_iterator it = sc->begin(); it != sc->end(); ++it){
                                    PGL::ShapePtr sh = dynamic_pointer_cast<PGL::Shape>(*it);
                                    if (sh) {
                                        pg->setCustomAppearance(sh->getAppearance());
                                        pg->customGeometry(sh->getGeometry());
                                    }
                                }
                                pg->removeCustomAppearance();
                            }
                            else { LsysWarning("Cannot find geometry for module "+m.name()); }
                        }

                    }
                }
            }
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

DeclareModuleBegin(Frame,"Draw the current turtle frame as 3 arrows (red=heading,blue=up,green=left). Params : 'size' (should be positive), 'cap_heigth_ratio' (in [0,1]), 'cap_radius_ratio' (should be positive).",ePrimitive)
{
#if PGL_VERSION >= 0x020A00
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0:  t.frame(); break;
		case 1:  t.frame(m._getReal(0)); break;
		case 2:  t.frame(m._getReal(0), m._getReal(1)); break;
		case 3:  t.frame(m._getReal(0), m._getReal(1), m._getReal(2)); break;
		case 4:  t.frame(m._getReal(0), m._getReal(1), m._getReal(2), m._getReal(3)); break;
		default: t.frame(m._getReal(0), m._getReal(1), m._getReal(2), m._getReal(3), m._getReal(4)); break;
	}
#else
#ifdef _MSC_VER
#pragma message("Frame module will be disabled. Upgrade PlantGL.")
#else
#warning Frame module will be disabled. Upgrade PlantGL.
#endif
#endif
}
DeclareModuleEnd

DeclareModuleBegin(Arrow,"Draw an arrow. Params : 'size' (should be positive), 'cap_heigth_ratio' (in [0,1]), 'cap_radius_ratio' (should be positive).",ePrimitive)
{
#if PGL_VERSION >= 0x030100
    size_t nbargs = m.size();
    switch (nbargs) {
        case 0:  t.arrow(); break;
        case 1:  t.arrow(m._getReal(0)); break;
        case 2:  t.arrow(m._getReal(0), m._getReal(1)); break;
        default:  t.arrow(m._getReal(0), m._getReal(1), m._getReal(2)); break;
    }
#else
#ifdef _MSC_VER
#pragma message("Arrow module will be disabled. Upgrade PlantGL.")
#else
#warning Arrow module will be disabled. Upgrade PlantGL.
#endif
#endif
}
DeclareModuleEnd

DeclareModuleBegin(elasticity,"Set Branch Elasticity. Params : 'elasticity' (optional, default= 0.0, should be between [0,1]).",eTropism)
{
	size_t nbargs = m.size();
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

DeclareModuleBegin(tropism,"Set Tropism. Params : 'tropism' (optional, Vector3, default= (1,0,0)).",eTropism)
{
	size_t nbargs = m.size();
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

DeclareModuleBegin(setcontour,"Set Cross Section of Generalized Cylinder. Params : 'Curve2D [, ccw]'.",ePrimitive)
{
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0: LsysWarning("missing argument to SetContour"); break;
		case 1:
			t.setCrossSection(bp::extract<Curve2DPtr>(m.getAt(0))()); 
			break;
		case 2:
			t.setCrossSection(bp::extract<Curve2DPtr>(m.getAt(0))(),bp::extract<bool>(m.getAt(1))()); 
			break;
	}
}
DeclareModuleEnd


DeclareModuleBegin(sectionResolution,"Set Resolution of Section of Cylinder. Params : 'resolution' (int).",ePrimitive)
{
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0: LsysWarning("missing argument to SectionResolution"); break;
		default:
			t.setSectionResolution(m._getInt(0)); 
			break;
	}
}
DeclareModuleEnd

DeclareModuleBegin(setguide,"Set Guide for turtle tracing. Params : 'Curve[2D|3D], length [,yorientation, ccw]'.",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0: 
			t.clearGuide(); break;
		case 1: 			
			LsysWarning("missing argument to SetGuide"); break;
		default:
			bp::extract<Curve2DPtr> ec2d(m.getAt(0));
			if (ec2d.check()) {
				if(nbargs == 2) t.setGuide(ec2d(),m._getReal(1)); 
				else if(nbargs == 3) t.setGuide(ec2d(),m._getReal(1),bp::extract<bool>(m.getAt(2))); 
				else t.setGuide(ec2d(),m._getReal(1),bp::extract<bool>(m.getAt(2)),bp::extract<bool>(m.getAt(3))); 
			}
			else t.setGuide(bp::extract<LineicModelPtr>(m.getAt(0))(),m._getReal(1)); 
			break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(endguide,"End Guide for turtle tracing.",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	t.clearGuide();
#endif
}
DeclareModuleEnd

DeclareModuleBegin(positiononguide,"Set position on Guide for turtle tracing.",ePrimitive)
{
#if PGL_VERSION >= 0x020B00
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0: 
			LsysWarning("missing argument to PositionOnGuide"); break;
		default:
			t.setPositionOnGuide(m._getReal(0));
		break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(sweep,"Produce a sweep surface. Params : 'path, section, length, dlength [, radius = 1, radiusvariation = None]'.",ePrimitive)
{
#if PGL_VERSION >= 0x020F00
	size_t nbargs = m.size();
	switch (nbargs) {
		case 0: 
		case 1: 
		case 2: 
		case 3: 
			LsysWarning("missing argument to sweep"); break;
		case 4:
            {
			    bp::extract<Curve2DPtr> ec2d(m.getAt(0));
			    if (ec2d.check()) t.sweep(ec2d(), bp::extract<Curve2DPtr>(m.getAt(1))(),m._getReal(2),m._getReal(3)); 
			    else t.sweep(bp::extract<LineicModelPtr>(m.getAt(0))(), bp::extract<Curve2DPtr>(m.getAt(1))(),m._getReal(2),m._getReal(3)); 
            }
            break;
		case 5:
            {
			    bp::extract<Curve2DPtr> ec2d(m.getAt(0));
			    if (ec2d.check()) t.sweep(ec2d(), bp::extract<Curve2DPtr>(m.getAt(1))(),m._getReal(2),m._getReal(3),m._getReal(4)); 
			    else t.sweep(bp::extract<LineicModelPtr>(m.getAt(0))(), bp::extract<Curve2DPtr>(m.getAt(1))(),m._getReal(2),m._getReal(3),m._getReal(4)); 
            }
            break;
		default:
            {
			    bp::extract<Curve2DPtr> ec2d(m.getAt(0));
			    if (ec2d.check()) t.sweep(ec2d(), bp::extract<Curve2DPtr>(m.getAt(1))(),m._getReal(2),m._getReal(3),m._getReal(4),bp::extract<QuantisedFunctionPtr>(m.getAt(5))()); 
			    else t.sweep(bp::extract<LineicModelPtr>(m.getAt(0))(), bp::extract<Curve2DPtr>(m.getAt(1))(),m._getReal(2),m._getReal(3),m._getReal(4),bp::extract<QuantisedFunctionPtr>(m.getAt(5))()); 
            }
			break;
	}
#endif
}
DeclareModuleEnd

DeclareModuleBegin(TextureScale,"Set the scale coefficient for texture application. Params : 'uscale, vscale' (default = 1,1) or 'scale'.",eTexture)
{
#if PGL_VERSION >= 0x020C00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  LsysWarning("missing argument to TextureScale");  break;
         case 1:  
             {
			 real_t valueU = m._getReal(0);
			 if (fabs(valueU) < GEOM_EPSILON ) LsysWarning("invalid argument to TextureVCoeff. Should be non null.");
			 else t.setTextureScale(valueU,valueU); 
			 break;
             }
         default:  
			 real_t valueU = m._getReal(0);
			 real_t valueV = m._getReal(1);
			 if (fabs(valueU) < GEOM_EPSILON || fabs(valueV) < GEOM_EPSILON) LsysWarning("invalid argument to TextureVCoeff. Should be non null.");
			 else t.setTextureScale(valueU,valueV); 
			 break;
	}
}
#endif
DeclareModuleEnd

DeclareModuleBegin(TextureUScale,"Set the u-scale coefficient for texture application. Params : 'uscale' (default = 1).",eTexture)
{
#if PGL_VERSION >= 0x020C00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  LsysWarning("missing argument to TextureUScale");  break;
         default:  
			 real_t value = m._getReal(0);
			 if (fabs(value) < GEOM_EPSILON) LsysWarning("invalid argument to TextureUScale. Should be non null.");
			 else t.setTextureUScale(value); 
			 break;
	}
}
#endif
DeclareModuleEnd

DeclareModuleBegin(TextureVScale,"Set the v-scale coefficient for texture application. Params : 'vscale' (default = 1).",eTexture)
{
#if PGL_VERSION >= 0x020C00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  LsysWarning("missing argument to TextureVScale");  break;
         default:  
			 real_t value = m._getReal(0);
			 if (fabs(value) < GEOM_EPSILON) LsysWarning("invalid argument to TextureVScale. Should be non null.");
			 else t.setTextureVScale(value); 
			 break;
	}
}
#endif
DeclareModuleEnd

DeclareModuleBegin(TextureTranslation,"Set the translation for texture application. Params : 'utranslation, vtranslation' (default = 0,0) or 'translation'.",eTexture)
{
#if PGL_VERSION >= 0x020C00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  LsysWarning("missing argument to TextureTranslation");  break;
         case 1:  t.setTextureTranslation(m._get<Vector2>(0)); break;
         default:  
			 t.setTextureTranslation(m._getReal(0),m._getReal(1)); 
			 break;
	}
}
#endif
DeclareModuleEnd

DeclareModuleBegin(TextureRotation,"Set the rotation for texture application. Params : 'angle, urotcenter, vrotcenter' (default = 0,0.5,0.5) or 'angle, rotcenter'.",eTexture)
{
#if PGL_VERSION >= 0x020C00
	size_t nbargs = m.size();
	switch (nbargs) {
         case 0:  LsysWarning("missing argument to TextureTranslation"); break;
         case 1:  t.setTextureRotation(m._getReal(0));  break;
		 case 2:
             {
               bp::extract<Vector2> ex(m.getAt(1));
               if (ex.check())t.setTextureRotation(m._getReal(0),ex()); 
               else t.setTextureRotation(m._getReal(0),m._getReal(1)); 
               break;
             }
         default:  
			 t.setTextureRotation(m._getReal(0),m._getReal(1),m._getReal(2)); 
			 break;
	}
}
#endif
DeclareModuleEnd

DeclareModuleBegin(TextureTransformation,"Set the transformation for texture application. Params : 'uscale, vscale, utranslation, vtranslation, angle, urotcenter, vrotcenter' (default = 1,1,0,0,0,0.5,0.5) or 'scale, translation, angle, rotcenter'.",eTexture)
{
#if PGL_VERSION >= 0x020C00
	size_t nbargs = m.size();
	if (nbargs < 4)LsysWarning("missing argument to TextureCoeff"); 
	if (nbargs == 4)
    {
        bp::extract<Vector2> ex(m.getAt(0));
        if (ex.check()){
            Vector2 s = ex();
            if (fabs(s.x()) < GEOM_EPSILON || fabs(s.y()) < GEOM_EPSILON) LsysWarning("invalid argument to TextureTransformation. Should be non null.");
            t.setTextureTransformation(s,m._get<Vector2>(1),m._getReal(2),m._get<Vector2>(3)); 
        }
    	else {
             if (nbargs < 7)LsysWarning("missing argument to TextureCoeff"); 
			 real_t valueU = m._getReal(0);
			 real_t valueV = m._getReal(1);
			 if (fabs(valueU) < GEOM_EPSILON || fabs(valueV) < GEOM_EPSILON) LsysWarning("invalid argument to TextureTransformation. Should be non null.");
			 else t.setTextureTransformation(valueU,valueV,m._getReal(2),m._getReal(3),m._getReal(4),m._getReal(5),m._getReal(6)); 
        }
	}
}
#endif
DeclareModuleEnd

DeclareSimpleModule(leftReflection,"The turtle change the left vector to have a symmetric behavior.",eRotation)
DeclareSimpleModule(upReflection,"The turtle change the up vector to have a symmetric behavior.",eRotation)
DeclareSimpleModule(headingReflection,"The turtle change the heading vector to have a symmetric behavior.",eRotation)

DeclareModuleBegin(startScreenProjection,"The turtle will create geometry in the screen coordinates system.",ePosition)
{
#if PGL_VERSION >= 0x021200
  t.setScreenCoordinatesEnabled(true);
  t.move(0,0,0);
#endif
}
DeclareModuleEnd

DeclareModuleBegin(endScreenProjection,"The turtle will create geometry in the world system (default behaviour).",ePosition)
{
#if PGL_VERSION >= 0x021200
  t.setScreenCoordinatesEnabled(false);
#endif
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
	None = new PredefinedModuleClass("","None", "None Module.");
	LeftBracket = new DeclaredModule(push)("[","SB");
	RightBracket = new DeclaredModule(pop)("]","EB");
	// ExactRightBracket = new DeclaredModule(pop)("=]");
	ExactRightBracket = new PredefinedModuleClass("=]","Match exactly a closing bracket",PredefinedModuleClass::ePatternMatching);
	F = new DeclaredModule(F)("F");
	f = new DeclaredModule(f)("f");
	nF = new DeclaredModule(nF)("nF");
	X = new PredefinedModuleClass("X","MouseIns","Module inserted just before module selected by user in visualisation.",PredefinedModuleClass::eStringManipulation); 
	Cut = new PredefinedModuleClass("%","Cut","Cut the remainder of the current branch in the string.",PredefinedModuleClass::eStringManipulation);
	Star = new PredefinedModuleClass("*","any","Used to match any module in rules predecessor. First argument will become name of the module.",PredefinedModuleClass::ePatternMatching);
	RepExp = new PredefinedModuleClass("x","repexp","Used to specify matching of a repetition of modules.",PredefinedModuleClass::ePatternMatching);
	RepExp->aliases.push_back("all");
	Or = new PredefinedModuleClass("or","||","Used to specify an alternative matching of modules.",PredefinedModuleClass::ePatternMatching);
	QueryPosition = new DeclaredModule(GetPos)("?P","GetPos");
	QueryHeading = new DeclaredModule(GetHead)("?H","GetHead");
	QueryUp = new DeclaredModule(GetUp)("?U","GetUp");
	QueryLeft = new DeclaredModule(GetLeft)("?L","GetLeft");
	QueryRigth = new DeclaredModule(GetRight)("?R","GetRight");
	QueryFrame = new DeclaredModule(GetFrame)("?F","GetFrame");
	StartGC = new DeclaredModule(startGC)("@Gc","StartGC");
	EndGC = new DeclaredModule(stopGC)("@Ge","EndGC");
	StartPolygon = new DeclaredModule(startPolygon)("{","BP");
	EndPolygon = new DeclaredModule(stopPolygon)("}","EP");
	PointPolygon = new DeclaredModule(polygonPoint)(".","PP");
	SetPosition = new DeclaredModule(MoveTo)("@M","MoveTo");
	SetPositionRel = new DeclaredModule(moveRel)("MoveRel");
	LineTo= new DeclaredModule(lineTo)("LineTo");
	LineOrientedTo= new DeclaredModule(orientedLineTo)("OLineTo");
	PinPoint= new DeclaredModule(pinPoint)("Pinpoint");
	LineRel= new DeclaredModule(lineRel)("LineRel");
	LineOrientedRel = new DeclaredModule( oLineRel )("OLineRel");
	PinPointRel= new DeclaredModule(pinPointRel)("PinpointRel");
    SetHeading = new DeclaredModule(SetHead)("@R","SetHead");
    EulerAngles = new DeclaredModule(eulerAngles)("EulerAngles");
	Left = new DeclaredModule(left)("+","Left");
	Right = new DeclaredModule(right)("-","Right");
	Up = new DeclaredModule(up)("^","Up");
	Down = new DeclaredModule(down)("&","Down");
	RollL = new DeclaredModule(rollL)("/","RollL");
	RollR = new DeclaredModule(rollR)("\\","RollR");
	iRollL = new DeclaredModule(iRollL)("iRollL");
	iRollR = new DeclaredModule(iRollR)("iRollR");
	TurnAround = new DeclaredModule(turnAround)("|","TurnAround");
    RollToVert = new DeclaredModule(rollToVert)("@v","RollToVert");
    RollToHorizontal = new DeclaredModule(rollToHorizontal)("@h","RollToHorizontal");
    Sphere = new DeclaredModule(sphere)("@O","Sphere");
    Box = new DeclaredModule(box)("@B","Box");
    Quad = new DeclaredModule(quad)("@b","Quad");
	Circle = new DeclaredModule(circle)("@o","Circle");
	Label = new DeclaredModule(label)("@L","Label");
	IncWidth = new DeclaredModule(incWidth)("_","IncWidth");
	DecWidth = new DeclaredModule(decWidth)("!","DecWidth");
	SetWidth = new DeclaredModule(setWidth)("SetWidth");
	IncColor = new DeclaredModule(incColor)(";","IncColor");
	DecColor = new DeclaredModule(decColor)(",","DecColor");
    SetColor = new DeclaredModule(setColor)("SetColor");
    InterpolateColors = new DeclaredModule(interpolateColors)("InterpolateColors");
    TextureBaseColor = new DeclaredModule(textureBaseColor)("TextureBaseColor");
    InterpolateTextureBaseColors = new DeclaredModule(interpolateTextureBaseColors)("InterpolateTextureBaseColors");
	DivScale = new DeclaredModule(divScale)("@Dd","DivScale");
	MultScale = new DeclaredModule(multScale)("@Di","MultScale");
	SetScale = new DeclaredModule(scale)("@D","SetScale");
	Surface = new DeclaredModule(surface)("surface");
	CpfgSurface = new DeclaredModule(surface)("~");
	PglShape = new DeclaredModule(pglshape)("@g","PglShape");
    Frame = new DeclaredModule(Frame)("Frame");
    Arrow = new DeclaredModule(Arrow)("Arrow");
	Elasticity = new DeclaredModule(elasticity)("@Ts","Elasticity");
	Tropism = new DeclaredModule(tropism)("@Tp","Tropism");
	SetContour = new DeclaredModule(setcontour)("SetContour");
	SectionResolution = new DeclaredModule(sectionResolution)("SectionResolution");
	SetGuide = new DeclaredModule(setguide)("SetGuide");
	EndGuide = new DeclaredModule(endguide)("EndGuide");
	Sweep = new DeclaredModule(sweep)("Sweep");
	PositionOnGuide = new DeclaredModule(positiononguide)("PositionOnGuide");
	TextureScale = new DeclaredModule(TextureScale)("TextureScale");
	TextureUScale = new DeclaredModule(TextureUScale)("TextureUScale");
	TextureVScale = new DeclaredModule(TextureVScale)("TextureVScale","TextureVCoeff");
	TextureTranslation = new DeclaredModule(TextureTranslation)("TextureTranslation");
	TextureRotation = new DeclaredModule(TextureRotation)("TextureRotation");
	TextureTransformation = new DeclaredModule(TextureTransformation)("TextureTransformation");
	LeftReflection = new DeclaredModule(leftReflection)("LeftReflection");
  UpReflection = new DeclaredModule(upReflection)("UpReflection");
  HeadingReflection = new DeclaredModule(headingReflection)("HeadingReflection");
  StartScreenProjection = new DeclaredModule(startScreenProjection)("@2D","StartScreenProjection");
  EndScreenProjection = new DeclaredModule(endScreenProjection)("@3D","EndScreenProjection");

	GetIterator = new PredefinedModuleClass("?I","GetIterator","Request an iterator over the current Lstring.",PredefinedModuleClass::ePatternMatching);
	GetModule = new PredefinedModuleClass("$","GetModule","Request a module of the current Lstring.",PredefinedModuleClass::ePatternMatching);
	New = new PredefinedModuleClass("new","newmodule","Create a new module whose name is given by first argument.",PredefinedModuleClass::eStringManipulation);
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
		"Texture",
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
