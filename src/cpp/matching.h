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
#include "patternstring.h"
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

  static void setInheritanceModuleMatchingActivated(bool b);
  static bool isInheritanceModuleMatchingActivated();

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
  typedef bool (*ModuleMatchingFuncType)(const ParamModule&, const PatternModule&, ArgList&);
  static ModuleMatchingFuncType ModuleMatchingFunc;

  typedef bool (*RightMatchingFuncType)(AxialTree::const_iterator, AxialTree::const_iterator, AxialTree::const_iterator,
										PatternString::const_iterator, PatternString::const_iterator,
										AxialTree::const_iterator&, AxialTree::const_iterator&, ArgList&);

  static RightMatchingFuncType RightMatchingFunc;

  typedef bool (*LeftMatchingFuncType)(AxialTree::const_iterator, AxialTree::const_iterator,
									   AxialTree::const_iterator, PatternString::const_reverse_iterator,
									   PatternString::const_reverse_iterator, AxialTree::const_iterator&, ArgList&);

  static LeftMatchingFuncType LeftMatchingFunc;

public:
	static bool compatible_classes(const ModuleClassPtr& module, 
							 const ModuleClassPtr& pattern) ;

	static bool module_match(const ParamModule& module, 
							 const PatternModule& pattern,
							 ArgList& params) ;


	static bool match(AxialTree::const_iterator  matching_start,
			   AxialTree::const_iterator  string_begin,
			   AxialTree::const_iterator  string_end,
			   PatternString::const_iterator  pattern_begin,
			   PatternString::const_iterator  pattern_end,
			   AxialTree::const_iterator& matching_end,
			   AxialTree::const_iterator& last_matched,
			   ArgList& params) ;

	static bool reverse_match(AxialTree::const_iterator matching_start,
					 AxialTree::const_iterator  string_begin,
					 AxialTree::const_iterator  string_end,
					 PatternString::const_reverse_iterator  pattern_rbegin,
					 PatternString::const_reverse_iterator  pattern_rend,
					 AxialTree::const_iterator& matching_end,
					 ArgList& params);


	static bool right_match(AxialTree::const_iterator  matching_start,
   	    AxialTree::const_iterator  string_begin,
		AxialTree::const_iterator  string_end,
		PatternString::const_iterator  pattern_begin,
		PatternString::const_iterator  pattern_end,
		AxialTree::const_iterator&  last_matched,
		AxialTree::const_iterator& matching_end,
		ArgList& params);

	static bool left_match(AxialTree::const_iterator  matching_start,
			  	    AxialTree::const_iterator  string_begin,
			  	    AxialTree::const_iterator  string_end,
					PatternString::const_reverse_iterator  pattern_rbegin,
					PatternString::const_reverse_iterator  pattern_rend,
					AxialTree::const_iterator& matching_end,
					ArgList& params);
};

/*---------------------------------------------------------------------------*/

class LPY_API MatchingImplementation : public MatchingEngine {
public:
	static bool simple_module_matching(const ParamModule& module, 
									   const PatternModule& pattern, 
									   ArgList& l);

	static bool module_matching_with_star(const ParamModule& module, 
									      const PatternModule& pattern, 
									      ArgList& l);

	static bool module_matching_with_star_and_valueconstraints(const ParamModule& module, 
									      const PatternModule& pattern, 
									      ArgList& l);

	static bool string_exact_match(AxialTree::const_iterator  matching_start,
						     AxialTree::const_iterator  string_begin,
		                     AxialTree::const_iterator  string_end,
							 PatternString::const_iterator  pattern_begin,
							 PatternString::const_iterator  pattern_end,
							 AxialTree::const_iterator& matching_end,
						     AxialTree::const_iterator& last_matched,
							 ArgList& params) ;

	static bool string_exact_reverse_match(AxialTree::const_iterator matching_start,
		                             AxialTree::const_iterator  string_begin,
									 AxialTree::const_iterator  string_end,
					                 PatternString::const_reverse_iterator  pattern_rbegin,
									 PatternString::const_reverse_iterator  pattern_rend,
									 AxialTree::const_iterator& matching_end,
									 ArgList& params);

	static bool string_right_match(AxialTree::const_iterator  matching_start,
						         AxialTree::const_iterator  string_begin,
		                         AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
							     AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params);

	static bool string_left_match(AxialTree::const_iterator matching_start,
		                             AxialTree::const_iterator  string_begin,
									 AxialTree::const_iterator  string_end,
					                 PatternString::const_reverse_iterator  pattern_rbegin,
									 PatternString::const_reverse_iterator  pattern_rend,
									 AxialTree::const_iterator& matching_end,
									 ArgList& params);

	static bool tree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_begin,
		                         AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
							     AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params);

	static bool tree_left_match(AxialTree::const_iterator  matching_start,
		                        AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								PatternString::const_reverse_iterator  pattern_rbegin,
								PatternString::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params);

	static bool mstree_left_match(AxialTree::const_iterator  matching_start,
		                        AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								PatternString::const_reverse_iterator  pattern_rbegin,
								PatternString::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params);

	static bool mltree_left_match(AxialTree::const_iterator  matching_start,
		                        AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								PatternString::const_reverse_iterator  pattern_rbegin,
								PatternString::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params);

	static bool mstree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_begin,
		                         AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
								 AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params);

	static bool mltree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_begin,
		                         AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
								 AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params);
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
