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

#ifndef __arg_collector_h__
#define __arg_collector_h__

#include <plantgl/python/boost_python.h>

#ifndef bp
#define bp boost::python
#endif

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
// #define USE_PYTHON_LIST_COLLECTOR

typedef bp::list DefaultArgType; 
inline boost::python::list toPyList( bp::list obj ) { return obj; }

#ifndef USE_PYTHON_LIST_COLLECTOR
#define USE_OBJECTVEC_COLLECTOR
#endif

typedef std::vector<boost::python::object> StdArgListType;
inline size_t len( const StdArgListType& obj ) { return obj.size(); }
inline boost::python::object toPyList( const StdArgListType& obj ) {
	return boost::python::object(obj);
}

inline boost::python::dict toPyDict( const StdArgListType& obj, const std::vector<std::string> names ) {
	StdArgListType::const_iterator itobj = obj.begin();
	
	boost::python::dict result;
	for(std::vector<std::string>::const_iterator itname = names.begin(); itname != names.end() && itobj != obj.end(); ++itobj, ++itname)
		result[*itname] = *itobj;

	return result;
}



class LPY_API PyObjRef {
public:
	PyObjRef(): __toDelete(false), __object(NULL) {}

	PyObjRef(const boost::python::object& obj , bool owner = true):
		__toDelete(owner), __object(owner?NULL:&obj) 
		{ if(owner)copy_obj(obj); }

	PyObjRef(const PyObjRef& other): 
		__toDelete(other.__toDelete), __object(other.__object) 
		{ if(__toDelete) copy_obj(*other.__object); }

	PyObjRef& operator=(const PyObjRef& other) {
		if(__toDelete  && __object) delete __object;
		__toDelete = other.__toDelete;
		if(!__toDelete) __object = other.__object;
		else copy_obj(*other.__object);
		return *this;
	}

	~PyObjRef() { if(__toDelete  && __object) delete __object; }

	inline const boost::python::object& get() const { return *__object; }

private:

	inline void copy_obj(const boost::python::object& obj) {
			__object = new bp::object(bp::handle<>(bp::borrowed<>(obj.ptr()))); 
	}

	bool __toDelete;
    const boost::python::object * __object;
};

class LPY_API ArgRefList {
public:
	ArgRefList(size_t i = 0) : __data(i) { }

	inline size_t size() const { return __data.size(); }
	inline bool empty() const { return __data.empty(); }
	inline void reserve(size_t s) { __data.reserve(s); }
	inline const boost::python::object& operator[](size_t i) const { return __data[i].get(); }
	inline void push_back(const boost::python::object& obj){ __data.push_back(PyObjRef(obj,true)); }
	inline void push_back_ref(const boost::python::object& obj){ __data.push_back(PyObjRef(obj,false)); }
	inline void append(const ArgRefList& l) { __data.insert(__data.end(),l.__data.begin(),l.__data.end()); }
	inline void prepend(const ArgRefList& l) { __data.insert(__data.begin(),l.__data.begin(),l.__data.end()); }
	inline boost::python::list toPyList() const {
		boost::python::list res;
		for(std::vector<PyObjRef>::const_iterator it = __data.begin(); it != __data.end(); ++it)  res.append(it->get());
		return res;
	}

protected:
	std::vector<PyObjRef> __data;
};

inline boost::python::list toPyList( const ArgRefList& obj ) { return obj.toPyList(); }
inline size_t len( const ArgRefList& obj ) { return obj.size(); }

#ifdef USE_PYTHON_LIST_COLLECTOR
typedef bp::list ArgList;
#else
#ifdef USE_OBJECTVEC_COLLECTOR
typedef StdArgListType ArgList;
#else
typedef ArgRefList ArgList;
#endif
#endif


/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
