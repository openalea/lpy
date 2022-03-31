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

#ifndef __PGL_LSYSERROR_H__
#define __PGL_LSYSERROR_H__

#include "lpy_config.h"
#include <string>
#include <plantgl/python/exception.h>


LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

typedef void (*PrintCallbackType)(const std::string &);

LPY_API void SetLsysErrorCallback(PrintCallbackType callback);

LPY_API void SetLsysWarningCallback(PrintCallbackType callback);

LPY_API void LsysError(const std::string& error);

LPY_API void LsysError(const std::string& error,const std::string& filename, int lineno);

LPY_API void LsysSyntaxError(const std::string& error);

LPY_API void LsysSyntaxError(const std::string& error,const std::string& filename, int lineno);

LPY_API void LsysWarning(const std::string& error);

LPY_API void LsysWarning(const std::string& error,const std::string& filename, int lineno);

#ifdef DEBUG
#define lpyassert(x) assert(x)
#else
#define lpyassert(x)
#endif

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif


