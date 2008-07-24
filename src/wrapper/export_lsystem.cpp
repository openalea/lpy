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
using namespace boost::python;
#include <string>
PYLSYS_USING_NAMESPACE

const LsysRule& prodRule(Lsystem * lsys, int pos, int group)
{ 
  if(group < 0)group += lsys->nbGroups();
  if(group < 0 || group >= lsys->nbGroups()){
	PyErr_SetString(PyExc_IndexError, "group index out of range");
    throw_error_already_set();
  }
  if(pos < 0)pos += lsys->nbProductionRules(group);
  if(pos < 0 || pos >= lsys->nbProductionRules(group)){
	PyErr_SetString(PyExc_IndexError, "rule index out of range");
    throw_error_already_set();
  }
  return lsys->prodRule(pos,group);
}

const LsysRule& decRule(Lsystem * lsys, int pos, int group)
{ 
  if(group < 0)group += lsys->nbGroups();
  if(group < 0 || group >= lsys->nbGroups()){
	PyErr_SetString(PyExc_IndexError, "group index out of range");
    throw_error_already_set();
  }
  if(pos < 0)pos += lsys->nbDecompositionRules(group);
  if(pos < 0 || pos >= lsys->nbDecompositionRules(group)){
	PyErr_SetString(PyExc_IndexError, "rule index out of range");
    throw_error_already_set();
  }
  return lsys->decRule(pos,group);
}

const LsysRule& homRule(Lsystem * lsys, int pos, int group)
{ 
  if(group < 0)group += lsys->nbGroups();
  if(group < 0 || group >= lsys->nbGroups()){
	PyErr_SetString(PyExc_IndexError, "group index out of range");
    throw_error_already_set();
  }
  if(pos < 0)pos += lsys->nbHomomorphismRules(group);
  if(pos < 0 || pos >= lsys->nbHomomorphismRules(group)){
	PyErr_SetString(PyExc_IndexError, "rule index out of range");
    throw_error_already_set();
  }
  return lsys->homRule(pos,group);
}


AxialTree lsys_axiom(Lsystem * lsys) { return lsys->getAxiom(); }
void lsys_setCode1(Lsystem * lsys, const std::string& code) { return lsys->set(code); }

AxialTree py_iter(Lsystem * lsys, size_t beg, size_t end, const AxialTree& wstring)
{ return lsys->iterate(beg,end,wstring); }

void export_Lsystem(){

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
	.def("compiled",&Lsystem::compiled)
	.def("compile",&Lsystem::compile)
	.def("clear", &Lsystem::clear)
	.def("empty", &Lsystem::empty)
	.def("code", &Lsystem::code)
	.def("read", &Lsystem::read)
	.def("set", &lsys_setCode1)
	.def("set", &Lsystem::set)
	.def("plot", (void(Lsystem::*)(AxialTree&))&Lsystem::plot)
	.def("iterate", (AxialTree(Lsystem::*)())&Lsystem::iterate)
	.def("iterate", (AxialTree(Lsystem::*)(size_t))&Lsystem::iterate)
    .def("iterate", &py_iter)
	.def("iterate", (AxialTree(Lsystem::*)(size_t,size_t,const AxialTree&,bool))&Lsystem::iterate)
	.def("interpret", (void(Lsystem::*)(AxialTree&))&Lsystem::interpret)
	.def("interpret", (void(Lsystem::*)(AxialTree&, PGL::Turtle&))&Lsystem::interpret)
	.def("sceneInterpretation", &Lsystem::sceneInterpretation)
	.def("homomorphism", &Lsystem::homomorphism)
	.def("nbProductionRules", &Lsystem::nbProductionRules)
	.def("nbDecompositionRules", &Lsystem::nbDecompositionRules)
	.def("nbHomomorphismRules", &Lsystem::nbHomomorphismRules)
	.def("nbTotalRules", &Lsystem::nbTotalRules)
	.def("nbGroups", &Lsystem::nbGroups)
	.def("prodRule", prodRule, return_internal_reference<>())
	.def("decRule", decRule, return_internal_reference<>())
	.def("homRule", homRule, return_internal_reference<>())
	.def("animate", (AxialTree(Lsystem::*)())&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double,size_t))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(double,size_t,size_t))&Lsystem::animate)
	.def("animate", (AxialTree(Lsystem::*)(const AxialTree&,double,size_t,size_t))&Lsystem::animate)
	.def("record",  (void(Lsystem::*)(const std::string&))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,size_t))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,size_t,size_t))&Lsystem::record)
	.def("record",  (void(Lsystem::*)(const std::string&,size_t,size_t))&Lsystem::record)
	.def("addRule",      (void(Lsystem::*)(const std::string&, int, size_t))&Lsystem::addRule)
	.def("addRule",      (void(Lsystem::*)(const LsysRule&, int, size_t))&Lsystem::addRule)
	.def("addProdRule",  &Lsystem::addProdRule, "Add Production rule")
	.def("addDecRule",   &Lsystem::addDecRule, "Add Decomposition rule")
	.def("addHomRule",   &Lsystem::addHomRule, "Add Homorphism rule")
    // .def("enableEarlyReturn", &Lsystem::enableEarlyReturn, "Allow an early return (for threaded application).")
    // .def("isEarlyReturnEnabled", &Lsystem::isEarlyReturnEnabled, "Tell if an early return is required (for threaded application).")
	.add_property("early_return",&Lsystem::isEarlyReturnEnabled,&Lsystem::enableEarlyReturn)
	.def("getLastIterationNb",&Lsystem::getLastIterationNb)
    .def("isRunning",   &Lsystem::isRunning, "Tell whether self is performing an action")
	;

}
