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

#include "patternstring.h"
#include "export_lstring.h"
#include <plantgl/python/export_list.h>

using namespace boost::python;
LPY_USING_NAMESPACE
#define bp boost::python


boost::python::object py_varnames(PatternString * tree)
{ return make_list(tree->getVarNames()); }



void export_PatternString() {

  class_<PatternString>
	("PatternString", init<>("PatternString()"))
	.def(init<const PatternString &>("PatternString(PatternString)"))
	.def(init<const PatternModule &>("PatternString(PatternModule)"))
	.def(init<std::string,optional<int> >("PatternString(str[,lineno])"))
	.def("__str__", &PatternString::str)
	.def("__repr__",&PatternString::repr)
    .def(lstring_func<PatternString>())
    .def( "varnames", &py_varnames )
    .def( "getVarNb", &PatternString::getVarNb )
	;



}