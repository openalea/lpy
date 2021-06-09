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


#include "error.h"
#include <boost/python.hpp>
#include <sstream>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

static PrintCallbackType errorCallback = 0;
static PrintCallbackType warningCallback = 0;

/*---------------------------------------------------------------------------*/

LPY_API void SetLsysErrorCallback(PrintCallbackType callback)
{
    errorCallback = callback;
}

LPY_API void SetLsysWarningCallback(PrintCallbackType callback)
{
    warningCallback = callback;
}

LPY_API void LsysError(const std::string& error)
{
    if (errorCallback)
        errorCallback(error);
    else
    {
        PyErr_SetString(PyExc_ValueError, error.c_str());
        boost::python::throw_error_already_set();
    }
}

LPY_API void LsysError(const std::string& error,const std::string& filename, int lineno)
{
    if (!filename.empty() || lineno >=0){
        std::stringstream stream;
        stream << (filename.empty()?"<string>":filename) << ':' << lineno << ':' << error;
        if (errorCallback)
            errorCallback(stream.str());
        else
        {
            PyErr_SetString(PyExc_ValueError, stream.str().c_str());
            boost::python::throw_error_already_set();
        }
    }
    else LsysError(error);
}

LPY_API void LsysSyntaxError(const std::string& error)
{
    if (errorCallback)
        errorCallback(error);
    else
    {
        PyErr_SetString(PyExc_SyntaxError, error.c_str());
        boost::python::throw_error_already_set();
    }
}

LPY_API void LsysSyntaxError(const std::string& error,const std::string& filename, int lineno)
{
    if (!filename.empty() || lineno >=0){
        std::stringstream stream;   
        stream << (filename.empty()?"<string>":filename) << ':' << lineno << ':' << error;
        if (errorCallback)
            errorCallback(stream.str());
        else
        {
            PyErr_SetString(PyExc_SyntaxError, stream.str().c_str());
            boost::python::throw_error_already_set();
        }
    }
    else LsysSyntaxError(error);
}

LPY_API void LsysWarning(const std::string& error)
{
    if (warningCallback)
        warningCallback(error);
    else
        PyErr_WarnEx(PyExc_Warning,error.c_str(),1);
}

LPY_API void LsysWarning(const std::string& error,const std::string& filename, int lineno)
{
    if (warningCallback)
        warningCallback(error);
    else
        PyErr_WarnExplicit(PyExc_Warning,error.c_str(),filename.empty()?"<string>":filename.c_str(),lineno,NULL,NULL);
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

