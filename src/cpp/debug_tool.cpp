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

#include "debug_tool.h"
#include <plantgl/tool/timer.h>
#include <plantgl/tool/util_hashmap.h>
#include <stdio.h>


TOOLS_USING_NAMESPACE

typedef pgl_hash_map<const char *, Timer> TimerMap; 
static TimerMap timers;

Timer& get_timer(const char * name) {
    TimerMap::iterator ittimer = timers.find(name);
    if (ittimer == timers.end()){
        timers[name] = Timer();
        return timers[name];
    }
    else return ittimer->second;
}

void LPY::start_timing(const char * name){
    get_timer(name).start();
}

void LPY::stop_timing(const char * name){
    get_timer(name).stop();
}

void LPY::print_timing(const char * name )
{
    if (name == 0) {
        for(TimerMap::const_iterator ittimer = timers.begin(); ittimer != timers.end(); ++ittimer)
            printf("%s : %f elapsed.\n", ittimer->first, ittimer->second.elapsedTime());
    }
    else {
        printf("%s : %f elapsed.\n", name, get_timer(name).elapsedTime());
    }

}

void LPY::reset_timing(){
    timers.clear();
}

typedef pgl_hash_map<const char *, int> ValueMap; 
static ValueMap debugvalues;

int& get_debugvalue(const char * name) {
    ValueMap::iterator itvalue = debugvalues.find(name);
    if (itvalue == debugvalues.end()){
        debugvalues[name] = 0;
        return debugvalues[name];
    }
    else return itvalue->second;
}


void LPY::set_debug_value(const char * name, int value)
{
    get_debugvalue(name) = value;
}

void LPY::inc_debug_value(const char * name)
{
    set_debug_value(name, get_debugvalue(name)+1);
}

void LPY::dec_debug_value(const char * name)
{
    set_debug_value(name, get_debugvalue(name)+1);
}

void LPY::print_debug_values(const char * name)
{

    if (name == 0) {
        for(ValueMap::const_iterator itvalue = debugvalues.begin(); itvalue != debugvalues.end(); ++itvalue)
            printf("%s : %i.\n", itvalue->first, itvalue->second);
    }
    else {
        printf("%s : %i.\n", name, get_debugvalue(name));
    }
}

void LPY::reset_debug_values()
{
    debugvalues.clear();
}
