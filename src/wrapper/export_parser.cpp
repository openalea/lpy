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

#include <boost/python.hpp>
#include <plantgl/python/export_list.h>

#include "lpy_parser.h"

using namespace boost::python;
LPY_USING_NAMESPACE

/* ----------------------------------------------------------------------- */
std::string getInitialisationBeginTag() { return LpyParsing::InitialisationBeginTag; }
std::string getVersionTag() { return LpyParsing::VersionTag; }

boost::python::object py_getformats() {
	return make_list<std::vector<float> >(LpyParsing::getSupportedFormat())();
}

bool isFormatDefined(const std::string& s) {  
	std::string::const_iterator it = s.begin(); 
	float format = LpyParsing::getFormatVersion(it,s.begin(),s.end()); 
	return it != s.begin(); 
}

void export_parser()
{
	def("lstring2py",(std::string(*)(const std::string&))&LpyParsing::lstring2py);

	class_<LpyParsing>("LpyParsing",no_init)
		.add_static_property("InitialisationBeginTag",&getInitialisationBeginTag)
		.add_static_property("VersionTag",&getVersionTag)
		.add_static_property("formats",&py_getformats)
		.add_static_property("DefaultFormat",make_getter(&LpyParsing::LPY_DEFAULT_FORMAT_VERSION))
		.def("isSupportedFormat",&LpyParsing::isSupportedFormat)
		.staticmethod("isSupportedFormat")
		.def("getFormatVersion",(float (*)(const std::string&)) &LpyParsing::getFormatVersion)
		.staticmethod("getFormatVersion")
		.def("isFormatDefined",&isFormatDefined)
		.staticmethod("isFormatDefined")
		.def("lstring2py",(std::string(*)(const std::string&))&LpyParsing::lstring2py)
		.staticmethod("lstring2py")
	;

}
