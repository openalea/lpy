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
#include <boost/python.hpp>

LPY_BEGIN_NAMESPACE

class MatchingEngine;

class MatchingEngine {

public:

	static bool match(AxialTree::const_iterator  string_begin,
			   AxialTree::const_iterator  string_end,
			   AxialTree::const_iterator  pattern_begin,
			   AxialTree::const_iterator  pattern_end,
			   AxialTree::const_iterator& matching_end,
			   boost::python::list& params) 
	{
		boost::python::list lp;
		for (AxialTree::const_iterator it2 = pattern_begin; it2 != pattern_end; ++it2){
			if(string_begin!=string_end || !string_begin->match(*it2,lp))return false;
			else { ++string_begin; }
		}
		params += lp;
		matching_end = string_begin;
		return true; 	
	}

	static bool reverse_match(AxialTree::const_iterator  string_begin,
					 AxialTree::const_iterator  string_end,
					 AxialTree::const_iterator  pattern_begin,
					 AxialTree::const_iterator  pattern_end,
					 AxialTree::const_iterator& matching_end,
					 boost::python::list& params) ;

	static bool right_match(AxialTree::const_iterator  string_begin,
					 AxialTree::const_iterator  string_end,
					 AxialTree::const_iterator  pattern_begin,
					 AxialTree::const_iterator  pattern_end,
					 AxialTree::const_iterator& matching_end,
					 boost::python::list& params) ;

	static bool left_match(AxialTree::const_iterator  string_begin,
			  	    AxialTree::const_iterator  string_end,
					AxialTree::const_iterator  pattern_begin,
					AxialTree::const_iterator  pattern_end,
					AxialTree::const_iterator& matching_end,
					boost::python::list& params) ;


};

LPY_END_NAMESPACE

#endif
