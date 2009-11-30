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

#include "lsyscontext.h"
#include "compilation.h"
#include <boost/python/make_constructor.hpp>
#include <plantgl/python/export_list.h>
#include <boost/python/raw_function.hpp>

using namespace boost::python;
LPY_USING_NAMESPACE
PGL_USING_NAMESPACE

PglTurtle * lsc_turtle(LsysContext * l){
  return &(l->turtle);
}

void py_backward() { LsysContext::currentContext()->backward(); }
void py_forward() { LsysContext::currentContext()->forward(); }
bool py_isForward() { return LsysContext::currentContext()->isForward(); }

std::string getInitialisationFunctionName() { return LsysContext::InitialisationFunctionName; }

LsysContext * create_a_context() { return new LocalContext(); }

boost::python::object py_LcDeclaredModules(LsysContext * c) {
	return make_list<ModuleClassList>(c->declaredModules())();
}

boost::python::object py_LcInitFrom(LsysContext * c, const std::string& code) {
	size_t pos = c->initialiseFrom(code);
	if (pos == std::string::npos) return object();
	else return object(pos);
}

boost::python::object py_lsys_getitem(LsysContext * lc, std::string name) {
	if (lc->hasObject(name)) return lc->getObject(name);
	else {
		PyErr_SetString(PyExc_KeyError, name.c_str());
		boost::python::throw_error_already_set();
	}
}

boost::python::object py_pproduce(bp::tuple args, bp::dict kw) {
	pproduce(args); return object();
}

void export_LsysContext(){

    class_<LsysContext,boost::noncopyable>
	("LsysContext", no_init ) // <>("LsysContext()"))
	.def( "__init__", make_constructor( &create_a_context ), "LsysContext()" ) 
	.add_property("turtle",make_getter(&LsysContext::turtle,return_value_policy<reference_existing_object>()))
	.add_static_property("InitialisationFunctionName",&getInitialisationFunctionName)
	.add_property("animation_timestep",&LsysContext::get_animation_timestep,&LsysContext::set_animation_timestep)
	.add_property("options",make_getter(&LsysContext::options,return_value_policy<reference_existing_object>()))
    .def("is_animation_timestep_to_default",&LsysContext::is_animation_timestep_to_default)
	.def("__str__",        &LsysContext::str)
	.def("__repr__",        &LsysContext::str)
	.def("clear",          &LsysContext::clear)
	.def("empty",          &LsysContext::empty)
	.def("consider",       &LsysContext::consider)
	.def("ignore",         &LsysContext::ignore)
	.def("isConsidered",   
	(bool (LsysContext::*)(const std::string& module) const)
						   &LsysContext::isConsidered)
	.def("isIgnored",      
	(bool (LsysContext::*)(const std::string& module) const)
						   &LsysContext::isIgnored)
	.def("isConsidered",   
	(bool (LsysContext::*)(const Module& module) const)
						   &LsysContext::isConsidered)
	.def("isIgnored",      
	(bool (LsysContext::*)(const Module& module) const)
						   &LsysContext::isIgnored)
	.def("ignoring",       &LsysContext::ignoring)
	.def("keyword",        &LsysContext::keyword)
	.def("declare",        (void(LsysContext::*)(const std::string&))&LsysContext::declare)
	.def("undeclare",      (void(LsysContext::*)(const std::string&))&LsysContext::undeclare)
	.def("isDeclared",     (bool(LsysContext::*)(const std::string&))&LsysContext::isDeclared)
	.def("declaredModules", &py_LcDeclaredModules)
	.def("setModuleScale",&LsysContext::setModuleScale)
	.def("getNamespace",   
	(void (LsysContext::*)(dict&) const)&LsysContext::getNamespace)
	.def("updateNamespace",   
	(void (LsysContext::*)(dict&) )&LsysContext::updateNamespace)
	.def("clearNamespace", (void (LsysContext::*)() )&LsysContext::clearNamespace)
	.def("start",          &LsysContext::start)
	.def("end",            (void(LsysContext::*)())&LsysContext::end)
	.def("startEach",      &LsysContext::startEach)
	.def("endEach",        (void(LsysContext::*)())&LsysContext::endEach)
	.def("initialise",     &LsysContext::initialise)
	.def("initialiseFrom", &py_LcInitFrom)
	.def("setStart",       &LsysContext::setStart)
	.def("setEnd",         &LsysContext::setEnd)
	.def("setStartEach",   &LsysContext::setEndEach)
	.def("evaluate",       &LsysContext::evaluate)
	.def("compile",        (void(LsysContext::*)(const std::string&))&LsysContext::compile)
	.def("compile",        (bp::object(LsysContext::*)(const std::string&,const std::string&))&LsysContext::compile)
	.def("__contains__",   &LsysContext::hasObject)
	.def("has_key",        &LsysContext::hasObject)
	.def("__getitem__",    &py_lsys_getitem)
	.def("get",            &LsysContext::getObject)
	.def("__setitem__",    &LsysContext::setObject)
	.def("__delitem__",    &LsysContext::delObject)
	.def("makeCurrent",    &LsysContext::makeCurrent)
	.def("isCurrent",      &LsysContext::isCurrent)
	.def("done",           &LsysContext::done)
	.def("currentContext", &LsysContext::currentContext,return_value_policy<reference_existing_object>())
	.staticmethod("currentContext")
	.def("current", &LsysContext::current,return_value_policy<reference_existing_object>())
	.staticmethod("current")
	.def("defaultContext", &LsysContext::defaultContext,return_value_policy<reference_existing_object>())
	.staticmethod("defaultContext")
	.def("default", &LsysContext::defaultContext,return_value_policy<reference_existing_object>())
	.staticmethod("default")
	.def("globalContext", &LsysContext::globalContext,return_value_policy<reference_existing_object>())
	.staticmethod("globalContext")
	.def("global", &LsysContext::global,return_value_policy<reference_existing_object>())
	.staticmethod("global")
	.def("backward",      &LsysContext::backward)
	.def("forward",      &LsysContext::forward)
	.def("isForward",      &LsysContext::isForward)
	.def("stop",      &LsysContext::stop)
	.def("nproduce",  (void (LsysContext::*)(const AxialTree&) )&LsysContext::nproduce)
    .def("nproduce",  (void (LsysContext::*)(const boost::python::list&) )&LsysContext::nproduce)
	.def("useGroup",  &LsysContext::useGroup)
    .def("getGroup",  &LsysContext::getGroup)
	.def("frameDisplay",  &LsysContext::frameDisplay,(bp::arg("enabled")=true))
    .def("isFrameDisplayed",  &LsysContext::isFrameDisplayed)
    .def("isSelectionRequired", &LsysContext::isSelectionRequired)
    .def("setSelectionRequired", &LsysContext::setSelectionRequired)
    .def("getIterationNb", &LsysContext::getIterationNb)
    .def("isAnimationEnabled",  &LsysContext::isAnimationEnabled)
	;

	def("consider",      &consider);
	def("ignore",      &ignore);
	def("context", &LsysContext::currentContext,return_value_policy<reference_existing_object>());
	def("backward",      &py_backward);
	def("forward",      &py_forward);
	def("isForward",      &py_isForward);
	def("nproduce",  (void (*)(const AxialTree&) )&nproduce);
    def("nproduce",  (void (*)(const boost::python::list&) )&nproduce);
    def("nproduce",  (void (*)(const std::string&) )&nproduce);
	def("pproduce",  bp::raw_function(py_pproduce,1));
	def("useGroup",  &useGroup);
    def("getGroup",  &getGroup);
	def("frameDisplay",  &frameDisplay,(bp::arg("enabled")=true));
    def("isFrameDisplayed",  &isFrameDisplayed);
    def("isSelectionRequired", &isSelectionRequired);
    def("setSelectionRequired", &setSelectionRequired);
    def("getIterationNb", &getIterationNb);
    def("isAnimationEnabled",  &isAnimationEnabled);
	def("declare", &declare);
	def("undeclare", &undeclare);
	def("isDeclared", &isDeclared);
	def("Stop", &Stop);

	def("__setCythonAvailable",&Compilation::setCythonAvailable);
	def("__setPythonExec",&Compilation::setPythonExec);

}
