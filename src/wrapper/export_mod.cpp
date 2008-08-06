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
#include <plantgl/tool/util_string.h>
#include <plantgl/scenegraph/pgl_version.h>
#if PGL_VERSION < 0x020700
#error You should upgrade your version of PlantGL
#endif
#include <plantgl/python/export_list.h>

using namespace boost::python;
LPY_USING_NAMESPACE

std::string getname(Module * m){
  return m->name();
}

std::string var_getname(LsysVar * m){
  return m->name();
}

AxialTree mods_add1(ParamModule * m1, ParamModule * m2){
  AxialTree res(*m1);
  return res+=*m2;
}

AxialTree mods_add2(ParamModule * m1, const tuple& m2){
  AxialTree res(*m1);
  return res+=ParamModule(m2);
}

AxialTree mods_add3(ParamModule * m1, const std::string& m2){
  AxialTree res(*m1);
  return res+=ParamModule(m2);
}

#define MATCHING(func)\
object func(ParamModule * m1,object b){ \
  ParamModule p("*"); \
  extract<std::string> st(b); \
  if(st.check())p = ParamModule::QueryModule(st()); \
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

MATCHING(match);
MATCHING(match1);
MATCHING(match2);
MATCHING(match3);


std::string mc_repr(ModuleClassPtr mc){
	std::string res = "ModuleClass('"+mc->name+"',"+TOOLS(number)(mc->getId());
	if (!mc->aliases.empty()){
		res+=",['" + mc->aliases[0];
		for(size_t i = 1; i < mc->aliases.size(); ++i)
			res+= "',"+mc->aliases[i];
		res += "']";
	}
	res += ")";
	return res;
}

boost::python::object py_modaliases(ModuleClass * m) {
	return make_list<std::vector<std::string> >(m->aliases)();
}

boost::python::object py_modclasses(ModuleClassTable * m) {
	return make_list<std::vector<ModuleClassPtr> >(m->getClasses())();
}

boost::python::object py_modnames(ModuleClassTable * m) {
	return make_list<std::vector<std::string> >(m->getNames())();
}

boost::python::object py_predefinedclasses() {
	return make_list<ModuleClassList>(ModuleClass::getPredefinedClasses())();
}

void export_Module(){

	class_<ModuleClass,ModuleClassPtr,boost::noncopyable>
	("ModuleClass", no_init)
	.add_property("name",make_getter(&ModuleClass::name))
	.add_property("id",&ModuleClass::getId)	
	.add_property("aliases",&py_modaliases)
	.add_property("documentation",&ModuleClass::getDocumentation)
	.def("__repr__",&mc_repr)
	.def("getReferenceCount",&ModuleClass::getReferenceCount)
	.def("isPredefined",&ModuleClass::isPredefined)
	.add_static_property("predefinedClasses",py_predefinedclasses)
	;

	class_<ModuleClassTable,boost::noncopyable>
	("ModuleClassTable", no_init)
	// .def("__repr__",&mc_repr)
	.def("get",&ModuleClassTable::get,return_value_policy<reference_existing_object>())
	.staticmethod("get")
	.add_property("mandatory_declaration",make_getter(&ModuleClassTable::mandatory_declaration),
									      make_setter(&ModuleClassTable::mandatory_declaration))
	.def("size",&ModuleClassTable::size)
	.def("empty",&ModuleClassTable::empty)
	.def("getClasses",&py_modclasses)
	.def("getNames",&py_modnames)
	.def("getClass",(ModuleClassPtr(ModuleClassTable::*)(const std::string&) )&ModuleClassTable::getClass)
	.def("getClass",(ModuleClassPtr(ModuleClassTable::*)(size_t) const )&ModuleClassTable::getClass)
	.def("remove",(bool(ModuleClassTable::*)(const ModuleClass *))&ModuleClassTable::remove)
	.def("remove",(bool(ModuleClassTable::*)(const std::string&))&ModuleClassTable::remove)
	.def("declare",(ModuleClassPtr(ModuleClassTable::*)(const std::string&))&ModuleClassTable::declare)
	.def("declare",(bool (ModuleClassTable::*)(ModuleClass *))&ModuleClassTable::declare)

	;

  class_<LsysVar>
	("LsysVar", init<const std::string&>("LsysVar(name)"))
	.def("__str__", &LsysVar::str)
	.def("__repr__", &LsysVar::str)
	.def("varname", &LsysVar::varname)
	.def("isArgs",  &LsysVar::isArgs)
	.add_property("name",var_getname,&LsysVar::setName)
	;

  class_<Module>
	("Module", init<const std::string&>("Module(name)"))
	.def(init<const Module &>("Module(Module)"))
	.def("__str__", &Module::str)
	.def("__repr__",&Module::repr)
	.add_property("name",getname,&Module::setName)
	.def("hasArg",  &Module::hasArg)
	.def("argSize", &Module::argSize)
	.def("__len__", &Module::argSize)
    .def(self == self)
    .def(self != self)
	.def("isLeftBracket", &Module::isLeftBracket)
	.def("isRightBracket", &Module::isRightBracket)
	.def("isBracket", &Module::isBracket)
	.def("isQuery", &Module::isQuery)
	.def("isNull", &Module::isNull)
	.def("isCut", &Module::isCut)
	.add_property("mclass", &Module::getClass)
	;

  {
  scope in_mod = class_<ParamModule,  bases<Module> >
	("ParamModule", init<const std::string&>("ParamModule(name[,args])"))
	.def(init<const ParamModule &>("ParamModule(ParamModule)"))
	.def(init<boost::python::tuple>("ParamModule(tuple)"))
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
	.def("__tuple__",  &ParamModule::tuple)
	.def("__getitem__",&ParamModule::getAt)
	.def("__setitem__",&ParamModule::setAt)
	.def("__delitem__",&ParamModule::delAt)
	.def("__getslice__",&ParamModule::getslice)
	.def("__delslice__",&ParamModule::delslice)
	.def("__iadd__",   &ParamModule::operator+=, return_internal_reference<1>())
	.def("__add__",   &ParamModule::operator+)
	.def("__add__",   &mods_add1)
	.def("__add__",   &mods_add2)
	.def("__add__",   &mods_add3)
    .def(self == self)
    .def(self != self)
	.def("match",  &match)
	.def("match1", &match1)
	.def("match2", &match2)
	.def("match3", &match3)
	.def("match", (bool(ParamModule::*)(const std::string&,size_t)const)
					  &ParamModule::match)
	.add_property("args",&ParamModule::getArgs,&ParamModule::setArgs)
	.add_static_property("matchingMethod",&ParamModule::getMatchingMethod)
	;
  enum_<ParamModule::eMatchingMethod>("eMatchingMethod")
	  .value("eSimple",ParamModule::eSimple)
	  .value("eWithStar",ParamModule::eWithStar)
	  .value("eWithStarNValueConstraint",ParamModule::eWithStarNValueConstraint);

  }
  def("QueryModule",  (ParamModule(*)(size_t, const std::string&))&ParamModule::QueryModule);
  def("QueryModule",  (ParamModule(*)(const std::string&))&ParamModule::QueryModule);
}