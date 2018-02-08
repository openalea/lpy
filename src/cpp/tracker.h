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

#ifndef __LSYS_TRACKER_H__
#define __LSYS_TRACKER_H__

#include "lpy_config.h"
#include <stddef.h>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

// #define TRACKER_ENABLED

#ifdef TRACKER_ENABLED

#define TRACKER_CLASS_APPLY(MACRO) \
	MACRO(Module) \
    MACRO(AxialTree) \
    MACRO(ConsiderFilter) \
	MACRO(PatternString) \
	MACRO(LsysContext) \
	MACRO(LsysOptionValue) \
	MACRO(LsysOption) \
	MACRO(LsysOptions) \
	MACRO(LsysRule) \
	MACRO(Lsystem) \
	MACRO(ModuleProperty) \
	MACRO(ModuleVTable) \
	MACRO(ModuleClass) \
	MACRO(ModuleClassTable) \
	MACRO(StringInterpreter) \
	MACRO(StringMatching)

#define TRACKER_CLASS_DECLARE(mclass) static size_t mclass;

class LPY_API Tracker {
public:
	TRACKER_CLASS_APPLY(TRACKER_CLASS_DECLARE)
	static void printReport();
};

#define IncTracker(classname) ++Tracker::classname;
#define DecTracker(classname) --Tracker::classname;

#else

#define IncTracker(classname) 
#define DecTracker(classname)

#endif


LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif
