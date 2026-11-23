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

#ifndef __pattern_module_h__
#define __pattern_module_h__

#include "module.h"
#include "global.h"

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API LsysVar {
  public:
	enum ConditionType {
		NoCondition,
		EqualValueCondition,
		FunctionalCondition
	};

	LsysVar(const std::string&);
	LsysVar(boost::python::object value);

	std::string str() const;

	inline const std::string& name() const { return __name; }
	inline void setName(const std::string& n) { __name = n; }

	std::string varname() const;
	bool isCompatible(const boost::python::object& value) const;
	void setCondition(const std::string& textualcondition, int lineno = -1);

	inline std::string textualcondition() const { return __textualcondition; }
	inline bool operator==(const LsysVar& other) const { return __name == other.__name; }

	inline const boost::python::object& getPyValue() const { return __pyvalue; }

    inline bool isNamed() const { return !(__name.empty() || __name[0] == '-' || 
		                                   (__name[0] == '*' && ( __name[1] == '-' || (__name[1] == '*' && __name[2] == '-')))); }
	inline bool isArgs() const { return !__name.empty() && __name[0] == '*' && (__name.end() == __name.begin()+1 ||__name[1] != '*'); }
	inline bool isKwds() const { return !__name.empty() && __name[0] == '*' && __name.end() != __name.begin()+1 && __name[1] == '*'; }
	inline bool hasCondition() const { return __conditionType != NoCondition; }

	void setUnnamed();

  protected:

	std::string __name;
	ConditionType __conditionType;
	std::string __textualcondition;
	boost::python::object __pyvalue;

};

/*---------------------------------------------------------------------------*/


class LPY_API PatternModule : public AbstractParamModule<LsysVar> {
public:

  PatternModule();
  PatternModule(const std::string& name, int lineno = -1);
  PatternModule(size_t classid, const std::string& args, int lineno = -1);

  virtual ~PatternModule();


  std::vector<std::string> getVarNames() const;
  size_t getVarNb() const;
  void setUnnamedVariables();
  void setUnnamedVariable(size_t);
  std::vector<size_t> getFirstClassId() const;
  std::vector<size_t> getLastClassId() const;
  std::vector<size_t> getBorderClassId(eDirection dir = eForward) const;

  /*bool match(const ParamModule&m) const;
  bool match(const ParamModule&m, ArgList&) const;
  bool match(const std::string&, size_t nbargs) const;*/

protected:
  void __processPatternModule(const std::string& argstr, int lineno = -1);

};

/*---------------------------------------------------------------------------*/


LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
#endif
