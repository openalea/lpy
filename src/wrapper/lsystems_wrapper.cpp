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

#include <iostream>
#include <boost/python.hpp>
#include "export_lsystem.h"
#include "moduleclass.h"
#include "lsyscontext.h"
#include "tracker.h"
#include <plantgl/gui/base/application.h>
#include <plantgl/python/exception_core.h>

using namespace boost::python;
LPY_USING_NAMESPACE


void cleanLsys() 
{
#ifdef TRACKER_ENABLED
	std::cerr << "****** pre-cleaning *******" << std::endl;
	Tracker::printReport();
#endif
	LsysContext::cleanContexts();
	ModuleClassTable::clearModuleClasses ();
	ViewerApplication::exit ();
#ifdef TRACKER_ENABLED
	std::cerr << "****** post-cleaning ******" << std::endl;
	Tracker::printReport();
#endif
}

BOOST_PYTHON_MODULE(__lpy_kernel__)
{
	define_stl_exceptions();
	export_Options();
	export_ModuleClass();
	export_Module();
	export_PatternModule();
    export_NodeModule();
	export_AxialTree();
	export_PatternString();
    export_Interpretation();
    export_Consider();
    export_LsysRule();
    export_LsysContext();
    export_Lsystem();
    export_plot();
	export_parser();
    export_StringMatching();
    export_Debugger();
	// def("cleanLsys",&cleanLsys);
	Py_AtExit(&cleanLsys);
};
