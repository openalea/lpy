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

#ifndef __PGL_AXIALTREE_ITER_H__
#define __PGL_AXIALTREE_ITER_H__

#include "axialtree.h"
#include "axialtree_manip.h"


LPY_BEGIN_NAMESPACE


/*---------------------------------------------------------------------------*/


template<class Lstring, class Iterator = typename Lstring::const_iterator>
struct PyLstringIterator{
public:
	typedef Lstring string_type;
	typedef Iterator iterator_type;
	typedef typename string_type::element_type element_type;

	iterator_type current;
	iterator_type end;
	PyLstringIterator(iterator_type _beg, iterator_type _end):
	    current(_beg),end(_end) {}

	PyLstringIterator(const Lstring& lstring):
	    current(lstring.begin()),end(lstring.end()) {}

	const element_type& next() {
		if(current == end) throw PythonExc_StopIteration();
		else { const element_type& val = *current; ++current; return val; }
	}

	size_t size() { return std::distance(current,end); }

	void toEndBracket( bool startingBeforePos = false)
	{ current = endBracket(current,end,startingBeforePos); }
};

typedef PyLstringIterator<AxialTree> PyAxialTreeIterator;

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
