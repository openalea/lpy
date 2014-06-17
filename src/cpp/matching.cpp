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

#include "matching.h"
#include "patternmodule.h"
#include "axialtree_manip.h"
#include "matching_tmpl.h"
#include <boost/python.hpp>
#include "argcollector_core.h"

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
MatchingEngine::eModuleMatchingMethod 
MatchingEngine::ModuleMatchingMethod = MatchingEngine::eDefaultModuleMatching;

MatchingEngine::ModuleMatchingFuncType MatchingEngine::ModuleMatchingFunc = &MatchingImplementation::module_matching_with_star_and_valueconstraints;


void MatchingEngine::setModuleMatchingMethod(MatchingEngine::eModuleMatchingMethod method)
{ 
	ModuleMatchingMethod = method; 
	switch(ModuleMatchingMethod){
	case eMSimple:
		ModuleMatchingFunc = &MatchingImplementation::simple_module_matching; break;
	case eMWithStar:
		ModuleMatchingFunc = &MatchingImplementation::module_matching_with_star; break;
	default:
		ModuleMatchingFunc = &MatchingImplementation::module_matching_with_star_and_valueconstraints; break;
	}
}

MatchingEngine::eModuleMatchingMethod LPY::MatchingEngine::getModuleMatchingMethod()
{ return ModuleMatchingMethod; }


MatchingEngine::eStringMatchingMethod 
MatchingEngine::StringMatchingMethod = MatchingEngine::eDefaultStringMatching;

MatchingEngine::RightMatchingFuncType MatchingEngine::RightMatchingFunc = &MatchingImplementation::tree_right_match;
MatchingEngine::LeftMatchingFuncType MatchingEngine::LeftMatchingFunc   = &MatchingImplementation::tree_left_match;

void MatchingEngine::setStringMatchingMethod(MatchingEngine::eStringMatchingMethod method)
{ 
	StringMatchingMethod = method; 
	switch(StringMatchingMethod){
		case eString:
			RightMatchingFunc = &MatchingImplementation::string_right_match /*StringMatcher<GetNext>::match*/;
			LeftMatchingFunc = &MatchingImplementation::string_left_match ; break; /*StringReverseMatcher<GetPrevious>::match*/
		case eMScaleAxialTree:
			RightMatchingFunc = &MatchingImplementation::mstree_right_match; 
			LeftMatchingFunc = &MatchingImplementation::mstree_left_match; break;
		case eMLevelAxialTree:
			RightMatchingFunc = &MatchingImplementation::mltree_right_match; 
			LeftMatchingFunc = &MatchingImplementation::mltree_left_match; break;
		case eAxialTree:
		default:
			RightMatchingFunc = &MatchingImplementation::tree_right_match;
			LeftMatchingFunc = &MatchingImplementation::tree_left_match; break;
	}
}

MatchingEngine::eStringMatchingMethod LPY::MatchingEngine::getStringMatchingMethod()
{ return StringMatchingMethod; }

/*---------------------------------------------------------------------------*/


bool MatchingEngine::module_match(const ParamModule& module, 
								  const PatternModule& pattern,
								  ArgList& params) 
{
	return (*ModuleMatchingFunc)(module,pattern,params);

}


bool MatchingEngine::match(AxialTree::const_iterator  matching_start,
						   AxialTree::const_iterator  string_begin,
						   AxialTree::const_iterator  string_end,
						   PatternString::const_iterator  pattern_begin,
						   PatternString::const_iterator  pattern_end,
						   AxialTree::const_iterator& matching_end,
						   AxialTree::const_iterator& last_matched,
						   ArgList& params) 
{
	return MatchingImplementation::string_exact_match(matching_start,string_begin,string_end,
						                              pattern_begin,pattern_end,
						                              matching_end,last_matched, params);
}

bool MatchingEngine::reverse_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   PatternString::const_reverse_iterator  pattern_rbegin,
								   PatternString::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	return MatchingImplementation::string_exact_reverse_match(matching_start,string_begin,string_end,
						                               pattern_rbegin,pattern_rend,
						                               matching_end,params);
}

bool MatchingEngine::right_match(AxialTree::const_iterator  matching_start,
  							     AxialTree::const_iterator  string_begin,
								 AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
								 AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return (*RightMatchingFunc)(matching_start,string_begin,string_end,
						        pattern_begin,pattern_end,
						        last_matched, matching_end,params);
/*	switch(StringMatchingMethod){
		case eString:
				return StringMatcher<GetNext>::
				match(matching_start, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
		case eMScaleAxialTree:
			return MatchingImplementation::mstree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              last_matched, matching_end,params);
		case eMLevelAxialTree:
			return MatchingImplementation::mltree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              last_matched, matching_end,params);
		case eAxialTree:
		default:
			return MatchingImplementation::tree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              last_matched, matching_end,params);
	}*/
}

bool MatchingEngine::left_match(AxialTree::const_iterator  matching_start,
								AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								PatternString::const_reverse_iterator  pattern_rbegin,
								PatternString::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params) 
{
	return (*LeftMatchingFunc)(matching_start,string_begin,string_end,pattern_rbegin,pattern_rend,matching_end,params);
/*	switch(StringMatchingMethod){
		case eString:
				return StringReverseMatcher<GetPrevious>::
				match(matching_start,string_begin,string_end,pattern_rbegin,pattern_rend,matching_end,params);
		case eMScaleAxialTree:
			return MatchingImplementation::mstree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
		case eMLevelAxialTree:
			return MatchingImplementation::mltree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
		case eAxialTree:
		default:
			return MatchingImplementation::tree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
	}*/
}

/*---------------------------------------------------------------------------*/


inline bool same_name(const ParamModule& module, const PatternModule& pattern){
	return module.sameName(pattern);
}

inline bool isinstance(const ParamModule& module, const PatternModule& pattern){
	return module.isinstance(pattern.getClass());
}

typedef bool (*COMPAREMODULE)(const ParamModule&, const PatternModule&);
static COMPAREMODULE compatibleName = &same_name;


inline bool same_class(const ModuleClassPtr& module, const ModuleClassPtr& pattern){
	return module == pattern;
}

inline bool issubclass(const ModuleClassPtr& module, const ModuleClassPtr& pattern){
	// printf("issubclass(%s,%s) == %s\n",module->name.c_str(),pattern->name.c_str(),(module->issubclass(pattern)?"True":"False"));
	return module->issubclass(pattern);
}

typedef bool (*COMPAREMODULECLASS)(const ModuleClassPtr&, const ModuleClassPtr&);
static COMPAREMODULECLASS compatibleClass = &same_class;

static bool INHERITEDCLASSCOMPARISON = false;

void MatchingEngine::setInheritanceModuleMatchingActivated(bool b)
{ 
	if(INHERITEDCLASSCOMPARISON != b){
		if (b) { 
			compatibleName = &isinstance; 
			compatibleClass = &issubclass;
		}
		else { 
			compatibleName = &same_name;  
			compatibleClass = &same_class;  
		}
		INHERITEDCLASSCOMPARISON = b;
	}
}

bool MatchingEngine::compatible_classes(const ModuleClassPtr& module, 
							 const ModuleClassPtr& pattern)
{
	if (pattern == ModuleClass::Star) return true;
	return compatibleClass(module,pattern);
}



bool MatchingEngine::isInheritanceModuleMatchingActivated()
{ return INHERITEDCLASSCOMPARISON; }

/*---------------------------------------------------------------------------*/

bool 
MatchingImplementation::simple_module_matching(const ParamModule& module, 
													 const PatternModule& pattern, 
													 ArgList& l)
{
  if( compatibleName(module,pattern) && module.argSize() == pattern.argSize()){
	  ArgsCollector::append_modargs(l,module.getParameterList());
	return true;
  }
  return false;
}

bool MatchingImplementation::module_matching_with_star(const ParamModule& module, 
													   const PatternModule& pattern, 
									                   ArgList& l)
{
  if (pattern.isStar()){
	size_t patternsize = pattern.argSize();
	if (patternsize == 0) return true;
	if (patternsize == 1) {
	  size_t modulesize = module.argSize();
	  LsysVar v = pattern.getAt(0);
	  if(v.isArgs()) { 
		  ArgList arg;
		  ArgsCollector::append_arg(arg,boost::python::object(module.name()));
		  ArgsCollector::append_modargs(arg,module.getParameterList());
		  ArgsCollector::append_as_arg(l,arg);
		  return true; 
	  }
	  else if(v.isKwds()) { 
		  if( module.getNamedParameterNb() != module.size()) {
			  return false;
		  }
		  boost::python::dict largs;
		  largs[boost::python::object("name")] = boost::python::object(module.name());
		  module.getNamedParameters(largs);
		  ArgsCollector::append_arg(l,largs);
		  return true; 
	  }
	  else {
		if(modulesize == 0){ ArgsCollector::append_arg(l,boost::python::object(module.name()));return true; }
	    else return false;
	  }
	}
	else {
	  size_t modulesize = module.size();
	  if (modulesize + 3 < patternsize) return false; // name,args,kwd can be extra args

	  LsysVar vlast = pattern.getAt(patternsize-1);
	  LsysVar vbeforelast = pattern.getAt(patternsize-2);

      bool vlastisArgs = vlast.isArgs();
      bool vbeforelastisArgs = vbeforelast.isArgs();
      bool vlastisKwds = vlast.isKwds();

	  if(!vlastisArgs && !vlastisKwds){
  		if (modulesize != patternsize-1) return false;
		ArgsCollector::append_arg(l,boost::python::object(module.name()));
		ArgsCollector::append_modargs(l,module.getParameterList());
		return true;
	  }


	  if (patternsize == 2 && vlastisKwds && vbeforelastisArgs){ // (**kwds,*args)
		  size_t nbNamedParameter = module.getNamedParameterNb();
		  if(nbNamedParameter > modulesize )  return false;
		  ArgsCollector::append_arg(l,module.getSlice(0,modulesize-nbNamedParameter));
		  
		  boost::python::dict largs;
		  largs[boost::python::object("name")] = boost::python::object(module.name());
		  module.getNamedParameters(largs);
          ArgsCollector::append_arg(l,largs);
		  return true;
	  }


	  size_t normalparam = patternsize - 1;
      if (vlastisArgs || vlastisKwds) --normalparam;
	  if (vbeforelastisArgs) --normalparam;

	  // check if normal number of parameter to retrieve is compatible
	  if( normalparam > modulesize) return false;

	  // retrieve name
	  ArgsCollector::append_arg(l,boost::python::object(module.name()));
	  // retrieve normal parameters
	  for(int i = 0; i < normalparam; i++)
		ArgsCollector::append_arg_ref(l,module.getAt(i));

      if (vbeforelastisArgs || vlastisArgs){
          if ( normalparam == 0 && !vlastisKwds) // (name,*args)
              ArgsCollector::append_arg(l,module.getPyArgs());          
          else if (normalparam == modulesize) // (name,x,*args) on A(1)
              ArgsCollector::append_arg(l,boost::python::list());
          else {
              int lastarrayarg = modulesize;
              if (vlastisKwds) {
                    lastarrayarg = std::max(normalparam, modulesize-module.getNamedParameterNb()) ;
              }
              if (lastarrayarg <= normalparam) 
                  ArgsCollector::append_arg(l,boost::python::list());
              else 
                  ArgsCollector::append_arg(l,module.getSlice(normalparam,lastarrayarg));
          }

      }

	  if (vlastisKwds){
          if (! vbeforelastisArgs && (module.getNamedParameterNb()+normalparam < modulesize) ) return false;
		  boost::python::dict largs;
          int startkwd = std::max<int>(0,normalparam - std::max<int>(0, modulesize-module.getNamedParameterNb()));

		  module.getNamedParameters(largs, startkwd);
		  ArgsCollector::append_arg(l,largs);
	  }


	  return true;
	}
  }
  else {
	if (!compatibleName(module,pattern)) return false;
	size_t patternsize = pattern.argSize();
    size_t modulesize = module.argSize();
	if (modulesize + 2 < patternsize) return false;

	if( patternsize == 0) return modulesize == 0;
	else if(patternsize == 1) {
	  LsysVar v = pattern.getAt(0);
	  if(v.isArgs()) { 
		ArgsCollector::append_arg(l,module.getPyArgs());
		return true; 
	  }
	  else if(v.isKwds()) { 
		  if( module.getNamedParameterNb() != module.size()) {
			  return false;
		  }
		  boost::python::dict largs;
		  module.getNamedParameters(largs);
		  ArgsCollector::append_arg(l,largs);
		  return true; 
	  }
	  else {
		  if(modulesize != 1) return false;
		  ArgsCollector::append_arg_ref(l,module.getAt(0));
		  return true;
	  }
	}
	else { // args >= 2

	  LsysVar vlast = pattern.getAt(patternsize-1);
	  LsysVar vbeforelast = pattern.getAt(patternsize-2);

      bool vlastisArgs = vlast.isArgs();
      bool vbeforelastisArgs = vbeforelast.isArgs();
      bool vlastisKwds = vlast.isKwds();

	  if(!vlastisArgs && !vlastisKwds){
  		if (modulesize != patternsize) return false;
		ArgsCollector::append_modargs(l,module.getParameterList());
		return true;
	  }


	  if (patternsize == 2 && vbeforelastisArgs && vlastisKwds){ // (*args,**kwds)
		  size_t nbNamedParameter = module.getNamedParameterNb();
		  if(nbNamedParameter > modulesize )  return false;
		  boost::python::dict largs;
		  module.getNamedParameters(largs);
          ArgsCollector::append_arg(l,module.getSlice(0,modulesize-nbNamedParameter));
		  ArgsCollector::append_arg(l,largs);
		  return true;
	  }


	  size_t normalparam = patternsize ;
	  if (vbeforelastisArgs) --normalparam;
	  if (vlastisArgs || vlastisKwds) --normalparam;

	  if( normalparam > modulesize) {
        return false;
      }
	  // retrieve normal parameters
	  for(int i = 0; i < normalparam; i++)
		ArgsCollector::append_arg_ref(l,module.getAt(i));

      if (vbeforelastisArgs || vlastisArgs){
          if (normalparam == modulesize ) // (x,*args) on A(1)
              ArgsCollector::append_arg(l,boost::python::list());
          else {
              int lastarrayarg = modulesize;
              if (vlastisKwds) {
                    lastarrayarg = std::max(normalparam, modulesize-module.getNamedParameterNb()) ;
              }
              if (lastarrayarg <= normalparam) 
                  ArgsCollector::append_arg(l,boost::python::list());
              else 
                  ArgsCollector::append_arg(l,module.getSlice(normalparam,lastarrayarg));
          }
          //    ArgsCollector::append_arg(l,module.getSlice(normalparam,s2));

      }

      if (vlastisKwds){
          if (! vbeforelastisArgs && (module.getNamedParameterNb()+normalparam < modulesize) ) {
                return false;
          }          
		  boost::python::dict largs;
          int startkwd = std::max<int>(0,normalparam - std::max<int>(0, modulesize-module.getNamedParameterNb()));
		  module.getNamedParameters(largs,startkwd);
		  ArgsCollector::append_arg(l,largs);
		  // normalparam = module.getNamedParameterNb();
	  }
      return true;

	}
  }
}

bool MatchingImplementation::module_matching_with_star_and_valueconstraints(
										  const ParamModule& module, 
									      const PatternModule& pattern, 
									      ArgList& l)
{ 
  if (pattern.isStar()){
	size_t patternsize = pattern.argSize();
	if (patternsize == 0) return true;
	if (patternsize == 1) {
	    size_t modulesize = module.argSize();
	    const LsysVar& v = pattern.getAt(0);
	    if(v.isArgs()) { 
			ArgList largs;
			if(v.isNamed() || v.hasCondition()){ // if necessary we retrieve the args
				ArgsCollector::append_arg(largs,bp::object(module.name()));
				ArgsCollector::append_modargs(largs,module.getParameterList()); 
			}
            // we check the condition
			if(!v.isCompatible(toPyList(largs))) return false;
            // we add the params to the result list
			if(v.isNamed()) ArgsCollector::append_as_arg(l,largs); 
			return true; 
	    }
        else if(v.isKwds()) { 
            if( module.getNamedParameterNb() != module.size()) {
              return false;
            }
            boost::python::dict lkwds;
            if(v.isNamed() || v.hasCondition()){ // if necessary we retrieve the args
                lkwds[boost::python::object("name")] = boost::python::object(module.name());
                module.getNamedParameters(lkwds);
            }
            // we check the condition
            if(!v.isCompatible(lkwds))return false;
            // we add the params to the result list
            if(v.isNamed())  ArgsCollector::append_arg(l,lkwds);
            return true; 
        }
      	else {
		  if(modulesize == 0){ 
             // we get the name
			 boost::python::object largs(module.name()); 
             // we check the condition
			 if (!v.isCompatible(largs)) return false;
             // we add the params to the result list
			 if (v.isNamed()) ArgsCollector::append_arg(l,largs); 
			 return true; 
		  }
		  else return false;
	    }
	}
	else { // patternsize > 2
	  size_t modulesize = module.argSize();
      if (modulesize + 3 < patternsize) return false; // name,args,kwd can be extra args

      const LsysVar& vlast = pattern.getAt(patternsize-1);
      const LsysVar& vbeforelast = pattern.getAt(patternsize-2);

      bool vlastisArgs = vlast.isArgs();
      bool vbeforelastisArgs = vbeforelast.isArgs();
      bool vlastisKwds = vlast.isKwds();

      if(!vlastisArgs && !vlastisKwds){
        if (modulesize != patternsize-1) return false;
      }


      if (patternsize == 2 && vlastisKwds && vbeforelastisArgs){ // (**kwds,*args)
          size_t nbNamedParameter = module.getNamedParameterNb();
          if(nbNamedParameter > modulesize)  return false;
          boost::python::list largs;
          if (vbeforelast.isNamed() || vbeforelast.hasCondition()) {
            largs = module.getSlice(0,modulesize-nbNamedParameter);
          }
          if (!vbeforelast.isCompatible(largs))return false;
          if (vbeforelast.isNamed()) ArgsCollector::append_arg(l,largs);

          boost::python::dict lkwd;
          if (vlast.isNamed() || vlast.hasCondition()) {
              lkwd[boost::python::object("name")] = boost::python::object(module.name());
              module.getNamedParameters(lkwd);
          }
          if (!vlast.isCompatible(lkwd)) return false;
          if (vlast.isNamed()) ArgsCollector::append_arg(l,lkwd);
          return true;
      }

      size_t normalparam = patternsize - 1;
      if (vlastisArgs || vlastisKwds) --normalparam;
      if (vbeforelastisArgs) --normalparam;

      // check if normal number of parameter to retrieve is compatible
      if( normalparam > modulesize) return false;



      // processing module name
	  const LsysVar& v1 = pattern.getAt(0);
      if (v1.isNamed() || v1.hasCondition()) {
        boost::python::object mname(module.name());
        if (!v1.isCompatible(mname))return false;
        if (v1.isNamed()) ArgsCollector::append_arg(l,mname);
      }


      // retrieve normal parameters
      for(int i = 0; i < normalparam; i++) {
        const LsysVar& v = pattern.getAt(i+1);
        if(!v.isCompatible(module.getAt(i))) return false; 
        if(v.isNamed())ArgsCollector::append_arg_ref(l,module.getAt(i));
      }

      // retrieve *args
      if (vbeforelastisArgs || vlastisArgs){
          const LsysVar& varg = (vlastisArgs?vlast:vbeforelast);
          boost::python::list argvalue;
          if (varg.isNamed() || varg.hasCondition()) {
              if ( normalparam == 0 && !vlastisKwds) // (name,*args)
                  argvalue = module.getPyArgs();          
              else if (normalparam == modulesize) // (name,x,*args) on A(1)
                  argvalue = boost::python::list();
              else {
                  int lastarrayarg = modulesize;
                  if (vlastisKwds) {
                        lastarrayarg = std::max(normalparam, modulesize-module.getNamedParameterNb()) ;
                  }
                  if (lastarrayarg <= normalparam) 
                      argvalue = boost::python::list();
                  else 
                      argvalue = module.getSlice(normalparam,lastarrayarg);
              }
          }
          if(!varg.isCompatible(argvalue)) return false; 
          if (varg.isNamed())ArgsCollector::append_arg(l,argvalue);

      }

      // retrieve **kwds
      if (vlastisKwds){
          if (! vbeforelastisArgs && (module.getNamedParameterNb()+normalparam < modulesize) ) return false;

          boost::python::dict kwdvalue;
          if (vlast.isNamed() || vlast.hasCondition()) {
            int startkwd = std::max<int>(0,normalparam - std::max<int>(0, modulesize-module.getNamedParameterNb()));
            module.getNamedParameters(kwdvalue, startkwd);
          }
          if(!vlast.isCompatible(kwdvalue)) return false; 
          if (vlast.isNamed())ArgsCollector::append_arg(l,kwdvalue);
      }


      return true;


/*
	  bool lastarg = false;
	  boost::python::object lastargval;
	  const LsysVar& v = pattern.getAt(patternsize-1);
	  if(!v.isNamed()){
		  if (modulesize != (patternsize-1+beg)) return false;
		  if (!v.isCompatible(module.getAt(patternsize-2+beg)))return false;
	  }
	  else {
		lastarg = true;
		if(beg == 1){
		  if(!v.isArgs()){
			if (modulesize != patternsize) return false;
			lastargval = module.getAt(patternsize-1);
		  }
		  else {
			if (modulesize < patternsize-1) return false;
			else if (modulesize == patternsize-1) lastargval = boost::python::list();
			lastargval = module.getSlice(patternsize-1,modulesize);
		  }
		}
		else {
		  if(!v.isArgs()){
			if (modulesize != patternsize-1) return false;
			lastargval = module.getAt(patternsize-2);
		  }
		  else {
			if (modulesize < patternsize-2) return false;
			else {
			  if(patternsize == 2){
				  lastargval = module.getPyArgs();
				  if(!v.isCompatible(lastargval))return false;
				  ArgsCollector::append_arg(l,lastargval); 
                  return true; 
			  }
			  else if (modulesize == patternsize-2) lastargval = boost::python::list();
			  else lastargval = module.getSlice(patternsize-2,modulesize);
			}
		  }
		}
		if(!v.isCompatible(lastargval))return false;
	  }
	  for(size_t i = 1; i < patternsize-1; i++){
	    const LsysVar& v = pattern.getAt(i);
	    if(!v.isCompatible(module.getAt(i-1+beg))) return false; 
		if(v.isNamed())ArgsCollector::append_arg_ref(l,module.getAt(i-1+beg));
	  }
	  if(lastarg)ArgsCollector::append_arg(l,lastargval);
	  return true;
	*/}
  }
  else { // pattern is not star

    if (!compatibleName(module,pattern)) return false;
    size_t patternsize = pattern.argSize();
    size_t modulesize = module.argSize();
    if (modulesize + 2 < patternsize) return false;

    if( patternsize == 0) return modulesize == 0;
    else if(patternsize == 1) {
      LsysVar v = pattern.getAt(0);
      if(v.isArgs()) { 
            ArgList largs;
            if(v.isNamed() || v.hasCondition()){ // if necessary we retrieve the args
                ArgsCollector::append_modargs(largs,module.getParameterList()); 
            }
            // we check the condition
            if(!v.isCompatible(toPyList(largs))) return false;
            // we add the params to the result list
            if(v.isNamed()) ArgsCollector::append_as_arg(l,largs); 
            return true; 
      }
      else if(v.isKwds()) { 
            if( module.getNamedParameterNb() != module.size()) return false;
            boost::python::dict lkwds;
            if(v.isNamed() || v.hasCondition()){ // if necessary we retrieve the args
                module.getNamedParameters(lkwds);
            }
            // we check the condition
            if(!v.isCompatible(lkwds))return false;
            // we add the params to the result list
            if(v.isNamed())  ArgsCollector::append_arg(l,lkwds);
            return true; 
      }
      else {
          if(modulesize != 1) return false;
          // we check the condition
          if (!v.isCompatible(module.getAt(0))) return false;
          // we add the params to the result list
          if (v.isNamed()) ArgsCollector::append_arg_ref(l,module.getAt(0));
          return true;
      }
    }
    else { // args >= 2

      LsysVar vlast = pattern.getAt(patternsize-1);
      LsysVar vbeforelast = pattern.getAt(patternsize-2);

      bool vlastisArgs = vlast.isArgs();
      bool vbeforelastisArgs = vbeforelast.isArgs();
      bool vlastisKwds = vlast.isKwds();

      if(!vlastisArgs && !vlastisKwds){
        if (modulesize != patternsize) return false;
      }

      if (patternsize == 2 && vbeforelastisArgs && vlastisKwds){ // (*args,**kwds)
          size_t nbNamedParameter = module.getNamedParameterNb();
          if(nbNamedParameter > modulesize)  return false;
          boost::python::list largs;
          if (vbeforelast.isNamed() || vbeforelast.hasCondition()) {
            largs = module.getSlice(0,modulesize-nbNamedParameter);
          }
          if (!vbeforelast.isCompatible(largs))return false;
          if (vbeforelast.isNamed()) ArgsCollector::append_arg(l,largs);

          boost::python::dict lkwd;
          if (vlast.isNamed() || vlast.hasCondition()) {
              module.getNamedParameters(lkwd);
          }
          if (!vlast.isCompatible(lkwd)) return false;
          if (vlast.isNamed()) ArgsCollector::append_arg(l,lkwd);
          return true;
      }


      size_t normalparam = patternsize ;
      if (vbeforelastisArgs) --normalparam;
      if (vlastisArgs || vlastisKwds) --normalparam;

      if( normalparam > modulesize)  return false;

      // retrieve normal parameters
      for(int i = 0; i < normalparam; i++) {
        const LsysVar& v = pattern.getAt(i);
        if(!v.isCompatible(module.getAt(i))) return false; 
        if(v.isNamed())ArgsCollector::append_arg_ref(l,module.getAt(i));
      }

      // retrieve *args
      if (vbeforelastisArgs || vlastisArgs){
          const LsysVar& varg = (vlastisArgs?vlast:vbeforelast);
          boost::python::list argvalue;
          if (varg.isNamed() || varg.hasCondition()) {
              if ( normalparam == 0 && !vlastisKwds) // (name,*args)
                  argvalue = module.getPyArgs();          
              else if (normalparam == modulesize) // (name,x,*args) on A(1)
                  argvalue = boost::python::list();
              else {
                  int lastarrayarg = modulesize;
                  if (vlastisKwds) {
                        lastarrayarg = std::max(normalparam, modulesize-module.getNamedParameterNb()) ;
                  }
                  if (lastarrayarg <= normalparam) 
                      argvalue = boost::python::list();
                  else 
                      argvalue = module.getSlice(normalparam,lastarrayarg);
              }
          }
          if(!varg.isCompatible(argvalue)) return false; 
          if (varg.isNamed())ArgsCollector::append_arg(l,argvalue);
      }

      if (vlastisKwds){
          if (! vbeforelastisArgs && (module.getNamedParameterNb()+normalparam < modulesize) ) {
                return false;
          }          
          boost::python::dict kwdvalue;
          if (vlast.isNamed() || vlast.hasCondition()) {
            int startkwd = std::max<int>(0,normalparam - std::max<int>(0, modulesize-module.getNamedParameterNb()));
            module.getNamedParameters(kwdvalue, startkwd);
          }
          if(!vlast.isCompatible(kwdvalue)) return false; 
          if (vlast.isNamed())ArgsCollector::append_arg(l,kwdvalue);
      }
      return true;

    }
	/*if (!compatibleName(module,pattern)) return false;
	int s = pattern.argSize();
    int s2 = module.argSize();
	if( s == 0) return s2 == 0;
	else {
	  bool lastarg = false;
	  boost::python::object lastargval;
	  const LsysVar& v = pattern.getAt(s-1);
	  if(!v.isNamed()){
		if (s2 != s) return false;
		if (!v.isCompatible(module.getAt(s-1)))return false;
	  }
	  else {
		lastarg = true;
		if(!v.isArgs()){
		  if (s2 != s) return false; 
		  else lastargval = module.getAt(s-1);
		}
		else {
		  if (s2 < s - 1) return false;
		  if(s == 1){ 
			  lastargval =  module.getPyArgs();
			  if(!v.isCompatible(lastargval))return false; 
			  ArgsCollector::append_arg(l,lastargval); return true; 
		  }
		  else if (s2 == s - 1) lastargval = boost::python::list();
		  else lastargval = module.getSlice(s-1,s2);
		}
		if(!v.isCompatible(lastargval)) return false; 
	  }
	  for(size_t i = 0; i < s-1; i++){
		const LsysVar& v = pattern.getAt(i);
		if(!v.isCompatible(module.getAt(i))) return false; 
		if(v.isNamed())ArgsCollector::append_arg_ref(l,module.getAt(i));
	  }
	  if(lastarg)ArgsCollector::append_arg(l,lastargval);
	}
	return true;*/
  }
}

/*---------------------------------------------------------------------------*/


bool MatchingImplementation::string_exact_match(AxialTree::const_iterator  matching_start,
						   AxialTree::const_iterator  string_begin,
						   AxialTree::const_iterator  string_end,
						   PatternString::const_iterator  pattern_begin,
						   PatternString::const_iterator  pattern_end,
						   AxialTree::const_iterator& matching_end,
						   AxialTree::const_iterator& last_matched,
						   ArgList& params) 
{
	return StringMatcher<>::
		match(matching_start, string_begin, string_end, pattern_begin, pattern_end, matching_end, last_matched, params);
}

bool MatchingImplementation::string_exact_reverse_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   PatternString::const_reverse_iterator  pattern_rbegin,
								   PatternString::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	return StringReverseMatcher<>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}

bool MatchingImplementation::string_right_match(AxialTree::const_iterator  matching_start,
						         AxialTree::const_iterator  string_begin,
		                         AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
							     AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params)

{
	return StringMatcher<GetNext>::
		match(matching_start, string_begin, string_end, pattern_begin, pattern_end, matching_end, last_matched, params);
}

bool MatchingImplementation::string_left_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   PatternString::const_reverse_iterator  pattern_rbegin,
								   PatternString::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	matching_start = StringPrevious<AxialTree::const_iterator,PatternString::const_reverse_iterator>::next(matching_start,pattern_rbegin,string_begin, string_end);
	return StringReverseMatcher<>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}

bool MatchingImplementation::tree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_beg,
								 AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
						         AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return TreeRightMatcher<>::
		match(matching_start, string_beg, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);

}




bool MatchingImplementation::tree_left_match(AxialTree::const_iterator  matching_start,
								AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								PatternString::const_reverse_iterator  pattern_rbegin,
								PatternString::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								ArgList& params) 
{

	// matching_start = GetFather<AxialTree::const_iterator,PatternString::const_reverse_iterator>::next(matching_start,pattern_rbegin,string_begin, string_end);
	return // StringReverseMatcher<GetFather>:: // 
		TreeLeftMatcher<>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}


bool MatchingImplementation::mstree_left_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   PatternString::const_reverse_iterator  pattern_rbegin,
								   PatternString::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	// matching_start = GetScalePredecessor<AxialTree::const_iterator,PatternString::const_reverse_iterator>::next(matching_start,pattern_rbegin,string_begin, string_end);
	// return StringReverseMatcher<GetScalePredecessor>::match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
	return TreeLeftMatcher<GetScalePredecessor>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}

bool MatchingImplementation::mltree_left_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_iterator  string_end,
								   PatternString::const_reverse_iterator  pattern_rbegin,
								   PatternString::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   ArgList& params)
{ 
	// matching_start = GetLevelPredecessor<AxialTree::const_iterator,PatternString::const_reverse_iterator>::next(matching_start,pattern_rbegin,string_begin, string_end);
	// return StringReverseMatcher<GetLevelPredecessor>:: 
	return TreeLeftMatcher<GetLevelPredecessor>::
		match(matching_start, string_begin, string_end, pattern_rbegin, pattern_rend, matching_end, params);
}


bool MatchingImplementation::mstree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_beg,
								 AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
						         AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return TreeRightMatcher<GetScaleSuccessor>::
		match(matching_start, string_beg, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
}

bool MatchingImplementation::mltree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_beg,
								 AxialTree::const_iterator  string_end,
								 PatternString::const_iterator  pattern_begin,
								 PatternString::const_iterator  pattern_end,
						         AxialTree::const_iterator&  last_matched,
								 AxialTree::const_iterator& matching_end,
								 ArgList& params) 
{
	return TreeRightMatcher<GetLevelSuccessor>::
		match(matching_start, string_beg, string_end, pattern_begin, pattern_end, last_matched, matching_end, params);
}

LPY_END_NAMESPACE

