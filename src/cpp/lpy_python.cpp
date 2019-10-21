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


#include "lpy_python.h"

/*---------------------------------------------------------------------------*/

PySeqIterator::PySeqIterator(boost::python::object seq) :
    __iter_obj( ), __valid(true)
{
    PyObject * pyiter = PyObject_GetIter( seq.ptr() ) ;
    __valid = (pyiter != NULL);
    __iter_obj =  boost::python::object(boost::python::handle<>( pyiter ) );
    _next();
}

bool PySeqIterator::is_valid() const { return __valid;}

boost::python::object PySeqIterator::next() 
{
    boost::python::object result = __next_obj;
    _next();
    return result;   

}

void PySeqIterator::_next() 
{
    if (__valid) {
        PyObject * item = PyIter_Next(__iter_obj.ptr());
        __valid = (item != NULL); 
        if (__valid)
            __next_obj = boost::python::object( boost::python::handle<PyObject>(item));
        else
            __next_obj = boost::python::object();
    }
}

