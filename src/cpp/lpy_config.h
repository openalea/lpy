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

/*! \def LPY_NODLL
    \brief Not creating dll

    Uncomment to use this functionnality
        Do nothing on other platform than windows
*/
/*! \def LPY_DLL
    \brief Using lib as a dll

    Uncomment to use this functionnality
        Do nothing on other platform than windows
*/
/*! \def LPY_MAKEDLL
    \brief Creating dll

    Uncomment to use this functionnality
        Do nothing on other platform than windows
*/
#if defined(_WIN32)
#if defined(LPY_NODLL)
#undef LPY_MAKEDLL
#undef LPY_DLL
#else
#ifndef LPY_DLL
#define LPY_DLL
#endif
#endif

#if defined(LPY_MAKEDLL)
#ifndef LPY_DLL
#define LPY_DLL
#endif
#endif

#ifdef LPY_DLL

#ifdef LPY_MAKEDLL             /* create a Geom DLL library */
#define LPY_API  __declspec(dllexport)
#undef LPY_FWDEF
#else                                                   /* use a Geom DLL library */
#define LPY_API  __declspec(dllimport)
#endif

#define LPY_TEMPLATE_API(T) template class LPY_API T;
#endif

#else // OS != _WIN32

#undef LPY_MAKEDLL             /* ignore these for other platforms */
#undef LPY_DLL

#endif

#ifndef LPY_API
#define LPY_API
#define LPY_TEMPLATE_API(T) 
#endif


/* ----------------------------------------------------------------------- */

#ifdef NO_NAMESPACE

#ifdef LPY_NAMESPACE
#undef LPY_NAMESPACE
#endif

#ifdef LPY_NAMESPACE_NAME
#undef LPY_NAMESPACE_NAME
#endif

#else

/// Macro that enable the tools namespace
#define LPY_NAMESPACE

#endif

#ifdef LPY_NAMESPACE


#ifndef LPY_NAMESPACE_NAME

/// Macro that contains the tools namespace name
#define LPY_NAMESPACE_NAME LPY
#endif

/// Macro for beginning the tools namespace.
#define LPY_BEGIN_NAMESPACE namespace LPY_NAMESPACE_NAME {

/// Macro for ending the tools namespace.
#define LPY_END_NAMESPACE };

/// Macro for using the tools namespace.
#define LPY_USING_NAMESPACE using namespace LPY_NAMESPACE_NAME;

/// Macro for using an object of the tools namespace.
#define LPY_USING(obj) using LPY_NAMESPACE_NAME::obj;

/// Macro to use an object from the tools namespace.
#define LPY(obj) LPY_NAMESPACE_NAME::obj

#else

#ifdef _MSC_VER 
#  pragma message "namespace LPY not used"
#else
#  warning namespace LPY not used
#endif

/// Macro for beginning the tools namespace.
#define LPY_BEGIN_NAMESPACE  

/// Macro for ending the tools namespace.
#define LPY_END_NAMESPACE  

/// Macro for using the tools namespace.
#define LPY_USING_NAMESPACE  

/// Macro for using an object of the tools namespace.
#define LPY_USING(obj)

/// Macro to use an object from the tools namespace.
#define LPY(obj) obj

#endif

/* ----------------------------------------------------------------------- */

// __config_h__
#endif
