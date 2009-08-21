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
#include "matching.h"
#include "lpy_parser.h"
#include <plantgl/tool/util_string.h>
#include <QtCore/QFileInfo>

using namespace boost::python;
TOOLS_USING_NAMESPACE
PGL_USING_NAMESPACE
LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

#ifdef MULTI_THREADED_LSYSTEM
#define ACQUIRE_RESSOURCE  LsysAcquirer ressource(this); 
#define RELEASE_RESSOURCE  
#else
#define ACQUIRE_RESSOURCE  
#define RELEASE_RESSOURCE
#endif

/*---------------------------------------------------------------------------*/

const std::string LpyParsing::InitialisationBeginTag("###### INITIALISATION ######");
const std::string LpyParsing::VersionTag("# -*- lpy version: %f -*-");

/*---------------------------------------------------------------------------*/

const float LpyParsing::LPY_FORMAT_VERSION = 1.0f;
const float LpyParsing::LPY_DEFAULT_FORMAT_VERSION = 1.0f;
const std::vector<float> LpyParsing::getSupportedFormat() { std::vector<float> res; res.push_back(LPY_FORMAT_VERSION); return res; }

bool LpyParsing::isSupportedFormat(float format) { return ( 1.0f <= format && format <= LPY_FORMAT_VERSION); }

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

inline 
void ToEndlineCheckColon(std::string::const_iterator& _it, std::string::const_iterator _end, std::string& filename, int& lineno){
    bool colon = false;
    while( _it!=_end && (*_it)!='\n') {
        ++_it;
        if ((*_it)!=':')colon = true;
    }
    if (!colon) LsysWarning("Colon missing.",filename,lineno);
    if(_it!=_end && (*_it)=='\n') { ++lineno; ++_it; }
}

inline 
void ToEndline(std::string::const_iterator& _it, std::string::const_iterator _end,int& lineno){
	while( _it!=_end && (*_it)!='\n') ++_it;
    if(_it!=_end && (*_it)=='\n') { ++lineno; ++_it; }
}

inline 
void ToEndlineA(std::string::const_iterator& _it,std::string::const_iterator _end, int& lineno){
	while( _it!=_end && (*_it)!='\n' && (*_it)!='A' && (*_it)!='n' && (*_it)!='p' && (*_it)!='#' ) ++_it;
    if(_it!=_end && (*_it)=='\n') 
    { 
        ++lineno; ++_it;
    }
}

#define toendline(a,b) ToEndline(a,b,lineno)
#define toendlineA(a,b) ToEndlineA(a,b,lineno)
#define toendlineC(a,b) ToEndlineCheckColon(a,b,filename,lineno)

/*---------------------------------------------------------------------------*/

inline 
bool notOnlySpace(std::string::const_iterator beg, std::string::const_iterator end){
	while( beg!=end) 
        if ( (*beg)!=' ' && (*beg)!='\t'&& (*beg)!='\n') return true;
        else ++beg;
	return false;
}

/*---------------------------------------------------------------------------*/

float LpyParsing::getFormatVersion(const std::string& lcode) {
  std::string::const_iterator it = lcode.begin();
  return getFormatVersion(it,lcode.end());
}

float LpyParsing::getFormatVersion(std::string::const_iterator& it, std::string::const_iterator endcode) {
  // Retrieve of lpy format version
  float lpyversion = LPY_DEFAULT_FORMAT_VERSION;
  size_t p = VersionTag.find("%f");
  std::string::const_iterator _it = it;

  if (has_pattern(_it,endcode,std::string(VersionTag.begin(),VersionTag.begin()+p))) {
	  std::string::const_iterator _it2 = _it;
	  while(_it != endcode && (*_it != ' ' && *_it != '\t'  && *_it != '\n')) { ++_it; }
	  std::string::const_iterator _it3 = _it;
	  if (has_pattern(_it,endcode,std::string(VersionTag.begin()+p+2,VersionTag.end())) && (_it != endcode && *_it == '\n')){
		lpyversion = atof(std::string(_it2,_it3).c_str());
		it = _it+1;
	  }
  }
  return lpyversion;
}

/*---------------------------------------------------------------------------*/


#define PROCESS_RULE(rulecode,code,addedcode,mode,group) \
	LsysRule& r = __addRule(rulecode,mode,group,lineno - std::count(rule.begin(),rule.end(),'\n')); \
    code += r.getCoreCode(); \
    addedcode += r.getCallerCode(); \
	rule.clear(); \

#define LsysParserSyntaxError(msg) LsysSyntaxError(msg,filename,lineno)
#define LsysParserWarning(msg) LsysWarning(msg,filename,lineno)

#define WindowSpecificEndline 13

size_t LsysContext::initialiseFrom(const std::string& _lcode)
{
	ContextMaintainer c(this);
#ifndef _WIN32
  std::string _lcode_ = _lcode;
  for(std::string::iterator _itEr = _lcode_.begin(); _itEr != _lcode_.end(); ++_itEr)
  	if (*_itEr == WindowSpecificEndline) _lcode_.erase(_itEr);
  const std::string& lcode = _lcode_;
#else
  const std::string& lcode = _lcode;
#endif
  return __initialiseFrom(lcode);
}

void 
Lsystem::set( const std::string&   _rules , std::string * pycode){
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
  std::string::const_iterator endcode = rules.end();
  std::string::const_iterator endpycode = endcode;
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
  // Retrieve of lpy format version
  float lpyversion = LpyParsing::getFormatVersion(_it2,endpycode);
  if (!LpyParsing::isSupportedFormat(lpyversion)){
	  std::stringstream stream;
	  stream << "Not supported lpy format : " << lpyversion << ". Supported = " << LpyParsing::LPY_FORMAT_VERSION << ".";
	  LsysError(stream.str(),filename,0);
  }
  // version has been found. lineno should be increment.
  else if (_it != _it2) { _it = _it2; ++lineno; }
  
  //  context initialisation
  size_t initpos = __context.__initialiseFrom(rules);
  if (initpos != std::string::npos) endpycode = rules.begin()+initpos;

  while(_it!=endpycode){
	switch(mode){
	case -1:
	  {
		switch(*_it){
	    case '#':
			while(_it != endpycode && *_it != '\n')++_it;
			break;
		case 'A':
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"Axiom") && 
			 (_it2 == rules.begin() || *(_it2-1) == '\n')){
            code += std::string(beg,_it2);
			beg = _it;
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
              axiom_lineno = lineno;
              code += LsysContext::AxiomVariable + " = ";
              code += LpyParsing::lstring2py(_it,endpycode,'\n',lineno);
			}
            else LsysParserSyntaxError("Cannot find ':' after Axiom");
			beg = _it;
		  }
		  else if(has_pattern(_it,endpycode,"AxialTree")){
			code += std::string(beg,_it)+'(';
			while (_it!=endpycode&&*_it!='(')_it++;
			if(_it!=endpycode){
			  _it++;
			  code += LpyParsing::lstring2py(_it,endpycode,')',lineno);
			  if(_it!=endpycode)_it++;
			}
			code += ')';
			beg = _it;
			toendlineA(_it,endpycode);
		  }
          else {if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'n':
		  _it2 = _it;
          if(has_pattern(_it,endpycode,"nproduce")){
			code += std::string(beg,_it2);
			code += "pproduce";
		    while(_it != endpycode && (*_it == ' ' || *_it == '\t') )++_it;
		    char endproduction = '\n';
			if(*_it == '(') { endproduction = ')'; ++_it; }
			if(_it!=endpycode){
			  code += LpyParsing::lstring2pyparam(_it,endpycode,endproduction,lineno);
			  if (endproduction == ')') ++_it;
			}
			beg = _it;
			toendlineA(_it,endpycode);
		  }
          else {if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'm':
		  _it2 = _it;
          if(has_pattern(_it,endpycode,"module")){
            code+=std::string(beg,_it2);
			LpyParsing::ModLineDeclatation modules = LpyParsing::parse_moddeclaration_line(_it,endpycode);
			code+="# "+std::string(_it2,_it);
			int scale = ModuleClass::DEFAULT_SCALE;
			if(!modules.second.empty())
			scale = extract<int>(__context.evaluate(modules.second))();
			for(LpyParsing::ModDeclarationList::const_iterator itmod = modules.first.begin(); 
				 itmod != modules.first.end(); ++itmod){
				ModuleClassPtr mod;
				if(!itmod->alias){
					mod = ModuleClassTable::get().declare(itmod->name);
				    __context.declare(mod);
					if(!itmod->parameters.empty()){
						// printf("%s\n",itmod->parameters.c_str());
						std::vector<std::string> args = LpyParsing::parse_arguments(itmod->parameters);
						for(std::vector<std::string>::const_iterator itarg = args.begin(); itarg != args.end(); ++itarg){
							if(!LpyParsing::isValidVariableName(*itarg))LsysError("Invalid parameter name '"+*itarg+"'","",lineno);
						}
						mod->setParameterNames(args);
					}
				}
				else {
					mod = ModuleClassTable::get().alias(itmod->name,itmod->parameters);
					__context.declareAlias(itmod->name,mod);
				}
				if(scale != ModuleClass::DEFAULT_SCALE)mod->setScale(scale);
			}
			beg = _it;
			toendlineA(_it,endpycode);
		  }
          else {if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'u':
		  _it2 = _it;
          if(has_pattern(_it,endpycode,"undeclare")){
            code+=std::string(beg,_it2);
			LpyParsing::ModNameList modules = LpyParsing::parse_modlist(_it,endpycode,false);
			code+="# "+std::string(_it2,_it);
			for(LpyParsing::ModNameList::const_iterator itmod = modules.begin(); 
				 itmod != modules.end(); ++itmod){
				ModuleClassPtr mod = ModuleClassTable::get().findClass(*itmod);
				if(mod)__context.undeclare(mod);
				else LsysError("Cannot undeclare a not declared module",filename,lineno);
			}
			beg = _it;
			toendlineA(_it,endpycode);
		  }
          else {if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'p':
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"production")){
            code+=std::string(beg,_it2);
			toendlineC(_it,endpycode);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 0;
		  }
		  else if(has_pattern(_it,endpycode,"produce")){
			  LsysError("Cannot use 'produce' outside production body. Use 'nproduce' instead.",filename,lineno);
		  }
		  else { if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'd':
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"derivation length")){
            code+=std::string(beg,_it2);
			beg = _it;
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
			  if(beg != endpycode)
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
		  else toendlineA(_it,endpycode);
		  break;
		case 'c':
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"consider")){
            code+=std::string(beg,_it2);
			beg = _it;
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
			  if(beg != endpycode)
				__context.consider(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for consider");
			}
            else LsysParserSyntaxError("Cannot find ':' after consider");
            code+="# "+std::string(_it2,_it);
			beg = _it;
		  }
		  else toendlineA(_it,endpycode);
		  break;
		case 'e':
          _it2 = _it;
		  if(has_pattern(_it,endpycode,"endlsystem")){
            LsysParserSyntaxError("endlsystem found before production statement.");
		  }
          else toendlineA(_it,endpycode);
		  break;
		case 'i':
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"ignore")){
            code+=std::string(beg,_it2);
			beg = _it;
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
			  if(beg != endpycode)
				__context.ignore(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for ignore");
			}
            else LsysParserSyntaxError("Cannot find ':' after ignore");
            code+="# "+std::string(_it2,_it);
			beg = _it;
		  }
		  else toendlineA(_it,endpycode);
		  break;
		default:
		  toendlineA(_it,endpycode);
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
		  if(has_pattern(_it,endpycode,"endlsystem")){
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
			}
			toendline(_it,endpycode);
            code+="# "+std::string(_it2,_it);
			mode = -1;
            beg = _it;
		  }
		  else if(has_pattern(_it,endpycode,"endgroup")){
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
			}
			toendline(_it,endpycode);
            code+="# "+std::string(_it2,_it);
			group = 0;
            beg = _it;
		  }
		  else {
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
			}
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'd':
		  if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(mode == 0 && has_pattern(_it,endpycode,"derivation length")){
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
			  if(beg != endpycode){
                if (notOnlySpace(beg,_it))
                    code+=LsysContext::DerivationLengthVariable+" = "+std::string(beg,_it);
                else LsysParserSyntaxError("Cannot find value for derivation length");
			  }
              else LsysParserSyntaxError("Cannot find value for derivation length");
			}
            else LsysParserSyntaxError("Cannot find ':' after derivation length");
		  }
		  else if(has_pattern(_it,endpycode,"decomposition")){
			mode = 1;
			toendlineC(_it,endpycode);
            code+='#'+std::string(_it2,_it);
			beg = _it;
		  }
		  else if(has_pattern(_it,endpycode,"def")){
			mode = -1;
			beg = _it2;
            LsysParserWarning("a python function is declared inside production section. Switching to python mode.");
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'h':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
          if(has_pattern(_it,endpycode,"homomorphism")){
			toendlineC(_it,endpycode);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 2;
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'g':
		  if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(mode == 0 && has_pattern(_it,endpycode,"group ")){
            beg = _it;
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')
			  _it++;
			if(_it!=endpycode && (*_it)==':'){
              if (notOnlySpace(beg,_it)){
                  group = __context.readInt(std::string(beg,_it));
                  if (group < 0) {
                      std::stringstream ss;
                      ss << "Invalid value for group : ";
                      ss << group << '.';
                      LsysError(ss.str(),filename,lineno);
                  }
                  // else if(debug) std::cerr << "Use group " << group << " from line " << lineno << std::endl;
              }
              else LsysParserSyntaxError("Cannot find value for group");
			  toendline(_it,endpycode);
              code+="# "+std::string(_it2,_it);
			}
            else LsysParserSyntaxError("Cannot find value and ':' after group");
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'p':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"production")){
			toendlineC(_it,endpycode);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 0;
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'm':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if((mode == 1||mode == 2) && has_pattern(_it,endpycode,"maximum depth")){
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')_it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
              // code+='#'+std::string(_it2,_it);
			  if(beg != endpycode){
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
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'c':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"consider")){
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')_it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
              code+="# "+std::string(_it2,_it);
			  if(beg != endpycode)
				__context.consider(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for consider");
			}
            else LsysParserSyntaxError("Cannot find ':' after consider");
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'i':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  _it2 = _it;
		  if(has_pattern(_it,endpycode,"ignore")){
			while( _it!=endpycode && (*_it)!=':' && (*_it)!='\n')_it++;
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
			  beg = _it;
			  toendline(_it,endpycode);
              code+="# "+std::string(_it2,_it);
			  if(beg != endpycode)
				__context.ignore(std::string(beg,_it));
              else LsysParserSyntaxError("Cannot find value for ignore");
			}
            else LsysParserSyntaxError("Cannot find ':' after ignore");
		  }
		  if(has_pattern(_it,endpycode,"interpretation")){
			toendlineC(_it,endpycode);
            code+="# "+std::string(_it2,_it);
			beg = _it;
			mode = 2;
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case '#': // python comments before rules
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group)
		  }
		  beg = _it;
		  toendline(_it,endpycode);
		  code += std::string(beg,_it);
		  break;
		case ' ':
		case '\t':
		  if(rule.empty())
              LsysParserSyntaxError("IndentationError: unexpected indent.");
		  beg = _it;
		  toendline(_it,endpycode);
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
		  toendline(_it,endpycode);
		  rule += std::string(beg,_it);
		  break;
		}
	  break;
	case 3:
	  if(_it != endpycode)code += std::string(_it,endpycode);
	  _it = endpycode;
	default:
	  break;
	}
  }
  if(!rule.empty()){
    PROCESS_RULE(rule,code,addedcode,mode,group)
  }
  if (mode == -1)
   code.append(beg,_it);
  if (!addedcode.empty())
	code+='\n'+addedcode;
  if(pycode) *pycode = code;
  __context.compile(code);
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
  __context.check_init_functions();
  RELEASE_RESSOURCE
}

/*---------------------------------------------------------------------------*/

void LsysRule::set( const std::string& rule ){
  std::string::const_iterator endheader = rule.begin();
  std::string::const_iterator starcode = endheader;
  bool foundendheader = false;
  bool arrow = false;
  std::string staticarrowtxt = "-static->";
  std::string staticmarkertxt = "@static";
  bool staticrule = false;
  // count number of lines
  __codelength = 0;
  for(std::string::const_iterator it = rule.begin(); it != rule.end(); ++it)
	  if (*it == '\n') ++__codelength;
  // identify header
  while(endheader != rule.end() && !foundendheader){
	  if(*endheader==':') { 
		  foundendheader = true; 
		  starcode = endheader+1; 
		  std::string::const_iterator marker = starcode;
		  while (marker!= rule.end() && (*marker == ' ' || *marker == '\t'))++marker;
		  if(distance(marker,rule.end())>=staticmarkertxt.size() && std::string(marker,marker+staticmarkertxt.size()) == staticmarkertxt){
			starcode = marker+staticmarkertxt.size();
			staticrule = true;
		  }
	  }
	  else if(*endheader=='-' && (endheader==rule.begin()|| *(endheader-1)!='-' )){
		  if(distance(endheader,rule.end())>=2 && *(endheader+1) == '-' && *(endheader+2) == '>'){
			foundendheader = true;
			arrow = true;
			starcode = endheader+3;
		  }
		  else if (distance(endheader,rule.end())>=staticarrowtxt.size() && std::string(endheader,endheader+staticarrowtxt.size()) == staticarrowtxt){
			foundendheader = true;
			arrow = true;
			staticrule = true;
			starcode = endheader+staticarrowtxt.size();
		  }
		  else endheader++;
      } 
      else endheader++;
  }
  if(endheader == rule.end()){
	LsysError("Ill-formed Rule : unfound delimiter ':' in "+rule,"",lineno);
  }
  // identify successor code
  if (arrow)__definition = " --> "+std::string(starcode,rule.end());
  else __definition =  std::string(starcode,rule.end());
  // parse header
  std::string header(rule.begin(),endheader);
  parseHeader(header);
  __hasquery = __predecessor.hasRequestModule() 
			|| __newleftcontext.hasRequestModule()
			|| __leftcontext.hasRequestModule()
			|| __newrightcontext.hasRequestModule()
			|| __rightcontext.hasRequestModule();
  // check variables
  if(staticrule) setStatic();
  else {
	if (LsysContext::current()->optimizationLevel >= 2)
		keepOnlyRelevantVariables();
	parseParameters();
  }
}

/*---------------------------------------------------------------------------*/
std::string::const_iterator next_token(std::string::const_iterator _it2,
											 std::string::const_iterator end)
{
    if(_it2 != end){
		  if(*_it2 == '"'){ // skip string
			_it2++;
			while(_it2 != end && *_it2 != '"')_it2++;
			if(_it2 != end)_it2++;
		  }
		  else if(*_it2 == '\''){ // skip string
			_it2++;
			while(_it2 != end && *_it2 != '\'')_it2++;
			if(_it2 != end)_it2++;
		  }
		  else if(*_it2 == '('){ // skip expression
			int nbOpenParenthesis = 1;
			while(_it2 != end && (*_it2 != ')' || nbOpenParenthesis > 0)){
				_it2++;
				if(_it2 != end)
					if(*_it2 == '(') ++nbOpenParenthesis;
					else if (*_it2 == ')') --nbOpenParenthesis;
					// skip strings
					else if(*_it2 == '"') { _it2++; while(_it2 != end && *_it2 != '"')_it2++; }
					else if(*_it2 == '\''){ _it2++; while(_it2 != end && *_it2 != '\'')_it2++; }
			}
			if(_it2 != end)_it2++;
		  }
		  else if(*_it2 == '['){ // skip array
			int nbOpenBracket = 1;
			while(_it2 != end && (*_it2 != ']' || nbOpenBracket > 0)){
				_it2++;
				if(_it2 != end)
					if(*_it2 == '[') ++nbOpenBracket;
					else if (*_it2 == ']') --nbOpenBracket;
					// skip strings
					else if(*_it2 == '"') { _it2++; while(_it2 != end && *_it2 != '"')_it2++; }
					else if(*_it2 == '\''){ _it2++; while(_it2 != end && *_it2 != '\'')_it2++; }
			}
			if(_it2 != end)_it2++;
		  }
		  else if(*_it2 == '{'){ // skip dict
			int nbOpenBracket = 1;
			while(_it2 != end && (*_it2 != '}' || nbOpenBracket > 0)){
				_it2++;
				if(_it2 != end)
					if(*_it2 == '{') ++nbOpenBracket;
					else if (*_it2 == '}') --nbOpenBracket;
					// skip strings
					else if(*_it2 == '"') { _it2++; while(_it2 != end && *_it2 != '"')_it2++; }
					else if(*_it2 == '\''){ _it2++; while(_it2 != end && *_it2 != '\'')_it2++; }
			}
			if(_it2 != end)_it2++;
		  }
		  else _it2++;
	}
	return _it2;
}
/*---------------------------------------------------------------------------*/
	
// cg < ncg << pred >> ncd > cd
void 
LsysRule::parseHeader( const std::string& header){
  std::string::const_iterator it = header.begin();
  std::string::const_iterator beg = header.begin();
  std::string::const_iterator end = header.end();
  std::string cg;
  std::string ncg;
  std::string pred;
  std::string ncd;
  std::string cd;
  bool begncd = false;
  while(it != end){
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
	it = next_token(it,end);
	// it++;
  }
  if(pred.empty()){
	pred = std::string(beg,it);
	if(pred.empty()) LsysError("Ill-formed Rule Header : No Predecessor found : "+header,"",lineno);
  }
  else if (beg !=end){
      if (begncd)ncd = std::string(beg,it);
      else cd = std::string(beg,it);
  }
  if (!ncg.empty() && !ncd.empty())LsysError("Ill-formed Rule Header : New left and right contexts found : "+header,"",lineno);

  __predecessor = PatternString(pred,lineno);
  if(!cg.empty())__leftcontext = PatternString(cg,lineno);
  if(!ncg.empty())__newleftcontext = PatternString(ncg,lineno);
  if(!cd.empty())__rightcontext = PatternString(cd,lineno);
  if(!ncd.empty())__newrightcontext = PatternString(ncd,lineno);
}


/*---------------------------------------------------------------------------*/

std::string LpyParsing::lstring2py(const std::string& lcode,
				 std::string::const_iterator& beg)
{ return lstring2py(beg,lcode.end()); }

std::string LpyParsing::lstring2py(const std::string& lcode)
{ std::string::const_iterator beg = lcode.begin();
  return lstring2py(beg,lcode.end()); }

/*---------------------------------------------------------------------------*/

std::string LpyParsing::lstring2py( std::string::const_iterator& beg,
								    std::string::const_iterator endpos,
								    char delim, int lineno){
  std::string result("[");
  std::vector<std::pair<size_t,std::string> > parsedstring = parselstring(beg, endpos, delim, lineno,true);
  bool first = true;
  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
	  it != parsedstring.end(); ++it){
		if(!first) result += ",";
		else first = false;
		if (it->first == ModuleClass::GetModule->getId()){
			result += it->second;
		}
		else {
			std::string mod_id = TOOLS(number)(it->first);
			bool has_arg = (it->second.size() > 0);
			if (has_arg) result += "(";
			result += mod_id;
			if (has_arg) result += ","+it->second+")";
		}
  } // end for
  result += "]";
  return result;
}

std::string LpyParsing::lstring2pyparam( std::string::const_iterator& beg,
								    std::string::const_iterator endpos,
								    char delim, int lineno,
									size_t * pprod_id){
  std::string result;
  std::string::const_iterator initbeg = beg;
  std::vector<std::pair<size_t,std::string> > parsedstring = parselstring(beg, endpos, delim, lineno,true);
  ParametricProduction pprod;
  if(parsedstring.empty()){ 
	  pprod.append_module_type("None");
  }
  else {
	  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
		  it != parsedstring.end(); ++it){
			  if (it->first == ModuleClass::GetModule->getId()){
				  pprod.append_variable_module();
				  result += "," + it->second;
			  }
			  else {
				  pprod.append_module_type(it->first);
				  if (!it->second.empty()) {
					  std::vector<std::string> args = parse_arguments(it->second);
					  for(std::vector<std::string>::const_iterator itArg = args.begin(); itArg != args.end(); ++itArg){
						  if(isAConstant(*itArg)){
							  pprod.append_module_value(LsysContext::current()->evaluate(*itArg));
						  }
						  else {
							  std::string m = "PackedArgs(";
							  if(itArg->size() > m.size() && std::string(itArg->begin(),itArg->begin()+m.size()) == m) {
								pprod.append_module_star_variable();
								result += "," + std::string(itArg->begin()+m.size(),itArg->end()-1); 
							  }
							  else {
								pprod.append_module_variable();
								result += "," + *itArg;
							  }
						  }
					  }
				  }
			  }
	  } // end for
  }
  size_t id = LsysContext::current()->add_pproduction(pprod);
  if(pprod_id) *pprod_id = id;
  size_t nbInitialLine = std::count(initbeg,beg,'\n');
  size_t nbResultLine = std::count(result.begin(),result.end(),'\n');
  std::string complementcode;
  for(;nbResultLine < nbInitialLine; ++nbResultLine) complementcode += '\n';
  result = "(" + TOOLS(number)(id) + result + complementcode+ ")";
  return result;
}


/*---------------------------------------------------------------------------*/


std::pair<size_t,std::string>
parseAModule( std::string::const_iterator& _it,
			  std::string::const_iterator endpos,
			  bool production,
			  int lineno)
{
	std::string::const_iterator _it2 = _it;
	bool first = true;
	size_t mod_id;
	size_t namesize = 0;
	std::string mod_args;
	ModuleClassPtr mod = ModuleClassTable::get().find(_it,endpos,namesize);
	if (!mod){
		LsysSyntaxError(std::string("Invalid symbol '")+*_it+"' in AxialTree.","",lineno);
		// mod = ModuleClassTable::get().declare(*_it); ++_it; 
	}
	else {_it += namesize; }
	while(_it != endpos && (*_it == ' ' || *_it == '\t'))++_it;
	mod_id = mod->getId();
	bool has_arg = false;
	bool isNewExpression = false; 
	if (mod == ModuleClass::GetModule  ){
		if (_it != endpos && *_it != '('){
			has_arg = true;
			_it2 = _it;
			while(_it != endpos && (*_it != '=' && *_it != ' ' && *_it != '\t' && *_it != '\n')) ++_it;
			mod_args = std::string(_it2,_it);
			if (!production){
				while(_it != endpos && (*_it == ' ' || *_it == '\t'))++_it;
				if(_it == endpos || *_it != '=') LsysSyntaxError("Invalid declaration of pattern.","",lineno);
				++_it;
				while(_it != endpos && (*_it == ' ' || *_it == '\t'))++_it;
				_it2 = _it;
				parseAModule(_it,endpos,production,lineno);
				if(_it2 != _it);
				mod_args += ","+std::string(_it2,_it);
			}
			isNewExpression = true;
		}
	}
	if(!isNewExpression) {
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
			if(_it == endpos) LsysSyntaxError("Invalid syntax in AxialTree","",lineno);
			_it2 = _it;
			std::string::const_iterator lastcoma = _it2;
			int parenthesis = 0;
			while(_it != endpos && (*_it != ')' || parenthesis > 0)){
				switch(*_it){
					case ')': parenthesis--; break;
					case '(': parenthesis++; break;
					case '#': while(_it != endpos && *_it != '\n')++_it;break;
					case '"': while(_it != endpos && *_it != '"')++_it;break;
					case '\'': while(_it != endpos && *_it != '\'')++_it;break;
					case ',': lastcoma = _it;break;
					default: break;
				}
				++_it;
				if(_it == endpos)
					LsysSyntaxError("Invalid syntax in AxialTree","",lineno);
			}
			if(_it != endpos)_it++;
			bool without_unpacking = true;
			if(lastcoma!= endpos && production){
				// Simulation of unpacking of arguments
				if(*lastcoma == ',')++lastcoma;
				while(lastcoma != endpos && (*lastcoma == ' ' || *lastcoma == '\t') )++lastcoma;
				if(*lastcoma == '*'){
					std::string::const_iterator lit = lastcoma+1;
					if (LpyParsing::isValidVariableName(lastcoma+1,_it-1)){
						mod_args += std::string(_it2,lastcoma);
						mod_args += "PackedArgs(";
						mod_args += std::string(lastcoma+1,_it);
						without_unpacking = false;
					}
				}
			}
			if(without_unpacking)  mod_args += std::string(_it2,_it-1);
			has_arg = (mod_args.size() > 0);
		}
	}
	std::pair<size_t,std::string> amod;
	amod.first = mod_id;
	amod.second = mod_args;
	return amod;
}

std::vector<std::pair<size_t,std::string> > 
LpyParsing::parselstring( std::string::const_iterator& beg,
					  std::string::const_iterator endpos,
					  char delim,
					  int lineno,
					  bool production)
{
  // std::cerr << "parse '" << std::string(beg,endpos) << "'" << std::endl;
  std::vector<std::pair<size_t,std::string> > result;
  std::string::const_iterator _it = beg;
  while(_it != endpos && (*_it == ' ' || *_it == '\t'))_it++;
  while(_it != endpos && *_it != delim){
	if(*_it == '#') {// skip comments
		++_it;
		if(LsysContext::current()->warnWithSharpModule()){
		    while(_it != endpos && (*_it == ' ' || *_it == '\t'))++_it;
			if(_it != endpos && *_it == '(') LsysSyntaxError("Found symbol '#' after Lstring. Considered as begining of comments in Lpy (Compatibility pb with cpfg). Use '_' instead.","",lineno);
			else LsysWarning("Found symbol '#' after Lstring. Considered as begining of comments","",lineno);
		}
		while(_it != endpos && *_it != '\n' && *_it != delim)++_it;
	}
	else if(*_it == '(')
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
		result.push_back(parseAModule(_it,endpos,production,lineno));
	}
  } // end while
  beg = _it;
  return result;
}

/*---------------------------------------------------------------------------*/

LpyParsing::ModDeclarationList 
LpyParsing::parse_moddeclist(std::string::const_iterator& beg,
						  std::string::const_iterator endpos,
						  char delim)
{
  size_t nb = 0;
  ModDeclarationList result;
  std::string::const_iterator _it = beg;
  while(_it != endpos && *_it != delim){
	  ++nb; 
	  while (_it != endpos &&(*_it == ' ' || *_it == '\t'))++_it;
	  if (_it == endpos || *_it == delim || *_it == '\n' || *_it == ':' || *_it == '#') break;
	  if (nb != 1){
		  if(*_it != ',') LsysSyntaxError(std::string("Invalid syntax in module declaration: Wait for ','instead of '")+*_it+"'");
		  ++_it;
	  }
	  while (_it != endpos &&(*_it == ' ' || *_it == '\t'))++_it;
	  if (_it == endpos || *_it == delim || *_it == '\n' || *_it == ':' || *_it == '#') break;
	  std::string::const_iterator bm = _it;
	  while(_it != endpos && *_it != ','  && *_it != ' ' && *_it != '\t' 
		                  && *_it != '\n' && *_it != ':' && *_it != '=' 
		                  && *_it != '('  && *_it != '#'  && *_it != delim) ++_it;
	  std::string name(bm,_it);
	  if(name.empty())  LsysSyntaxError("Invalid empty name in declaration of "+TOOLS::number(nb)+" module.");
	  else { result.push_back(ModDeclaration(name)); }
	  while (_it != endpos && (*_it == ' ' || *_it == '\t'))++_it;
	  if(_it == endpos) break;
	  if(*_it != ',' && *_it != '\n' && *_it != ':' && *_it != '=' && *_it != '(' && *_it != '#')
		  LsysSyntaxError("Invalid syntax in module declaration: Wait for ',','=',':','(','#'.");
	  if(*_it == ':' || *_it == '#' || *_it == '\n') break;
	  switch(*_it){
		case '=':
			++_it;
			while (_it == endpos && (*_it == ' ' || *_it == '\t'))++_it;
			bm = _it;
			while (_it != endpos && (*_it == '_' || isalnum(*_it)))++_it;
			result.back().alias = true;
			result.back().parameters = std::string(bm,_it);
			break;
		case '(':
			bm = _it+1;
			_it = next_token(_it,endpos);
			if(*(_it-1) != ')')
				LsysSyntaxError("Invalid syntax in module declaration: Wait for ')'.");
			result.back().parameters = std::string(bm,_it-1);
			break;
		case ',':
		default:
			break;
	  }
 }
  beg = _it;
  return result;
}

/*---------------------------------------------------------------------------*/

LpyParsing::ModNameList 
LpyParsing::parse_modlist(std::string::const_iterator& beg,
						  std::string::const_iterator endpos,
						  char delim)
{
  bool first = true;
  ModNameList result;
  std::string::const_iterator _it = beg;
  while(_it != endpos && *_it != delim){
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	if (_it == endpos || *_it == delim || *_it == ':' || *_it == '#') break;
	if (!first){
		if(*_it != ',') LsysSyntaxError("Invalid syntax in module declaration: Wait for ','.");
		++_it;
		while (_it != endpos && (*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
		if (_it == endpos || *_it == delim)
			LsysSyntaxError("Invalid syntax in module declaration: Require module name.");
	}
	else first = false;
	std::string::const_iterator bm = _it;
	while(_it != endpos && *_it != ',' && *_it != ' ' && *_it != '\t' && *_it != '\n' && *_it != ':' && *_it != '=' && *_it != '#' && *_it != delim) ++_it;
	if (bm != _it) result.push_back(std::string (bm,_it));
  }
  beg = _it;
  return result;
}

/*---------------------------------------------------------------------------*/

LpyParsing::ModLineDeclatation 
LpyParsing::parse_moddeclaration_line(std::string::const_iterator& beg,
								 std::string::const_iterator endpos,
								 char delim)
{
  ModLineDeclatation result;
  std::string::const_iterator _it = beg;
  result.first = LpyParsing::parse_moddeclist(_it,endpos,delim);
  std::string scalevalue;
  if (_it != endpos && *_it == ':') {
    ++_it;
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	if (!has_pattern(_it,endpos,"scale")){
		LsysSyntaxError("Invalid module attribute in module declaration ('scale' missing)");
	}
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	if (_it == endpos || *_it == delim || *_it != '=')
		LsysSyntaxError("Invalid module attribute in module declaration ('=' missing).");
	else _it++;
	while ((*_it == ' ' || *_it == '\t' || *_it == '\n')&& *_it != delim)++_it;
	std::string::const_iterator bm = _it;
	while(_it != endpos && *_it != ',' && *_it != ' ' && *_it != '\t' && *_it != '\n' && *_it != '#' && *_it != delim) ++_it;
	if (bm != _it){
		scalevalue = std::string(bm,_it);
		result.second = scalevalue;
	}
  }
  beg = _it;
  return result;
}


/*---------------------------------------------------------------------------*/

std::string LpyParsing::trim(const std::string& str)
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

/*---------------------------------------------------------------------------*/

std::vector<std::string> LpyParsing::parse_arguments(std::string::const_iterator beg,
										 std::string::const_iterator end)
{
	std::vector<std::string> result;
    std::string::const_iterator _it = beg;
	std::string::const_iterator _it2 = _it;
	while(_it2 != end){
		while(_it2 != end && *_it2 != ',' && *_it2 != ')'){
			_it2 = next_token(_it2,end);
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

bool LpyParsing::isValidVariableName(std::string::const_iterator beg,
						 			 std::string::const_iterator end)
{
	if (beg == end)return false;
	while(beg != end && (*beg == ' ' || *beg == '\t'))++beg;
	if (beg == end)return false;
	if(*beg == '*' && MatchingEngine::getModuleMatchingMethod() != MatchingEngine::eMSimple){
		++beg;
		if(beg == end)return false;
		else {while(beg != end && (*beg == ' ' || *beg == '\t'))++beg; }
	}
	bool b = true;
	if(beg != end && (isalpha(*beg) || *beg == '_')) ++beg;
	else return false;
	while(beg != end && (b = (isalnum(*beg) || *beg == '_')))beg++;
	return b;
}

std::pair<std::string,std::string> LpyParsing::parse_variable(std::string::const_iterator beg,
						 						             std::string::const_iterator end,
															 int lineno)
{
	std::string::const_iterator it = beg;
	while (it != end && *it == ' ' && *it == '\t')++it;
	std::string::const_iterator begname = it;
	std::string::const_iterator begfilter = begname;
	if(it == end)LsysError("Error parsing variable name (1)","",lineno);
	if(*it == '-'){ ++it;  
		while (it != end && *it == ' ' && *it == '\t')++it;
		if(it == end) return std::pair<std::string,std::string>("-","");
		else LsysError("Error parsing variable name with '-' (3)","",lineno);
	}
	if(*it == '*'){ ++it; begfilter = it; }
	if(it != end && (isalpha(*it) || *it == '_'))++it;
	else LsysError("Error parsing variable name (2)","",lineno);
	while(it != end && (isalnum(*it) || *it == '_'))++it;
	std::string varname(begname,it);
	while (it != end && *it == ' ' && *it == '\t')++it;
	if(it == end) return std::pair<std::string,std::string>(varname,"");
	if (*it == 'i' && (++it) != end &&  *it == 'f'){
		while (it != end && *it == ' ' && *it == '\t')++it;
		if(it == end)LsysError("Error parsing filter of variable","",lineno);
		begfilter = it;
	}
	return std::pair<std::string,std::string>(varname,std::string(begfilter,end));
}


/*---------------------------------------------------------------------------*/

bool LpyParsing::isAConstant(std::string::const_iterator beg,
							 std::string::const_iterator end){    
	std::string::const_iterator it = beg;
	while (it != end && *it == ' ' && *it == '\t')++it;
	bool isd = false;
	if(*it == '+' || *it == '-' || *it == '.'  || (isd = isdigit(*it))){
		// look for a number
		++it;
		if(it == end) return isd;
		while(it != end && isdigit(*it))++it;
		if(it == end) return true;
		if(*it == '.') ++it;
		if(it == end) return true;
		while(it != end && isdigit(*it))++it;
		if(it == end) return true;
		if(*it == 'e') ++it;
		if(it == end) return false;
		if(*it == '+' || *it == '-') ++it;
		if(it == end) return false;
		while(it != end && isdigit(*it))++it;
		while (it != end && *it == ' ' && *it == '\t')++it;
		if(it == end) return true;
		else return false;
	}
	else if(std::distance(it,end) > 4 && std::string(it,end) == "None") return true;
	return false;
}