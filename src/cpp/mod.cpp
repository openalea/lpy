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


#include "mod.h"
#include "matching.h"
#include "lsyscontext.h"
#include "lpy_parser.h"
#include "tracker.h"
#include <strstream>
#include <plantgl/math/util_vector.h>

using namespace boost::python;
LPY_USING_NAMESPACE

Module::Module() : 
	__mclass(ModuleClass::None)
{ IncTracker(Module) }

Module::Module(const std::string& c) : 
	__mclass(ModuleClassTable::get().getClass(c))
{ IncTracker(Module) }

Module::Module(const Module& m) :
	__mclass(m.__mclass)
{ IncTracker(Module) }

Module::Module(size_t classid):
	__mclass(ModuleClassTable::get().getClass(classid))
{ IncTracker(Module) }

Module::~Module()
{ 
	DecTracker(Module)
	//  std::cerr << "Delete Module" << std::endl;
}

const std::string& 
Module::name() const 
{ 
	return __mclass->name;
}

void 
Module::setName(const std::string& c) 
{ 
	__mclass = ModuleClassTable::get().getClass(c);
}

void Module::setClass(size_t cid)
{
	__mclass = ModuleClassTable::get().getClass(cid);
}

bool Module::sameName(const Module& m) const
{
	return __mclass == m.__mclass;
}

bool 
Module::hasArg() const 
{ return argSize() != 0; }

int 
Module::argSize() const 
{ return 0; }

std::string 
Module::str() const 
{ return name(); }


std::string 
Module::repr() const 
{ return name(); }

bool Module::isLeftBracket() const
{ 
	return __mclass == ModuleClass::LeftBracket;
}

bool Module::isRightBracket() const
{ 
	return __mclass == ModuleClass::RightBracket;
}

bool Module::isExactRightBracket() const
{ 
	return __mclass == ModuleClass::ExactRightBracket;
}

bool Module::isBracket() const
{ 
	return __mclass == ModuleClass::LeftBracket ||  
		   __mclass == ModuleClass::RightBracket || 
		   __mclass == ModuleClass::ExactRightBracket;
}

bool Module::isQuery() const
{ 
	return __mclass == ModuleClass::QueryPosition || 
		   __mclass == ModuleClass::QueryHeading ||
		   __mclass == ModuleClass::QueryUp || 
		   __mclass == ModuleClass::QueryLeft || 
		   __mclass == ModuleClass::QueryRigth;
}

bool Module::isCut() const
{ 
	return __mclass == ModuleClass::Cut;
}

bool Module::isNull() const
{ 
	return __mclass == ModuleClass::None || __mclass == ModuleClass::Star;
}

bool Module::isStar() const
{ 
	return __mclass == ModuleClass::Star;
}

bool 
Module::operator==(const Module& n) const
{ 
	return (__mclass == n.__mclass); 
}

/*---------------------------------------------------------------------------*/

LsysVar::LsysVar(const std::string& n):
__name(n){}

std::string LsysVar::str() const
{ return __name; }

const std::string& LsysVar::name() const
{ return __name; }

std::string LsysVar::varname() const
{ if (__name.empty()) return __name;
  else if (__name[0] == '*') return std::string(__name.begin()+1,__name.end());
  else return __name;
}

void LsysVar::setName(const std::string& n)
{ __name = n; }

bool LsysVar::isArgs() const
{ return !__name.empty() && __name[0] == '*'; }

/*---------------------------------------------------------------------------*/

#include <iostream>

#ifdef VECTORMODULE

ParamModule::ParameterList toParameterList(const boost::python::list& t){
  ParamModule::ParameterList result;
  object iter_obj = object( handle<>( PyObject_GetIter( t.ptr() ) ) );
  try { 
      while( 1 ) result.push_back(iter_obj.attr( "next" )());
  }
  catch( error_already_set ){ PyErr_Clear(); }
  return result;
}

boost::python::list toPyList( const ParamModule::ParameterList& t){
  boost::python::list result;
  for(ParamModule::ParameterList::const_iterator it = t.begin(); it != t.end(); ++it)
      result.append(*it);
  return result;
}

#define appendParam(args,p) args.push_back(p)

#else
#define toParameterList(t) t
#define toPyList(t) t
#define appendParam(args,p) args.append(p)
#endif


ParamModule::ParamModule():
Module(),__args() {}

ParamModule::ParamModule(size_t classid):
    Module(classid),__args()
{
}

ParamModule::ParamModule(const std::string& name) :
    Module(),__args()
{
  std::string::const_iterator _it = name.begin();
  while(_it != name.end() && *_it != '(')_it++;
  if(_it == name.end()){
	setName(name);
  }
  else {
	setName(std::string(name.begin(),_it));
	_it++;
	if(_it != name.end()){
	  std::string::const_iterator _it2 = name.end()-1;
	  while(_it2 != _it && *_it2 != ')')_it2--;
      object o = LsysContext::currentContext()->evaluate('['+std::string(_it,_it2)+']');
	  if(o != object()) __args = toParameterList(extract<list>(o)());
	}
  }
}

ParamModule::ParamModule(size_t classid, const std::string& args):
    Module(classid),__args()
{
	if (!args.empty()){
      object o = LsysContext::currentContext()->evaluate('['+args+']');
	  if(o != object()){
		__args = toParameterList(extract<list>(o)());
	  }
	}
}

ParamModule 
ParamModule::QueryModule(const std::string& name) 
{
	std::vector<std::pair<size_t,std::string> > parsedstring = LpyParsing::parselstring(name);
  if(parsedstring.size() != 1)LsysError("Invalid query module "+name);
  ParamModule m(parsedstring[0].first);
  std::vector<std::string> args = LpyParsing::parse_arguments(parsedstring[0].second);
  for(std::vector<std::string>::const_iterator itarg = args.begin(); itarg != args.end(); ++itarg){
    bool notvar = false;
	if (MatchingEngine::getModuleMatchingMethod() == MatchingEngine::eMWithStarNValueConstraint){
		object o = LsysContext::currentContext()->try_evaluate(*itarg);
		if(o != object()){ appendParam(m.__args,o); notvar = true; }
	}
	if (!notvar){
		if(LpyParsing::isValidVariableName(*itarg))
			appendParam(m.__args,object(LsysVar(*itarg)));
		else LsysError(*itarg+" is not defined");
	}
  }
  return m;
}

ParamModule 
ParamModule::QueryModule(size_t classid, const std::string& argstr) 
{
  ParamModule m(classid);
  std::vector<std::string> args = LpyParsing::parse_arguments(argstr);
  for(std::vector<std::string>::const_iterator itarg = args.begin(); itarg != args.end(); ++itarg){
    bool notvar = false;
	if (MatchingEngine::getModuleMatchingMethod() == MatchingEngine::eMWithStarNValueConstraint){
		object o = LsysContext::currentContext()->try_evaluate(*itarg);
		if(o != object()){ appendParam(m.__args,o); notvar = true; }
	}
	if (!notvar){
		if(LpyParsing::isValidVariableName(*itarg))
			appendParam(m.__args,object(LsysVar(*itarg)));
		else LsysError(*itarg+" is not defined");
	}
  }
  return m;
}

ParamModule::ParamModule(const ParamModule& mod):
Module(mod),__args(mod.__args) {}

ParamModule::ParamModule(const std::string& name, const boost::python::list& arg):
Module(name),__args(toParameterList(arg)) {}

ParamModule::ParamModule(size_t classid, const boost::python::list& arg):
Module(classid),__args(toParameterList(arg)) {}

ParamModule::ParamModule(boost::python::tuple t)
{  
  extract<size_t> id_extract(t[0]);
  if (id_extract.check())setClass(id_extract());
  else setName(extract<std::string>(t[0]));
  object iter_obj = object( handle<>( PyObject_GetIter( t.ptr() ) ) );
  iter_obj.attr( "next" )();
  try { while( 1 ) appendParam(__args,iter_obj.attr( "next" )()); }
  catch( error_already_set ){ PyErr_Clear(); }
}


ParamModule::ParamModule(boost::python::list t)
{ 
  extract<size_t> id_extract(t[0]);
  if (id_extract.check())setClass(id_extract());
  else setName(extract<std::string>(t[0]));
  t[0].del();
  __args = toParameterList(t);
}

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a):
Module(name){ appendParam(__args,a); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b):
Module(name){ appendParam(__args,a); appendParam(__args,b); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b,
						 const boost::python::object& c):
Module(name){ appendParam(__args,a); appendParam(__args,b); appendParam(__args,c); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b,
						 const boost::python::object& c,
						 const boost::python::object& d):
Module(name){ appendParam(__args,a); appendParam(__args,b); 
			  appendParam(__args,c); appendParam(__args,d); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b,
						 const boost::python::object& c,
						 const boost::python::object& d,
						 const boost::python::object& e):
Module(name){ appendParam(__args,a); appendParam(__args,b); 
			  appendParam(__args,c); appendParam(__args,d);
			  appendParam(__args,e); }

ParamModule::~ParamModule() 
{ 
#ifdef VECTORMODULE
   __args.clear();
#endif
}

bool ParamModule::operator==(const ParamModule& n) const
{ 
	return (sameName(n) && __args == n.__args); 
}

int
ParamModule::argSize() const 
{ 
#ifdef VECTORMODULE
    return __args.size();
#else
    return extract<int>(__args.attr("__len__")());
#endif
}

list
ParamModule::getArgs() const 
{ return toPyList(__args); }

void 
ParamModule::setArgs(const list& l)
{ __args = toParameterList(l); }

int 
ParamModule::_getInt(int i) const 
{ 
	assert(__args.size() > i);
    extract<int> ext(__args[i]); 
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for int.";
        LsysError(str.str());
    }         
    return ext();
}

real_t 
ParamModule::_getReal(int i) const 
{ 
	assert(__args.size() > i);
    extract<real_t> ext(__args[i]); 
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for float.";
        LsysError(str.str());
    }         
    return ext();
}

std::string 
ParamModule::_getString(int i) const 
{ 
	assert(__args.size() > i);
	extract<char const*>ext(__args[i]);
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for string.";
        LsysError(str.str());
    }
	char const* c_str = ext();
	if (!c_str) return std::string("");
    else return std::string(c_str);
   //  return extract<std::string>(__args[i])(); 
}

LsysVar 
ParamModule::_getVar(int i) const 
{ 
 	assert(__args.size() > i);
    extract<LsysVar> ext(__args[i]); 
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for variable.";
        LsysError(str.str());
    }         
    return ext();
    // return extract<LsysVar>(__args[i])(); 
}

bool ParamModule::hasOnlyVar() const
{ 
  for(size_t i = 0; i < argSize(); i++)
	if(!extract<LsysVar>(getAt(i)).check())return false;
  return true;
}

std::vector<std::string> 
ParamModule::getVarNames() const
{ 
  std::vector<std::string> res;
  for(size_t i = 0; i < argSize(); i++){
	extract<LsysVar> a(getAt(i));
	if(a.check()){
	  LsysVar v = a();
	  res.push_back(v.varname());
	}
  }
  return res;
}

void 
ParamModule::_setValues(real_t x,real_t y,real_t z)
{ 
  int nbArg = argSize();
  if (nbArg > 3) nbArg = 3;
  switch(nbArg){
  case 3:
	__args[2] = object(z);
  case 2:
	__args[1] = object(y);
	__args[0] = object(x);
	break;
  case 1:
      {
        extract<float> ext(__args[0]); 
        if (ext.check()){
            __args[0] = object(x);
        }
        else {
            int size = extract<int>(__args[0].attr("__len__"));
            if (size > 0)__args[0][0] = x;
            if (size > 1)__args[0][1] = y;
            if (size > 2)__args[0][2] = z;
        }
	    break;
      }
  case 0:
      appendParam(__args,object(TOOLS(Vector3(x,y,z))));
	// appendParam(__args,object(y));
	// appendParam(__args,object(z));
  default :
	break;
  }
}

std::string 
ParamModule::str() const {
  std::string st = name();
  if(hasArg())st +='('+_strArg()+')'; 
  return st;
}

std::string 
ParamModule::repr() const {
  std::string st = name();
  if(hasArg())st +='('+_reprArg()+')'; 
  return st;
}

boost::python::tuple 
ParamModule::tuple() const {
  boost::python::tuple res(name());
  res += boost::python::tuple(__args);
  return res;
}

std::string 
ParamModule::_strArg() const 
{ 
  boost::python::str res(",");
  list args;
#ifdef VECTORMODULE
  for (ParameterList::const_iterator it = __args.begin(); it != __args.end(); ++it)
       args.append(boost::python::str(*it));
#else
  try { 
      object iter_obj = object( handle<>( PyObject_GetIter( __args.ptr() ) ) );
      while( 1 )
        {
          object obj = iter_obj.attr( "next" )();
          args.append(boost::python::str(obj));
        }
    }
  catch( error_already_set ){ PyErr_Clear(); }
#endif
  return extract<std::string>(res.join(args));
}

std::string 
ParamModule::_reprArg() const 
{ 
  boost::python::str res(",");
  list args;
  boost::python::object repr = GlobalContext::getFunctionRepr();
#ifdef VECTORMODULE
  
  for (ParameterList::const_iterator it = __args.begin(); it != __args.end(); ++it)
       // args.append(it->attr("__repr__")());
	   args.append(repr(*it));
#else
  try { 
      object iter_obj = object( handle<>( PyObject_GetIter( __args.ptr() ) ) );
      while( 1 )
        {
          object obj = iter_obj.attr( "next" )();
          object val = repr(obj);
          args.append(val);
        }
    }
  catch( error_already_set ){ PyErr_Clear(); }
#endif
  return extract<std::string>(res.join(args));
}

object 
ParamModule::getAt(int i) const
{ 
  assert(__args.size() > i);
  return __args[i];
}

object 
ParamModule::getslice(int i,int j) const
{ 
	assert( i <= j && j <= __args.size() );
#ifdef VECTORMODULE
    boost::python::list res;
    for(ParameterList::const_iterator it = __args.begin()+i; it != __args.begin()+j; ++it)
        res.append(*it);
    return res;
#else
    return __args.attr("__getslice__")(i,j);
#endif
}

void ParamModule::setAt(int i,object o)
{ 
  assert(__args.size() > i);
  __args[i] = o;
}

void ParamModule::delAt(int i)
{ 
  assert(__args.size() > i);
#ifdef VECTORMODULE
  __args.erase(__args.begin()+i);
#else
  __args[i].del();
#endif
}

void ParamModule::delslice(int i,int j)
{ 
	assert( i <= j && j <= __args.size() );
#ifdef VECTORMODULE
  __args.erase(__args.begin()+i,__args.begin()+j);
#else
  __args.attr("__delslice__")(i,j);
#endif
}

size_t ParamModule::len()
{
  return argSize();
}

ParamModule& 
ParamModule::operator+=(const object& o)
{ 
  appendParam(__args,o);
  return *this;
}

ParamModule 
ParamModule::operator+(const object& o) const
{ 
  ParamModule res(*this);
  return res += o;
}

/*---------------------------------------------------------------------------*/
bool ParamModule::match(const std::string& _name, size_t nbargs) const
{ return name() == _name && argSize() == nbargs; }

bool ParamModule::match(const ParamModule& pattern) const 
{ list l; return MatchingEngine::module_match(*this,pattern,l); }

bool ParamModule::match(const ParamModule& pattern, list& l) const 
{ return MatchingEngine::module_match(*this,pattern,l); }

