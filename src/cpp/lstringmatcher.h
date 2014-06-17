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

#include "axialtree.h"
#include "lsyscontext.h"


#ifndef __lstring_matcher_h__
#define __lstring_matcher_h__

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/


class LstringMatcher : public TOOLS(RefCountObject) {
public:
	AxialTree::const_iterator begin;
	AxialTree::const_iterator end;

	AxialTree::const_iterator leftpos;

	AxialTree::const_iterator rightpos;
	AxialTree::const_iterator rightlastmatch;

	LstringMatcher(AxialTree::const_iterator _begin,
				   AxialTree::const_iterator _end,
				   AxialTree::const_iterator _leftpos,
				   AxialTree::const_iterator _rightpos,
				   AxialTree::const_iterator _rightlastmatch);

	void set(AxialTree::const_iterator _begin,
			 AxialTree::const_iterator _end,
			 AxialTree::const_iterator _leftpos,
			 AxialTree::const_iterator _rightpos,
			 AxialTree::const_iterator _rightlastmatch);

	bool pInLeftContext(size_t patternid, boost::python::dict& args);
	bool inLeftContext(const PatternString& pattern, boost::python::dict& args);

	bool pInRightContext(size_t patternid, boost::python::dict& args);
	bool inRightContext(const PatternString& pattern, boost::python::dict& args);
private:
	void update_returned_args(boost::python::dict& args, const std::vector<std::string>& varnames, const ArgList& values) const;
};

typedef RCPtr<LstringMatcher> LstringMatcherPtr;

class LsysContext;

struct LstringMatcherMaintainer {
    LsysContext * context;

    LstringMatcherMaintainer(LstringMatcherPtr lmatcher, LsysContext * _context = NULL  ) ;

    ~LstringMatcherMaintainer();
};
/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
