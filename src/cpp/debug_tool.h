/* ---------------------------------------------------------------------------
 #
 #       L-Py: L-systems in Python
 #
 #       Copyright UMR Cirad/Inria/Inra Agap - Virtual Plant Team
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

#ifndef __debug_tool_h__
#define __debug_tool_h__

#include "lpy_config.h"
#include <string>



LPY_BEGIN_NAMESPACE

void LPY_API start_timing(const char * name);
void LPY_API stop_timing(const char * name);

void LPY_API print_timing(const char * name = 0);
void LPY_API reset_timing();


class LPY_API ProfilingTimer {
public:
    ProfilingTimer(const char * _name) : name(_name) {
        start_timing(name.c_str());
    }
    ~ProfilingTimer()  {
        stop_timing(name.c_str());
    }
protected:
    std::string name;
};

#ifdef LPY_DEBUG
    #define PROFILINGTIMER(n) ProfilingTimer timer_##n(#n)
    #define STARTTIMING(n) start_timing(#n)
    #define STOPTIMING(n) stop_timing(#n)
#else
    #define PROFILINGTIMER(n)
    #define STARTTIMING(n) 
    #define STOPTIMING(n) 
#endif

void LPY_API set_debug_value(const char * name, int value);
void LPY_API inc_debug_value(const char * name);
void LPY_API dec_debug_value(const char * name);

void LPY_API print_debug_values(const char * name = 0);
void LPY_API reset_debug_values();

LPY_END_NAMESPACE

#endif
