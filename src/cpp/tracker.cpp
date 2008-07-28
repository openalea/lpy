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

#include "tracker.h"
#include <iostream>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

#ifdef TRACKER_ENABLED

size_t Tracker::Module(0);
size_t Tracker::AxialTree(0);
size_t Tracker::LsysContext(0);
size_t Tracker::LsysOptionValue(0);
size_t Tracker::LsysOption(0);
size_t Tracker::LsysOptions(0);
size_t Tracker::LsysRule(0);
size_t Tracker::Lsystem(0);
size_t Tracker::ModuleClass(0);
size_t Tracker::ModuleClassTable(0);
size_t Tracker::StringInterpreter(0);
size_t Tracker::StringMatching(0);

void Tracker::printReport(){
	std::cerr << "Module count : " << Tracker::Module << std::endl;
	std::cerr << "AxialTree count : " << Tracker::AxialTree << std::endl;
	std::cerr << "LsysContext count : " << Tracker::LsysContext << std::endl;
	std::cerr << "LsysOptionValue count : " << Tracker::LsysOptionValue << std::endl;
	std::cerr << "LsysOption count : " << Tracker::LsysOption << std::endl;
	std::cerr << "LsysOptions count : " << Tracker::LsysOptions << std::endl;
	std::cerr << "LsysRule count : " << Tracker::LsysRule << std::endl;
	std::cerr << "Lsystem count : " << Tracker::Lsystem << std::endl;
	std::cerr << "ModuleClass count : " << Tracker::ModuleClass << std::endl;
	std::cerr << "ModuleClassTable count : " << Tracker::ModuleClassTable << std::endl;
	std::cerr << "StringInterpreter count : " << Tracker::StringInterpreter << std::endl;
	std::cerr << "StringMatching count : " << Tracker::StringMatching << std::endl;
}

#endif

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
