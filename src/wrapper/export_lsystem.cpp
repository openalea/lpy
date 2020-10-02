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

const LsysRule& py_productionRule(Lsystem * lsys, int pos = 0, int group = 0)
{ 
  check_group(group,lsys->nbGroups());
  check_ruleid(pos,lsys->nbProductionRules(group));
  return lsys->productionRule(pos,group);
}

const LsysRule& py_decompositionRule(Lsystem * lsys, int pos = 0, int group = 0)
{ 
  check_group(group,lsys->nbGroups());
  check_ruleid(pos,lsys->nbDecompositionRules(group));
  return lsys->decompositionRule(pos,group);
}

const LsysRule& py_interpretationRule(Lsystem * lsys, int pos, int group)
{ 
  check_group(group,lsys->nbGroups());
  check_ruleid(pos,lsys->nbInterpretationRules(group));
  return lsys->interpretationRule(pos,group);
}


AxialTree lsys_axiom(Lsystem * lsys) { return lsys->getAxiom(); }

object lsys_setCode(Lsystem * lsys, const std::string& code, 
									 const boost::python::dict& parameters = boost::python::dict(), 
									 bool debug = false) { 
	if (!debug) { lsys->set(code, NULL, parameters); return object(); }
	else {
		std::string pycode;
		lsys->set(code, &pycode, parameters); 
		return object(pycode);
	}
}


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
  enum_<Lsystem::eRuleType>("eRuleType")
	  .value("eProduction",Lsystem::eProduction)
	  .value("eDecomposition",Lsystem::eDecomposition)
  	  .value("eInterpretation",Lsystem::eInterpretation)
	  .export_values()
	  ;
  
  class_<Lsystem,boost::noncopyable>
	  ("Lsystem", init<optional<std::string,boost::python::dict> >("Lsystem([filename])", args("filename","globals")))
	.enable_pickling()
	.add_property("axiom",&lsys_axiom,(void(Lsystem::*)(const AxialTree&))&Lsystem::setAxiom)
	.add_property("derivationLength",&Lsystem::derivationLength,&Lsystem::setDerivationLength)
	.add_property("decompositionMaxDepth",&Lsystem::decompositionMaxDepth,&Lsystem::setDecompositionMaxDepth)
	.add_property("interpretationMaxDepth",&Lsystem::interpretationMaxDepth,&Lsystem::setInterpretationMaxDepth)
	.add_property("filename",&Lsystem::getFilename,&Lsystem::setFilename)
	.def("__str__", &Lsystem::str)
	//.def("__repr__", &Lsystem::str)
	.def("context", (LsysContext*(Lsystem::*)())&Lsystem::context,return_internal_reference<>(),"Return execution context of the L-system. See also execContext.")
	.def("execContext", (LsysContext*(Lsystem::*)())&Lsystem::context,return_internal_reference<>(),"Return the execution context of the L-system.")
	.def("makeCurrent",    &Lsystem::makeCurrent)
	.def("isCurrent",      &Lsystem::isCurrent)
	.def("done",           &Lsystem::done)
	.def("isCompiled",&Lsystem::isCompiled)
	.def("compile",&Lsystem::compile)
	.def("clear", &Lsystem::clear)
	.def("empty", &Lsystem::empty)
	.def("code", &Lsystem::code)
	.def("read", &Lsystem::read,"Read the content of a file and execute it",(boost::python::arg("filename"),boost::python::arg("parameters")=boost::python::dict()))
	.def("setCode", &lsys_setCode,"Set Lsystem code. If debug parameter is set to True, the translated Python code is returned.",(boost::python::arg("code"),boost::python::arg("parameters")=boost::python::dict(),boost::python::arg("debug")=false))
	.def("derive", (AxialTree(Lsystem::*)())&Lsystem::derive)
	.def("derive", (AxialTree(Lsystem::*)(size_t))&Lsystem::derive)
	.def("derive", (AxialTree(Lsystem::*)(const AxialTree&))&Lsystem::derive)
	.def("derive", (AxialTree(Lsystem::*)(const AxialTree&,size_t))&Lsystem::derive)
    .def("derive", (AxialTree(Lsystem::*)(const AxialTree&,size_t,size_t,bool))&Lsystem::derive,(bp::arg("workstring"),bp::arg("starting_iter"),bp::arg("nb_iter"),bp::arg("previouslyinterpreted")=false))
    .def("partial_derivation", &Lsystem::partial_derivation,(bp::arg("workstring"),bp::arg("starting_position"),bp::arg("length")))
	.def("turtle_interpretation", (void(Lsystem::*)(AxialTree&))&Lsystem::turtle_interpretation,"Apply interpretation with execContext().turtle.")
	.def("turtle_interpretation", (void(Lsystem::*)(AxialTree& , PGL::Turtle&))&Lsystem::turtle_interpretation,"Apply interpretation with given turtle.")
	.def("sceneInterpretation", &Lsystem::sceneInterpretation,"Apply interpretation with execContext().turtle and return resulting scene.")
	.def("stepInterpretation", &Lsystem::stepInterpretation,"Apply interpretation step by step and display construction of the scene.")
	.def("plot", (void(Lsystem::*)(AxialTree&,bool))&Lsystem::plot,(bp::arg("lstring"),bp::arg("checkLastComputedScene")=false),"Apply interpretation with execContext().turtle and plot the resulting scene. If checkLastComputedScene, check whether during last iteration a scene was computed. If yes reuse it.")
	.def("interpret", &Lsystem::interpret,"Apply interpretation rule and gives the resulting string.")
	.def("nbProductionRules", &Lsystem::nbProductionRules, (bp::arg("group")=0))
	.def("nbDecompositionRules", &Lsystem::nbDecompositionRules, (bp::arg("group")=0))
	.def("nbInterpretationRules", &Lsystem::nbInterpretationRules, (bp::arg("group")=0))
	.def("nbTotalRules", &Lsystem::nbTotalRules,"Return total number of rules considering all groups")
	.def("nbGroups", &Lsystem::nbGroups,"Return number of groups")
	.def("productionRule", py_productionRule, return_internal_reference<>(), (bp::arg("ruleid")=0,bp::arg("group")=0))
	.def("decompositionRule", py_decompositionRule, return_internal_reference<>(), (bp::arg("ruleid")=0,bp::arg("group")=0))
	.def("interpretationRule", py_interpretationRule, return_internal_reference<>(), (bp::arg("ruleid")=0,bp::arg("group")=0))

	.def("animate", (AxialTree(Lsystem::*)())&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double,size_t))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double,size_t,size_t))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(const AxialTree&,double,size_t,size_t))&Lsystem::animate)
	.def("record",  (void(Lsystem::*)(const std::string&,const std::string&))&Lsystem::record,(bp::arg("prefix"),bp::arg("suffix")="png"))
	.def("record",  (void(Lsystem::*)(const std::string&,size_t, const std::string&))&Lsystem::record,(bp::arg("prefix"),bp::arg("nbiteration"),bp::arg("suffix")="png"))
	.def("record",  (void(Lsystem::*)(const std::string&,size_t, size_t, const std::string&))&Lsystem::record,(bp::arg("prefix"),bp::arg("begin"),bp::arg("nbiteration"),bp::arg("suffix")="png"))
	.def("record",  (void(Lsystem::*)(const std::string&,const AxialTree&,size_t,size_t,const std::string&))&Lsystem::record,(bp::arg("prefix"),bp::arg("workstring"),bp::arg("begin"),bp::arg("nbiteration"),bp::arg("suffix")="png"))

	.def("addRule",      (void(Lsystem::*)(const std::string&, int, size_t, const ConsiderFilterPtr))&Lsystem::addRule, "Add a rule", 
						 (bp::arg("code"),bp::arg("ruletype")=Lsystem::eProduction,bp::arg("group")=0,bp::arg("filter")=ConsiderFilterPtr()))
	.def("addRule",      (void(Lsystem::*)(const std::string&, int, size_t))&Lsystem::addRule, "Add a rule", 
						 (bp::arg("code"),bp::arg("ruletype")=Lsystem::eProduction,bp::arg("group")=0))
	.def("addRule",      (void(Lsystem::*)(const LsysRule&, int, size_t))&Lsystem::addRule,"Add a rule", 
						 (bp::arg("rule"),bp::arg("ruletype")=Lsystem::eProduction,bp::arg("group")=0))
	.def("addProductionRule",  (void(Lsystem::*)(const std::string&, size_t, const ConsiderFilterPtr))&Lsystem::addProductionRule, "Add Production rule", 
						 (bp::arg("code"),bp::arg("group")=0,bp::arg("filter")=ConsiderFilterPtr()))
	.def("addProductionRule",  (void(Lsystem::*)(const std::string&, size_t))&Lsystem::addProductionRule, "Add Production rule", 
						 (bp::arg("code"),bp::arg("group")=0))

	.def("addDecompositionRule",   (void(Lsystem::*)(const std::string&, size_t, const ConsiderFilterPtr))&Lsystem::addDecompositionRule, "Add Decomposition rule", 
						 (bp::arg("code"),bp::arg("group")=0,bp::arg("filter")=ConsiderFilterPtr()))
	.def("addDecompositionRule",   (void(Lsystem::*)(const std::string&, size_t))&Lsystem::addDecompositionRule, "Add Decomposition rule", 
						 (bp::arg("code"),bp::arg("group")=0))

	.def("addInterpretationRule",   (void(Lsystem::*)(const std::string&, size_t, const ConsiderFilterPtr))&Lsystem::addInterpretationRule, "Add Interpretation rule", 
						 (bp::arg("code"),bp::arg("group")=0,bp::arg("filter")=ConsiderFilterPtr()))
	.def("addInterpretationRule",   (void(Lsystem::*)(const std::string&, size_t))&Lsystem::addInterpretationRule, "Add Interpretation rule", 
						 (bp::arg("code"),bp::arg("group")=0))
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
