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

#ifndef __LPY_PARSER_H__
#define __LPY_PARSER_H__

#include "pylsys_config.h"
#include <string>
#include <vector>

PYLSYS_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/


std::string PYLSYS_API lstring2py(const std::string& lcode);

std::string PYLSYS_API lstring2py(const std::string& lcode,
				 std::string::const_iterator& beg);

std::string PYLSYS_API lstring2py(std::string::const_iterator& beg,
				 std::string::const_iterator endpos,
				 char delim = '\n',
				 int lineno = -1);


std::vector<std::pair<size_t,std::string> > parselstring(
				 std::string::const_iterator& beg,
				 std::string::const_iterator endpos,
				 char delim = '\n',
				 int lineno = -1);

inline std::vector<std::pair<size_t,std::string> > parselstring(const std::string& lcode)
{ std::string::const_iterator beg = lcode.begin(); return parselstring(beg,lcode.end()); }

std::vector<std::string> parse_moddeclaration(std::string::const_iterator& beg,
											  std::string::const_iterator endpos,
											  char delim = '\n');

inline std::vector<std::string> parse_moddeclaration(std::string modules)
{ std::string::const_iterator it = modules.begin(); return parse_moddeclaration(it, modules.end()); }

std::vector<std::string> parse_arguments(std::string::const_iterator beg,
										 std::string::const_iterator end);

inline std::vector<std::string> parse_arguments(const std::string& args)
{ return parse_arguments(args.begin(),args.end()); }

std::string trim(const std::string& str);

bool isValidVariableName(const std::string& args);

/*---------------------------------------------------------------------------*/

PYLSYS_END_NAMESPACE

#endif
