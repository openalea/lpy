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

#ifndef __PGL_STRINGMATCHING_H__
#define __PGL_STRINGMATCHING_H__

#include <vector>
#include "error.h"

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API StringMatching {
public:
    enum eIdPolicy {
        eIncreasing,
        eFixed
    };

protected:
    struct LPY_API StringMark {
        StringMark(size_t o = 0,size_t t = 0,eIdPolicy i = eIncreasing):original(o),target(t),idpolicy(i) {}
        size_t original;
        size_t target;
        eIdPolicy idpolicy;
    };

    typedef std::vector<StringMark> StringMarkList;
    StringMarkList __matching;

public:
    struct LPY_API const_iterator {
        friend class StringMatching;
    protected:
        StringMarkList::const_iterator __iter;
        StringMarkList::const_iterator __end;
        size_t __target;
        size_t __original;
        const_iterator();
    public:
        void increment();
        const_iterator& operator++() { increment(); return *this; }
        const_iterator operator++(int i) { const_iterator t = *this; for (int j =0; j < i; j++)increment(); return t; }
        const size_t& operator*( ) const { return __original; }
        const size_t& original( ) const { return __original; }
        const size_t& target( ) const { return __target; }
        const size_t& next( ) { increment(); return __original; }
        bool isOnMark() const;
        eIdPolicy currentIdPolicy() const;

    };

    StringMatching();
    ~StringMatching();
    
    StringMatching::const_iterator begin() const;
    StringMatching::const_iterator end() const;

    /**! append a new matching interval. distance are given relative to previous marks.
         for instance, with append(3,1);append(1,1);
         second interval will actually correspond to module 4 for target and 2 for source. */
    void append(size_t original, size_t target);

    void addIdentity(size_t length);

    bool isIdentity() const;
    void clear();

    StringMatching operator+(const StringMatching& newmatching);
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif
