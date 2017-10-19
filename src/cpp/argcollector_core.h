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

#ifndef __arg_collector_core_h__
#define __arg_collector_core_h__

#include "argcollector.h"
#include "module.h"


LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

template<class ArgType>
struct ArgListCollector { };

template <>
struct ArgListCollector<bp::list> {
public:
	typedef bp::list element_type;
	static inline void append_args(element_type& value, const element_type& elements){
		value += elements;
	}

	static inline void append_as_arg(element_type& value, const element_type& elements){
		value.append(elements);
	}

	static inline void append_arg(element_type&  value, const bp::object& element){
		value.append(element);
	}

	static inline void append_arg_ref(element_type&  value, const bp::object& element){
		value.append(element);
	}

	static inline void append_n_arg(element_type&  value, size_t n, const bp::object& element){
		for (size_t i = 0; i < n; ++i)
			value.append(element);
	}

	static inline void prepend_args(element_type& value, element_type& elements){
		elements += value;
		value = elements;
	}

	static element_type fusion_args(const std::vector<element_type>& values){
		element_type res;
		if(!values.empty()){
			size_t nbvar = len(values[0]);
			for(size_t i = 0; i < nbvar; ++i){
				element_type resi;
				for(std::vector<element_type>::const_iterator it = values.begin(); it != values.end(); ++it)
					resi.append((*it)[i]);
				res.append(resi);
			}
		}
		return res;
	}
	static inline void append_modargs(element_type&  value, const ParamModule::ParameterList& elements){
	   for(ParamModule::ParameterList::const_iterator it = elements.begin(); it != elements.end(); ++it)
			value.append(*it);
	}
};

template <>
struct ArgListCollector<StdArgListType> {
public:
	typedef StdArgListType element_type;
	static inline void append_args(element_type& value, const element_type& elements){
		value.insert(value.end(),elements.begin(),elements.end());
	}

	static inline void append_as_arg(element_type& value, const element_type& elements){
		value.push_back(boost::python::object(elements));
	}

	static inline void append_arg(element_type&  value, const bp::object& element){
		value.push_back(element);
	}

	static inline void append_arg_ref(element_type&  value, const bp::object& element){
		value.push_back(element);
	}


	static inline void append_n_arg(element_type&  value, size_t n, const bp::object& element){
		value.insert(value.end(),n,element);
	}

	static inline void prepend_arg(element_type& value, const bp::object& element){
		value.insert(value.begin(),element);
	}

	static inline void prepend_args(element_type& value, const element_type& elements){
		value.insert(value.begin(),elements.begin(),elements.end());
	}

	static inline void prepend_n_arg(element_type&  value, size_t n, const bp::object& element){
		value.insert(value.begin(),n,element);
	}

	static element_type fusion_args(const std::vector<element_type>& values){
		element_type res;
		if(!values.empty()){
			size_t nbvar = values[0].size();
			for(size_t i = 0; i < nbvar; ++i){
				bp::list resi;
				for(std::vector<element_type>::const_iterator it = values.begin(); it != values.end(); ++it)
					resi.append((*it)[i]);
				res.push_back(boost::python::object(resi));
			}
		}
		return res;
	}
	static inline void append_modargs(element_type&  value, const ParamModule::ParameterList& elements){
		value.insert(value.end(),elements.begin(),elements.end());
	}

};


template <>
struct ArgListCollector<ArgRefList> {
public:
	typedef ArgRefList element_type;
	static inline void append_args(element_type& value, const element_type& elements){
		value.append(elements);
	}

	static inline void append_as_arg(element_type& value, const element_type& elements){
		value.push_back(elements.toPyList());
	}
	static inline void append_arg(element_type&  value, const bp::object& element){
		value.push_back(element);
	}

	static inline void append_arg_ref(element_type&  value, const bp::object& element){
		value.push_back_ref(element);
	}


	static inline void append_n_arg(element_type&  value, size_t n, const bp::object& element){
		for(size_t i = 0; i < n; ++i)
			value.push_back(element);
	}

	static inline void prepend_args(element_type& value, element_type& elements){
		value.prepend(elements);
	}

	static element_type fusion_args(const std::vector<element_type>& values){
		element_type res;
		if(!values.empty()){
			size_t nbvar = values[0].size();
			for(size_t i = 0; i < nbvar; ++i){
				bp::list resi;
				for(std::vector<element_type>::const_iterator it = values.begin(); it != values.end(); ++it)
					resi.append((*it)[i]);
				res.push_back(boost::python::object(resi));
			}
		}
		return res;
	}
	static inline void append_modargs(element_type&  value, const ParamModule::ParameterList& elements){
		for(ParamModule::ParameterList::const_iterator it = elements.begin(); it != elements.end(); ++it)
			value.push_back_ref(*it);
	}

};


typedef ArgListCollector<ArgList> ArgsCollector; 


/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
