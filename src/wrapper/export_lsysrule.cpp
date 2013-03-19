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

#include "lsysrule.h"
#include "lsyscontext.h"
using namespace boost::python;
#include <string>
# include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <plantgl/python/export_list.h>

LPY_USING_NAMESPACE

AxialTree::const_iterator getPos(const AxialTree& tree, int pos){
  if(pos < 0)pos += tree.size();
  AxialTree::const_iterator beg = tree.begin();
  if(pos > 0 && pos < tree.size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return beg;
}


object match(LsysRule * rule,const AxialTree& tree, int pos, const AxialTree& dest) {
  AxialTree::const_iterator beg = getPos(tree,pos);
  AxialTree::const_iterator endpos;
  ArgList args;
  if(!rule->match(tree,beg,dest,endpos,args))return object(false);
  return make_tuple(tree.pos(endpos),args);
}

object match2(LsysRule * rule,const AxialTree& tree, int pos ) {
  AxialTree::const_iterator beg = getPos(tree,pos);
  AxialTree::const_iterator endpos;
  AxialTree dest;
  ArgList args;
  if(!rule->match(tree,beg,dest,endpos,args))return object(false);
  return make_tuple(tree.pos(endpos),args);
}

object match1(LsysRule * rule,const AxialTree& tree) {
    return match2(rule,tree,0);
}

object reverse_match(LsysRule * rule,const AxialTree& tree, int pos, AxialTree& dest) {
  AxialTree::const_iterator beg = getPos(tree,pos);
  AxialTree::const_iterator endpos;
  ArgList args;
  if(!rule->reverse_match(tree,beg,dest,endpos,args))return object(false);
  return make_tuple(tree.pos(endpos),args);
}

boost::python::object reverse_match2(LsysRule * rule,const AxialTree& tree, int pos) {
  AxialTree::const_iterator beg = getPos(tree,pos);
  AxialTree::const_iterator endpos;
  AxialTree dest;
  ArgList args;
  if(!rule->reverse_match(tree,beg,dest,endpos,args))return object(false);
  return make_tuple(tree.pos(endpos),args);
}

boost::python::object reverse_match1(LsysRule * rule,const AxialTree& tree) {
    return reverse_match2(rule,tree,-1);
}

bool applyTo(LsysRule * rule,AxialTree& tree,  ArgList args) {
	return rule->applyTo(tree,args);
}

object rule_call(LsysRule * rule) {
  bool success;
  AxialTree res = rule->apply(&success);
  if(!success)return object();
  else return object(res);
}

object rule_call_with_args(LsysRule * rule, boost::python::list pyargs) {
  bool success;
  ArgList args;
  object iter_obj = object( handle<>( PyObject_GetIter( pyargs.ptr() ) ) );
  while( true )
  {
	 object obj;
     try {  obj = iter_obj.attr( "next" )(); }
     catch( error_already_set ){  PyErr_Clear(); break; }
	 args.push_back(obj);
  }
  AxialTree res = rule->apply(args,&success);
  if(!success)return object();
  else return object(res);
}


void Lr_set(LsysRule * rule, const std::string& code) {
  rule->set(code);
  rule->compile();
}

std::string py_rpr_ag(const ArgList * obj) {
  boost::python::list res(*obj);
  boost::python::object repr = GlobalContext::getFunctionRepr();
  return extract<std::string>(repr(res));
}

std::string py_str_ag(const ArgList * obj) {
  boost::python::list res(*obj);
  return extract<std::string>(str(res));
}


object Lr_param(LsysRule * rule) {  return make_list(rule->formalParameters()); }

void export_LsysRule(){

#ifdef USE_OBJECTVEC_COLLECTOR
  class_<ArgList>
	  ("ArgList", init<optional<size_t,bp::object> >("ArgList(size,obj)"))
        .def(vector_indexing_suite<ArgList,true>())
		.def("__repr__",&py_rpr_ag)
		.def("__str__",&py_str_ag)
		;
#endif


  class_<LsysRule>
	("LsysRule", init<optional<size_t,size_t,char> >("LsysRule(id,group,prefix)"))
	.def("__str__", &LsysRule::str)
	//.def("__repr__", &LsysRule::str)
	.def("__call__", &rule_call)
	.def("__call__", &rule_call_with_args)
	.add_property("id",&LsysRule::getId,&LsysRule::setId)
	.add_property("groupId",&LsysRule::getGroupId,&LsysRule::setGroupId)
	.add_property("lineno",make_getter(&LsysRule::lineno))
	.add_property("codelength",&LsysRule::getCodeLength)
	.add_property("static",&LsysRule::isStatic)
	.add_property("__static_production__",&LsysRule::getStaticProduction)
	.def("predecessor",&LsysRule::predecessor, boost::python::return_internal_reference<1>())
	.def("leftContext", &LsysRule::leftContext, boost::python::return_internal_reference<1>())
	.def("newLeftContext", &LsysRule::newLeftContext, boost::python::return_internal_reference<1>())
	.def("rightContext", &LsysRule::rightContext, boost::python::return_internal_reference<1>())
	.def("newRightContext", &LsysRule::newRightContext, boost::python::return_internal_reference<1>())
	.def("function",   &LsysRule::function, boost::python::return_internal_reference<1>())
	.def("definition", &LsysRule::definition, boost::python::return_internal_reference<1>())
	.def("isCompiled",&LsysRule::isCompiled)
	.def("compile",(void(LsysRule::*)())&LsysRule::compile)
	.def("recompile",(void(LsysRule::*)())&LsysRule::recompile)
	.def("compile",(void(LsysRule::*)(dict&))&LsysRule::compile)
	.def("clear", &LsysRule::clear)
	.def("nbParameters", &LsysRule::nbParameters)
	.def("parameterNames", &Lr_param)
	.def("nbContexts", &LsysRule::nbContexts)
	.def("isContextFree", &LsysRule::isContextFree)
	.def("hasQuery", &LsysRule::hasQuery)
	.def("functionName", &LsysRule::functionName)
	.def("name", &LsysRule::name)
	.def("code", &LsysRule::getCode)
	.def("set", &Lr_set)
	.def("match", &match)
	.def("match", &match2)
	.def("match", &match1)
	.def("reverse_match", &reverse_match)
	.def("reverse_match", &reverse_match2)
	.def("reverse_match", &reverse_match1)
	.def("process", &LsysRule::process)
	.def("applyTo", &applyTo)
	.def("forwardCompatible", &LsysRule::forwardCompatible)
	.def("backwardCompatible", &LsysRule::backwardCompatible)
	.def("consider", (void(LsysRule::*)(const std::string&))&LsysRule::consider)
	.def("consider", (void(LsysRule::*)(const ConsiderFilterPtr))&LsysRule::consider)
	.def("ignore", &LsysRule::ignore)
	.add_property("considerFilter",&LsysRule::getConsiderFilter,(void(LsysRule::*)(const ConsiderFilterPtr))&LsysRule::consider)	;
}

