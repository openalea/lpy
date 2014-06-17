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
#include "lpy_parser.h"
#include <plantgl/tool/util_string.h>
#include <plantgl/scenegraph/pgl_version.h>
#include <plantgl/python/export_list.h>
#include <plantgl/python/extract_list.h>
#include <plantgl/python/export_refcountptr.h>

using namespace boost::python;
#define bp boost::python
LPY_USING_NAMESPACE





std::string mc_repr(ModuleClassPtr mc){
	std::string res = "ModuleClass('"+mc->name+"',"+TOOLS(number)(mc->getId());
	if (!mc->aliases.empty()){
		res+=",['" + mc->aliases[0];
		for(size_t i = 1; i < mc->aliases.size(); ++i)
			res+= "','"+mc->aliases[i];
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

PredefinedModuleClass::eCategory py_get_category(ModuleClass * mod)
{
	if(mod->isPredefined()) return ((PredefinedModuleClass*)mod)->getCategory();
	else return PredefinedModuleClass::eUserDefined;
}

boost::python::object py_getParameterNames(const ModuleClass * mod) {
	return make_list(mod->getParameterNames())();
}

void py_setParameterNames(ModuleClass * mod, boost::python::object names) {
	mod->setParameterNames(extract_vec<std::string>(names)());
}


ModuleClassPtr py_getMClass(const std::string& name) {
	return ModuleClassTable::get().find(name);
}

boost::python::object py_getBases(const ModuleClass * mod) {
	return make_list(mod->getBases())();
}

void py_setBases(ModuleClass * mod, boost::python::object bases) {
	mod->setBases(extract_vec<ModuleClassPtr>(bases)());
}


void export_ModuleClass(){


	class_<ModuleClass,ModuleClassPtr,boost::noncopyable>
	("ModuleClass", no_init)
	.add_property("name",make_getter(&ModuleClass::name))
	.add_property("id",&ModuleClass::getId)	
	.add_property("aliases",&py_modaliases)
	.add_property("documentation",&ModuleClass::getDocumentation)
	.def("__repr__",&mc_repr)
	.def("getReferenceCount",&ModuleClass::use_count)
	.def("isPredefined",&ModuleClass::isPredefined)
	.add_static_property("predefinedClasses",py_predefinedclasses)
	.add_property("scale",&ModuleClass::getScale,&ModuleClass::setScale)
	.add_property("category",&ModuleClass::getScale,&ModuleClass::setScale)
	.add_property("bases",&py_getBases,&py_setBases)
	.add_property("parameterNames",&py_getParameterNames,&py_setParameterNames)
	.def("getParameterPosition",&ModuleClass::getParameterPosition)
	.def("hasParameter",&ModuleClass::hasParameter)
	.def("get",&py_getMClass,args("name"),"Get a module class from the name of the module")
	.staticmethod("get")
	.def("issubclass",&ModuleClass::issubclass)
    .def("getNamedParameterNb",&ModuleClass::getNamedParameterNb)
    .add_static_property("DEFAULT_SCALE",make_getter(&ModuleClass::DEFAULT_SCALE))
	;

	enum_<PredefinedModuleClass::eCategory>("ModuleCategory")
		.value("None",PredefinedModuleClass::eNone)
		.value("Structure",PredefinedModuleClass::eStructure)
		.value("Rotation",PredefinedModuleClass::eRotation)
		.value("Position",PredefinedModuleClass::ePosition)
		.value("Scale",PredefinedModuleClass::eScale)
		.value("Primitive",PredefinedModuleClass::ePrimitive)
		.value("Width",PredefinedModuleClass::eWidth)
		.value("Color",PredefinedModuleClass::eColor)
	    .value("Tropism",PredefinedModuleClass::eTropism)
		.value("Request",PredefinedModuleClass::eRequest)
		.value("StringManipulation",PredefinedModuleClass::eStringManipulation)
		.value("PatternMatching",PredefinedModuleClass::ePatternMatching)
		.value("UserDefined",PredefinedModuleClass::eUserDefined)
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
	.def("find",(ModuleClassPtr(ModuleClassTable::*)(const std::string&) const)&ModuleClassTable::find)
	.def("find",(ModuleClassPtr(ModuleClassTable::*)(size_t) const )&ModuleClassTable::find)
	.def("remove",(bool(ModuleClassTable::*)(const ModuleClass *))&ModuleClassTable::remove)
	.def("remove",(bool(ModuleClassTable::*)(const std::string&))&ModuleClassTable::remove)
	.def("declare",(ModuleClassPtr(ModuleClassTable::*)(const std::string&))&ModuleClassTable::declare)
	.def("declare",(bool (ModuleClassTable::*)(ModuleClass *))&ModuleClassTable::declare)

	;
    ModuleClassList modulelist = ModuleClassTable::get().getClasses();
    for (ModuleClassList::const_iterator itmod = modulelist.begin(); itmod != modulelist.end(); ++itmod){
        if (LpyParsing::isValidVariableName((*itmod)->name))
            scope().attr((*itmod)->name.c_str()) = object(*itmod);
        for (std::vector<std::string>::const_iterator italias = (*itmod)->aliases.begin(); italias != (*itmod)->aliases.end(); ++italias){
            if (LpyParsing::isValidVariableName(*italias))
                scope().attr(italias->c_str()) = object(*itmod);
        }
    }
}