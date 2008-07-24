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
using namespace boost::python;
PYLSYS_USING_NAMESPACE

int findmod(AxialTree * tree,
		 const ParamModule& mod, 
		 int start, int stop){
  if(start < 0)start += tree->size();
  if(stop <= 0)stop  += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(start > 0 && start < tree->size())beg += start;
  else if(start != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator end = tree->end();
  if(stop > 0 && stop <= tree->size())end = tree->begin() + stop;
  else {
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->find(mod,beg,end);
  if(res == tree->end())return -1;
  else return tree->pos(res);
}
/*
int find(AxialTree * tree,
		 const std::string& name, 
		 size_t nbparam,
		 int start, int stop){
  if(start < 0)start += tree->size();
  if(stop <= 0)stop  += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(start > 0 && start < tree->size())beg += start;
  else if(start != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator end = tree->end();
  if(stop > 0 && stop <= tree->size())end = tree->begin() + stop;
  else {
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->find(name,nbparam,beg,end);
  if(res == tree->end())return -1;
  else return tree->pos(res);
}

int findletter(AxialTree * tree,
		 const std::string& name, 
		 int start, int stop){
  if(start < 0)start += tree->size();
  if(stop <= 0)stop  += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(start > 0 && start < tree->size())beg += start;
  else if(start != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator end = tree->end();
  if(stop > 0 && stop <= tree->size())end = tree->begin() + stop;
  else {
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->find(name,beg,end);
  if(res == tree->end())return -1;
  else return tree->pos(res);
}*/

int find(AxialTree * tree,const std::string& name, int start, int stop)
{ return findmod(tree,ParamModule::QueryModule(name),start,stop); }

int find(AxialTree * tree, const std::string& name, int start)
{ return findmod(tree,ParamModule::QueryModule(name),start,0); }

// int find(AxialTree * tree, const std::string& name, size_t nbparam)
// { return findmod(tree,name,nbparam,0,0); }

int find(AxialTree * tree, const std::string& name)
{ return findmod(tree,ParamModule::QueryModule(name),0,0); }

int find(AxialTree * tree, const ParamModule& mod, int start, int stop)
{ return findmod(tree,mod,start,stop); }

int find(AxialTree * tree, const ParamModule& mod, int start)
{ return findmod(tree,mod,start,0); }

int find(AxialTree * tree, const ParamModule& mod)
{ return findmod(tree,mod,0,0); }

object roots(AxialTree * tree)
{ 
  std::vector<AxialTree::const_iterator> res = tree->roots();
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

int father(AxialTree * tree, int pos)
{ 
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos <= tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->father(beg);
  if (res == tree->end()) return -1;
  else return tree->pos(res);
}

object sons(AxialTree * tree, int pos)
{ 
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  std::vector<AxialTree::const_iterator> res = tree->sons(beg);
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

object lateralSons(AxialTree * tree, int pos)
{ 
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  std::vector<AxialTree::const_iterator> res = tree->lateralSons(beg);
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

int directSon(AxialTree * tree, int pos)
{ 
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->directSon(beg);
  if (res == tree->end()) return -1;
  else return tree->pos(res);
}

int endBracket2(AxialTree * tree, int pos, bool startingBeforePos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->endBracket(beg,startingBeforePos);
  if(res == tree->end())return -1;
  else return tree->pos(res);
}

int endBracket(AxialTree * tree, int pos) {
  return endBracket2(tree,pos,false);
}

int beginBracket2(AxialTree * tree, int pos, bool startingAfterPos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  AxialTree::const_iterator res = tree->beginBracket(beg,startingAfterPos);
  if(res == tree->end())return -1;
  else return tree->pos(res);
}

int beginBracket(AxialTree * tree, int pos, bool startingAfterPos) {
    return beginBracket2(tree, pos, false);
}

bool match(AxialTree * tree, const ParamModule& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->match(a,beg);
}

bool match(AxialTree * tree, const tuple& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->match(ParamModule(a),beg);
}

bool match(AxialTree * tree, const AxialTree& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->match(a,beg);
}

bool match(AxialTree * tree, const std::string& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->match(AxialTree::QueryTree(a),beg);
}

bool match(AxialTree * tree, const list& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->match(AxialTree(a),beg);
}

bool reverse_match(AxialTree * tree, const ParamModule& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->reverse_match(a,beg);
}

bool reverse_match(AxialTree * tree, const tuple& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->reverse_match(ParamModule(a),beg);
}

bool reverse_match(AxialTree * tree, const AxialTree& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->reverse_match(a,beg);
}

bool reverse_match(AxialTree * tree, const std::string& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->reverse_match(AxialTree::QueryTree(a),beg);
}

bool reverse_match(AxialTree * tree, const list& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->reverse_match(AxialTree(a),beg);
}

bool leftmatch(AxialTree * tree, const ParamModule& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->leftmatch(AxialTree(a),beg);
}

bool leftmatch(AxialTree * tree, const tuple& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->leftmatch(AxialTree(ParamModule(a)),beg);
}

bool leftmatch(AxialTree * tree, const AxialTree& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->leftmatch(a,beg);
}

bool leftmatch(AxialTree * tree, const std::string& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->leftmatch(AxialTree::QueryTree(a),beg);
}

bool leftmatch(AxialTree * tree, const list& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->leftmatch(AxialTree(a),beg);
}

bool rightmatch(AxialTree * tree, const ParamModule& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->rightmatch(AxialTree(a),beg);
}

bool rightmatch(AxialTree * tree, const tuple& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->rightmatch(AxialTree(ParamModule(a)),beg);
}

bool rightmatch(AxialTree * tree, const AxialTree& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->rightmatch(a,beg);
}

bool rightmatch(AxialTree * tree, const std::string& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->rightmatch(AxialTree::QueryTree(a),beg);
}

bool rightmatch(AxialTree * tree, const list& a, int pos) {
  if(pos < 0)pos += tree->size();
  AxialTree::const_iterator beg = tree->begin();
  if(pos > 0 && pos < tree->size())beg += pos;
  else if(pos != 0){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return tree->rightmatch(AxialTree(a),beg);
}

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
    .def( "find", (int (*)(AxialTree*,const ParamModule&))&find ) 
    .def( "find", (int (*)(AxialTree*,const ParamModule&,int))&find ) 
    .def( "find", (int (*)(AxialTree*,const ParamModule&,int,int))&find ) 
    .def( "find", (int (*)(AxialTree*,const std::string&))&find ) 
    .def( "find", (int (*)(AxialTree*,const std::string&,int))&find ) 
    .def( "find", (int (*)(AxialTree*,const std::string&,int,int))&find ) 
    .def( "replace",&replace ) 
    .def( "match",  (bool (*)(AxialTree*,const AxialTree&,int))&match ) 
    .def( "match",  (bool (*)(AxialTree*,const ParamModule&,int))&match ) 
    .def( "match",  (bool (*)(AxialTree*,const list&,int))&match ) 
    .def( "match",  (bool (*)(AxialTree*,const tuple&,int))&match ) 
    .def( "match",  (bool (*)(AxialTree*,const std::string&,int))&match ) 
    .def( "reverse_match",  (bool (*)(AxialTree*,const AxialTree&,int))&reverse_match ) 
    .def( "reverse_match",  (bool (*)(AxialTree*,const ParamModule&,int))&reverse_match ) 
    .def( "reverse_match",  (bool (*)(AxialTree*,const list&,int))&reverse_match ) 
    .def( "reverse_match",  (bool (*)(AxialTree*,const tuple&,int))&reverse_match ) 
    .def( "reverse_match",  (bool (*)(AxialTree*,const std::string&,int))&reverse_match ) 
    .def( "leftmatch",  (bool (*)(AxialTree*,const AxialTree&,int))&leftmatch ) 
    .def( "leftmatch",  (bool (*)(AxialTree*,const ParamModule&,int))&leftmatch ) 
    .def( "leftmatch",  (bool (*)(AxialTree*,const list&,int))&leftmatch ) 
    .def( "leftmatch",  (bool (*)(AxialTree*,const tuple&,int))&leftmatch ) 
    .def( "leftmatch",  (bool (*)(AxialTree*,const std::string&,int))&leftmatch ) 
    .def( "rightmatch",  (bool (*)(AxialTree*,const AxialTree&,int))&rightmatch ) 
    .def( "rightmatch",  (bool (*)(AxialTree*,const ParamModule&,int))&rightmatch ) 
    .def( "rightmatch",  (bool (*)(AxialTree*,const list&,int))&rightmatch ) 
    .def( "rightmatch",  (bool (*)(AxialTree*,const tuple&,int))&rightmatch ) 
    .def( "rightmatch",  (bool (*)(AxialTree*,const std::string&,int))&rightmatch ) 
    .def( "roots", (object (*)(AxialTree*))&roots ) 
    .def( "father", (int (*)(AxialTree*,int))&father ) 
    .def( "sons", (object (*)(AxialTree*,int))&sons ) 
    .def( "lateralSons", (object (*)(AxialTree*,int))&lateralSons ) 
    .def( "directSon", (int (*)(AxialTree*,int))&directSon ) 
    .def( "endBracket", (int (*)(AxialTree*,int))&endBracket ) 
    .def( "endBracket", &endBracket2 ) 
    .def( "beginBracket", (int (*)(AxialTree*,int))&beginBracket ) 
    .def( "beginBracket", &beginBracket2 ) 
    .def( "wellBracketed", &AxialTree::wellBracketed )
    .def( "isAPath", &AxialTree::isAPath )
    .def( "hasQueryModule", &AxialTree::hasQueryModule )
	;
    axialtree_from_str();
    def("QueryTree", &AxialTree::QueryTree);

}