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

#include "patternmodule.h"
#include "matching.h"
#include "export_module.h"
#include <plantgl/python/export_list.h>

using namespace boost::python;
#define bp boost::python
LPY_USING_NAMESPACE

std::string var_getname(LsysVar * m){
  return m->name();
}

object qm_varnames(PatternModule * mod) { return make_list(mod->getVarNames())(); }
object var_value(LsysVar * var) { return var->getPyValue(); }

void export_PatternModule(){


  class_<LsysVar>
	("LsysVar", init<const std::string&>("LsysVar(name)"))
	.def("__str__", &LsysVar::str)
	.def("__repr__", &LsysVar::str)
	.def("varname", &LsysVar::varname)
	.def("isArgs",  &LsysVar::isArgs)
    .def("isKwds",  &LsysVar::isKwds)
    .def("value",  &var_value)
	.add_property("name",var_getname,&LsysVar::setName)
	;

  {
  scope in_mod = class_<PatternModule,  bases<Module> >
	("PatternModule", init<const std::string&,int >("PatternModule(name[,lineno])",(bp::arg("name"),bp::arg("lineno")=-1)))
	.def(init<size_t, const std::string&,int >("PatternModule(id,args[,lineno])",(bp::arg("id"),bp::arg("params"),bp::arg("lineno")=-1)))
	.def(init<const PatternModule &>("PatternModule(PatternModule)"))
    .def(module_func<PatternModule>())
	.add_static_property("matchingMethod",&MatchingEngine::getModuleMatchingMethod)
	.def("getVarNb",&PatternModule::getVarNb)
    .def("varnames",&qm_varnames)
	;

  enum_<MatchingEngine::eModuleMatchingMethod>("eMatchingMethod")
	  .value("eSimple",MatchingEngine::eMSimple)
	  .value("eWithStar",MatchingEngine::eMWithStar)
	  .value("eWithStarNValueConstraint",MatchingEngine::eMWithStarNValueConstraint)
	  .export_values();

  }

}