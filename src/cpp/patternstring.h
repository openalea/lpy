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

#ifndef __pattern_lstring_h__
#define __pattern_lstring_h__

#include "abstractlstring.h"
#include "patternmodule.h"
#include <queue>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API PatternString : public AbstractLString<PatternModule> {
public:
  typedef AbstractLString<PatternModule> BaseType;

  PatternString();
  PatternString(const PatternString&);
  PatternString(const PatternModule&);
  PatternString(const_iterator beg, const_iterator end);
  PatternString(const std::string&, int lineno = -1);

  ~PatternString();
  // PatternString& operator=(const PatternString&);
 
  // Get the list of all variables used
  std::vector<std::string> getVarNames() const;
  size_t getVarNb() const;
  void setUnnamedVariables();
  void setUnnamedVariable(size_t);
  std::vector<size_t> getFirstClassId() const;
  std::vector<size_t> getLastClassId() const;

  std::string str() const;
  inline const char * c_str() const { return str().c_str(); }
  std::string repr() const;

};

/*---------------------------------------------------------------------------*/


class PatternStringManager {
	friend class PatternString;
public:

	static PatternStringManager& get(); 
	~PatternStringManager();

	const PatternString& get_pattern(size_t pid);
	size_t register_pattern(const PatternString& pattern);
	void remove_pattern(size_t pid);

protected:
	typedef std::vector<PatternString> PatternStringMap;

	static PatternStringManager * Instance;

	PatternStringManager();

	PatternStringMap __patterns;
	std::queue<size_t> __free_indices;
	PatternString __nullpattern;

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
