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
#include "patternstring.h"
#include "nodemodule.h"
#include "axialtree_manip.h"
#include "axialtree_iter.h"
#include "export_lstring.h"
#include <plantgl/python/export_list.h>
using namespace boost::python;
LPY_USING_NAMESPACE
#define bp boost::python


AxialTree::const_iterator int_to_iter(AxialTree * tree, int pos)
{ 
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos <= tree->size())beg += pos;
  else if (pos == INT_MAX) return tree->end();
  else if(pos != 0){	
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return beg;
}

boost::python::object iter_to_int(AxialTree * tree, AxialTree::const_iterator pos)
{ 
  if (pos == tree->end()) return boost::python::object();
  else return boost::python::object(tree->pos(pos));
}

boost::python::object veciter_to_list(AxialTree * tree, std::vector<AxialTree::const_iterator> res)
{
  if (res.empty()) return object();
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

object py_find_mod2(AxialTree * tree, const PatternModule& mod, int start, int stop){
	return iter_to_int(tree,tree->find(mod,int_to_iter(tree,start),int_to_iter(tree,stop)));
}

object py_find_mod(AxialTree * tree, const PatternModule& mod, int start, int stop){
	return iter_to_int(tree,tree->find(mod,int_to_iter(tree,start),int_to_iter(tree,stop)));
}

object py_find_str(AxialTree * tree,const std::string& name, int start, int stop)
{ return py_find_mod(tree,PatternModule(name),start,stop); }


/*
object py_roots(AxialTree * tree)
{ return veciter_to_list(tree,tree->roots()); } 

object py_parent(AxialTree * tree, int pos)
{ return iter_to_int(tree,tree->parent(int_to_iter(tree,pos))); }

object py_children(AxialTree * tree, int pos)
{ return veciter_to_list(tree,tree->children(int_to_iter(tree,pos))); }

object py_lateral_children(AxialTree * tree, int pos)
{ return veciter_to_list(tree,tree->lateral_children(int_to_iter(tree,pos))); }

object py_direct_child(AxialTree * tree, int pos)
{ return iter_to_int(tree,tree->direct_child(int_to_iter(tree,pos))); }

object py_endBracket(AxialTree * tree, int pos, bool startingBeforePos) 
{ return iter_to_int(tree,tree->endBracket(int_to_iter(tree,pos),startingBeforePos)); }

object py_beginBracket(AxialTree * tree, int pos, bool startingAfterPos) 
{ return iter_to_int(tree,tree->beginBracket(int_to_iter(tree,pos),startingAfterPos)); }

object py_complex1(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,tree->complex(int_to_iter(tree,pos),scale)); }

object py_complex(AxialTree * tree, int pos)
{ return iter_to_int(tree,tree->complex(int_to_iter(tree,pos))); }

object py_successor_at_scale(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,successor_at_scale(int_to_iter(tree,pos),scale,tree->const_end())); }

object py_successor_at_level(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,successor_at_level(int_to_iter(tree,pos),scale,tree->const_end())); }

object py_predecessor_at_scale(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,predecessor_at_scale(int_to_iter(tree,pos),scale,tree->const_begin(),tree->const_end())); }

object py_predecessor_at_level(AxialTree * tree, int pos, int scale)
{ return iter_to_int(tree,predecessor_at_level(int_to_iter(tree,pos),scale,tree->const_begin(),tree->const_end())); }
*/

#define PY_MATCH_WRAPPER(matchfunc) \
  bool py_##matchfunc(AxialTree * tree, const PatternModule& a, int pos) \
  {  return tree->matchfunc(a,int_to_iter(tree,pos)); } \
  \
  bool py_##matchfunc(AxialTree * tree, const PatternString& a, int pos)  \
  {  return tree->matchfunc(a,int_to_iter(tree,pos)); } \
  \
  bool py_##matchfunc(AxialTree * tree, const std::string& a, int pos)  \
  {  return tree->matchfunc(PatternString(a),int_to_iter(tree,pos)); } \

/*  \
  bool py_##matchfunc(AxialTree * tree, const list& a, int pos)  \
  {  return tree->matchfunc(PatternString(a),int_to_iter(tree,pos)); } \
  bool py_##matchfunc(AxialTree * tree, const tuple& a, int pos)  \
  \
  {  return tree->matchfunc(PatternModule(a),int_to_iter(tree,pos)); }  */

#define PY_MATCH_WRAPPER_DEC(matchfunc) \
	def( #matchfunc,  (bool (*)(AxialTree*,const PatternString&,int))&py_##matchfunc ) \
    .def( #matchfunc,  (bool (*)(AxialTree*,const PatternModule&,int))&py_##matchfunc )  \
    .def( #matchfunc,  (bool (*)(AxialTree*,const std::string&,int))&py_##matchfunc )  \

/*    .def( #matchfunc,  (bool (*)(AxialTree*,const list&,int))&py_##matchfunc )  \
    .def( #matchfunc,  (bool (*)(AxialTree*,const tuple&,int))&py_##matchfunc )  */

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
  PatternModule first;
	extract<PatternModule> m(a);
	if(m.check()){
	  return replace2(tree,m(),b);
	}
	else {
	  PatternString first;
	  extract<std::string> st(a);
	  if(st.check())
		first = PatternString(st());
	  else {
		first = extract<PatternString>(a)();
	  }
	  return replace2(tree,first,b);
	}
  return AxialTree();
}

/*
boost::python::object py_varnames(AxialTree * tree)
{ return make_list(tree->getVarNames()); }
*/

struct axialtree_from_str {
  static void* convertible(PyObject* py_obj){
    if( !PyString_Check( py_obj ) ) return 0; 
    return py_obj; 
  } 
  static void construct( PyObject* obj, boost::python::converter::rvalue_from_python_stage1_data* data){ 
   typedef boost::python::converter::rvalue_from_python_storage<AxialTree> vector_storage_t;  
   vector_storage_t* the_storage = reinterpret_cast<vector_storage_t*>( data ); 
   void* memory_chunk = the_storage->storage.bytes; 
   // boost::python::str py_str( boost::python::handle<>( boost::python::borrowed( obj ) ) ); 
   new (memory_chunk) AxialTree(extract<std::string>(obj)); 
   data->convertible = memory_chunk; 
  } 
  axialtree_from_str() { 
	boost::python::converter::registry::push_back( &convertible, &construct, boost::python::type_id<AxialTree>()); 
  } 
}; 

PyAxialTreeIterator py_at_iter(AxialTree * tree) { return PyAxialTreeIterator(*tree); }
PyAxialTreeIterator py_ati_iter(PyAxialTreeIterator * it) { return *it; }

AxialTree py_add_ax_tuple(AxialTree * first, const boost::python::tuple& pattern) 
{  AxialTree res(*first); res.append(ParamModule(pattern));  return res; }

AxialTree& py_iadd_ax_tuple(AxialTree * first, const boost::python::tuple& pattern) 
{   first->append(ParamModule(pattern));  return *first; }

void py_insert_ax_tuple(AxialTree * first, int i, const boost::python::tuple& pattern) 
{   first->setItemAt(i,ParamModule(pattern));  }

NodeModule py_node(AxialTree * lstring, int i) 
{   return NodeModule(int_to_iter(lstring,i),lstring->const_begin(),lstring->const_end());  }

void export_AxialTree() {

  class_<AxialTree>
	("AxialTree", init<>("AxialTree()"))
	.def(init<const AxialTree &>("AxialTree(AxialTree)"))
	.def(init<const ParamModule &>("AxialTree(Module)"))
	.def(init<std::string>("AxialTree(string)"))
	.def(init<const list&>("AxialTree(list)"))
	.def(init<const tuple&>("AxialTree(tuple)"))
    .def(lstring_func<AxialTree>())
	.def("__str__", &AxialTree::str)
	.def("__repr__",&AxialTree::repr)
	.def("str_slice", (std::string(AxialTree::*)(int,int)const)&AxialTree::str_slice)
	.def("__iadd__", &py_iadd_ax_tuple, return_internal_reference<1>())
	.def("__add__", &py_add_ax_tuple)
	.def("insertAt",&py_insert_ax_tuple)

/*	.def("__iadd__", (AxialTree& (AxialTree::*)(const list&))&AxialTree::operator+=, return_internal_reference<1>())
	.def("__add__", (AxialTree (AxialTree::*)(const list&))&AxialTree::operator+)
	.def("insertAt",(void (AxialTree::*)(int,const list&))&AxialTree::insertAt)
 */
    .def( "count", (size_t(AxialTree::*)(const std::string& name)const)&AxialTree::count ) 
    .def( "count", (size_t(AxialTree::*)(const std::string& name, size_t nbparam)const)&AxialTree::count ) 
    .def( "count", (size_t(AxialTree::*)(const ParamModule&)const)&AxialTree::count ) 
    .def( "find", &py_find_mod, (bp::arg("pattern"),bp::arg("start")=0,bp::arg("end")=INT_MAX) ) 
    .def( "find", &py_find_str, (bp::arg("pattern"),bp::arg("start")=0,bp::arg("end")=INT_MAX)) 
    .def( "replace",&replace ) 
	.PY_MATCH_WRAPPER_DEC(match)
	.PY_MATCH_WRAPPER_DEC(reverse_match)
	.PY_MATCH_WRAPPER_DEC(leftmatch)
	.PY_MATCH_WRAPPER_DEC(rightmatch)
    .def( "__iter__", &py_at_iter )
    .def( "node", &py_node )
	;
    axialtree_from_str();

  class_<PyAxialTreeIterator >
	("AxialTreeIterator", init<AxialTree>("AxialTreeIterator(AxialTree)"))
	.def("next",&PyAxialTreeIterator::next,return_internal_reference<>(), (bp::arg("onlyConsidered")=false))
	.def("current",&PyAxialTreeIterator::currentValue,return_internal_reference<>())
	.def("__length_hint__",&PyAxialTreeIterator::size)
    .def( "__iter__", &py_ati_iter )
	.def( "toEndBracket", &PyAxialTreeIterator::toEndBracket, return_internal_reference<>(), (bp::arg("startingBeforePos")=false) )
	;

}