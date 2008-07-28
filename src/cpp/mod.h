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

#ifndef __LSYS_MODULES_H__
#define __LSYS_MODULES_H__

#include "error.h"
#include <vector>
#include <boost/python.hpp>

#include "moduleclass.h"

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API Module {
public:
  Module();
  Module(const std::string& c);
  Module(size_t classid);
  Module(const Module&);

  virtual ~Module();

  const std::string& name() const;
  void setName(const std::string& c);
  bool sameName(const Module&) const;
  inline ModuleClassPtr getClass() const { assert(__mclass != NULL); return __mclass; }

  virtual bool hasArg() const;
  virtual int argSize() const;

  virtual std::string str() const;
  virtual std::string repr() const;
  bool operator==(const Module&) const;
  inline bool operator!=(const Module& other) const { return !operator==(other); }

  bool isLeftBracket() const;
  bool isRightBracket() const;
  bool isExactRightBracket() const;
  bool isBracket() const;
  bool isQuery() const;
  bool isCut() const;
  bool isNull() const;
  bool isStar() const;
  bool isConsidered() const;
  bool isIgnored() const;

protected:
  void setClass(size_t cid);

private:
  ModuleClassPtr __mclass;
};

/*---------------------------------------------------------------------------*/

class LPY_API LsysVar {
  public:
	LsysVar(const std::string&);
	std::string str() const;
	const std::string& name() const;
	std::string varname() const;
	void setName(const std::string& );
	bool isArgs() const;
  protected:
	std::string __name;
};

/*---------------------------------------------------------------------------*/
#define VECTORMODULE

class LPY_API ParamModule : public Module {
public:
#ifdef VECTORMODULE
    typedef std::vector<boost::python::object> ParameterList;
#else
  typedef boost::python::list ParameterList;
#endif

  static ParamModule QueryModule(const std::string& name);
  static ParamModule QueryModule(size_t classid, const std::string& args);
  ParamModule(const std::string& name);
  ParamModule(const ParamModule& name);
  ParamModule(size_t classid);
  ParamModule(size_t classid, const std::string& args);
  ParamModule(boost::python::tuple t);
  ParamModule(boost::python::list t);

  ParamModule(const std::string& name, 
			  const boost::python::list& args);
  ParamModule(size_t classid, 
			  const boost::python::list& args);

  ParamModule(const std::string& name, 
			  const boost::python::object& args);

  ParamModule(const std::string& name, 
			  const boost::python::object&,
			  const boost::python::object&);
  ParamModule(const std::string& name, 
			  const boost::python::object&,
			  const boost::python::object&,
			  const boost::python::object&);

  ParamModule(const std::string& name, 
			  const boost::python::object&,
			  const boost::python::object&,
			  const boost::python::object&,
			  const boost::python::object&);

  ParamModule(const std::string& name, 
			  const boost::python::object&,
			  const boost::python::object&,
			  const boost::python::object&,
			  const boost::python::object&,
			  const boost::python::object&);

  virtual ~ParamModule();

  int argSize() const ;
  boost::python::list getArgs() const ;
  void  setArgs(const boost::python::list& list) ;

  boost::python::object getAt(int) const;
  boost::python::object getslice(int, int) const;
  void setAt(int,boost::python::object);
  void delAt(int);
  void delslice(int,int);
  size_t len();
  ParamModule& operator+=(const boost::python::object&);
  ParamModule operator+(const boost::python::object&) const;
  bool operator==(const ParamModule&) const;
  inline bool operator!=(const ParamModule& other) const { return !operator==(other); }

  virtual std::string str() const ;
  virtual std::string repr() const;
  virtual boost::python::tuple tuple() const;

  virtual int  _getInt(int)   const;
  virtual real_t _getReal(int) const;
  virtual std::string _getString(int) const;
  virtual LsysVar _getVar(int) const;
  bool hasOnlyVar() const;
  std::vector<std::string> getVarNames() const;

  virtual void _setValues(real_t,real_t,real_t) ;
  void _setValues(const TOOLS(Vector3)& v) { _setValues(v.x(),v.y(),v.z()); }

  virtual std::string _strArg() const ;
  virtual std::string _reprArg() const ;

  bool match(const ParamModule&m) const;
  bool match(const ParamModule&m,boost::python::list&) const;

  bool match1(const ParamModule&) const;
  bool match2(const ParamModule&) const;
  bool match3(const ParamModule&) const;

  bool match1(const ParamModule&,boost::python::list&) const;
  bool match2(const ParamModule&,boost::python::list&) const;
  bool match3(const ParamModule&,boost::python::list&) const;

  bool match(const std::string&, size_t nbargs) const;

  enum eMatchingMethod {
	eSimple = 0,
	eWithStar,
	eWithStarNValueConstraint,
	eDefaultMatchingMethod = eWithStarNValueConstraint
  };

  static void setMatchingMethod(eMatchingMethod);
  static eMatchingMethod getMatchingMethod();

  inline void interpret(PGL::Turtle& t) { getClass()->interpret(*this,t); }
protected:
  ParamModule();
  ParameterList __args;

private:
  typedef bool (ParamModule::*MatchingFunc)(const ParamModule&) const;
  typedef bool (ParamModule::*MatchingFuncWithArg)(const ParamModule&,boost::python::list&) const;
  static MatchingFunc MATCHINGFUNC;
  static MatchingFuncWithArg MATCHINGFUNCARG;
  static eMatchingMethod MATCHINGMETHOD;
};

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
#endif
