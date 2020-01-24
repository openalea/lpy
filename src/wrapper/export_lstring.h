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

#include "abstractlstring.h"
#include <plantgl/python/boost_python.h>
#include <plantgl/python/export_list.h>

LPY_USING_NAMESPACE


template<class LString>
LString py_lstring_mult(const LString& lstring, int i) {
	LString res(lstring);
	res.mult(i);
	return res;
}

template<class LString>
LString& py_lstring_imult(LString* lstring, int i) {
	lstring->mult(i);
	return *lstring;
}

template<class LString>
size_t normedindex(LString * tree, boost::python::object pos, bool start = true)
{ 
  int ipos = 0;   
  size_t len = tree->size();
  if (pos == boost::python::object()) {
    ipos = (start?0:len);
  }
  else {
    ipos = boost::python::extract<int>(pos)();
  } 
  while (ipos < 0) ipos += len;
  while (ipos > len) ipos -= len;
  return (size_t)ipos;
}


template<class LString>
boost::python::object iter_to_int(LString * tree, typename LString::const_iterator pos)
{ 
  if (pos == tree->end()) return boost::python::object();
  else return boost::python::object(tree->pos(pos));
}

template<class LString>
boost::python::object veciter_to_list(LString * tree, std::vector<typename LString::const_iterator> res)
{
  typedef std::vector<typename LString::const_iterator> IterVector;
  typedef typename IterVector::const_iterator IterVectorIterator;
  if (res.empty()) return boost::python::object();
  else if (res.size() == 1) 
	return boost::python::object(tree->pos(res[0]));
  else {
	boost::python::list l;
	for(IterVectorIterator _it = res.begin(); _it != res.end(); _it++)
		  l.append(tree->pos(*_it));
	return l;
  }
}


template<class LString>
typename LString::const_iterator int_to_iter(LString * tree, int pos)
{ 
  size_t len= tree->size();
  if(pos < 0)pos += len;
  typename LString::const_iterator beg = tree->begin();
  if(pos > 0 && pos <= len)beg += pos;
  else if(pos != 0){
    PyErr_SetString(PyExc_IndexError, "index out of range");
    boost::python::throw_error_already_set();
  }
  return beg;
}


template<class LString>
boost::python::object py_roots(LString * tree, ConsiderFilter * filter = NULL)
{ return veciter_to_list(tree,tree->roots(filter)); }

template<class LString>
boost::python::object py_parent(LString * tree, int pos, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,tree->parent(int_to_iter(tree,pos), ConsiderFilterPtr(filter))); }

template<class LString>
boost::python::object py_children(LString * tree, int pos, ConsiderFilter * filter = NULL)
{ return veciter_to_list(tree,tree->children(int_to_iter(tree,pos),filter)); }

template<class LString>
boost::python::object py_lateral_children(LString * tree, int pos, ConsiderFilter * filter = NULL)
{ return veciter_to_list(tree,tree->lateral_children(int_to_iter(tree,pos),filter)); }

template<class LString>
boost::python::object py_direct_child(LString * tree, int pos, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,tree->direct_child(int_to_iter(tree,pos),filter)); }

template<class LString>
boost::python::object py_typed_children(LString * tree, int pos, char edge_type, ConsiderFilter * filter = NULL)
{ if (edge_type == '<') return py_direct_child<LString>(tree,pos,filter);
  else if (edge_type == '+') return py_lateral_children<LString>(tree,pos,filter);
  else {
    PyErr_SetString(PyExc_ValueError, "invalid edge type");
    boost::python::throw_error_already_set();
    return boost::python::object();
  }
}

template<class LString>
boost::python::object py_endBracket(LString * tree, int pos, bool startingBeforePos) 
{ return iter_to_int(tree,tree->endBracket(int_to_iter(tree,pos),startingBeforePos)); }

template<class LString>
boost::python::object py_beginBracket(LString * tree, int pos, bool startingAfterPos) 
{ return iter_to_int(tree,tree->beginBracket(int_to_iter(tree,pos),startingAfterPos)); }

template<class LString>
boost::python::object py_complex1(LString * tree, int pos, int scale, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,tree->complex(int_to_iter(tree,pos),scale,filter)); }

template<class LString>
boost::python::object py_complex(LString * tree, int pos, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,tree->complex(int_to_iter(tree,pos),filter)); }

template<class LString>
boost::python::object py_components(LString * tree, int pos, ConsiderFilter * filter = NULL)
{ return veciter_to_list(tree,tree->components(int_to_iter(tree,pos),filter) ); }

template<class LString>
boost::python::object py_components_at_scale(LString * tree, int pos, int scale, ConsiderFilter * filter = NULL)
{ return veciter_to_list(tree,tree->components_at_scale(int_to_iter(tree,pos),scale,filter)); }

template<class LString>
boost::python::object py_successor_at_scale(LString * tree, int pos, int scale, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,successor_at_scale(int_to_iter(tree,pos),scale,tree->const_end(),filter)); }

template<class LString>
boost::python::object py_successor_at_level(LString * tree, int pos, int scale, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,successor_at_level(int_to_iter(tree,pos),scale,tree->const_end(),filter)); }

template<class LString>
boost::python::object py_predecessor_at_scale(LString * tree, int pos, int scale, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,predecessor_at_scale(int_to_iter(tree,pos),scale,tree->const_begin(),tree->const_end(),filter)); }

template<class LString>
boost::python::object py_predecessor_at_level(LString * tree, int pos, int scale, ConsiderFilter * filter = NULL)
{ return iter_to_int(tree,predecessor_at_level(int_to_iter(tree,pos),scale,tree->const_begin(),tree->const_end(),filter)); }

template<class LString>
LString& py_iadd_lstring(LString * first, const LString& pattern) 
{ first->append(pattern);  return *first; }

template<class LString>
LString& py_iadd_lstring_mod(LString * first, const typename LString::ModuleType& pattern) 
{ first->append(pattern);  return *first; }

template<class LString>
LString py_add_lstring_mod(LString * first, const typename LString::ModuleType& pattern) 
{  LString res(*first); res.append(pattern);  return res; }

template<class LString>
LString py_add_lstring(LString * first, const LString& pattern) 
{  LString res(*first); res.append(pattern);  return res; }

template<class LString>
void py_append_lstring(LString * first, const std::string& pattern) { first->append(LString(pattern)); }

template<class LString>
void py_prepend_lstring(LString * first, const std::string& pattern) { first->prepend(LString(pattern)); }

template<class LString>
void py_insert_lstring(LString * first, int i, const std::string& pattern) { first->insertItemAt(i,LString(pattern)); }


template<class LString>
boost::python::object py_getslice(LString * tree, boost::python::slice sl) 
{ 
    size_t start = normedindex(tree,sl.start());
    size_t stop = normedindex(tree,sl.stop(), false);
    LString result = tree->template getRange<LString>(start, stop); 
    return boost::python::object(result);
}

template<class LString>
void py_delslice(LString * tree, boost::python::slice sl) 
{ 
    size_t start = normedindex(tree,sl.start());
    size_t stop = normedindex(tree,sl.stop(), false);
    tree->removeRange(start, stop); 
}

template<class LString>
class lstring_func : public boost::python::def_visitor<lstring_func<LString> >
{
    typedef LString LStringType;
    typedef typename LString::ModuleType Module;

    friend class boost::python::def_visitor_access;

    template <class classT>
    void visit(classT& c) const
	{
		c.def("argSize", &LString::size)
		 .def("empty",  &LString::empty)
		 .def("clear",  &LString::clear)
		 .def("__len__", &LString::size)
		 .def("__getitem__",&LString::getItemAt, boost::python::return_internal_reference<1>())
         .def("__getitem__",&py_getslice<LString>)
		 .def("__setitem__",&LString::setItemAt)
		 .def("__delitem__",&LString::removeItemAt)
		 .def("__delitem__",&py_delslice<LString>)
		 .def("append",(void(LString::*)(const Module&))&LString::append)
		 .def("append",(void(LString::*)(const LString&))&LString::append)
		 .def("append",&py_append_lstring<LString>)
		 .def("__iadd__",&py_iadd_lstring<LString>, boost::python::return_internal_reference<1>())
		 .def("__iadd__",&py_iadd_lstring_mod<LString>, boost::python::return_internal_reference<1>())
		 .def("__add__",&py_add_lstring<LString>)
		 .def("__add__",&py_add_lstring_mod<LString>)
		 .def("prepend",(void(LString::*)(const Module&))&LString::prepend)
		 .def("prepend",(void(LString::*)(const LString&))&LString::prepend)
		 .def("prepend",&py_prepend_lstring<LString>)
		 .def("insertAt",(void(LString::*)(int,const Module&))&LString::insertItemAt)
		 .def("insertAt",(void(LString::*)(int,const LString&))&LString::insertItemAt)
		 .def("insertAt",&py_insert_lstring<LString>)
		 .def("__mul__",&py_lstring_mult<LString>)
		 .def("__imul__",&py_lstring_imult<LString>, boost::python::return_internal_reference<1>())

		 .def(boost::python::self == boost::python::self)
		 .def(boost::python::self != boost::python::self)

		 .def( "wellBracketed", &LString::wellBracketed )
		 .def( "isAPath", &LString::isAPath )
		 .def( "hasRequestModule", &LString::hasRequestModule )

		 .def( "roots",  &py_roots<LString>, (boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "parent", &py_parent<LString>, (boost::python::arg("pos"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "children",   &py_children<LString>, (boost::python::arg("pos"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "children",   &py_typed_children<LString>, (boost::python::arg("pos"),boost::python::arg("edge_type"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 
		 .def( "lateral_children", &py_lateral_children<LString>, (boost::python::arg("pos"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "direct_child", &py_direct_child<LString>, (boost::python::arg("pos"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "endBracket", &py_endBracket<LString>, (bp::arg("startingBeforePos")=false) ) 
		 .def( "beginBracket", &py_beginBracket<LString>, (bp::arg("startingAfterPos")=false) ) 
         .def( "complex", &py_complex1<LString>, (boost::python::arg("pos"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
         .def( "complex", &py_complex<LString>, (boost::python::arg("pos"),boost::python::arg("scale"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
         .def( "components", &py_components<LString>, (boost::python::arg("pos"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
         .def( "components_at_scale", &py_components_at_scale<LString>, (boost::python::arg("pos"),boost::python::arg("scale"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "successor_at_scale", &py_successor_at_scale<LString>, (boost::python::arg("pos"),boost::python::arg("scale"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "successor_at_level", &py_successor_at_level<LString>, (boost::python::arg("pos"),boost::python::arg("level"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "predecessor_at_scale", &py_predecessor_at_scale<LString>, (boost::python::arg("pos"),boost::python::arg("scale"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 .def( "predecessor_at_level", &py_predecessor_at_level<LString>, (boost::python::arg("pos"),boost::python::arg("level"),boost::python::arg("filter") = ConsiderFilterPtr()) ) 
		 ;
    }
};



