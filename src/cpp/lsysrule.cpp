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
#include "lpy_parser.h"
#include "lsyscontext.h"
#include "tracker.h"
#include <boost/version.hpp>
#include <sstream>
using namespace boost::python;
LPY_USING_NAMESPACE

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
__definition(other.__definition),
__hasquery(other.__hasquery),
__function(other.__function),
lineno(other.lineno){
  IncTracker(LsysRule)
}

LsysRule::LsysRule( size_t id, size_t gid, char prefix, int _lineno):
__id(id),
__gid(gid),
__prefix(prefix),
__hasquery(false),
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
  __definition.clear();
  __function = object();
  __hasquery = false;
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

/*
from pgl_mod import *
a = LsysRule("""
B(y) << D(w) < A(x) > C(z) :
    if y > 2:
            produce A(x+1)F
    elif w > 2:
            produce D(x+1)F@U
    else:
            produce B(y)+F(z)
""")
print(a.code())
*/

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
std::string 
LsysRule::getCallerCode() const{
  if (__formalparameters.empty()) return "";
  std::stringstream res;
  res << "def " << callerFunctionName() << "(args=[]) : return " << functionName() << "(*args)\n";
  return res.str();
}
void LsysRule::compile(){
	if (!compiled()){
	  __function = LsysContext::currentContext()->compile(__formalparameters.empty()?functionName():callerFunctionName(),getCode());
	// __function = LsysContext::currentContext()->compile(functionName(),getCode());
	}
}

void LsysRule::importPyFunction(){
  if (!compiled())
      __function = LsysContext::currentContext()->getObject(__formalparameters.empty()?functionName():callerFunctionName());
      // __function = LsysContext::currentContext()->getObject(functionName());
}

void LsysRule::__precall_function( size_t nbargs ) const
{
  LsysContext::currentContext()->reset_nproduction();
  if (nbargs != __formalparameters.size()) {
      std::stringstream res;
      res << name() << " takes exactly " << __formalparameters.size() << " argument(s) (" << nbargs << " given).\n";
      LsysError(res.str());
    }
}

/*
boost::python::object LsysRule::__call_function( const boost::python::tuple& arglist)
{
    std::cerr << "Call object" << std::endl;
    PyObject * result = PyEval_CallObject(__function.ptr(), arglist.ptr());
    std::cerr << "End call object" << std::endl;
    if (result == NULL) LsysError("NULL result");
    return object(handle<>(allow_null(result)));
}*/

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

/*
tuple list2tuple(const list& l){
    size_t lsize = len(l);
    PyObject * pytuple =  PyTuple_New(lsize);
    if (pytuple == NULL) LsysError("Cannot create args tuple");
    // for (int i = 0; i < lsize; ++i)
    //     PyTuple_SetItem(pytuple,i,l[i].operator boost::python::api::object().ptr());
    return tuple(handle<>(pytuple));
}*/

#if BOOST_VERSION < 103400
#warning Redefine len on a boost python object
inline size_t len( const object& obj ) { return extract<size_t>(obj.attr("__len__")()); }
#endif

object 
LsysRule::apply( const list& args ) const
{ 
  // if (!compiled())compile();
  if (!compiled()) LsysError("Python code of rule not compiled");
  size_t argsize = len(args);
  __precall_function(argsize);
  return  __postcall_function(argsize?__function(args):__function()); 
  // return  __postcall_function(argsize?__call_function(list2tuple(args)):__function()); 
}

object 
LsysRule::apply( const tuple& args ) const
{ 
  if (!compiled()) LsysError("Python code of rule not compiled");
  size_t argsize = len(args);
  __precall_function(argsize);
  return  __postcall_function(argsize?__function(args):__function()); 
  // return  __postcall_function(argsize?__call_function(args):__function()); 
}

object 
LsysRule::apply( ) const
{ 
  if (!compiled()) LsysError("Python code of rule not compiled");
  __precall_function();
  return __postcall_function(__function()); 
}


void 
LsysRule::parseParameters(){
  __formalparameters.clear();
  if(!__leftcontext.empty()){
	for(AxialTree::const_iterator _it = __leftcontext.begin(); 
	_it !=__leftcontext.end(); ++_it){
	  for(size_t i = 0; i < _it->argSize(); i++){
		extract<LsysVar> var(_it->getAt(i));
		if(var.check())	__formalparameters.push_back(var().varname());
	  }
	}
  }
  if(!__newleftcontext.empty()){
	for(AxialTree::const_iterator _it = __newleftcontext.begin(); 
		_it !=__newleftcontext.end(); ++_it)
	  for(size_t i = 0; i < _it->argSize(); ++i){
		extract<LsysVar> var(_it->getAt(i));
		if(var.check())	__formalparameters.push_back(var().varname());
	  }
  }
  if(!__predecessor.empty()){
	for(AxialTree::const_iterator _it = __predecessor.begin(); 
		_it !=__predecessor.end(); ++_it)
	  for(size_t i = 0; i < _it->argSize(); ++i){
		extract<LsysVar> var(_it->getAt(i));
		if(var.check())	__formalparameters.push_back(var().varname());
	  }
  }
  if(!__newrightcontext.empty()){
	for(AxialTree::const_iterator _it = __newrightcontext.begin(); 
		_it !=__newrightcontext.end(); ++_it)
	  for(size_t i = 0; i < _it->argSize(); i++){
		extract<LsysVar> var(_it->getAt(i));
		if(var.check())	__formalparameters.push_back(var().varname());
	  }
  }
  if(!__rightcontext.empty()){
	for(AxialTree::const_iterator _it = __rightcontext.begin(); 
		_it !=__rightcontext.end(); ++_it)
	  for(size_t i = 0; i < _it->argSize(); i++){
		extract<LsysVar> var(_it->getAt(i));
		if(var.check())	__formalparameters.push_back(var().varname());
	  }
  }
  if(redundantParameter()){
      LsysError("Ill-formed Rule Header : Multiple definition of parameter : "+uname());
  }
}

bool
LsysRule::redundantParameter() const {
    if (__formalparameters.empty()) return false;
    std::vector<std::string>::const_iterator _first = __formalparameters.begin();
    std::vector<std::string>::const_iterator _last = __formalparameters.end() - 1;
    while (_first != _last) {
      if (std::find(_first + 1,__formalparameters.end(),*_first) 
				!= __formalparameters.end()) return true;
      _first++;
    };
    return false;
}
        
/*---------------------------------------------------------------------------*/

bool
LsysRule::match(const AxialTree& src,
			   AxialTree::const_iterator pos,
			   const AxialTree& dest,
			   AxialTree::const_iterator& endpos,
               boost::python::list& args,
               eDirection direction) const 
{
  boost::python::list args_pred;
  AxialTree::const_iterator endpos1;
  AxialTree::const_iterator last_match = pos;
  if (direction == eForward){
   if(!src.match(__predecessor,pos,endpos1,last_match,args_pred))
 	 return false;
  }
  else{
    AxialTree::const_iterator tmp;
    if(!src.reverse_match(__predecessor,pos,tmp,args_pred))
 	   return false;
    endpos1 = (pos == src.end()?pos:pos+1);
    pos = tmp;
  }
  AxialTree::const_iterator endpos2;
  if(!__leftcontext.empty()){
	  if(!src.leftmatch(__leftcontext,direction == eForward?pos:pos+1,endpos2,args))
	  return false;
  }
  if(direction == eForward && !__newleftcontext.empty()){
	boost::python::list args_ncg;
	if(!dest.leftmatch(__newleftcontext,dest.end(),endpos2,args_ncg))return false;
	args += args_ncg;
  }
  args += args_pred;
  if(direction == eBackward && !__newrightcontext.empty()){
	boost::python::list args_ncg;
	if(!dest.rightmatch(__newrightcontext,dest.begin(),last_match,endpos2,args_ncg))return false;
	args += args_ncg;
  }
  if(!__rightcontext.empty()){
	boost::python::list args_cd;
	if(!src.rightmatch(__rightcontext,endpos1,last_match,endpos2,args_cd))return false;
	args += args_cd;
  }
  if (direction == eForward)
    endpos = endpos1;
  else
    endpos = pos;
  return true;
}

bool
LsysRule::applyTo( AxialTree& dest, 
				   const boost::python::list& args, 
				   size_t * length,
				   eDirection direction) const {
  object result = apply(args);
  if(result == object())return false;
  else {
    AxialTree prod = extract<AxialTree>(result)();
	if (prod.size() == 1 && prod[0].getClass() == ModuleClass::Star){ 
		if(length!=NULL)*length = 0;
	}
	else {
		if(length!=NULL)*length = prod.size();
		if(direction == eForward) dest += prod;
		else dest.prepend(prod);
	}
  }
  return true;
}


AxialTree
LsysRule::process( const AxialTree& src ) const {
  AxialTree dest;
  AxialTree::const_iterator _it = src.begin();
  while(_it != src.end()){
	boost::python::list args;
	if(!match(src,_it,dest,_it,args)){
	  dest.push_back(_it);
	  ++_it;
	}
	else { applyTo(dest,args); }
  }
  return dest;
}

/*---------------------------------------------------------------------------*/
