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

#ifndef __matching_h__
#define __matching_h__

#include "axialtree.h"
#include "axialtree_manip.h"
#include <boost/python.hpp>

LPY_BEGIN_NAMESPACE

class MatchingEngine;

class MatchingEngine {

public:

	static bool match(AxialTree::const_iterator  string_start,
			   AxialTree::const_iterator  string_end,
			   AxialTree::const_iterator  pattern_begin,
			   AxialTree::const_iterator  pattern_end,
			   AxialTree::const_iterator& matching_end,
			   boost::python::list& params) 
	{
		AxialTree::const_iterator it = string_start;
		boost::python::list lp;
		for (AxialTree::const_iterator it2 = pattern_begin; it2 != pattern_end; ++it2){
			if( it == string_end || !it->match(*it2,lp))return false;
			else { ++it; }
		}
		params += lp;
		matching_end = it;
		return true; 	
	}

	static bool reverse_match(AxialTree::const_iterator string_start,
					 AxialTree::const_iterator  string_begin,
					 AxialTree::const_reverse_iterator  pattern_rbegin,
					 AxialTree::const_reverse_iterator  pattern_rend,
					 AxialTree::const_iterator& matching_end,
					 boost::python::list& params)
	{ 
		// if(isEnd(string_start)) return false;
		AxialTree::const_iterator it = string_start;
		boost::python::list lp;
		for (AxialTree::const_reverse_iterator it2 = pattern_rbegin; it2 != pattern_rend; ){
			boost::python::list lmp;
			if(!it->match(*it2,lmp)) return false; 
			else { 
				++it2;
				if (it == string_begin){
					if (it2 != pattern_rend) return false;
				}
				else --it;
				lmp += lp; lp = lmp; 
			}
		}
		params += lp;
		matching_end = it;
		return true; 
	}


	static bool right_match(AxialTree::const_iterator  string_begin,
					 AxialTree::const_iterator  string_end,
					 AxialTree::const_iterator  pattern_begin,
					 AxialTree::const_iterator  pattern_end,
					 AxialTree::const_iterator& matching_end,
					 boost::python::list& params) {
	AxialTree::const_iterator it = string_begin;
	AxialTree::const_iterator it2 = pattern_begin;
	while(it != string_end && it2 != pattern_end){
	if(it->isIgnored()){ ++it; }
	if(it2->isStar()){
		boost::python::list lp;
		if(it->match(*it2,lp)){ params += lp; ++it; ++it2; }
		else return false;
	}
	else if(!it2->isBracket()){
	  if(!it->isBracket()) {
		boost::python::list lp;
		if(it->match(*it2,lp)){ params += lp; ++it; ++it2; }
		else return false;
	  }
	  else { // it->isBracket()
		if(it->isRightBracket())return false;
		else if(it->isLeftBracket()) {
		  it = endBracket(it,string_end);
		  if(it != string_end)++it;
		}
	  }
	}
	else { // it2->isBracket()
	  if(it2->isRightBracket()){
		if(it->isRightBracket()) { ++it; ++it2; }
		else { 
		  if(it2->isExactRightBracket())return false;
		  else {
            // search start before it to avoid matching A[B]C with A[B[]C]C
			it = endBracket(it,string_end,true);
			if(it!=string_end)++it; 
			++it2; 
		  }
		}
	  }
	  else { // it2->isLeftBracket()
		if(!it->isLeftBracket())return false;
		else { ++it; ++it2;	}
	  }
	}
  }
  if(it2 == pattern_end){
	matching_end = it;
	return true;
  }
  else return false;
}
	static bool left_match(AxialTree::const_iterator  string_begin,
			  	    AxialTree::const_iterator  string_end,
					AxialTree::const_iterator  pattern_begin,
					AxialTree::const_iterator  pattern_end,
					AxialTree::const_iterator& matching_end,
					boost::python::list& params) ;


};

LPY_END_NAMESPACE

#endif
