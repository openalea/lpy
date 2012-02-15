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

#include "stringmatching.h"
#include <boost/python.hpp>
using namespace boost::python;
LPY_USING_NAMESPACE


object values(StringMatching::const_iterator& it){
    return make_tuple(it.original(),it.target());
}

object nextValues(StringMatching::const_iterator& it){
    it.increment();
    return values(it);
}

void export_StringMatching(){


    scope s = class_<StringMatching>
	("StringMatching", init<>("StringMatching()"))
	// .def("__str__", &StringMatching::str)
	// .def("__repr__",&StringMatching::repr)
	.def("begin",  &StringMatching::begin)
	.def("end", &StringMatching::end)
	// .def("__len__", &Module::argSize)
	.def("append", &StringMatching::append)
	.def("addIdentity", &StringMatching::addIdentity)
	;

  class_<StringMatching::const_iterator>
	("const_iterator", no_init)
	 .def("increment", &StringMatching::const_iterator::increment)
	 .def("values", &values)
	 .def("original", &StringMatching::const_iterator::original, return_value_policy<return_by_value>())
	 .def("target", &StringMatching::const_iterator::target,return_value_policy<return_by_value>())
	 .def("next", &StringMatching::const_iterator::next,return_value_policy<return_by_value>())
	 .def("nextValues", &nextValues)
	;

}