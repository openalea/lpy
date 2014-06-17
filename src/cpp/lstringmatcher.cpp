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

#include "lstringmatcher.h"
#include "matching.h"

LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

LstringMatcher::LstringMatcher(AxialTree::const_iterator _begin,
				   AxialTree::const_iterator _end,
				   AxialTree::const_iterator _leftpos,
				   AxialTree::const_iterator _rightpos,
				   AxialTree::const_iterator _rightlastmatch):
  begin(_begin), end(_end), leftpos(_leftpos), 
  rightpos(_rightpos), rightlastmatch(_rightlastmatch)
{
}

/*---------------------------------------------------------------------------*/

void
LstringMatcher::set(AxialTree::const_iterator _begin,
					 AxialTree::const_iterator _end,
					 AxialTree::const_iterator _leftpos,
					 AxialTree::const_iterator _rightpos,
					 AxialTree::const_iterator _rightlastmatch)
{
	begin = _begin ; end = _end;
	leftpos = _leftpos ; 
	rightpos = _rightpos ; rightlastmatch = _rightlastmatch ; 
}

/*---------------------------------------------------------------------------*/
void LstringMatcher::update_returned_args(boost::python::dict& args, const std::vector<std::string>& varnames, const ArgList& values) const
{
	std::vector<std::string>::const_iterator itVar = varnames.begin();
	ArgList::const_iterator itValue = values.begin();
	for(;itVar != varnames.end() && itValue != values.end(); ++itVar, ++itValue)
		args[*itVar] = *itValue;
}

bool LstringMatcher::pInLeftContext(size_t patternid, boost::python::dict& args)
{
   PatternString leftcontext = PatternStringManager::get().get_pattern(patternid);
   return inLeftContext(leftcontext,args);
}

bool LstringMatcher::inLeftContext(const PatternString& pattern, boost::python::dict& args)
{
	AxialTree::const_iterator endposLeft;
	ArgList values;
	if(!pattern.empty()){
		if(!MatchingEngine::left_match(leftpos,begin,end,
			pattern.const_rbegin(),pattern.const_rend(),
			endposLeft,values)) return false;
		leftpos = endposLeft;
		update_returned_args(args, pattern.getVarNames(), values);
	}
	return true;
}

/*---------------------------------------------------------------------------*/


bool LstringMatcher::pInRightContext(size_t patternid, boost::python::dict& args)
{
   PatternString pattern = PatternStringManager::get().get_pattern(patternid);
   return inRightContext(pattern,args);
}

bool LstringMatcher::inRightContext(const PatternString& pattern, boost::python::dict& args)
{
	AxialTree::const_iterator endposRigth;
	ArgList values;
	if(!pattern.empty()){
		if(!MatchingEngine::right_match(rightpos,begin,end,
			pattern.const_begin(),pattern.const_end(),
			rightlastmatch, endposRigth,values)) 
			return false;
		rightpos = endposRigth;
		update_returned_args(args, pattern.getVarNames(), values);
	}
	return true;
}

/*---------------------------------------------------------------------------*/

LstringMatcherMaintainer::LstringMatcherMaintainer(LstringMatcherPtr lmatcher, LsysContext * _context) : 
        context(_context?_context:LsysContext::current())
    { context->registerLstringMatcher(lmatcher); }

LstringMatcherMaintainer::~LstringMatcherMaintainer() { context->registerLstringMatcher();  }

/*---------------------------------------------------------------------------*/
