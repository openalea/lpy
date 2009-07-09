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

#include "lsysrule.h"
#include "matching.h"
#include "lpy_parser.h"
#include "lsyscontext.h"
#include "tracker.h"
#include "argcollector_core.h"
#include <boost/version.hpp>
#include <sstream>
using namespace boost::python;
LPY_USING_NAMESPACE
#define bp boost::python

/*---------------------------------------------------------------------------*/

/*LsysRule::LsysRule():
__id(0),
__gid(0),
__prefix('p'),
__hasquery(false),
lineno(-1){
  IncTracker(LsysRule)
}*/

LsysRule::LsysRule(const LsysRule& other):
__id(other.__id),
__gid(other.__gid),
__prefix(other.__prefix),
__predecessor(other.__predecessor),
__leftcontext(other.__leftcontext),
__newleftcontext(other.__newleftcontext),
__rightcontext(other.__rightcontext),
__newrightcontext(other.__newrightcontext),
__formalparameters(other.__formalparameters),
__nbParams(other.__nbParams),
__definition(other.__definition),
__hasquery(other.__hasquery),
__isStatic(other.__isStatic),
__function(other.__function),
lineno(other.lineno){
  IncTracker(LsysRule)
}

LsysRule::LsysRule( size_t id, size_t gid, char prefix, int _lineno):
__id(id),
__gid(gid),
__prefix(prefix),
__nbParams(0),
__hasquery(false),
__isStatic(false),
lineno(_lineno){
  IncTracker(LsysRule)
}

LsysRule::~LsysRule() { DecTracker(LsysRule) }

bool LsysRule::compiled() const {
  return __function != object();
}

size_t LsysRule::nbContexts() const {
  size_t c = (__leftcontext.empty()?0:1);
  c += (__newleftcontext.empty()?0:1);
  c += (__newrightcontext.empty()?0:1);
  c += (__rightcontext.empty()?0:1);
  return c;
}

void LsysRule::clear(){
  __id = 0;
  __gid = 0;
  __prefix = 'p';
  __predecessor.clear();
  __newleftcontext.clear();
  __leftcontext.clear();
  __rightcontext.clear();
  __newrightcontext.clear();
  __formalparameters.clear();
  __nbParams = 0;
  __definition.clear();
  __function = object();
  __hasquery = false;
  __isStatic = false;
  lineno = -1;
}

std::string LsysRule::str() const {
  std::string res = name();
  if(res.empty())res += "(none)";
  res += " : \n";
  if(__definition.empty())res += "\t(none)";
  else res += __definition;
  return res;
}

std::string LsysRule::name() const {
  std::string res;
  if (!__leftcontext.empty())
		  res = __leftcontext.str() + " < ";
  if (!__newleftcontext.empty())
	res += __newleftcontext.str() + " << ";
  res += __predecessor.str();
  if (!__newrightcontext.empty())
	res +=  " >> " + __newrightcontext.str();
  if (!__rightcontext.empty())
	res +=  " > " + __rightcontext.str();
  return res;
}

std::string LsysRule::uname() const {
  std::string res;
  if (!__leftcontext.empty())
		  res = "'"+__leftcontext.str() + "' < ";
  if (!__newleftcontext.empty())
	res += "'"+__newleftcontext.str() + "' << ";
  res += "'"+__predecessor.str()+"'";
  if (!__newrightcontext.empty())
	res +=  " >> '" + __newrightcontext.str()+"'";
  if (!__rightcontext.empty())
	res +=  " > '" + __rightcontext.str()+"'";
  return res;
}

std::string LsysRule::functionName() const {
  std::stringstream ss;
  ss << "__" << __prefix << "_" << __gid << "_" << __id << "_";
  std::string name = __leftcontext.str();
  std::string::const_iterator _it;
  for (_it = name.begin();_it != name.end(); ++_it){
	if (isalpha(*_it)) ss << *_it;
	else ss << '_';
  }
  name = __newleftcontext.str();
  for (_it = name.begin(); _it != name.end(); ++_it){
	if (isalpha(*_it)) ss << *_it;
	else ss << '_';
  }
  name = __predecessor.str();
  for (_it = name.begin(); _it != name.end(); ++_it){
	if (isalpha(*_it)) ss << *_it;
	else ss << '_';
  }
  name = __newrightcontext.str();
  for (_it = name.begin(); _it != name.end(); ++_it){
	if (isalpha(*_it)) ss << *_it;
	else ss << '_';
  }
  name = __rightcontext.str();
  for (_it = name.begin(); _it != name.end(); ++_it){
	if (isalpha(*_it)) ss << *_it;
	else ss << '_';
  }
  return ss.str();
}

std::string LsysRule::callerFunctionName() const { return functionName()+"_caller"; }

std::string 
LsysRule::getCode() const{
    return getCoreCode() +getCallerCode();
}

std::string 
LsysRule::getCoreCode() const{
  std::stringstream res;
  int llineno = 0;
  res << "def " << functionName() << "(";
  if(!__formalparameters.empty())
    for(std::vector<std::string>::const_iterator _it = __formalparameters.begin();
    _it != __formalparameters.end(); ++_it){
	     if(_it != __formalparameters.begin()) 
            res << ',';
	    res << *_it;
  }
  res << ") :"; //  #" << name() << "\n";
  std::string definition;
  std::string::const_iterator _beg = __definition.begin();
  std::string::const_iterator _it = _beg;
  while( _it != __definition.end()){
	if (*_it == '\n') { ++llineno; _it++;}
	else if (*_it == '#') { while(_it != __definition.end() && *_it != '\n') _it++; }
	else if (*_it == 'p' && std::distance(_it,__definition.end())> 7){
	  if(std::string(_it,_it+7) == "produce"){
		definition.insert(definition.end(),_beg,_it);
		_it += 7;
		definition += "return ";
		definition += LpyParsing::lstring2py(_it,__definition.end(),'\n',lineno==-1?lineno:lineno+llineno);
		_beg = _it;
	  }
	  else _it++;
	}
	else if(*_it == 'n' && std::distance(_it,__definition.end())> 8){
	  if(std::string(_it,_it+8) == "nproduce"){
		definition.insert(definition.end(),_beg,_it);
		_it += 8;
		definition += "nproduce(";
		definition += LpyParsing::lstring2py(_it,__definition.end(),'\n',lineno==-1?lineno:lineno+llineno);
        definition += ')';
		_beg = _it;
	  }
	  else _it++;
	}
	else _it++;
  }
  if(_beg!=_it)definition += std::string(_beg,_it);
  if(definition.empty()) res << "\t\tpass\n";
  else {
	res << definition;
	if (*(__definition.end()-1) != '\t' && *(__definition.end()-1) != '\n') res << "\n";
  }
  return res.str();
}

#ifdef USE_PYTHON_LIST_COLLECTOR
#define MAX_LRULE_DIRECT_ARITY  0
#else
#define MAX_LRULE_DIRECT_ARITY  15
#endif

std::string 
LsysRule::getCallerCode() const{
  if (__nbParams <= MAX_LRULE_DIRECT_ARITY) return "";
  std::stringstream res;
  res << "def " << callerFunctionName() << "(args=[]) : return " << functionName() << "(*args)\n";
  return res.str();
}
void LsysRule::compile(){
	if (!compiled()){
	  __function = LsysContext::currentContext()->compile(__nbParams<= MAX_LRULE_DIRECT_ARITY?functionName():callerFunctionName(),getCode());
	// __function = LsysContext::currentContext()->compile(functionName(),getCode());
	}
}

void LsysRule::importPyFunction(){
  if (!compiled())
      __function = LsysContext::currentContext()->getObject(__nbParams<=MAX_LRULE_DIRECT_ARITY?functionName():callerFunctionName());
      // __function = LsysContext::currentContext()->getObject(functionName());
  if(__isStatic){
	  __isStatic = false;
	  if(__nbParams==0) __staticResult = extract<AxialTree>(apply())();
	  else {
		  ArgList args;
		  for (size_t i =0; i < __nbParams; ++i)args.push_back(object());
		  __staticResult = extract<AxialTree>(apply(args))();
	  }
	  __isStatic = true;
  }
}

void LsysRule::__precall_function( size_t nbargs ) const
{
  LsysContext::currentContext()->reset_nproduction();
  if (nbargs != __nbParams) {
      std::stringstream res;
      res << name() << " takes exactly " << __nbParams << " argument(s) (" << nbargs << " given).\n";
      LsysError(res.str());
    }
}
void LsysRule::__precall_function( size_t nbargs, const ArgList& args ) const
{
  LsysContext::currentContext()->reset_nproduction();
  if (nbargs != __nbParams) {
      std::stringstream res;
      res << name() << " takes exactly " << __nbParams << " argument(s) (" << nbargs << " given).\n"
		  << bp::extract<std::string>(bp::str(bp::object(args)))();
      LsysError(res.str());
    }
}


boost::python::object LsysRule::__postcall_function( boost::python::object res ) const
{
  boost::python::object fres;
  if (res == object()) 
  { 
      // no production. look for nproduction
      AxialTree nprod = LsysContext::currentContext()->get_nproduction(); 
      if (nprod.empty()) return object();
      else { 
          fres = object(nprod);
          LsysContext::currentContext()->reset_nproduction(); // to avoid deep copy
          return fres;
      }
  }
  else {
      // production. add nproduction if needed
      AxialTree nprod = LsysContext::currentContext()->get_nproduction(); 
      AxialTree pres = AxialTree(extract<boost::python::list>(res));
      if (!nprod.empty()){
          LsysContext::currentContext()->reset_nproduction(); //  to avoid deep copy
          nprod += pres;
          return object(nprod);
      }
      else return object(pres);
  }
}


//#if BOOST_VERSION < 103400
//#warning Redefine len on a boost python object
//inline size_t len( const object& obj ) { return extract<size_t>(obj.attr("__len__")()); }

//#endif

boost::python::object LsysRule::__call_function( size_t nbargs, const ArgList& args ) const
{
	switch(nbargs){
		case 0: return __function(); break;
#if MAX_LRULE_DIRECT_ARITY > 0
		case 1: return __function(args[0]); break;
		case 2: return __function(args[0],args[1]); break;
		case 3: return __function(args[0],args[1],args[2]); break;
		case 4: return __function(args[0],args[1],args[2],args[3]); break;
		case 5: return __function(args[0],args[1],args[2],args[3],args[4]); break;
		case 6: return __function(args[0],args[1],args[2],args[3],args[4],args[5]); break;
		case 7: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6]); break;
		case 8: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]); break;
		case 9: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8]); break;
		case 10: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9]); break;
		case 11: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10]); break;
		case 12: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10],args[11]); break;
		case 13: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10],args[11],args[12]); break;
		case 14: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10],args[11],args[12],args[13]); break;
		case 15: return __function(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10],args[11],args[12],args[13],args[14]); break;
#endif
		default: return __function(args); break;
	}
}

object 
LsysRule::apply( const ArgList& args ) const
{ 
  if(__isStatic) return object(__staticResult);
  if (!compiled()) LsysError("Python code of rule not compiled");
  size_t argsize = len(args);
  __precall_function(argsize,args);
  return __postcall_function(__call_function(argsize,args)); 
}



object 
LsysRule::apply( ) const
{ 
  if(__isStatic) return object(__staticResult);
  if (!compiled()) LsysError("Python code of rule not compiled");
  __precall_function();
  return __postcall_function(__function()); 
}

template<class T>
inline void extend_vec(T& v, const T& v2) { v.insert(v.end(),v2.begin(),v2.end()); }

void 
LsysRule::parseParameters(){
  __formalparameters.clear();
  if(!__leftcontext.empty())
	extend_vec(__formalparameters,__leftcontext.getVarNames());
  if(!__newleftcontext.empty())
	extend_vec(__formalparameters,__newleftcontext.getVarNames());
  if(!__predecessor.empty())
	extend_vec(__formalparameters,__predecessor.getVarNames());  
  if(!__newrightcontext.empty())
	extend_vec(__formalparameters,__newrightcontext.getVarNames());  
  if(!__rightcontext.empty())
	extend_vec(__formalparameters,__rightcontext.getVarNames());
  int rp = redundantParameter();
  if(rp != -1){
      LsysError("Ill-formed Rule Header : Multiple definition of parameter "+__formalparameters[rp]+": "+uname());
  }
  __nbParams = __formalparameters.size();
}

int
LsysRule::redundantParameter() const {
    if (__formalparameters.empty()) return -1;
    std::vector<std::string>::const_iterator _first = __formalparameters.begin();
    std::vector<std::string>::const_iterator _last = __formalparameters.end() - 1;
    while (_first != _last) {
      if (std::find(_first + 1,__formalparameters.end(),*_first) 
				!= __formalparameters.end()) return distance(__formalparameters.begin(),_first);
      _first++;
    };
    return -1;
}
        
/*---------------------------------------------------------------------------*/

bool
LsysRule::match(const AxialTree& src,
			   AxialTree::const_iterator pos,
			   const AxialTree& dest,
			   AxialTree::const_iterator& endpos,
               ArgList& args,
               eDirection direction) const 
{
  args.reserve(__nbParams);
  ArgList args_pred;
  AxialTree::const_iterator endpos1;
  AxialTree::const_iterator last_match = pos;
  if (direction == eForward){
   if(!MatchingEngine::match(pos,src.const_end(),__predecessor.const_begin(),__predecessor.const_end(),endpos1,last_match,args_pred))
   // if(!src.match(__predecessor,pos,endpos1,last_match,args_pred))
 	 return false;
  }
  else{
    AxialTree::const_iterator tmp;
    if(!MatchingEngine::reverse_match(pos,src.const_begin(),src.const_end(),
		                              __predecessor.const_rbegin(),__predecessor.const_rend(),
									  tmp,args_pred))
    //if(!src.reverse_match(__predecessor,pos,tmp,args_pred))
 	   return false;
    endpos1 = (pos == src.end()?pos:pos+1);
    pos = tmp;
  }
  AxialTree::const_iterator endpos2;
  if(!__leftcontext.empty()){
      if(!MatchingEngine::left_match(direction == eForward?pos:pos+1,src.const_begin(),src.const_end(),
		                              __leftcontext.const_rbegin(),__leftcontext.const_rend(),
									  endpos2,args))
	  // if(!src.leftmatch(__leftcontext,direction == eForward?pos:pos+1,endpos2,args))
	  return false;
  }
  if(direction == eForward && !__newleftcontext.empty()){
	ArgList args_ncg;
	// if(!dest.leftmatch(__newleftcontext,dest.end(),endpos2,args_ncg))return false;
    if(!MatchingEngine::left_match(dest.const_end(),dest.const_begin(),dest.const_end(),
		                          __newleftcontext.const_rbegin(),__newleftcontext.const_rend(),
								  endpos2,args_ncg))return false;
	ArgsCollector::append_args(args,args_ncg);
  }
  ArgsCollector::append_args(args,args_pred);
  if(direction == eBackward && !__newrightcontext.empty()){
	ArgList args_ncd;
	// if(!dest.rightmatch(__newrightcontext,dest.begin(),last_match,endpos2,args_ncd))return false;
    if(!MatchingEngine::right_match(dest.const_begin(),dest.const_end(),
		                          __newrightcontext.const_begin(),__newrightcontext.const_end(),
								  last_match,endpos2,args_ncd))return false;
	ArgsCollector::append_args(args,args_ncd);
  }
  if(!__rightcontext.empty()){
	ArgList args_cd;
	// if(!src.rightmatch(__rightcontext,endpos1,last_match,endpos2,args_cd))return false;
    if(!MatchingEngine::right_match(endpos1,src.const_end(),
		                          __rightcontext.const_begin(),__rightcontext.const_end(),
								  last_match,endpos2,args_cd))return false;
	ArgsCollector::append_args(args,args_cd);
  }
  if (direction == eForward)
    endpos = endpos1;
  else
    endpos = pos;
  return true;
}

bool
LsysRule::applyTo( AxialTree& dest, 
				   const ArgList& args, 
				   size_t * length,
				   eDirection direction) const {
  
   AxialTree prod;
   if(__isStatic) prod = __staticResult;
   else {
	object result = apply(args);
	if(result == object())return false;
	else prod = extract<AxialTree>(result)();
   }
   ModuleClassPtr cl;
   if (!prod.empty() && ((cl=prod[0].getClass()) == ModuleClass::Star || cl == ModuleClass::None)){ 
		if(length!=NULL)*length = 0;
   }
   else {
	if(length!=NULL)*length = prod.size();
	if(direction == eForward) dest += prod;
	else dest.prepend(prod);
   }
   return true;
}


AxialTree
LsysRule::process( const AxialTree& src ) const {
  AxialTree dest;
  AxialTree::const_iterator _it = src.begin();
  while(_it != src.end()){
	ArgList args;
	if(!match(src,_it,dest,_it,args)){
	  dest.push_back(_it);
	  ++_it;
	}
	else { applyTo(dest,args); }
  }
  return dest;
}

/*---------------------------------------------------------------------------*/
