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

/*---------------------------------------------------------------------------*/

class LPY_API MatchingEngine {

public:
	  enum eModuleMatchingMethod {
			eMSimple = 0,
			eMWithStar,
			eMWithStarNValueConstraint,
			eDefaultModuleMatching = eMWithStarNValueConstraint
	  };

  static void setModuleMatchingMethod(eModuleMatchingMethod);
  static eModuleMatchingMethod getModuleMatchingMethod();

  enum eStringMatchingMethod {
			eString = 0,
			eAxialTree,
			eMLevelAxialTree,
			eMScaleAxialTree,
			eDefaultStringMatching = eAxialTree
  };

  static void setStringMatchingMethod(eStringMatchingMethod);
  static eStringMatchingMethod getStringMatchingMethod();

private:
  static eModuleMatchingMethod ModuleMatchingMethod;
  static eStringMatchingMethod StringMatchingMethod;

public:
	static bool module_match(const ParamModule& module, 
							 const ParamModule& pattern,
							 boost::python::list& params) ;


	static bool match(AxialTree::const_iterator  matching_start,
			   AxialTree::const_iterator  string_end,
			   AxialTree::const_iterator  pattern_begin,
			   AxialTree::const_iterator  pattern_end,
			   AxialTree::const_iterator& matching_end,
			   AxialTree::const_iterator& last_matched,
			   boost::python::list& params) ;

	static bool reverse_match(AxialTree::const_iterator matching_start,
					 AxialTree::const_iterator  string_begin,
					 AxialTree::const_iterator  string_end,
					 AxialTree::const_reverse_iterator  pattern_rbegin,
					 AxialTree::const_reverse_iterator  pattern_rend,
					 AxialTree::const_iterator& matching_end,
					 boost::python::list& params);


	static bool right_match(AxialTree::const_iterator  matching_start,
		AxialTree::const_iterator  string_end,
		AxialTree::const_iterator  pattern_begin,
		AxialTree::const_iterator  pattern_end,
		AxialTree::const_iterator  last_matched,
		AxialTree::const_iterator& matching_end,
		boost::python::list& params);

	static bool left_match(AxialTree::const_iterator  matching_start,
			  	    AxialTree::const_iterator  string_begin,
			  	    AxialTree::const_iterator  string_end,
					AxialTree::const_reverse_iterator  pattern_rbegin,
					AxialTree::const_reverse_iterator  pattern_rend,
					AxialTree::const_iterator& matching_end,
					boost::python::list& params);
};

/*---------------------------------------------------------------------------*/

class LPY_API MatchingImplementation : public MatchingEngine {
public:
	static bool simple_module_matching(const ParamModule& module, 
									   const ParamModule& pattern, 
									   boost::python::list& l);

	static bool module_matching_with_star(const ParamModule& module, 
									      const ParamModule& pattern, 
									      boost::python::list& l);

	static bool module_matching_with_star_and_valueconstraints(const ParamModule& module, 
									      const ParamModule& pattern, 
									      boost::python::list& l);

	static bool string_exact_match(AxialTree::const_iterator  matching_start,
		                     AxialTree::const_iterator  string_end,
							 AxialTree::const_iterator  pattern_begin,
							 AxialTree::const_iterator  pattern_end,
							 AxialTree::const_iterator& matching_end,
						     AxialTree::const_iterator& last_matched,
							 boost::python::list& params) ;

	static bool string_exact_reverse_match(AxialTree::const_iterator matching_start,
		                             AxialTree::const_iterator  string_begin,
									 AxialTree::const_iterator  string_end,
					                 AxialTree::const_reverse_iterator  pattern_rbegin,
									 AxialTree::const_reverse_iterator  pattern_rend,
									 AxialTree::const_iterator& matching_end,
									 boost::python::list& params);

	static bool tree_right_match(AxialTree::const_iterator  matching_start,
		                         AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
							     AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 boost::python::list& params);

	static bool tree_left_match(AxialTree::const_iterator  matching_start,
		                        AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								boost::python::list& params);

	static bool mstree_left_match(AxialTree::const_iterator  matching_start,
		                        AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								boost::python::list& params);

	static bool mltree_left_match(AxialTree::const_iterator  matching_start,
		                        AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								boost::python::list& params);

	static bool mstree_right_match(AxialTree::const_iterator  matching_start,
		                         AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
								 AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 boost::python::list& params);

	static bool mltree_right_match(AxialTree::const_iterator  matching_start,
		                         AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
								 AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 boost::python::list& params);
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
