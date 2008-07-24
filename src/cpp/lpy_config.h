/* -*-c++-*-
 *  ----------------------------------------------------------------------------
 *
 *       L-Py: L-systems in Python
 *
 *       Copyright 2003-2008 UMR Cirad/Inria/Inra Dap - Virtual Plant Team
 *
 *       File author(s): F. Boudon (frederic.boudon@cirad.fr)
 *
 *  ----------------------------------------------------------------------------
 *
 *                      GNU General Public Licence
 *
 *       This program is free software; you can redistribute it and/or
 *       modify it under the terms of the GNU General Public License as
 *       published by the Free Software Foundation; either version 2 of
 *       the License, or (at your option) any later version.
 *
 *       This program is distributed in the hope that it will be useful,
 *       but WITHOUT ANY WARRANTY; without even the implied warranty of
 *       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 *       GNU General Public License for more details.
 *
 *       You should have received a copy of the GNU General Public
 *       License along with this program; see the file COPYING. If not,
 *       write to the Free Software Foundation, Inc., 59
 *       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ----------------------------------------------------------------------------
 */


#ifndef __lpy_config_h__
#define __lpy_config_h__

/* ----------------------------------------------------------------------- */

/*! \def PYLSYS_NODLL
    \brief Not creating dll

    Uncomment to use this functionnality
        Do nothing on other platform than windows
*/
/*! \def PYLSYS_DLL
    \brief Using lib as a dll

    Uncomment to use this functionnality
        Do nothing on other platform than windows
*/
/*! \def PYLSYS_MAKEDLL
    \brief Creating dll

    Uncomment to use this functionnality
        Do nothing on other platform than windows
*/
#if defined(_WIN32)
#if defined(PYLSYS_NODLL)
#undef PYLSYS_MAKEDLL
#undef PYLSYS_DLL
#else
#ifndef PYLSYS_DLL
#define PYLSYS_DLL
#endif
#endif

#if defined(PYLSYS_MAKEDLL)
#ifndef PYLSYS_DLL
#define PYLSYS_DLL
#endif
#endif

#ifdef PYLSYS_DLL

#ifdef PYLSYS_MAKEDLL             /* create a Geom DLL library */
#define PYLSYS_API  __declspec(dllexport)
#undef PYLSYS_FWDEF
#else                                                   /* use a Geom DLL library */
#define PYLSYS_API  __declspec(dllimport)
#endif

#define PYLSYS_TEMPLATE_API(T) template class PYLSYS_API T;
#endif

#else // OS != _WIN32

#undef PYLSYS_MAKEDLL             /* ignore these for other platforms */
#undef PYLSYS_DLL

#endif

#ifndef PYLSYS_API
#define PYLSYS_API
#define PYLSYS_TEMPLATE_API(T) 
#endif


/* ----------------------------------------------------------------------- */

#ifdef NO_NAMESPACE

#ifdef PYLSYS_NAMESPACE
#undef PYLSYS_NAMESPACE
#endif

#ifdef PYLSYS_NAMESPACE_NAME
#undef PYLSYS_NAMESPACE_NAME
#endif

#else

/// Macro that enable the tools namespace
#define PYLSYS_NAMESPACE

#endif

#ifdef PYLSYS_NAMESPACE


#ifndef PYLSYS_NAMESPACE_NAME

/// Macro that contains the tools namespace name
#define PYLSYS_NAMESPACE_NAME PYLSYS
#endif

/// Macro for beginning the tools namespace.
#define PYLSYS_BEGIN_NAMESPACE namespace PYLSYS_NAMESPACE_NAME {

/// Macro for ending the tools namespace.
#define PYLSYS_END_NAMESPACE };

/// Macro for using the tools namespace.
#define PYLSYS_USING_NAMESPACE using namespace PYLSYS_NAMESPACE_NAME;

/// Macro for using an object of the tools namespace.
#define PYLSYS_USING(obj) using PYLSYS_NAMESPACE_NAME::obj;

/// Macro to use an object from the tools namespace.
#define PYLSYS(obj) PYLSYS_NAMESPACE_NAME::obj

#else

#ifdef _MSC_VER 
#  pragma message "namespace PYLSYS not used"
#else
#  warning namespace PYLSYS not used
#endif

/// Macro for beginning the tools namespace.
#define PYLSYS_BEGIN_NAMESPACE  

/// Macro for ending the tools namespace.
#define PYLSYS_END_NAMESPACE  

/// Macro for using the tools namespace.
#define PYLSYS_USING_NAMESPACE  

/// Macro for using an object of the tools namespace.
#define PYLSYS_USING(obj)

/// Macro to use an object from the tools namespace.
#define PYLSYS(obj) obj

#endif

/* ----------------------------------------------------------------------- */

// __config_h__
#endif
