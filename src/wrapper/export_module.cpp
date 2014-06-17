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

#include "axialtree.h"
#include "packedargs.h"
#include "matching.h"
#include "export_module.h"
#include <plantgl/tool/util_string.h>
#include <plantgl/scenegraph/pgl_version.h>
#if PGL_VERSION < 0x020700
#error You should upgrade your version of PlantGL
#endif
#include <plantgl/python/export_list.h>
#include <plantgl/python/extract_list.h>
#include <plantgl/python/export_refcountptr.h>

using namespace boost::python;
#define bp boost::python
LPY_USING_NAMESPACE

std::string getname(Module * m){
  return m->name();
}


#define MATCHING(func)\
object func(ParamModule * m1,object b){ \
  ParamModule p("*"); \
  extract<std::string> st(b); \
  if(st.check())p = ParamModule::PatternModule(st()); \
  else{ \
	extract<ParamModule> m(b); \
	if(m.check())p = m(); \
	else { \
	  extract<tuple> tu(b);\
	  if(tu.check())p = ParamModule(tu()); \
	  else { \
		AxialTree ax = extract<AxialTree>(b)(); \
		if(ax.size() != 1){ \
		  PyErr_SetString(PyExc_ValueError, "AxialTree should be of size 1"); \
		  throw_error_already_set(); \
		} \
	  }	\
	} \
  }	\
  list arg; \
  if(m1->func(p,arg)){ \
	dict d; \
	std::vector<std::string> n = p.getVarNames(); \
	int s = extract<int>(arg.attr("__len__")()); \
	int s2 = n.size(); \
	for(int i = 0; i < s2; i++){ \
	  if(i<s)d[n[i]]=arg[i]; \
	  else d[n[i]]= object(); \
	} \
	for(int j = s2; j < s; j++){ \
	  d[(std::string("unamed")+(char)('0'+j-s2)).c_str()]=arg[j]; \
	} \
	return make_tuple(true,d); \
  } \
  else return make_tuple(false,dict()); \
} 

// MATCHING(match);
/*MATCHING(match1);
MATCHING(match2);
MATCHING(match3);*/




void export_Module(){

	class_<PackedArgs> ("PackedArgs", init<boost::python::list>("PackedArgs"));

  class_<Module>
	("Module", init<const std::string&>("Module(name)"))
    .def(init<const Module &>("Module(Module)"))
    .def(init<const ModuleClassPtr>("Module(Module)"))
	.def("__str__", &Module::str)
	.def("__repr__",&Module::repr)
	.add_property("name",getname,&Module::setName)
    .def(self == self)
    .def(self != self)
	.def("isLeftBracket", &Module::isLeftBracket)
	.def("isRightBracket", &Module::isRightBracket)
	.def("isBracket", &Module::isBracket)
	.def("isRequest", &Module::isRequest)
	.def("isNull", &Module::isNull)
	.def("isCut", &Module::isCut)
	.add_property("mclass", &Module::getClass)
	.def("scale", &Module::scale)
	.def("isinstance", &Module::isinstance)
	.def("isIgnored", &Module::isIgnored)
	.def("isConsidered", &Module::isConsidered)
	;

  {
  scope in_mod = class_<ParamModule,  bases<Module> >
	("ParamModule", init<const std::string&>("ParamModule(name[,args])"))
	.def(init<const ParamModule &>("ParamModule(ParamModule)"))
    .def(init<boost::python::tuple>("ParamModule(tuple)"))
    .def(init<const ModuleClassPtr,boost::python::tuple>("ParamModule(mclass,tuple)"))
	.def(init<boost::python::list>("ParamModule(list)"))
	.def(init<const std::string&,
	          const boost::python::object &>("ParamModule(name[,args])"))
	.def(init<const std::string&,
	          const boost::python::object &,
			  const boost::python::object &>("ParamModule(name[,args])"))
	.def(init<const std::string&,
	          const boost::python::object &,
			  const boost::python::object &,
			  const boost::python::object &>("ParamModule(name[,args])"))
	.def(init<const std::string&,
	          const boost::python::object &,
			  const boost::python::object &,
			  const boost::python::object &,
			  const boost::python::object &>("ParamModule(name[,args])"))
	.def(init<const std::string&,
	          const boost::python::object &,
			  const boost::python::object &,
			  const boost::python::object &,
			  const boost::python::object &,
			  const boost::python::object &>("ParamModule(name[,args])"))
    
    .def(module_func<ParamModule>())

//	.def("match",  &match)
	.def("match", (bool(ParamModule::*)(const std::string&,size_t)const)
					  &ParamModule::match)
	.def("__eq__", (bool(ParamModule::*)(const std::string&)const) &ParamModule::operator==)
	.def("__eq__", (bool(ParamModule::*)(const ParamModule&)const) &ParamModule::operator==)
	;

  }
}