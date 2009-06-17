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

#include "matching.h"
#include "axialtree_manip.h"
#include "matching_tmpl.h"
#include <boost/python.hpp>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
MatchingEngine::eModuleMatchingMethod 
MatchingEngine::ModuleMatchingMethod = MatchingEngine::eDefaultModuleMatching;

void MatchingEngine::setModuleMatchingMethod(MatchingEngine::eModuleMatchingMethod method)
{ ModuleMatchingMethod = method; }

MatchingEngine::eModuleMatchingMethod LPY::MatchingEngine::getModuleMatchingMethod()
{ return ModuleMatchingMethod; }

MatchingEngine::eStringMatchingMethod 
MatchingEngine::StringMatchingMethod = MatchingEngine::eDefaultStringMatching;

void MatchingEngine::setStringMatchingMethod(MatchingEngine::eStringMatchingMethod method)
{ 
	StringMatchingMethod = method; 
}

MatchingEngine::eStringMatchingMethod LPY::MatchingEngine::getStringMatchingMethod()
{ return StringMatchingMethod; }

/*---------------------------------------------------------------------------*/


bool MatchingEngine::module_match(const ParamModule& module, 
								  const ParamModule& pattern,
								  ArgList& params) 
{
  switch(ModuleMatchingMethod){
   case eMSimple:
	return MatchingImplementation::simple_module_matching(module,pattern,params);
   case eMWithStar:
	return MatchingImplementation::module_matching_with_star(module,pattern,params);
   default:
	return MatchingImplementation::module_matching_with_star_and_valueconstraints(module,pattern,params);
  }
}


bool MatchingEngine::match(AxialTree::const_iterator  matching_start,
						   AxialTree::const_iterator  string_end,
						   AxialTree::const_iterator  pattern_begin,
						   AxialTree::const_iterator  pattern_end,
						   AxialTree::const_iterator& matching_end,
						   AxialTree::const_iterator& last_matched,
						   ArgList& params) 
{
	return MatchingImplementation::string_exact_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              matching_end,last_matched, params);
}

bool MatchingEngine::reverse_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   AxialTree::const_reverse_iterator  pattern_rbegin,
								   AxialTree::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	return MatchingImplementation::string_exact_reverse_match(matching_start,string_begin,string_end,
						                               pattern_rbegin,pattern_rend,
						                               matching_end,params);
}

bool MatchingEngine::right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
								 AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	switch(StringMatchingMethod){
		case eString:
				return StringMatcher<GetNext>::
				match(matching_start, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
		case eMScaleAxialTree:
			return MatchingImplementation::mstree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              last_matched, matching_end,params);
		case eMLevelAxialTree:
			return MatchingImplementation::mltree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              last_matched, matching_end,params);
		case eAxialTree:
		default:
			return MatchingImplementation::tree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              last_matched, matching_end,params);
	}
}

bool MatchingEngine::left_match(AxialTree::const_iterator  matching_start,
								AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params) 
{
	switch(StringMatchingMethod){
		case eString:
				return StringReverseMatcher<GetPrevious>::
				match(matching_start,string_begin,string_end,pattern_rbegin,pattern_rend,matching_end,params);
		case eMScaleAxialTree:
			return MatchingImplementation::mstree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
		case eMLevelAxialTree:
			return MatchingImplementation::mltree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
		case eAxialTree:
		default:
			return MatchingImplementation::tree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
	}
}

/*---------------------------------------------------------------------------*/

bool 
MatchingImplementation::simple_module_matching(const ParamModule& module, 
													 const ParamModule& pattern, 
													 ArgList& l)
{
  if( module.sameName(pattern) && module.argSize() == pattern.argSize()){
	  ArgsCollector::append_modargs(l,module.getParameterList());
	return true;
  }
  return false;
}

bool MatchingImplementation::module_matching_with_star(const ParamModule& module, 
													   const ParamModule& pattern, 
									                   ArgList& l)
{
  if (pattern.isStar()){
	size_t s = pattern.argSize();
	if (s == 0) return true;
	if (s == 1) {
	  size_t s2 = module.argSize();
	  boost::python::object of = pattern.getAt(0);
	  LsysVar v = boost::python::extract<LsysVar>(of);
	  if(v.isArgs()) { 
		  ArgsCollector::append_arg(l,boost::python::object(module.name()));
		  ArgsCollector::append_modargs(l,module.getParameterList());
		  return true; 
	  }
	  else {
		if( s2 == 0){ ArgsCollector::append_arg(l,boost::python::object(module.name()));return true; }
	    else return false;
	  }
	}
	else {
	  size_t s2 = module.argSize();
	  if (s2 < s-2) return false;
	  boost::python::object of = pattern.getAt(s-1);
	  LsysVar v = boost::python::extract<LsysVar>(of)();
	  if(!v.isArgs()){
  		if (s2 != s-1) return false;
		ArgsCollector::append_arg(l,boost::python::object(module.name()));
		ArgsCollector::append_modargs(l,module.getParameterList());
		return true;
	  }
	  ArgsCollector::append_arg(l,boost::python::object(module.name()));
	  if(s != 2){
		for(int i = 0; i < s-2; i++)
		  ArgsCollector::append_arg(l,module.getAt(i));
		if(s2 > s-2)ArgsCollector::append_arg(l,module.getslice(s-2,s2));
		else ArgsCollector::append_arg(l,boost::python::list());
	  }
	  else ArgsCollector::append_arg(l,module.getArgs());
	  return true;
	}
  }
  else if(module.isStar())return module_matching_with_star(pattern,module,l);
  else {
	if (!module.sameName(pattern)) return false;
	size_t s = pattern.argSize();
    size_t s2 = module.argSize();
	if (s2 < s-1) return false;
	if( s == 0) return s2 == 0;
	else{
	  boost::python::object of = pattern.getAt(s-1);
	  LsysVar v = boost::python::extract<LsysVar>(of)();
	  if(!v.isArgs()) {
		if(s2 == s) { ArgsCollector::append_modargs(l,module.getParameterList()); return true; }
		else return false;
	  }
	  if(s!=1){
		for(int i = 0; i < s-1; i++)
		  ArgsCollector::append_arg(l,module.getAt(i));
		if(s2 > s-1)ArgsCollector::append_arg(l,module.getslice(s-1,s2));
		else ArgsCollector::append_arg(l,boost::python::list());
	  }
	  else ArgsCollector::append_modargs(l,module.getParameterList());
	  return true;
	}
	return true;
  }
}

bool MatchingImplementation::module_matching_with_star_and_valueconstraints(
										  const ParamModule& module, 
									      const ParamModule& pattern, 
									      ArgList& l)
{ 
  if (pattern.isStar()){
	size_t s = pattern.argSize();
	if (s == 0) return true;
	if (s == 1) {
	  size_t s2 = module.argSize();
	  boost::python::object of = pattern.getAt(0);
	  boost::python::extract<LsysVar> e(of);
	  if(!e.check()){
		  if (s2 != 1) return false;
		  if (of != module.getAt(0))return false;
		  return true;
	  }
	  else {
		LsysVar v = e();
		if(v.isArgs()) { 
			ArgList largs;
			ArgsCollector::append_arg(largs,bp::object(module.name()));
			ArgsCollector::append_modargs(largs,module.getParameterList()); 
			if (!v.isCompatible(bp::object(largs)))return false;
			ArgsCollector::append_arg(l,boost::python::object(largs)); 
			return true; 
		}
		else {
		  if(s2 == 0){ 
			 boost::python::object largs(module.name()); 
			 if (!v.isCompatible(largs))return false;
			 ArgsCollector::append_arg(l,largs); 
			 return true; 
		  }
		  else return false;
		}
	  }
	}
	else {
	  size_t s2 = module.argSize();
	  int beg = 0;
	  boost::python::object o1 = pattern.getAt(0);
	  boost::python::extract<LsysVar> e1(o1);
	  if(!e1.check()){
		  if (s2 < s-1) return false;
		  if (o1 != module.getAt(0))return false;
		  beg = 1;
	  }
	  else if (s2 < s-2) return false;
	  else {
		  boost::python::object no(module.name());
		  if (!e1().isCompatible(no))return false;
		  ArgsCollector::append_arg(l,no);
	  }
	  boost::python::object of = pattern.getAt(s-1);
	  bool lastarg = false;
	  boost::python::object lastargval;
	  boost::python::extract<LsysVar> ef(of);
	  if(!ef.check()){
		if(beg == 1){
		  if (s2 != s) return false;
		  if (of != module.getAt(s-1))return false;
		}
		else {
		  if (s2 != s-1) return false;
		  if (of != module.getAt(s-2))return false;
		}
	  }
	  else {
		lastarg = true;
		LsysVar v = ef();
		if(beg == 1){
		  if(!v.isArgs()){
			if (s2 != s) return false;
			lastargval = module.getAt(s-1);
		  }
		  else {
			if (s2 < s-1) return false;
			else if (s2 == s-1) lastargval = boost::python::list();
			lastargval = module.getslice(s-1,s2);
		  }
		}
		else {
		  if(!v.isArgs()){
			if (s2 != s-1) return false;
			lastargval = module.getAt(s-2);
		  }
		  else {
			if (s2 < s-2) return false;
			else {
			  if(s == 2){
				  if(!v.isCompatible(module.getArgs()))return false;
				  ArgsCollector::append_modargs(l,module.getParameterList()); 
                  return true; 
			  }
			  else if (s2 == s-2) lastargval = boost::python::list();
			  else lastargval = module.getslice(s-2,s2);
			}
		  }
		}
		if(!v.isCompatible(lastargval))return false;
	  }
	  for(size_t i = 1; i < s-1; i++){
		boost::python::object oi = pattern.getAt(i);
		boost::python::extract<LsysVar> e(oi);
		if(!e.check()){
		  if (oi != module.getAt(i-1+beg))return false;
		}
		else { 
			if(!e().isCompatible(module.getAt(i-1+beg))) return false; 
			ArgsCollector::append_arg(l,module.getAt(i-1+beg));
		}
	  }
	  if(lastarg)ArgsCollector::append_arg(l,lastargval);
	  return true;
	}
  }
  else if(module.isStar())return module_matching_with_star_and_valueconstraints(pattern,module,l);
  else {
	if (!module.sameName(pattern)) return false;
	int s = pattern.argSize();
    int s2 = module.argSize();
	if( s == 0) return s2 == 0;
	else {
	  bool lastarg = false;
	  boost::python::object lastargval;
	  boost::python::object of = pattern.getAt(s-1);
	  boost::python::extract<LsysVar> ef(of);
	  if(!ef.check()){
		if (s2 != s) return false;
		if (of != module.getAt(s-1))return false;
	  }
	  else {
		lastarg = true;
		LsysVar v = ef();
		if(!v.isArgs()){
		  if (s2 != s) return false; 
		  else lastargval = module.getAt(s-1);
		}
		else {
		  if (s2 < s - 1) return false;
		  if(s == 1){ 
			  if(!v.isCompatible(module.getArgs()))return false; 
			  ArgsCollector::append_modargs(l,module.getParameterList()); return true; 
		  }
		  else if (s2 == s - 1) lastargval = boost::python::list();
		  else lastargval = module.getslice(s-1,s2);
		}
		if(!v.isCompatible(lastargval)) return false; 
	  }
	  for(size_t i = 0; i < s-1; i++){
		boost::python::object oi = pattern.getAt(i);
		boost::python::extract<LsysVar> e(oi);
		if(!e.check()){
		  if (oi != module.getAt(i))return false;
		}
		else {
			if(!e().isCompatible(module.getAt(i))) return false; 
			ArgsCollector::append_arg(l,module.getAt(i));
		}
	  }
	  if(lastarg)ArgsCollector::append_arg(l,lastargval);
	}
	return true;
  }
}

/*---------------------------------------------------------------------------*/


bool MatchingImplementation::string_exact_match(AxialTree::const_iterator  matching_start,
						   AxialTree::const_iterator  string_end,
						   AxialTree::const_iterator  pattern_begin,
						   AxialTree::const_iterator  pattern_end,
						   AxialTree::const_iterator& matching_end,
						   AxialTree::const_iterator& last_matched,
						   ArgList& params) 
{
	return StringMatcher<>::
		match(matching_start, string_end, pattern_begin, pattern_end, matching_end, last_matched, params);
}

bool MatchingImplementation::string_exact_reverse_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   AxialTree::const_reverse_iterator  pattern_rbegin,
								   AxialTree::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	return StringReverseMatcher<>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}


bool MatchingImplementation::tree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
						         AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return TreeRightMatcher<>::
		match(matching_start, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
}




bool MatchingImplementation::tree_left_match(AxialTree::const_iterator  matching_start,
								AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params) 
{
	return TreeLeftMatcher<>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}


bool MatchingImplementation::mstree_left_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   AxialTree::const_reverse_iterator  pattern_rbegin,
								   AxialTree::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	return TreeLeftMatcher<GetScalePredecessor>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}

bool MatchingImplementation::mltree_left_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   AxialTree::const_reverse_iterator  pattern_rbegin,
								   AxialTree::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	return TreeLeftMatcher<GetLevelPredecessor>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}


bool MatchingImplementation::mstree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
						         AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return TreeRightMatcher<GetScaleSuccessor>::
		match(matching_start, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
}

bool MatchingImplementation::mltree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
						         AxialTree::const_iterator  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return TreeRightMatcher<GetLevelSuccessor>::
		match(matching_start, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
}

LPY_END_NAMESPACE

