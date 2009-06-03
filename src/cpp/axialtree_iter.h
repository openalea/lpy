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
#include <plantgl/python/exception.h>


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
	    current(_beg),end(_end), nbBracket(0), processfirst(false) {}

	PyLstringIterator(const Lstring& lstring):
	    current(lstring.begin()),end(lstring.end()), nbBracket(0), processfirst(false) {}

	const element_type& next(bool onlyConsidered = false) {
		if(current == end) throw PythonExc_StopIteration();
		if(processfirst == true){
			if(onlyConsidered) current = next_module(current,end);
			else ++current; 
			if(current == end) throw PythonExc_StopIteration();
		}
		else processfirst = true;
		return process_next(onlyConsidered);
	}

	size_t size() { return std::distance(current,end); }

	const element_type& toEndBracket( bool startingBeforePos = false)
	{ 
		if(current == end) throw PythonExc_StopIteration();
		current = endBracket(current,end,startingBeforePos); 
		return process_next();
	}

	const element_type& currentValue() const { 
		if(current == end) throw PythonExc_IndexError();
		return *current; 
	}

protected:

	const element_type& process_next(bool onlyConsidered = false){
		const element_type& val = *current;
		// Iterator should only stay on the sub tree whithin which it was created. Check whether it is true  
		if (val.isRightBracket()) {
			--nbBracket;
			if( nbBracket < 0  && end != (current+1)){
					end = current+1;
			}
		}
		else if(val.isLeftBracket()) ++nbBracket;
		return val; 
	}

	size_t nbBracket;
	bool processfirst;
};

typedef PyLstringIterator<AxialTree> PyAxialTreeIterator;

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
