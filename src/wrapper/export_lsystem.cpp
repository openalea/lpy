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

#include "lsystem.h"
#include <plantgl/python/export_list.h>
using namespace boost::python;
#define bp boost::python

#include <string>
LPY_USING_NAMESPACE


void check_group(int& group, size_t nbGroup) {
  if(group < 0)group += nbGroup;
  if(group < 0 || group >= nbGroup){
	PyErr_SetString(PyExc_IndexError, "group index out of range");
    throw_error_already_set();
  }
}

void check_ruleid(int& id, size_t nbRule) {
  if(id < 0)id += nbRule;
  if(id < 0 || id >= nbRule){
	PyErr_SetString(PyExc_IndexError, "rule index out of range");
    throw_error_already_set();
  }
}

const LsysRule& prodRule(Lsystem * lsys, int pos = 0, int group = 0)
{ 
  check_group(group,lsys->nbGroups());
  check_ruleid(pos,lsys->nbProductionRules(group));
  return lsys->prodRule(pos,group);
}

const LsysRule& decRule(Lsystem * lsys, int pos = 0, int group = 0)
{ 
  check_group(group,lsys->nbGroups());
  check_ruleid(pos,lsys->nbDecompositionRules(group));
  return lsys->decRule(pos,group);
}

const LsysRule& homRule(Lsystem * lsys, int pos, int group)
{ 
  check_group(group,lsys->nbGroups());
  check_ruleid(pos,lsys->nbHomomorphismRules(group));
  return lsys->homRule(pos,group);
}


AxialTree lsys_axiom(Lsystem * lsys) { return lsys->getAxiom(); }

void lsys_setCode1(Lsystem * lsys, const std::string& code) { return lsys->set(code); }
object lsys_setCode2(Lsystem * lsys, const std::string& code, bool debug) { 
	if (!debug) { lsys->set(code); return object(); }
	else {
		std::string pycode;
		lsys->set(code,&pycode); 
		return object(pycode);
	}
}

AxialTree py_iter(Lsystem * lsys, size_t beg, size_t end, const AxialTree& wstring)
{ return lsys->iterate(beg,end,wstring); }

void py_set_debugger(Lsystem * lsys, Lsystem::Debugger * debugger)
{ return lsys->setDebugger(Lsystem::DebuggerPtr(debugger)); }

object py_get_rule_fonction_table(const Lsystem * lsys)
{
	return make_dict(lsys->get_rule_fonction_table())();
}

void export_Lsystem(){
  enum_<eDirection>("eDirection")
	  .value("eForward",eForward)
	  .value("eBackward",eBackward)
	  .export_values()
	  ;
  
  class_<Lsystem,boost::noncopyable>
	("Lsystem", init<optional<std::string> >("Lsystem([filename])"))
	.add_property("axiom",&lsys_axiom,(void(Lsystem::*)(const AxialTree&))&Lsystem::setAxiom)
	.add_property("derivationLength",&Lsystem::derivationLength,&Lsystem::setDerivationLength)
	.add_property("decompositionMaxDepth",&Lsystem::decompositionMaxDepth,&Lsystem::setDecompositionMaxDepth)
	.add_property("homomorphismMaxDepth",&Lsystem::homomorphismMaxDepth,&Lsystem::setHomomorphismMaxDepth)
	.add_property("filename",&Lsystem::getFilename,&Lsystem::setFilename)
	.def("__str__", &Lsystem::str)
	//.def("__repr__", &Lsystem::str)
	.def("context", &Lsystem::context,return_internal_reference<>())
	.def("makeCurrent",    &Lsystem::makeCurrent)
	.def("isCurrent",      &Lsystem::isCurrent)
	.def("done",           &Lsystem::done)
	.def("isCompiled",&Lsystem::isCompiled)
	.def("compile",&Lsystem::compile)
	.def("clear", &Lsystem::clear)
	.def("empty", &Lsystem::empty)
	.def("code", &Lsystem::code)
	.def("read", &Lsystem::read)
	.def("set", &lsys_setCode1)
	.def("set", &lsys_setCode2)
	.def("plot", (void(Lsystem::*)(AxialTree&,bool))&Lsystem::plot,(bp::arg("lstring"),bp::arg("checkLastComputedScene")=false))
	.def("iterate", (AxialTree(Lsystem::*)())&Lsystem::iterate)
	.def("iterate", (AxialTree(Lsystem::*)(size_t))&Lsystem::iterate)
    .def("iterate", &py_iter)
	.def("iterate", (AxialTree(Lsystem::*)(size_t,size_t,const AxialTree&,bool))&Lsystem::iterate)
	.def("interpret", (void(Lsystem::*)(AxialTree&))&Lsystem::interpret)
	.def("interpret", (void(Lsystem::*)(AxialTree&, PGL::Turtle&))&Lsystem::interpret)
	.def("sceneInterpretation", &Lsystem::sceneInterpretation)
	.def("homomorphism", &Lsystem::homomorphism)
	.def("nbProductionRules", &Lsystem::nbProductionRules, (bp::arg("group")=0))
	.def("nbDecompositionRules", &Lsystem::nbDecompositionRules, (bp::arg("group")=0))
	.def("nbHomomorphismRules", &Lsystem::nbHomomorphismRules, (bp::arg("group")=0))
	.def("nbTotalRules", &Lsystem::nbTotalRules)
	.def("nbGroups", &Lsystem::nbGroups)
	.def("prodRule", prodRule, return_internal_reference<>(), (bp::arg("ruleid")=0,bp::arg("group")=0))
	.def("decRule", decRule, return_internal_reference<>(), (bp::arg("ruleid")=0,bp::arg("group")=0))
	.def("homRule", homRule, return_internal_reference<>(), (bp::arg("ruleid")=0,bp::arg("group")=0))
	.def("animate", (AxialTree(Lsystem::*)())&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double,size_t))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double,size_t,size_t))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(const AxialTree&,double,size_t,size_t))&Lsystem::animate)
	.def("record",  (void(Lsystem::*)(const std::string&))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,size_t))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,size_t,size_t))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,size_t,size_t))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,const AxialTree&,size_t,size_t))&Lsystem::record)
	.def("addRule",      (void(Lsystem::*)(const std::string&, int, size_t))&Lsystem::addRule)
	.def("addRule",      (void(Lsystem::*)(const LsysRule&, int, size_t))&Lsystem::addRule)
	.def("addProdRule",  &Lsystem::addProdRule, "Add Production rule")
	.def("addDecRule",   &Lsystem::addDecRule, "Add Decomposition rule")
	.def("addHomRule",   &Lsystem::addHomRule, "Add Homorphism rule")
    // .def("enableEarlyReturn", &Lsystem::enableEarlyReturn, "Allow an early return (for threaded application).")
    // .def("isEarlyReturnEnabled", &Lsystem::isEarlyReturnEnabled, "Tell if an early return is required (for threaded application).")
	.add_property("early_return",&Lsystem::isEarlyReturnEnabled,&Lsystem::enableEarlyReturn)
	.def("getLastIterationNb",&Lsystem::getLastIterationNb)
	.def("getLastComputedScene",&Lsystem::getLastComputedScene)
    .def("isRunning",   &Lsystem::isRunning, "Tell whether self is performing an action")
    .def("forceRelease",&Lsystem::forceRelease, "Force release of running state in case of violent ending. Not recommended. Use with care.")
    .def("setDebugger",&py_set_debugger)
    .def("getDebugger",&Lsystem::getDebugger)
    .def("hasDebugger",&Lsystem::hasDebugger)
    .def("clearDebugger",&Lsystem::clearDebugger)
	.def("get_rule_fonction_table",&py_get_rule_fonction_table)
	;

}
