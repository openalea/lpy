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

#include "lsystem.h"
#include "lpy_parser.h"
#include <plantgl/tool/util_string.h>
#include <QtCore/QFileInfo>

using namespace boost::python;
TOOLS_USING_NAMESPACE
PGL_USING_NAMESPACE
PYLSYS_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

#ifdef MULTI_THREADED_LSYSTEM
#define ACQUIRE_RESSOURCE  LsysAcquirer ressource(this); 
#define RELEASE_RESSOURCE  
#else
#define ACQUIRE_RESSOURCE  
#define RELEASE_RESSOURCE
#endif




inline bool
has_pattern(std::string::const_iterator& pos,
	        std::string::const_iterator end,
			const std::string& pattern){
  size_t s = pattern.size();
  if (std::distance(pos,end) >= s && 
	  std::string(pos,pos+s) == pattern){
	pos+=s;
	return true;
  }
  else return false;
}

inline bool
has_pattern(const std::string& src,
			std::string::const_iterator& pos,
			const std::string& pattern){
 return has_pattern(pos,src.end(),pattern);
}


inline 
void ToEndlineCheckColon(const std::string& text,std::string::const_iterator& _it, std::string& filename, int& lineno){
    bool colon = false;
    while( _it!=text.end() && (*_it)!='\n') {
        ++_it;
        if ((*_it)!=':')colon = true;
    }
    if (!colon) LsysWarning("Colon missing.",filename,lineno);
    if(_it!=text.end() && (*_it)=='\n') { ++lineno; ++_it; /* std::cerr << 'b' << lineno << ':' << std::distance(text.begin(),_it) << std::endl; */ }
}


inline 
void ToEndline(const std::string& text,std::string::const_iterator& _it,int& lineno){
	while( _it!=text.end() && (*_it)!='\n') ++_it;
    if(_it!=text.end() && (*_it)=='\n') { ++lineno; ++_it; /* std::cerr << 'b' << lineno << ':' << std::distance(text.begin(),_it) << std::endl; */ }
}

inline 
void ToEndlineA(const std::string& text,std::string::const_iterator& _it,int& lineno){
    while( _it!=text.end() && (*_it)!='\n' && (*_it)!='A' && (*_it)!='n' ) ++_it;
    if(_it!=text.end() && ((*_it)=='\n' || (*_it)=='A' || (*_it)=='n')) 
    { 
        if((*_it)=='\n'){
            ++lineno;
//            std::cerr << 'c' << lineno << ':' << std::distance(text.begin(),_it)  << std::endl;
            ++_it; 
        }
    }
}

#define toendline(a,b) ToEndline(a,b,lineno)
#define toendlineA(a,b) ToEndlineA(a,b,lineno)
#define toendlineC(a,b) ToEndlineCheckColon(a,b,filename,lineno)

inline 
bool notOnlySpace(std::string::const_iterator beg, std::string::const_iterator end){
	while( beg!=end) 
        if ( (*beg)!=' ' && (*beg)!='\t'&& (*beg)!='\n') return true;
        else ++beg;
	return false;
}

#define PROCESS_RULE(rulecode,code,addedcode,mode,group) \
	LsysRule& r = __addRule(rulecode,mode,group,lineno - std::count(rule.begin(),rule.end(),'\n')); \
    code += r.getCoreCode(); \
    addedcode += r.getCallerCode(); \
	rule.clear(); \

#define LsysParserSyntaxError(msg) LsysSyntaxError(msg,filename,lineno)
#define LsysParserWarning(msg) LsysWarning(msg,filename,lineno)

#define WindowSpecificEndline 13

void 
Lsystem::set( const std::string&   _rules , bool debug){
  ACQUIRE_RESSOURCE
  std::string filename = getFilename();
  __clear();
  if(!filename.empty())setFilename(filename);
  filename = getShortFilename();
  ContextMaintainer m(&__context);
#ifndef _WIN32
  std::string _rules_ = _rules;
  for(std::string::iterator _itEr = _rules_.begin(); _itEr != _rules_.end(); ++_itEr)
  	if (*_itEr == WindowSpecificEndline) _rules_.erase(_itEr);
  const std::string& rules = _rules_;
#else
  const std::string& rules = _rules;
#endif
  std::string::const_iterator _it = rules.begin();
  std::string::const_iterator _it2 = _it;
  std::string::const_iterator beg = rules.begin();
  bool inmodulecode = false;
  std::string modulecode;
  std::string rule;
  std::string code;
  std::string addedcode;
  int axiom_lineno;
  int max_derivation_lineno;
  int decomposition_max_depth_lineno;
  int homomorphism_max_depth_lineno;
  int omode = -1;
  int mode = -1;
  int lineno = 1;
  int group = 0;
  while(_it!=rules.end()){
	switch(mode){
	case -1:
	  {
		switch(*_it){
		case 'A':
		  _it2 = _it;
		  if(has_pattern(rules,_it,"Axiom") && 
			 (_it2 == rules.begin() || *(_it2-1) == '\n')){
            code += std::string(beg,_it2);
			beg = _it;
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
              axiom_lineno = lineno;
              code += LsysContext::AxiomVariable + " = ";
              code += lstring2py(_it,rules.end(),'\n',lineno);
			}
            else LsysParserSyntaxError("Cannot find ':' after Axiom");
			beg = _it;
		  }
		  else if(has_pattern(rules,_it,"AxialTree")){
			code += std::string(beg,_it)+'(';
			while (_it!=rules.end()&&*_it!='(')_it++;
			if(_it!=rules.end()){
			  _it++;
			  code += lstring2py(_it,rules.end(),')',lineno);
			  if(_it!=rules.end())_it++;
			}
			code += ')';
			beg = _it;
			toendlineA(rules,_it);
		  }
          else {if(_it!=rules.end())++_it; toendlineA(rules,_it); }
		  break;
		case 'n':
		  _it2 = _it;
          if(has_pattern(rules,_it,"nproduce")){
			code += std::string(beg,_it)+'(';
			if(_it!=rules.end()){
			  code += lstring2py(_it,rules.end(),'\n',lineno);
			}
			code += ')';
			beg = _it;
			toendlineA(rules,_it);
		  }
          else {if(_it!=rules.end())++_it; toendlineA(rules,_it); }
		  break;
		case 'm':
		  _it2 = _it;
          if(has_pattern(rules,_it,"module")){
            code+=std::string(beg,_it2);
			std::vector<std::string> modules = parse_moddeclaration(_it,rules.end());
			code+="# "+std::string(_it2,_it);
			for(std::vector<std::string>::const_iterator itmod = modules.begin(); itmod != modules.end(); ++itmod){
				ModuleClassPtr mod = ModuleClassTable::get().declare(*itmod);
				__context.declare(mod);
			}
			beg = _it;
			toendlineA(rules,_it);
		  }
          else {if(_it!=rules.end())++_it; toendlineA(rules,_it); }
		  break;
		case 'p':
		  _it2 = _it;
		  if(has_pattern(rules,_it,"production")){
            code+=std::string(beg,_it2);
			toendlineC(rules,_it);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 0;
		  }
		  else toendlineA(rules,_it);
		  break;
		case 'd':
		  _it2 = _it;
		  if(has_pattern(rules,_it,"derivation length")){
            code+=std::string(beg,_it2);
			beg = _it;
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
			  if(beg != rules.end())
                if (notOnlySpace(beg,_it)){
                    code+=LsysContext::DerivationLengthVariable+" = "+std::string(beg,_it);
                    max_derivation_lineno = lineno-1;
                }
                else LsysParserSyntaxError("Cannot find value for derivation length");
              else LsysParserSyntaxError("Cannot find value for derivation length");
			}
            else LsysParserSyntaxError("Cannot find ':' after derivation length");
			beg = _it;
		  }
		  else toendlineA(rules,_it);
		  break;
		case 'c':
		  _it2 = _it;
		  if(has_pattern(rules,_it,"consider")){
            code+=std::string(beg,_it2);
			beg = _it;
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
			  if(beg != rules.end())
				__context.consider(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for consider");
			}
            else LsysParserSyntaxError("Cannot find ':' after consider");
            code+="# "+std::string(_it2,_it);
			beg = _it;
		  }
		  else toendlineA(rules,_it);
		  break;
		case 'e':
          _it2 = _it;
		  if(has_pattern(rules,_it,"endlsystem")){
            LsysParserSyntaxError("endlsystem found before production statement.");
		  }
          else toendlineA(rules,_it);
		  break;
		case 'i':
		  _it2 = _it;
		  if(has_pattern(rules,_it,"ignore")){
            code+=std::string(beg,_it2);
			beg = _it;
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
			  if(beg != rules.end())
				__context.ignore(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for ignore");
			}
            else LsysParserSyntaxError("Cannot find ':' after ignore");
            code+="# "+std::string(_it2,_it);
			beg = _it;
		  }
		  else toendlineA(rules,_it);
		  break;
		default:
		  toendlineA(rules,_it);
		  break;
		}
	  }
	  break;
	case 0:
	case 1:
	case 2:
		switch(*_it){
		case 'e':
          _it2 = _it;
		  if(has_pattern(rules,_it,"endlsystem")){
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
			}
			toendline(rules,_it);
            code+="# "+std::string(_it2,_it);
			mode = -1;
            beg = _it;
		  }
		  else if(has_pattern(rules,_it,"endgroup")){
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
			}
			toendline(rules,_it);
            code+="# "+std::string(_it2,_it);
			group = 0;
            beg = _it;
		  }
		  else {
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
			}
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'd':
		  if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(mode == 0 && has_pattern(rules,_it,"derivation length")){
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
			  if(beg != rules.end()){
                if (notOnlySpace(beg,_it))
                    code+=LsysContext::DerivationLengthVariable+" = "+std::string(beg,_it);
                else LsysParserSyntaxError("Cannot find value for derivation length");
			  }
              else LsysParserSyntaxError("Cannot find value for derivation length");
			}
            else LsysParserSyntaxError("Cannot find ':' after derivation length");
		  }
		  else if(has_pattern(rules,_it,"decomposition")){
			mode = 1;
			toendlineC(rules,_it);
            code+='#'+std::string(_it2,_it);
			beg = _it;
		  }
		  else if(has_pattern(rules,_it,"def")){
			mode = -1;
			beg = _it2;
            LsysParserWarning("a python function is declared inside production section. Switching to python mode.");
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'h':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
          if(has_pattern(rules,_it,"homomorphism")){
			toendlineC(rules,_it);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 2;
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'g':
		  if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(mode == 0 && has_pattern(rules,_it,"group ")){
            beg = _it;
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=rules.end() && (*_it)==':'){
              if (notOnlySpace(beg,_it)){
                  group = __context.readInt(std::string(beg,_it));
                  if (group < 0) {
                      std::stringstream ss;
                      ss << "Invalid value for group : ";
                      ss << group << '.';
                      LsysError(ss.str(),filename,lineno);
                  }
                  else if(debug) std::cerr << "Use group " << group << " from line " << lineno << std::endl;
              }
              else LsysParserSyntaxError("Cannot find value for group");
			  toendline(rules,_it);
              code+="# "+std::string(_it2,_it);
			}
            else LsysParserSyntaxError("Cannot find value and ':' after group");
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'p':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(has_pattern(rules,_it,"production")){
			toendlineC(rules,_it);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 0;
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'm':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if((mode == 1||mode == 2) && has_pattern(rules,_it,"maximum depth")){
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')_it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
              // code+='#'+std::string(_it2,_it);
			  if(beg != rules.end()){
                  if (notOnlySpace(beg,_it)){
                      std::string var;
                      switch (mode){
                        case 1:
                            var = LsysContext::DecompositionMaxDepthVariable;
                            decomposition_max_depth_lineno = lineno-1;
                            break;
                        case 2:
                            var = LsysContext::HomomorphismMaxDepthVariable;
                            homomorphism_max_depth_lineno = lineno-1;
                            break;
                        default:
                            LsysParserSyntaxError("Maximum depth is not allowed for production");
                            break;
                      }   
                    code+=var+" = "+std::string(beg,_it);
                  }
                else LsysParserSyntaxError("Cannot find value for maximum depth");
			  }
              else LsysParserSyntaxError("Cannot find value for maximum depth");
			}
            else LsysParserSyntaxError("Cannot find ':' after maximum depth");
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'c':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(has_pattern(rules,_it,"consider")){
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')_it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
              code+="# "+std::string(_it2,_it);
			  if(beg != rules.end())
				__context.consider(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for consider");
			}
            else LsysParserSyntaxError("Cannot find ':' after consider");
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'i':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(has_pattern(rules,_it,"ignore")){
			while( _it!=rules.end() && (*_it)!=':' && (*_it)!='\n')_it++;
			if(_it!=rules.end() && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(rules,_it);
              code+="# "+std::string(_it2,_it);
			  if(beg != rules.end())
				__context.ignore(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for ignore");
			}
            else LsysParserSyntaxError("Cannot find ':' after ignore");
		  }
		  if(has_pattern(rules,_it,"interpretation")){
			toendlineC(rules,_it);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 2;
		  }
		  else {
			beg = _it;
			toendline(rules,_it);
			rule += std::string(beg,_it);
		  }
		  break;
		case '#': // python comments before rules
		  toendline(rules,_it);
		  rule += std::string(beg,_it);
		  break;
		case ' ':
		case '\t':
		  if(rule.empty())
              LsysParserSyntaxError("Ill-formed construct.");
		  beg = _it;
		  toendline(rules,_it);
		  rule += std::string(beg,_it);
		  break;
		case '\n':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
          code += '\n';
		  _it++;          
          ++lineno;
          // std::cerr << lineno << ':' << std::distance<std::string::const_iterator>(rules.begin(),_it)  << std::endl;
          break;
#ifndef _WIN32
		case WindowSpecificEndline: // Window end line style
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it++;          
          ++lineno;
          // std::cerr << lineno << ':' << std::distance<std::string::const_iterator>(rules.begin(),_it)  << std::endl;
          break;
#endif
		default:
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  beg = _it;
		  toendline(rules,_it);
		  rule += std::string(beg,_it);
		  break;
		}
	  break;
	case 3:
	  if(_it != rules.end())code += std::string(_it,rules.end());
	  _it = rules.end();
	default:
	  break;
	}
  }
  if(!rule.empty()){
    PROCESS_RULE(rule,code,addedcode,mode,group)
  }
  if (mode == -1)
   code.append(beg,_it);
  code+='\n'+addedcode;
  if(debug) std::cerr << code << std::endl;
  __context.execute(code);
  __importPyFunctions();
  if (__context.hasObject(LsysContext::AxiomVariable)){
      try
      { __axiom = AxialTree(extract<boost::python::list>(__context.getObject(LsysContext::AxiomVariable))); }
      catch(error_already_set const &)
      { 
          PyErr_Clear();
          LsysError("Axiom has an invalid value.",filename,axiom_lineno); 
      }
  }
  if (__context.hasObject(LsysContext::DerivationLengthVariable)){
      try
      { __max_derivation = extract<int>(__context.getObject(LsysContext::DerivationLengthVariable)); }
      catch(error_already_set const &)
      { 
          PyErr_Clear();
          extract<float> t(__context.getObject(LsysContext::DerivationLengthVariable)); 
          if (t.check()) {
              __max_derivation = (int)t();
              LsysWarning("Floating point value is given for derivation Length. Rounding.",filename,max_derivation_lineno); 
          }
          else LsysError("Derivation Length has an invalid value.",filename,max_derivation_lineno); 
      }
  }
  if (__context.hasObject(LsysContext::DecompositionMaxDepthVariable)){
      try
      { __decomposition_max_depth = extract<int>(__context.getObject(LsysContext::DecompositionMaxDepthVariable)); }
      catch(error_already_set const &)
      { 
          PyErr_Clear();
          extract<float> t(__context.getObject(LsysContext::DecompositionMaxDepthVariable)); 
          if (t.check()) {
              __decomposition_max_depth = (int)t();
              LsysWarning("Floating point value is given for decomposition maximum length. Rounding.",filename,decomposition_max_depth_lineno); 
          }
          else LsysError("Decomposition maximum length has an invalid value.",filename,decomposition_max_depth_lineno); 
      }
  }
  if (__context.hasObject(LsysContext::HomomorphismMaxDepthVariable)){
      try
      { __homomorphism_max_depth = extract<int>(__context.getObject(LsysContext::HomomorphismMaxDepthVariable)); }
      catch(error_already_set const &)
      { 
          PyErr_Clear();
          extract<float> t(__context.getObject(LsysContext::HomomorphismMaxDepthVariable)); 
          if (t.check()) {
              __homomorphism_max_depth = (int)t();
              LsysWarning("Floating point value is given for homomorphism maximum length. Rounding.",filename,homomorphism_max_depth_lineno); 
          }
          else LsysError("Homomorphism maximum length has an invalid value.",filename,homomorphism_max_depth_lineno); 
      }
  }
  __context.initialise();
  RELEASE_RESSOURCE
}

/*---------------------------------------------------------------------------*/

void LsysRule::set( const std::string& rule ){
  std::string::const_iterator endheader = rule.begin();
  bool foundendheader = false;
  bool arrow = false;
  while(endheader != rule.end() && !foundendheader){
      if(*endheader==':') foundendheader = true;
      else if(*endheader=='-' && (endheader==rule.begin()|| *(endheader-1)!='-' )
          && (distance(endheader,rule.end())>=2 && *(endheader+1) == '-' && *(endheader+2) == '>')){
        foundendheader = true;
        arrow = true;
      }
      else endheader++;
  }
  if(endheader == rule.end()){
	LsysError("Ill-formed Rule : unfound delimiter ':' in "+rule,"",lineno);
  }
  std::string header(rule.begin(),endheader);
  parseHeader(header);
  __hasquery = __predecessor.hasQueryModule() 
			|| __newleftcontext.hasQueryModule()
			|| __leftcontext.hasQueryModule()
			|| __newrightcontext.hasQueryModule()
			|| __rightcontext.hasQueryModule();
  parseParameters();
  if (arrow)parseDefinition("produce "+std::string(endheader+3,rule.end()));
  else parseDefinition(std::string(endheader+1,rule.end()));
}

/*---------------------------------------------------------------------------*/
	
// cg < ncg << pred >> ncd > cd
void 
LsysRule::parseHeader( const std::string& header){
  std::string::const_iterator it = header.begin();
  std::string::const_iterator beg = header.begin();
  std::string cg;
  std::string ncg;
  std::string pred;
  std::string ncd;
  std::string cd;
  bool begncd = false;
  while(it != header.end()){
	if(*it == '<'){
	  if(!pred.empty())LsysError("Ill-formed Rule Header : "+header,"",lineno);
	  if(*(it+1) == '<'){
		if(ncg.empty())ncg = std::string(beg,it);
		else LsysError("Ill-formed Rule Header : "+header,"",lineno);
		it++;
	  }
	  else {
		if(cg.empty())cg = std::string(beg,it);
		else LsysError("Ill-formed Rule Header : "+header,"",lineno);
	  }
	  beg = it+1;
	}
	else if(*it == '>'){      
      if(*(it+1) == '>') {
          if (begncd || !ncd.empty())LsysError("Ill-formed Rule Header : Two new left contexts found : "+header,"",lineno);
          else { 
              if(pred.empty()) pred = std::string(beg,it); 
              else LsysError("Ill-formed Rule Header : Two Predecessors found : "+header,"",lineno);          
              begncd = true; it++; 
          }
      }
      else {
          if (begncd) { 
              ncd = std::string(beg,it); begncd = false;  }
          else {
            if(pred.empty()) pred = std::string(beg,it);
            else LsysError("Ill-formed Rule Header : Two Predecessors found : "+header,"",lineno);
          }
      }
      if(pred.empty())LsysError("Ill-formed Rule Header : No Predecessor found : "+header,"",lineno);
	  beg = it+1;
    }
	it++;
  }
  if(pred.empty()){
	pred = std::string(beg,it);
	if(pred.empty()) LsysError("Ill-formed Rule Header : No Predecessor found : "+header,"",lineno);
  }
  else if (beg != header.end()){
      if (begncd)ncd = std::string(beg,it);
      else cd = std::string(beg,it);
  }
  if (!ncg.empty() && !ncd.empty())LsysError("Ill-formed Rule Header : New left and right contexts found : "+header,"",lineno);

  __predecessor = AxialTree::QueryTree(pred);
  if(!cg.empty())__leftcontext = AxialTree::QueryTree(cg);
  if(!ncg.empty())__newleftcontext = AxialTree::QueryTree(ncg);
  if(!cd.empty())__rightcontext = AxialTree::QueryTree(cd);
  if(!ncd.empty())__newrightcontext = AxialTree::QueryTree(ncd);
}

void
LsysRule::parseDefinition(const std::string& definition){
  std::string::const_iterator _it = definition.begin();
  while(_it != definition.end() && (*_it == ' ' || *_it == '\t'))_it++;
  if(_it == definition.end()) __definition = "\tpass\n";
  else {
	if(*_it == '\n'){
	  __definition = std::string(_it+1,definition.end());
	  size_t i = 0;
	  _it = __definition.begin();
	  while(_it != __definition.end() && (*_it == ' ' || *_it == '\t'))
	  { _it++; i++; }
	  if(_it == __definition.end())
		__definition = "\tpass\n";
	}
	else {
	  __definition = '\t' + std::string(_it,definition.end());
	}
  }
}

/*---------------------------------------------------------------------------*/

std::string PYLSYS::lstring2py(const std::string& lcode,
				 std::string::const_iterator& beg)
{ return lstring2py(beg,lcode.end()); }

std::string PYLSYS::lstring2py(const std::string& lcode)
{ std::string::const_iterator beg = lcode.begin();
  return lstring2py(beg,lcode.end()); }

std::string PYLSYS::lstring2py( std::string::const_iterator& beg,
								std::string::const_iterator endpos,
								char delim,
								int lineno){
  std::string result("[");
  std::vector<std::pair<size_t,std::string> > parsedstring = parselstring(beg, endpos, delim, lineno);
  bool first = true;
  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
	  it != parsedstring.end(); ++it){
		std::string mod_id = TOOLS(number)(it->first);
		bool has_arg = (it->second.size() > 0);
		if(!first) result += ",";
		else first = false;
		if (has_arg) result += "(";
		result += mod_id;
		if (has_arg) result += ","+it->second+")";
  } // end for
  result += "]";
  return result;
}

/*---------------------------------------------------------------------------*/

std::vector<std::pair<size_t,std::string> > 
PYLSYS::parselstring( std::string::const_iterator& beg,
					  std::string::const_iterator endpos,
					  char delim,
					  int lineno)
{
  // std::cerr << "parse '" << std::string(beg,endpos) << "'" << std::endl;
  std::vector<std::pair<size_t,std::string> > result;
  std::string::const_iterator _it = beg;
  while(_it != endpos && (*_it == ' ' || *_it == '\t'))_it++;
  std::string::const_iterator _it2 = _it;
  bool first = true;
  while(_it != endpos && *_it != delim){
	if(*_it == '(')
		LsysSyntaxError("Found module named  '('","",lineno);
	else if(*_it == ')')
		LsysSyntaxError("Found module named  ')'","",lineno);
	else if(*_it == ' ' || *_it == '\t'){
      // skip space and tabulation in the string
	  _it++;
    }
#ifndef _WIN32
	else if(*_it == WindowSpecificEndline) {
	  // if windows end line char appear, skip
	  _it++;
	}
#endif
	else {
		size_t mod_id;
		std::string mod_args;
		ModuleClassPtr mod = ModuleClassTable::get().find(_it,endpos);
		if (mod.isNull()){
			LsysSyntaxError(std::string("Invalid symbol '")+*_it+"' in AxialTree.","",lineno);
			// mod = ModuleClassTable::get().declare(*_it); ++_it; 
		}
		else {_it += mod->name.size(); }
		mod_id = mod->getId(); 
		bool has_arg = false;
		if (mod == ModuleClass::CpfgSurface){
			if (_it != endpos && *_it != '('){
				has_arg = true;
				mod_args = std::string("'")+*_it+"',";
				_it++;
			}
		}
		if(_it != endpos && *_it == '('){
			// look for parameters
			++_it;
			if(_it == endpos){
				LsysSyntaxError("Invalid syntax in AxialTree","",lineno);
			}
			_it2 = _it;
			int parenthesis = 0;
			while(_it != endpos && (*_it != ')' || parenthesis > 0)){
				if(*_it == ')') parenthesis--;
				else if (*_it == '(') parenthesis++;
				_it++;
				if(_it == endpos){
					LsysSyntaxError("Invalid syntax in AxialTree","",lineno);
				}
			}
			if(_it != endpos)_it++;
			mod_args += std::string(_it2,_it-1);
			has_arg = (mod_args.size() > 0);
		}
		std::pair<size_t,std::string> amod;
		amod.first = mod_id;
		amod.second = mod_args;
		result.push_back(amod);
	}
  } // end while
  beg = _it;
  return result;
}

/*---------------------------------------------------------------------------*/

std::vector<std::string> PYLSYS::parse_moddeclaration(std::string::const_iterator& beg,
													  std::string::const_iterator endpos,
													  char delim)
{
  bool first = true;
  std::vector<std::string> result;
  std::string::const_iterator _it = beg;
  while(_it != endpos && *_it != delim){
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	if (_it == endpos || *_it == delim || *_it == ':' || *_it == '#') break;
	if (!first){
		if(*_it != ',')
			LsysSyntaxError("Invalid syntax in module declaration");
		++_it;
		while (_it != endpos && (*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
		if (_it == endpos || *_it == delim)
			LsysSyntaxError("Invalid syntax in module declaration");
	}
	else first = false;
	std::string::const_iterator bm = _it;
	while(_it != endpos && *_it != ',' && *_it != ' ' && *_it != '\t' && *_it != '\n' && *_it != ':' && *_it != '#' && *_it != delim) ++_it;
	if (bm != _it){
		std::string m(bm,_it);
		result.push_back(m);
	}
  }
  std::string scalevalue;
  if (_it != endpos && *_it == ':') {
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	if (!has_pattern(_it,endpos,"scale"))
		LsysSyntaxError("Invalid module attribute in module declaration");
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	if (_it == endpos || *_it == delim || *_it != '=')
		LsysSyntaxError("Invalid module attribute in module declaration");
	else _it++;
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	std::string::const_iterator bm = _it;
	while(_it != endpos && *_it != ',' && *_it != ' ' && *_it != '\t' && *_it != '\n' && *_it != '#' && *_it != delim) ++_it;
	if (bm != _it){
		scalevalue = std::string(bm,_it);
	}
  }
  beg = _it;
  return result;
}


/*---------------------------------------------------------------------------*/

std::string PYLSYS::trim(const std::string& str)
{
	/// triming name
	std::string::const_iterator _itb = str.begin();
	std::string::const_iterator _ite = str.end();
	while(_itb!=_ite && (*_itb == ' ' || *_itb == '\t' || *_itb == '\n'))_itb++;
	if (_itb == _ite) return std::string();
	char c = *(_ite-1);
	while(_ite != _itb && (c == ' ' || c == '\t' || c == '\n')){ _ite--; c = *(_ite-1); }
	return std::string(_itb,_ite);
}

std::vector<std::string> PYLSYS::parse_arguments(std::string::const_iterator beg,
										 std::string::const_iterator end)
{
	std::vector<std::string> result;
    std::string::const_iterator _it = beg;
	std::string::const_iterator _it2 = _it;
	while(_it2 != end){
		while(_it2 != end && *_it2 != ',' && *_it2 != ')'){
		  if(*_it2 == '"'){ // skip string
			_it2++;
			while(_it2 != end && *_it2 != '"')_it2++;
		  }
		  else if(*_it2 == '\''){ // skip string
			_it2++;
			while(_it2 != end && *_it2 != '\'')_it2++;
		  }
		  else if(*_it2 == '('){ // skip expression
			_it2++;
			int nbOpenParenthesis = 0;
			while(_it2 != end && *_it2 != ')' && nbOpenParenthesis != 0){
				_it2++;
				if(_it2 != end)
					if(*_it2 == '(') ++nbOpenParenthesis;
					else if (*_it2 == ')') --nbOpenParenthesis;
			}
		  }
		  else if(*_it2 == '['){ // skip array
			_it2++;
			int nbOpenBracket = 0;
			while(_it2 != end && *_it2 != ']' && nbOpenBracket != 0){
				_it2++;
				if(_it2 != end)
					if(*_it2 == '[') ++nbOpenBracket;
					else if (*_it2 == ']') --nbOpenBracket;
			}
		  }
		  else if(*_it2 == '{'){ // skip dict
			_it2++;
			int nbOpenBracket = 0;
			while(_it2 != end && *_it2 != '}' && nbOpenBracket != 0){
				_it2++;
				if(_it2 != end)
					if(*_it2 == '{') ++nbOpenBracket;
					else if (*_it2 == '}') --nbOpenBracket;
			}
		  }
		  if(_it2 != end)_it2++;
		}
		if(_it != _it2){
			/// triming name
			std::string arg = trim(std::string(_it,_it2));
			if(!arg.empty()) result.push_back(arg);
		}
		if (_it2 != end && *_it2 == ',') { ++_it2;_it = _it2; }
	}
	return result;
}

bool PYLSYS::isValidVariableName(const std::string& arg)
{
	if (arg.empty())return false;
	std::string::const_iterator _si = arg.begin();
	if(*_si == '*' && ParamModule::getMatchingMethod() != ParamModule::eSimple){
		if(arg.size() == 1)return false;
		else ++_si;
	}
	bool b = true;
	while(_si != arg.end() && (b = (isalnum(*_si) || *_si == '_')))_si++;
	return b;
}


/*---------------------------------------------------------------------------*/

