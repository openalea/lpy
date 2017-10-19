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

#include "lpy_config.h"
#include <string>
#include <vector>
#include <plantgl/tool/util_types.h>
#include <plantgl/tool/util_hashmap.h>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
class LPY_API LpyParsing {
public:

/*---------------------------------------------------------------------------*/
	// static variables
	static const float LPY_FORMAT_VERSION;
	static const float LPY_DEFAULT_FORMAT_VERSION;
    static const std::string InitialisationBeginTag;
    static const std::string VersionTag;
/*---------------------------------------------------------------------------*/

	/// Format checking function
	static const std::vector<float> getSupportedFormat() ; 
	static bool isSupportedFormat(float format);
	static float getFormatVersion(const std::string& lcode) ; 
	static float getFormatVersion(std::string::const_iterator& it,
								  std::string::const_iterator begpos,
								  std::string::const_iterator endpos) ; 
/*---------------------------------------------------------------------------*/
    /// transforming text describing an lstring  in python code
	static std::string lstring2py(const std::string& lcode);

	static std::string lstring2py(const std::string& lcode,
				 std::string::const_iterator& beg);

	static std::string lstring2py(std::string::const_iterator& beg,
				 std::string::const_iterator endpos,
				 char delim = '\n',
				 int lineno = -1,
                 int * nbModules = NULL);


	static std::string lstring2pyparam(std::string::const_iterator& beg,
				 std::string::const_iterator endpos,
				 char delim = '\n',
				 int lineno = -1,
				 size_t * pprod_id = NULL);


	static std::vector<std::pair<size_t,std::string> > parselstring(
				 std::string::const_iterator& beg,
				 std::string::const_iterator endpos,
				 char delim = '\n',
				 int lineno = -1,
				 bool production = false);

	static inline std::vector<std::pair<size_t,std::string> > parselstring(const std::string& lcode, int lineno = -1,
					                                                       bool production = false)

		{ std::string::const_iterator beg = lcode.begin(); return parselstring(beg,lcode.end(),'\n',lineno); }


/*---------------------------------------------------------------------------*/
	/// Module declaration
	struct ModDeclaration {
		std::string name;
		bool alias;
		std::string parameters;
		ModDeclaration(const std::string& _name, const std::string& _parameters = "", bool _alias = false):
		 name(_name),parameters(_parameters),alias(_alias) { }
	};

	// typedef std::pair<std::string,std::string> ModDeclaration;
	typedef std::vector<ModDeclaration> ModDeclarationList;
	typedef std::vector<std::string> ModNameList;

	static ModDeclarationList parse_moddeclist(std::string::const_iterator& beg,
											  std::string::const_iterator endpos,
											  char delim = '\n');

	static inline ModDeclarationList parse_moddeclist(std::string modules)
		{ std::string::const_iterator it = modules.begin(); return parse_moddeclist(it, modules.end()); }

	static ModNameList parse_modlist(std::string::const_iterator& beg,
									 std::string::const_iterator endpos,
									 char delim = '\n');

	static inline ModNameList parse_modlist(std::string modules)
		{ std::string::const_iterator it = modules.begin(); return parse_modlist(it, modules.end()); }

	typedef pgl_hash_map_string<std::string> MetaModDeclaration;
	typedef std::pair<ModDeclarationList,MetaModDeclaration> ModLineDeclaration;
	static ModLineDeclaration parse_moddeclaration_line(std::string::const_iterator& beg,
											  std::string::const_iterator endpos,
											  char delim = '\n');

/*---------------------------------------------------------------------------*/
	/// Modules and variables parsing
	static std::vector<std::string> parse_arguments(std::string::const_iterator beg,
						 						    std::string::const_iterator end);

	static inline std::vector<std::string> parse_arguments(const std::string& args)
		{ return parse_arguments(args.begin(),args.end()); }

	static std::string trim(const std::string& str);
	static std::string removeSpaces(const std::string& str);

	static bool isValidVariableName(const std::string& args)
		{ return isValidVariableName(args.begin(),args.end()); }

	static bool isValidVariableName(std::string::const_iterator beg,
						 			std::string::const_iterator end);

	static std::pair<std::string,std::string> parse_variable(std::string::const_iterator beg,
						 						             std::string::const_iterator end,
															 int lineno = -1);

	inline static std::pair<std::string,std::string> parse_variable(std::string vardef, int lineno = -1)
	{ return parse_variable(vardef.begin(),vardef.end(),lineno); }

	static inline bool isAConstant(std::string str) { return isAConstant(str.begin(),str.end()); }

	static bool isAConstant(std::string::const_iterator beg,
			std::string::const_iterator end);


};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
