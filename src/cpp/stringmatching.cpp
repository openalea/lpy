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
#include "tracker.h"
#include <iostream>

LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

StringMatching::const_iterator::const_iterator()
{
}

void StringMatching::const_iterator::increment(){
    ++__target;
    if (__iter == __end)++__original;
    else {
        if ((__iter+1) == __end){
            if (__iter->idpolicy == eIncreasing)++__original;
        }
        else {
            if(__target == (__iter+1)->target){
                while ( ((__iter+1) != __end) && (__target == (__iter+1)->target) ){
                    ++__iter;
                    __original = __iter->original;
                }
            }
            else {
                if (__iter->idpolicy == eIncreasing)++__original;
            }
        }
    }
}

bool StringMatching::const_iterator::isOnMark() const
{
    if (__iter == __end)return false;
    return (__target == __iter->target);
}

StringMatching::eIdPolicy 
StringMatching::const_iterator::currentIdPolicy() const
{
    if (__iter == __end)return (__iter-1)->idpolicy;
    return __iter->idpolicy;
}

/*---------------------------------------------------------------------------*/

StringMatching::StringMatching(){
    __matching.push_back(StringMark(0,0,eIncreasing));
	IncTracker(StringMatching)
}

StringMatching::~StringMatching()
{
	DecTracker(StringMatching)
}

void StringMatching::clear()
{
    __matching.clear();
    __matching.push_back(StringMark(0,0,eIncreasing));
}
/*---------------------------------------------------------------------------*/

StringMatching::const_iterator StringMatching::begin() const
{
    StringMatching::const_iterator it;
    it.__iter = __matching.begin();
    it.__end = __matching.end();
    if(it.__iter != it.__end){
        size_t firstval = it.__iter->target;
        while((it.__iter+1) != it.__end && (it.__iter+1)->target == firstval)++it.__iter;
    }
    it.__target = it.__iter->target;
    it.__original = it.__iter->original;
    return it;
}

StringMatching::const_iterator StringMatching::end() const
{
    StringMatching::const_iterator it;
    it.__iter = __matching.end();
    it.__end = __matching.end();
    it.__original = (it.__iter-1)->original;
    it.__target = (it.__iter-1)->target;
    return it;
}

/*---------------------------------------------------------------------------*/

void StringMatching::append(size_t original, size_t target)
{
    StringMarkList::iterator it = __matching.end()-1;
    if(target == 1 && original == 1){
        addIdentity(original);
    }
    else {
        it->idpolicy = eFixed;
        __matching.push_back(StringMark(it->original+original,it->target+target,eIncreasing));
    }
}

void StringMatching::addIdentity(size_t length)
{
    StringMarkList::iterator it = __matching.end()-1;
    if (it == __matching.begin()){
        it->idpolicy = eIncreasing;
        __matching.push_back(StringMark(it->original+length,it->target+length,eIncreasing));
    }
    else {
        StringMarkList::const_iterator itp = it-1;
        if( itp->idpolicy == eIncreasing){
            it->target+=length;
            it->original+=length;
        }
        else {
            it->idpolicy = eIncreasing;
            __matching.push_back(StringMark(it->original+length,it->target+length,eIncreasing));
        }
    }
}

bool StringMatching::isIdentity() const
{
    if (__matching.empty()) return true;
    else {
        bool identity = true;
        for(StringMarkList::const_iterator it = __matching.begin();it != __matching.end();++it)
            if( it->idpolicy == eFixed)return false;
        return true;
    }
}

StringMatching StringMatching::operator+(const StringMatching& newmatching)
{
   StringMatching matching;
   // StringMark::const_iterator itn = newmatching.begin();
   StringMarkList::const_iterator itn = newmatching.__matching.begin();
   // matching.push_back(__matching.begin());
   for(StringMarkList::const_iterator it = __matching.begin();it != __matching.end();++it){
       StringMark s;
       while (itn->original < it->target)++itn;
       --itn;
       if (itn->original == it->target){
           s.original = it->original;
           s.target = itn->target;
       }

       if( it->idpolicy == eFixed){
           
       }
       else {
       }
   }
   return matching;
}

/*---------------------------------------------------------------------------*/
