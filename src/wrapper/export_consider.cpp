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

#include "consider.h"
#include <plantgl/python/export_refcountptr.h>

using namespace boost::python;
#define bp boost::python
LPY_USING_NAMESPACE


void export_Consider(){

    enum_<eConsiderMethod>("eConsiderMethod")
	  .value("eConsider",eConsider)
	  .value("eIgnore",eIgnore)
	  .export_values()
	  ;

    class_<ConsiderFilter,ConsiderFilterPtr,boost::noncopyable>
	("ConsiderFilter", "Filter on the module to consider in the context of the rules", 
		init<std::string,optional<eConsiderMethod> >("Create a filter of module",args("modules","method") ))

	// .def("clear",          &ConsiderFilter::clear)
	// .def("empty",          &ConsiderFilter::empty)
	.def("isConsidered",   
	(bool (ConsiderFilter::*)(const std::string& module) const)
						   &ConsiderFilter::isConsidered)
	.def("isIgnored",      
	(bool (ConsiderFilter::*)(const std::string& module) const)
						   &ConsiderFilter::isIgnored)
	.def("isConsidered",   
	(bool (ConsiderFilter::*)(const Module& module) const)
						   &ConsiderFilter::isConsidered)
	.def("isIgnored",      
	(bool (ConsiderFilter::*)(const Module& module) const)
						   &ConsiderFilter::isIgnored)
	.def("isConsidered",   
	(bool (ConsiderFilter::*)(const ModuleClassPtr module) const)
						   &ConsiderFilter::isConsidered)
	.def("isIgnored",      
	(bool (ConsiderFilter::*)(const ModuleClassPtr module) const)
						   &ConsiderFilter::isIgnored)

	.def("ignoring",       &ConsiderFilter::ignoring)
	.def("keyword",        &ConsiderFilter::keyword)

	.def("str",            &ConsiderFilter::str)

	.def("makeCurrent",    &ConsiderFilter::makeCurrent)
	.def("done",		   &ConsiderFilter::done)
	.def("isCurrent",	   &ConsiderFilter::isCurrent)

	.def("makeNoneCurrent",    &ConsiderFilter::makeNoneCurrent)
	.staticmethod("makeNoneCurrent")
	.def("doneNone",		   &ConsiderFilter::doneNone)
	.staticmethod("doneNone")
	.def("isNoneCurrent",	   &ConsiderFilter::isNoneCurrent)
	.staticmethod("isNoneCurrent")

	.def("current",	   &ConsiderFilter::current)
	.staticmethod("current")

	.def("consider",       &ConsiderFilter::consider)
	.staticmethod("consider")

	.def("ignore",         &ConsiderFilter::ignore)
	.staticmethod("ignore")


	;



}
