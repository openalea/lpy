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


#include "module.h"
#include "patternmodule.h"
#include "matching.h"
#include "lsyscontext.h"
#include "lpy_parser.h"
#include "tracker.h"
#include "packedargs.h"
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
    __mclass(ModuleClassTable::get().find(classid))
{ IncTracker(Module) }

Module::Module(const ModuleClassPtr m):
    __mclass(m)
{ IncTracker(Module) }

Module::~Module()
{ 
	DecTracker(Module)
	//  std::cerr << "Delete Module" << std::endl;
}


std::string 
Module::str() const 
{ return name(); }


std::string 
Module::repr() const 
{ return name(); }


/*---------------------------------------------------------------------------*/

boost::python::object LPY::getFunctionRepr() { return GlobalContext::getFunctionRepr(); }

/*---------------------------------------------------------------------------*/

// #include <iostream>

#define appendParam(args,p) args.push_back(p)

/*---------------------------------------------------------------------------*/

void processArgList(ModuleClassPtr mclass, ParamModule::ParameterList& args, boost::python::object arg, size_t start = 0){
    extract<boost::python::dict> isdict(arg);
    if (!isdict.check()){
        object iter_obj = object( handle<>( PyObject_GetIter( arg.ptr() ) ) );
        for(size_t i = 0; i < start; ++i) iter_obj.attr( "next" )();
        try { while( true ) appendParam(args,iter_obj.attr( "next" )()); }
        catch( error_already_set ){ PyErr_Clear(); }
    }
    else {
        boost::python::object iter_obj =  isdict().iteritems();
        size_t nbstdparam = args.size();
        if (nbstdparam + len(arg) < mclass->getNamedParameterNb()){
                std::stringstream str;
                str << mclass->name << " takes exactly " << mclass->getNamedParameterNb()<< " (" << nbstdparam + len(arg) << " given)";
                LsysError(str.str());                
        }
        pgl_hash_set<size_t> missingargs;

        while( true )
        {
            boost::python::object obj; 
            try {  obj = iter_obj.attr( "next" )(); }
            catch( boost::python::error_already_set ){ PyErr_Clear(); break; }

            std::string pname = extract<std::string>( obj[0] )();
            size_t pposition = mclass->getParameterPosition(pname);
            if (pposition == ModuleClass::NOPOS) {
                std::stringstream str;
                str << "Invalid parameter name '" << pname << "' for module '" << mclass->name << "'.";
                LsysError(str.str());                
            }
            else if (pposition < nbstdparam) {
                std::stringstream str;
                str << mclass->name << " got multiple values for parameter '" << pname << "'.";
                LsysError(str.str());                
            }
            else {
                size_t nbactualparam = args.size();
                if(nbactualparam > pposition) {
                    args[pposition] = obj[1];
                    pgl_hash_set<size_t>::const_iterator itmarg = missingargs.find(pposition);
                    if (itmarg != missingargs.end())
                        missingargs.erase(itmarg);
                }
                else {
                    for (size_t i = nbactualparam ; i < pposition; ++i ){
                        appendParam(args,object());
                        missingargs.insert(i);
                    }
                    appendParam(args,obj[1]);
                }
            }
        }
        if (missingargs.size() > 0) {
                std::stringstream str;
                str << mclass->name << " takes exactly " << mclass->getNamedParameterNb()<< " (" << missingargs.size() << " missing)";
                LsysError(str.str());                
        }
    }
}

void processLastArg(ModuleClassPtr mclass, ParamModule::ParameterList& args, boost::python::object arg){
	extract<PackedArgs> pka(arg);
	if(pka.check()){ 
		processArgList(mclass,args,pka().args);
	}
	else { appendParam(args,arg); }
}

void processConstruction(ParamModule& module, 
					ParamModule::ParameterList& args, 
					boost::python::object arg, size_t start = 0){
  if(module.getClass() == ModuleClass::New)
  {
	  module.setName(extract<std::string>(arg[start]));
	  start += 1;
  }
  size_t l = len(arg);
  if (l > 0) {
      args.reserve(l);

      for(size_t i = start; i < l-1; ++i){
        appendParam(args,arg[i]); 
      }
      if(l > start){processLastArg(module.getClass(), args,arg[l-1]);}
  }
}


/*---------------------------------------------------------------------------*/

ParamModule::ParamModule():
BaseType() {}

ParamModule::ParamModule(size_t classid):
    BaseType(classid) {}

ParamModule::ParamModule(const std::string& name) :
    BaseType() 
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
	  if(o != object()) processConstruction(*this,__args(),extract<list>(o)());
	}
  }
}

ParamModule::ParamModule(size_t classid, const std::string& args):
    BaseType(classid) 
{
	if (!args.empty()){
      object o = LsysContext::currentContext()->evaluate('['+args+']');
	  if(o != object()){
		processConstruction(*this,__args(),extract<list>(o)());
	  }
	}
}


ParamModule::ParamModule(const ParamModule& mod):
BaseType(mod)  {}

ParamModule::ParamModule(const std::string& name, const boost::python::list& arg):
BaseType(name) 
{ processConstruction(*this,__args(),arg); }

ParamModule::ParamModule(size_t classid, const boost::python::list& arg):
BaseType(classid) 
{ processConstruction(*this,__args(),arg); }



ParamModule::ParamModule(const ModuleClassPtr m, 
              const boost::python::tuple& args):
    BaseType(m) 
{ processConstruction(*this,__args(),args); }


ParamModule::ParamModule(boost::python::tuple t):
 BaseType() 
{  
  extract<size_t> id_extract(t[0]);
  if (id_extract.check()) setClass(id_extract());
  else setName(extract<std::string>(t[0]));
  processConstruction(*this,__args(),t,1);
}


ParamModule::ParamModule(boost::python::list t):
 BaseType() 
{ 
  extract<size_t> id_extract(t[0]);
  if (id_extract.check())setClass(id_extract());
  else setName(extract<std::string>(t[0]));
  processConstruction (*this,__args(),t,1);
}

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a):
BaseType(name) { processLastArg(getClass(),__args(),a); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b):
BaseType(name) { appendParam(__args(),a); processLastArg(getClass(), __args(),b); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b,
						 const boost::python::object& c):
BaseType(name) 
{ appendParam(__args(),a); appendParam(__args(),b); processLastArg(getClass(),__args(),c); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b,
						 const boost::python::object& c,
						 const boost::python::object& d):
BaseType(name) 
 { appendParam(__args(),a); appendParam(__args(),b); 
			  appendParam(__args(),c); processLastArg(getClass(), __args(),d); }

ParamModule::ParamModule(const std::string& name, 
						 const boost::python::object& a,
						 const boost::python::object& b,
						 const boost::python::object& c,
						 const boost::python::object& d,
						 const boost::python::object& e):
BaseType(name)
{ appendParam(__args(),a); appendParam(__args(),b); 
  appendParam(__args(),c); appendParam(__args(),d);
  processLastArg(getClass(), __args(),e); }



ParamModule::~ParamModule() 
{ 
}

void ParamModule::appendArgumentList(const boost::python::object& arglist) 
{
    processArgList(getClass(),__args(),arglist);
}



int 
ParamModule::_getInt(int i) const 
{ 
	const ParameterList& p = __constargs();
	assert(p.size() > i);
    extract<int> ext(p[i]); 
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
	const ParameterList& p = __constargs();
	assert(p.size() > i);
   extract<real_t> ext(p[i]); 
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for float.";
        LsysError(str.str());
    }         
    return ext();
}

bool ParamModule::_getBool(int i) const
{
	const ParameterList& p = __constargs();
	assert(p.size() > i);
    extract<bool> ext(p[i]); 
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for bool.";
        LsysError(str.str());
    }         
    return ext();
}


std::string 
ParamModule::_getString(int i) const 
{ 
	const ParameterList& p = __constargs();
	assert(p.size() > i);
	extract<char const*>ext(p[i]);
    if (!ext.check()){
        std::stringstream str;
        str << "Invalid type for " << i << "th parameter in module '" << name() << "'. Looking for string.";
        LsysError(str.str());
    }
	char const* c_str = ext();
	if (!c_str) 
		return std::string("");
    else return std::string(c_str);
}

void 
ParamModule::_setValues(real_t x,real_t y,real_t z)
{ 
  ParameterList& args = __args();
  size_t nbArg = args.size();
  if (nbArg > 3) nbArg = 3;
  switch(nbArg){
  case 3:
	args[2] = object(z);
  case 2:
	args[1] = object(y);
	args[0] = object(x);
	break;
  case 1:
      {
       extract<float> ext(args[0]); 
        if (ext.check()){
            args[0] = object(x);
        }
        else {
            int size = len(args[0]);
            if (size > 0)args[0].attr("__setitem__")(0,x);
            if (size > 1)args[0].attr("__setitem__")(1,y);
            if (size > 2)args[0].attr("__setitem__")(2,z);
        }
	    break;
      }
  case 0:
      appendParam(args,object(TOOLS(Vector3(x,y,z))));
	// appendParam(__args,object(y));
	// appendParam(__args,object(z));
  default :
	break;
  }
}

void ParamModule::_setFrameValues(const TOOLS(Vector3)& p, const TOOLS(Vector3)& h, 
								  const TOOLS(Vector3)& u, const TOOLS(Vector3)& l)
{
  ParameterList& args = __args();
  size_t nbArg = args.size();
  if(nbArg >= 1 )args[0] = object(p);
  else args.push_back(object(p));
  if(nbArg >= 2 )args[1] = object(h);
  else args.push_back(object(h));
  if(nbArg >= 3 )args[2] = object(u);
  else args.push_back(object(u));
  if(nbArg >= 4 )args[3] = object(l);
  else args.push_back(object(l));
}

/*---------------------------------------------------------------------------*/

bool ParamModule::match(const std::string& _name, size_t nbargs) const
{ return name() == _name && argSize() == nbargs; }

bool ParamModule::match(const PatternModule& pattern) const 
{ ArgList l; return MatchingEngine::module_match(*this,pattern,l); }

bool ParamModule::match(const PatternModule& pattern, ArgList& l) const 
{ return MatchingEngine::module_match(*this,pattern,l); }

