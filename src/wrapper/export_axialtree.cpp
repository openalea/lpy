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
#include "axialtree_manip.h"
#include "axialtree_iter.h"
#include <plantgl/python/export_list.h>
using namespace boost::python;
LPY_USING_NAMESPACE
#define bp boost::python


AxialTree::const_iterator int_to_iter(AxialTree * tree, int pos)
{ 
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos <= tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return beg;
}

int iter_to_int(AxialTree * tree, AxialTree::const_iterator pos)
{ 
  if (pos == tree->end()) return -1;
  else return tree->pos(pos);
}

boost::python::object veciter_to_list(AxialTree * tree, std::vector<AxialTree::const_iterator> res)
{
  if (res.empty()) return object(-1);
  else if (res.size() == 1) 
	return object(tree->pos(res[0]));
  else {
	list l;
	for(std::vector<AxialTree::const_iterator>::const_iterator _it = res.begin();
		_it != res.end(); _it++)
		  l.append(tree->pos(*_it));
	return l;
  }
}

int findmod(AxialTree * tree, const ParamModule& mod, int start, int stop){
  return iter_to_int(tree,tree->find(mod,int_to_iter(tree,start),int_to_iter(tree,stop)));
}

int find(AxialTree * tree,const std::string& name, int start, int stop)
{ return findmod(tree,ParamModule::QueryModule(name),start,stop); }

object py_roots(AxialTree * tree)
{ return veciter_to_list(tree,tree->roots()); }

int py_father(AxialTree * tree, int pos)
{ return iter_to_int(tree,tree->father(int_to_iter(tree,pos))); }

object py_sons(AxialTree * tree, int pos)
{ return veciter_to_list(tree,tree->sons(int_to_iter(tree,pos))); }

object py_lateralSons(AxialTree * tree, int pos)
{ return veciter_to_list(tree,tree->lateralSons(int_to_iter(tree,pos))); }

int py_directSon(AxialTree * tree, int pos)
{ return iter_to_int(tree,tree->directSon(int_to_iter(tree,pos))); }

int py_endBracket(AxialTree * tree, int pos, bool startingBeforePos) 
{ return iter_to_int(tree,tree->endBracket(int_to_iter(tree,pos),startingBeforePos)); }

int py_beginBracket(AxialTree * tree, int pos, bool startingAfterPos) 
{ return iter_to_int(tree,tree->beginBracket(int_to_iter(tree,pos),startingAfterPos)); }

int py_complex1(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,tree->complex(int_to_iter(tree,pos),scale)); }

int py_complex(AxialTree * tree, int pos)
{ return iter_to_int(tree,tree->complex(int_to_iter(tree,pos))); }

int py_successor_at_scale(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,successor_at_scale(int_to_iter(tree,pos),scale,tree->const_end())); }

int py_successor_at_level(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,successor_at_level(int_to_iter(tree,pos),scale,tree->const_end())); }

int py_predecessor_at_scale(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,predecessor_at_scale(int_to_iter(tree,pos),scale,tree->const_begin(),tree->const_end())); }

int py_predecessor_at_level(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,predecessor_at_level(int_to_iter(tree,pos),scale,tree->const_begin(),tree->const_end())); }

#define PY_MATCH_WRAPPER(matchfunc) \
  bool py_##matchfunc(AxialTree * tree, const ParamModule& a, int pos) \
  {  return tree->matchfunc(a,int_to_iter(tree,pos)); } \
  \
  bool py_##matchfunc(AxialTree * tree, const tuple& a, int pos)  \
  {  return tree->matchfunc(ParamModule(a),int_to_iter(tree,pos)); } \
  \
  bool py_##matchfunc(AxialTree * tree, const AxialTree& a, int pos)  \
  {  return tree->matchfunc(a,int_to_iter(tree,pos)); } \
  \
  bool py_##matchfunc(AxialTree * tree, const std::string& a, int pos)  \
  {  return tree->matchfunc(AxialTree::QueryTree(a),int_to_iter(tree,pos)); } \
  \
  bool py_##matchfunc(AxialTree * tree, const list& a, int pos)  \
  {  return tree->matchfunc(AxialTree(a),int_to_iter(tree,pos)); } \

#define PY_MATCH_WRAPPER_DEC(matchfunc) \
	def( #matchfunc,  (bool (*)(AxialTree*,const AxialTree&,int))&py_##matchfunc ) \
    .def( #matchfunc,  (bool (*)(AxialTree*,const ParamModule&,int))&py_##matchfunc )  \
    .def( #matchfunc,  (bool (*)(AxialTree*,const list&,int))&py_##matchfunc )  \
    .def( #matchfunc,  (bool (*)(AxialTree*,const tuple&,int))&py_##matchfunc )  \
    .def( #matchfunc,  (bool (*)(AxialTree*,const std::string&,int))&py_##matchfunc )  \

PY_MATCH_WRAPPER(match)
PY_MATCH_WRAPPER(reverse_match)
PY_MATCH_WRAPPER(leftmatch)
PY_MATCH_WRAPPER(rightmatch)

template <class T>
AxialTree
replace2(AxialTree * tree, const T& first, const object& b) {
  ParamModule second("*");
  extract<tuple> tu(b);
  if(tu.check()){
	  second = ParamModule(tu());
	  return tree->replace(first,second);
  }
  else{
	extract<ParamModule> m(b);
	if(m.check()){
	  second = m();
	  return tree->replace(first,second);
	}
	else {
	  AxialTree second;
	  extract<std::string> st(b);
	  if(st.check())
		second = AxialTree(st());
	  else {
		extract<AxialTree> m(b);
		if(m.check())
		  second = m();
		else
		  second = AxialTree(extract<list>(b)());
	  }
	  return tree->replace(first,second);
	}
  }
  return AxialTree();
}
AxialTree 
replace(AxialTree * tree, const object& a, const object& b) {
  ParamModule first("*");
  extract<tuple> tu(a);
  if(tu.check()){
	  return replace2(tree,ParamModule(tu()),b);
  }
  else{
	extract<ParamModule> m(a);
	if(m.check()){
	  return replace2(tree,m(),b);
	}
	else {
	  AxialTree first;
	  extract<std::string> st(a);
	  if(st.check())
		first = AxialTree::QueryTree(st());
	  else {
		extract<AxialTree> m(a);
		if(m.check())
		  first = m();
		else
		  first = AxialTree(extract<list>(a)());
	  }
	  return replace2(tree,first,b);
	}
  }
  return AxialTree();
}

boost::python::object py_varnames(AxialTree * tree)
{ return make_list(tree->getVarNames()); }

struct axialtree_from_str {
  static void* convertible(PyObject* py_obj){
    if( !PyString_Check( py_obj ) ) return 0; 
    return py_obj; 
  } 
  static void construct( PyObject* obj, boost::python::converter::rvalue_from_python_stage1_data* data){ 
   typedef boost::python::converter::rvalue_from_python_storage<AxialTree> vector_storage_t;  
   vector_storage_t* the_storage = reinterpret_cast<vector_storage_t*>( data ); 
   void* memory_chunk = the_storage->storage.bytes; 
   boost::python::str py_str( boost::python::handle<>( boost::python::borrowed( obj ) ) ); 
   new (memory_chunk) AxialTree(extract<std::string>(py_str)); 
   data->convertible = memory_chunk; 
  } 
  axialtree_from_str() { 
	boost::python::converter::registry::push_back( &convertible, &construct, boost::python::type_id<AxialTree>()); 
  } 
}; 

PyAxialTreeIterator py_at_iter(AxialTree * tree) { return PyAxialTreeIterator(*tree); }
PyAxialTreeIterator py_ati_iter(PyAxialTreeIterator * it) { return *it; }

void export_AxialTree() {

  class_<AxialTree>
	("AxialTree", init<>("AxialTree()"))
	.def(init<const AxialTree &>("AxialTree(AxialTree)"))
	.def(init<const ParamModule &>("AxialTree(Module)"))
	.def(init<std::string>("AxialTree(string)"))
	.def(init<const list&>("AxialTree(list)"))
	.def(init<const tuple&>("AxialTree(tuple)"))
    .def(self == self)
    .def(self != self)
//	.def("__iter__", iterator<AxialTree>())
	.def("__str__", &AxialTree::str)
	.def("__repr__",&AxialTree::repr)
	.def("__len__", &AxialTree::size)
	.def("__iadd__", (AxialTree& (AxialTree::*)(const std::string&))&AxialTree::operator+=, return_internal_reference<1>())
	.def("__iadd__", (AxialTree& (AxialTree::*)(const AxialTree&))&AxialTree::operator+=, return_internal_reference<1>())
	.def("__iadd__", (AxialTree& (AxialTree::*)(const ParamModule&))&AxialTree::operator+=, return_internal_reference<1>())
	.def("__iadd__", (AxialTree& (AxialTree::*)(const list&))&AxialTree::operator+=, return_internal_reference<1>())
	.def("__iadd__", (AxialTree& (AxialTree::*)(const tuple&))&AxialTree::operator+=, return_internal_reference<1>())
	.def("__add__", (AxialTree (AxialTree::*)(const std::string&))&AxialTree::operator+)
	.def("__add__", (AxialTree (AxialTree::*)(const AxialTree&))&AxialTree::operator+)
	.def("__add__", (AxialTree (AxialTree::*)(const ParamModule&))&AxialTree::operator+)
	.def("__add__", (AxialTree (AxialTree::*)(const list&))&AxialTree::operator+)
	.def("__add__", (AxialTree (AxialTree::*)(const tuple&))&AxialTree::operator+)
	.def("__imul__", &AxialTree::operator*=, return_internal_reference<1>())
	.def("__mul__", &AxialTree::operator*)
	.def("clear",   &AxialTree::clear)
	.def("empty",   &AxialTree::empty)
	.def("insertAt",(void (AxialTree::*)(int,const std::string&))&AxialTree::insertAt)
	.def("insertAt",(void (AxialTree::*)(int,const AxialTree&))&AxialTree::insertAt)
	.def("insertAt",(void (AxialTree::*)(int,const ParamModule&))&AxialTree::insertAt)
	.def("insertAt",(void (AxialTree::*)(int,const list&))&AxialTree::insertAt)
	.def("insertAt",(void (AxialTree::*)(int,const tuple&))&AxialTree::insertAt)
    .def( "__getitem__", &AxialTree::getAt , return_internal_reference<1>() )
    .def( "__getslice__",&AxialTree::getRange  )
	.def( "__setitem__", (void (AxialTree::*)(int,const std::string&))&AxialTree::setAt)
	.def( "__setitem__", (void (AxialTree::*)(int,const tuple&))&AxialTree::setAt)
	.def( "__setitem__", (void (AxialTree::*)(int,const ParamModule&))&AxialTree::setAt)
    .def( "__delitem__", &AxialTree::remove ) 
    .def( "__delslice__",&AxialTree::removeRange ) 
    .def( "count", (size_t(AxialTree::*)(const std::string& name)const)&AxialTree::count ) 
    .def( "count", (size_t(AxialTree::*)(const std::string& name, size_t nbparam)const)&AxialTree::count ) 
    .def( "count", (size_t(AxialTree::*)(const ParamModule&)const)&AxialTree::count ) 
    .def( "find", (int (*)(AxialTree*,const ParamModule&,int,int))&find, (bp::arg("pattern"),bp::arg("start")=0,bp::arg("end")=-1) ) 
    .def( "find", (int (*)(AxialTree*,const std::string&,int,int))&find, (bp::arg("pattern"),bp::arg("start")=0,bp::arg("end")=-1)) 
    .def( "replace",&replace ) 
	.PY_MATCH_WRAPPER_DEC(match)
	.PY_MATCH_WRAPPER_DEC(reverse_match)
	.PY_MATCH_WRAPPER_DEC(leftmatch)
	.PY_MATCH_WRAPPER_DEC(rightmatch)
    .def( "roots", (object (*)(AxialTree*))&py_roots ) 
    .def( "father", (int (*)(AxialTree*,int))&py_father, args("pos") ) 
    .def( "sons", (object (*)(AxialTree*,int))&py_sons, args("pos") ) 
    .def( "lateralSons", (object (*)(AxialTree*,int))&py_lateralSons, args("pos") ) 
    .def( "directSon", (int (*)(AxialTree*,int))&py_directSon, args("pos") ) 
    .def( "endBracket", (int (*)(AxialTree*,int))&py_endBracket, (bp::arg("startingBeforePos")=false) ) 
	.def( "beginBracket", (int (*)(AxialTree*,int))&py_beginBracket, (bp::arg("startingAfterPos")=false) ) 
    .def( "wellBracketed", &AxialTree::wellBracketed )
    .def( "isAPath", &AxialTree::isAPath )
    .def( "complex", &py_complex1, args("pos") ) 
    .def( "complex", &py_complex, args("pos","scale") ) 
    .def( "successor_at_scale", &py_successor_at_scale, args("pos","scale") ) 
    .def( "successor_at_level", &py_successor_at_level, args("pos","level") ) 
    .def( "predecessor_at_scale", &py_predecessor_at_scale, args("pos","scale") ) 
    .def( "predecessor_at_level", &py_predecessor_at_level, args("pos","level") ) 
    .def( "hasQueryModule", &AxialTree::hasQueryModule )
    .def( "varnames", &py_varnames )
    .def( "__iter__", &py_at_iter )
	;
    axialtree_from_str();
    def("QueryTree", &AxialTree::QueryTree);

  class_<PyAxialTreeIterator >
	("AxialTreeIterator", init<AxialTree>("AxialTreeIterator(AxialTree)"))
	.def("next",&PyAxialTreeIterator::next,return_internal_reference<>(), (bp::arg("onlyConsidered")=false))
	.def("current",&PyAxialTreeIterator::currentValue,return_internal_reference<>())
	.def("__length_hint__",&PyAxialTreeIterator::size)
    .def( "__iter__", &py_ati_iter )
	.def( "toEndBracket", &PyAxialTreeIterator::toEndBracket, return_internal_reference<>(), (bp::arg("startingBeforePos")=false) )
	;

}