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
#include <plantgl/python/extract_list.h>
#include <QtCore/QFileInfo>
#include <initializer_list>

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
	        std::string::const_iterator beg,
	        std::string::const_iterator end,
			const std::string& pattern){

  size_t s = pattern.size();
  if ( (std::distance(pos,end) >= s) && 
	  (std::string(pos,pos+s) == pattern) ){
	pos+=s;
	return true;
  }
  else return false;
}

inline bool
has_pattern(const std::string& src,
			std::string::const_iterator& pos,
			const std::string& pattern){
 return has_pattern(pos,src.begin(),src.end(),pattern);
}

inline bool
has_keyword_pattern(std::string::const_iterator& pos,
	                std::string::const_iterator beg,
	                std::string::const_iterator end,
			        const std::string& pattern){
  size_t s = pattern.size();
  if (std::distance(pos,end) < s) return false;
  std::string::const_iterator res = pos+s;
  if ((std::string(pos,pos+s) == pattern)  && 
	  ( (pos==beg) || !isalnum(*(pos-1)) )  && 
	  ( (res==end) || !isalnum(*(res)) )
	 ){
	pos=res;
	return true;
  }
  else return false;
}

inline bool
has_keyword_pattern(const std::string& src,
			std::string::const_iterator& pos,
			const std::string& pattern){
 return has_keyword_pattern(pos,src.begin(),src.end(),pattern);
}

/*---------------------------------------------------------------------------*/

inline 
void ToEndline(std::string::const_iterator& _it, std::string::const_iterator _end,int& lineno){
	while( _it!=_end && (*_it)!='\n') ++_it;
    if(_it!=_end && (*_it)=='\n') { ++lineno; ++_it; }
}

inline 
void ToEndlineA(std::string::const_iterator& _it,std::string::const_iterator _end, int& lineno){
	while( _it!=_end && (*_it)!='\n' && (*_it)!='A' && (*_it)!='I' && (*_it)!='a' && (*_it)!='m' && (*_it)!='n' && (*_it)!='p' && (*_it)!='#' && (*_it)!='\'' && (*_it)!='"' ) ++_it;
    if(_it!=_end && (*_it)=='\n') 
    { 
        ++lineno; ++_it;
    }
}

#define toendline(a,b) ToEndline(a,b,lineno)
#define toendlineA(a,b) ToEndlineA(a,b,lineno)

/*---------------------------------------------------------------------------*/

inline 
bool notOnlySpace(std::string::const_iterator beg, std::string::const_iterator end){
	while( beg!=end) 
        if ( (*beg)!=' ' && (*beg)!='\t'&& (*beg)!='\n') return true;
        else ++beg;
	return false;
}

/*---------------------------------------------------------------------------*/
namespace {

using SourceIterator = std::string::const_iterator;

enum class Directive {
#define LPY_DIRECTIVE(id, keyword, payload) id,
#include "lsystem_directives.inc"
#undef LPY_DIRECTIVE
};

enum class DirectivePayload {
    ColonPayload,
    ValueAfterColon,
    ValueBeforeColon,
    SectionHeader,
    LineTail,
    ModuleDeclarations,
    ModuleNames,
    NamedSource,
    AxialPayload
};

struct DirectiveSyntax {
    const char* keyword;
    DirectivePayload payload;
};

const DirectiveSyntax& directiveSyntax(Directive directive)
{
    static constexpr DirectiveSyntax syntax[] = {
#define LPY_DIRECTIVE(id, keyword, payload) {keyword, DirectivePayload::payload},
#include "lsystem_directives.inc"
#undef LPY_DIRECTIVE
    };
    return syntax[static_cast<size_t>(directive)];
}

bool matchDirective(SourceIterator& it, SourceIterator begin, SourceIterator end,
                    Directive directive)
{
    return has_keyword_pattern(it, begin, end, directiveSyntax(directive).keyword);
}

SourceIterator findDirectiveColon(SourceIterator it, SourceIterator end)
{
    while (it != end && *it != ':' && *it != '\n') ++it;
    return it;
}

SourceIterator directiveLineEnd(SourceIterator it, SourceIterator end)
{
    const auto newline = std::find(it, end, '\n');
    return newline == end ? end : newline + 1;
}

enum class DirectiveValuePosition { BeforeColon, AfterColon };

struct DirectiveLine {
    SourceIterator valueBegin;
    SourceIterator valueEnd;
    SourceIterator end;
    bool hasColon;

    std::string value() const { return std::string(valueBegin, valueEnd); }
};

DirectiveLine readDirectiveLine(SourceIterator afterKeyword, SourceIterator end,
                                DirectiveValuePosition position = DirectiveValuePosition::AfterColon)
{
    const auto colon = findDirectiveColon(afterKeyword, end);
    if (colon == end || *colon != ':')
        return {colon, colon, colon, false};

    const auto lineEnd = directiveLineEnd(colon + 1, end);
    if (position == DirectiveValuePosition::BeforeColon)
        return {afterKeyword, colon, lineEnd, true};
    return {colon + 1, lineEnd, lineEnd, true};
}

struct ImportDirective {
    std::string moduleName;
    SourceIterator end;
};

ImportDirective readImportDirective(SourceIterator afterKeyword, SourceIterator end)
{
    const auto valueBegin = afterKeyword == end ? end : afterKeyword + 1;
    const auto lineEnd = directiveLineEnd(valueBegin, end);
    return {LpyParsing::trim(std::string(valueBegin, lineEnd)), lineEnd};
}

struct SectionLine {
    SourceIterator end;
    bool warnMissingColon;
};

SectionLine readSectionLine(SourceIterator afterKeyword, SourceIterator end)
{
    auto it = afterKeyword;
    bool warnMissingColon = true;
    // Preserve the legacy warning for section lines.
    while (it != end && *it != '\n') {
        ++it;
        if (it == end || *it != ':') warnMissingColon = false;
    }
    if (it != end) ++it;
    return {it, warnMissingColon};
}

} // namespace

// Read up to a delimiter outside Python strings, comments and nested groups.
static std::string::const_iterator
readPythonFragment(std::string::const_iterator it,
                   std::string::const_iterator end,
                   const std::string& delimiters,
                   int lineno = -1)
{
    std::vector<char> closing;
    while (it != end) {
        const char token = *it;
        if (closing.empty() && delimiters.find(token) != std::string::npos)
            return it;

        switch (token) {
        case '\'':
        case '"': {
            ++it;
            const bool triple = end - it >= 2 && *it == token && *(it + 1) == token;
            if (triple) it += 2;
            bool terminated = false;
            while (it != end) {
                if (*it == '\\') {
                    ++it;
                    if (it != end) ++it;
                }
                else if (*it == token &&
                         (!triple || (end - it >= 3 && *(it + 1) == token && *(it + 2) == token))) {
                    it += triple ? 3 : 1;
                    terminated = true;
                    break;
                }
                else ++it;
            }
            if (!terminated)
                LsysSyntaxError("Unterminated string in Python fragment.", "", lineno);
            continue;
        }
        case '#':
            while (it != end && *it != '\n') ++it;
            continue;
        case '(':
            closing.push_back(')');
            break;
        case '[':
            closing.push_back(']');
            break;
        case '{':
            closing.push_back('}');
            break;
        case ')':
        case ']':
        case '}':
            if (closing.empty() || closing.back() != token)
                LsysSyntaxError("Unexpected closing delimiter in Python fragment.", "", lineno);
            else closing.pop_back();
            break;
        }
        ++it;
    }
    if (!closing.empty())
        LsysSyntaxError("Unclosed delimiter in Python fragment.", "", lineno);
    return end;
}

/*---------------------------------------------------------------------------*/
std::string::const_iterator next_token(std::string::const_iterator _it2,
									   std::string::const_iterator end,
									   bool withbrackets = true)
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
				if(_it2 != end) {
					if(*_it2 == '(') ++nbOpenParenthesis;
					else if (*_it2 == ')') --nbOpenParenthesis;
					// skip strings
					else if(*_it2 == '"') { _it2++; while(_it2 != end && *_it2 != '"')_it2++; }
					else if(*_it2 == '\''){ _it2++; while(_it2 != end && *_it2 != '\'')_it2++; }
                }
			}
			if(_it2 != end)_it2++;
		  }
		 else if(withbrackets && *_it2 == '['){ // skip array
			int nbOpenBracket = 1;
			while(_it2 != end && (*_it2 != ']' || nbOpenBracket > 0)){
				_it2++;
				if(_it2 != end) {
					if(*_it2 == '[') ++nbOpenBracket;
					else if (*_it2 == ']') --nbOpenBracket;
					// skip strings
					else if(*_it2 == '"') { _it2++; while(_it2 != end && *_it2 != '"')_it2++; }
					else if(*_it2 == '\''){ _it2++; while(_it2 != end && *_it2 != '\'')_it2++; }
                }
			}
			if(_it2 != end)_it2++;
		  }
		  else if(withbrackets && *_it2 == '{'){ // skip dict
			int nbOpenBracket = 1;
			while(_it2 != end && (*_it2 != '}' || nbOpenBracket > 0)){
				_it2++;
				if(_it2 != end) {
					if(*_it2 == '{') ++nbOpenBracket;
					else if (*_it2 == '}') --nbOpenBracket;
					// skip strings
					else if(*_it2 == '"') { _it2++; while(_it2 != end && *_it2 != '"')_it2++; }
					else if(*_it2 == '\''){ _it2++; while(_it2 != end && *_it2 != '\'')_it2++; }
                }
			}
			if(_it2 != end)_it2++;
		  }
		  else _it2++;
	}
	return _it2;
}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

float LpyParsing::getFormatVersion(const std::string& lcode) {
  std::string::const_iterator it = lcode.begin();
  return getFormatVersion(it,it,lcode.end());
}

float LpyParsing::getFormatVersion(std::string::const_iterator& it, std::string::const_iterator begcode, std::string::const_iterator endcode) {
  // Retrieve of lpy format version
  float lpyversion = LPY_DEFAULT_FORMAT_VERSION;
  size_t p = VersionTag.find("%f");
  std::string::const_iterator _it = it;

  if (has_pattern(_it,begcode,endcode,std::string(VersionTag.begin(),VersionTag.begin()+p))) {
	  std::string::const_iterator _it2 = _it;
	  while(_it != endcode && (*_it != ' ' && *_it != '\t'  && *_it != '\n')) { ++_it; }
	  std::string::const_iterator _it3 = _it;
	  if (has_pattern(_it,begcode,endcode,std::string(VersionTag.begin()+p+2,VersionTag.end())) && (_it != endcode && *_it == '\n')){
		lpyversion = atof(std::string(_it2,_it3).c_str());
		it = _it+1;
	  }
  }
  return lpyversion;
}

/*---------------------------------------------------------------------------*/


#define PROCESS_RULE(rulecode,code,addedcode,mode,group,consider) \
	LsysRule& r = __addRule(rulecode,mode,group,lineno - std::count(rule.begin(),rule.end(),'\n'),consider); \
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

#include <fstream>

void 
Lsystem::set( const std::string&   _rules , std::string * pycode, 
			  const boost::python::dict& parameters){
  ACQUIRE_RESSOURCE
  std::string filename = getFilename();
  __clear();
  if(!filename.empty())setFilename(filename);
  filename = getShortFilename();
//  printf("A\n");
  int lineno = 1;
  std::string _nrules;
  {
      std::string::const_iterator lbeg = _rules.begin();
      for(std::string::const_iterator _it = lbeg; _it != _rules.end(); ++_it){
        if (*_it == '%'){
            std::string::const_iterator _it2 = _it;
            if(matchDirective(_it,_rules.begin(),_rules.end(),Directive::Pastefile) ) {
                _nrules.append(lbeg, _it2);
                _it++;
                std::string::const_iterator begname = _it;
                toendline(_it,_rules.end());
                if(begname != _rules.end()) {
                    if (notOnlySpace(begname,_it)){
                      std::string modulename = LpyParsing::trim(std::string(begname,_it));
                      std::ifstream subfile(modulename.c_str());
                      std::stringstream buffer; 
                      buffer << subfile.rdbuf();
                      subfile.close();
                      _nrules += buffer.str();
                    }
                    else LsysParserSyntaxError("invalid file to paste");
                }
                else LsysParserSyntaxError("invalid file to paste");
                lbeg = _it;
            }
			else if(matchDirective(_it,_rules.begin(),_rules.end(),Directive::Pastemodule) ) {
                _nrules.append(lbeg, _it2);
                _it++;
                std::string::const_iterator begname = _it;
                toendline(_it,_rules.end());
                if(begname != _rules.end()) {
                    if (notOnlySpace(begname,_it)){
                      std::string modulename = LpyParsing::trim(std::string(begname,_it));
					  boost::python::object lpymodulefinder = boost::python::import("openalea.lpy").attr("determine_lpymodule_path");
					  modulename = boost::python::call<std::string>(lpymodulefinder.ptr(), boost::python::object(modulename));
                      std::ifstream subfile(modulename.c_str());
                      std::stringstream buffer; 
                      buffer << subfile.rdbuf();
                      subfile.close();
                      _nrules += buffer.str();
                    }
                    else LsysParserSyntaxError("invalid file to paste");
                }
                else LsysParserSyntaxError("invalid file to paste");
                lbeg = _it;
            }				
        }
      }
      _nrules.append(lbeg, _rules.end());
  }


#ifndef _WIN32
  std::string _rules_ = _nrules;
  for(std::string::iterator _itEr = _rules_.begin(); _itEr != _rules_.end(); ++_itEr)
  	if (*_itEr == WindowSpecificEndline) _rules_.erase(_itEr);
  const std::string& rules = _rules_;
#else
  const std::string& rules = _nrules;
#endif

  ContextMaintainer m(&__context);

  std::string::const_iterator begcode = rules.begin();
  std::string::const_iterator _it = begcode;
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
  bool axiom_is_function = false;
  int max_derivation_lineno = -1;
  int decomposition_max_depth_lineno = -1;
  int homomorphism_max_depth_lineno = -1;
  int omode = -1;
  int mode = -1;
  lineno = 1;
  int group = 0;
  ConsiderFilterPtr currentConsider;
  // Retrieve of lpy format version
  float lpyversion = LpyParsing::getFormatVersion(_it2,begcode,endpycode);
  if (!LpyParsing::isSupportedFormat(lpyversion)){
	  std::stringstream stream;
	  stream << "Not supported lpy format : " << lpyversion << ". Supported = " << LpyParsing::LPY_FORMAT_VERSION << ".";
	  LsysError(stream.str(),filename,0);
  }
  // version has been found. lineno should be increment.
  else if (_it != _it2) { _it = _it2; ++lineno; }
  
  //  context initialisation
  size_t initpos = __context.__initialiseFrom(rules);
  if (len(parameters) > 0){
	  __context.updateNamespace(parameters);
  }

  if (initpos != std::string::npos) endpycode = rules.begin()+initpos;

  const auto readValueDirective = [&](Directive id) {
    const auto& syntax = directiveSyntax(id);
    const auto position = syntax.payload == DirectivePayload::ValueBeforeColon
        ? DirectiveValuePosition::BeforeColon : DirectiveValuePosition::AfterColon;
    const bool requireValue = syntax.payload == DirectivePayload::ValueAfterColon ||
                              syntax.payload == DirectivePayload::ValueBeforeColon;
    const DirectiveLine directive = readDirectiveLine(_it, endpycode, position);
    if (!directive.hasColon) {
      const std::string expected = position == DirectiveValuePosition::BeforeColon
          ? "Cannot find value and ':' after " : "Cannot find ':' after ";
      LsysParserSyntaxError(expected + syntax.keyword);
    }
    else if (requireValue && !notOnlySpace(directive.valueBegin, directive.valueEnd)) {
      // Values after the colon were diagnosed after consuming the line.
      const bool afterNewline = position == DirectiveValuePosition::AfterColon &&
                                *(directive.end - 1) == '\n';
      LsysSyntaxError(std::string("Cannot find value for ") + syntax.keyword, filename, lineno + afterNewline);
    }
    return directive;
  };

  const auto consumeDirective = [&](SourceIterator end) {
    if (_it != end && *(end - 1) == '\n') ++lineno;
    _it = end;
  };

  const auto enterRuleSection = [&](int section, const char* commentPrefix) {
    const auto directive = readSectionLine(_it, endpycode);
    if (directive.warnMissingColon) LsysParserWarning("Colon missing.");
    consumeDirective(directive.end);
    code += commentPrefix + std::string(_it2, _it);
    beg = _it;
    mode = section;
  };

  while(_it!=endpycode){
	// printf("******'%c' %i\n",*_it,std::distance(begcode,_it));
	switch(mode){
	case -1:
	  {
		switch(*_it){
	    case '#':
			while(_it != endpycode && *_it != '\n')++_it;
			break;
	    case '\'':
			++_it;
			while(_it != endpycode && *_it != '\'')++_it;
			if (_it != endpycode) ++_it;
			break;
	    case '"':
			++_it;
			while(_it != endpycode && *_it != '"')++_it;
			if (_it != endpycode) ++_it;
			break;
		case 'a':
		case 'A':
		  _it2 = _it;
		  if((matchDirective(_it, begcode, endpycode, Directive::Axiom) ||
                  matchDirective(_it, begcode, endpycode, Directive::LowercaseAxiom)) &&
			 (_it2 == rules.begin() || *(_it2-1) == '\n')){
            code += std::string(beg,_it2);
			_it = findDirectiveColon(_it, endpycode);
			if(_it!=endpycode && (*_it)==':'){
			  _it++;
              axiom_lineno = lineno;
              std::string axiomcode = LsysContext::AxiomVariable + " = ";
              int axiomsize = 0;
              axiomcode += LpyParsing::lstring2py(_it,endpycode,'\n',lineno,&axiomsize);
              ++lineno;
              ++_it;
              if (axiomsize == 0 && (*_it == '\t' || *_it == ' ')){
                  code += "def "+LsysContext::AxiomVariable+"() :\n";
                  axiom_is_function = true;
              }
              else {
                  code += axiomcode + *(_it-1);
              }

			}
            else LsysParserSyntaxError("Cannot find ':' after Axiom");
			beg = _it;
 		  }
          else {if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'n':
		  _it2 = _it;
          if(matchDirective(_it,begcode,endpycode,Directive::Nproduce)){
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
          if(matchDirective(_it,begcode,endpycode,Directive::Module)){
            code+=std::string(beg,_it2);
			LpyParsing::ModLineDeclaration modules = LpyParsing::parse_moddeclaration_line(_it,endpycode);
			int scale = ModuleClass::DEFAULT_SCALE;
			ModuleClassList inheritance;
			pgl_hash_map_string<boost::python::object> properties;
			// for (pgl_hash_map_string<std::string>::iterator itmeta = modules.second.begin(); itmeta != modules.second.end(); ++itmeta)
			//	printf("'%s' = '%s'\n",itmeta->first.c_str(),itmeta->second.c_str());
			if(!modules.second.empty()){
				pgl_hash_map_string<std::string>::iterator it = modules.second.find("scale");
				if (it != modules.second.end()) {
					scale = extract<int>(__context.evaluate(it->second))();
					modules.second.erase(it);
				}
				it = modules.second.find("base");
				if (it != modules.second.end()) {
					std::string inherit = LpyParsing::removeSpaces(it->second);
					if(!inherit.empty()) {
						if (inherit[0] == '[' || inherit[0] == '('){
							inherit.insert(inherit.begin()+1,'\'');
							size_t len = inherit.size();
							if (inherit[len-1] != ']' && inherit[len-1] != ')')
								LsysError("Invalid base modules '"+it->second+"'","",lineno);
							inherit.insert(inherit.end()-1,'\'');
							std::string::iterator itinh = inherit.begin();
							size_t nbelem = 0;
							while (itinh != inherit.end()){
								itinh = std::find<std::string::iterator>(itinh,inherit.end(),',');
								if (itinh != inherit.end()){
									nbelem += 1;
									inherit.insert(itinh+1,'\'');
									inherit.insert(itinh,'\'');
									itinh += 2;
								}
							}
							if (nbelem == 0) {
								inherit.insert(inherit.end()-1,',');
							}
						}
						else {
							inherit.insert(inherit.begin(),'\'');
							inherit.insert(inherit.begin(),'[');
							inherit.insert(inherit.end(),'\'');
							inherit.insert(inherit.end(),']');
						}
						std::vector<std::string> inheritedclassnames = extract_vec<std::string>(__context.evaluate(inherit))();
						for(std::vector<std::string>::const_iterator iticl = inheritedclassnames.begin(); iticl != inheritedclassnames.end(); ++iticl){
							ModuleClassPtr bsclass = ModuleClassTable::get().find(*iticl);
							if (!bsclass) LsysError("Undefined base class '"+*iticl+"'.","",lineno);
							inheritance.push_back(bsclass);
						}
					}
					modules.second.erase(it);
				}
			}
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
					// clean parameter set in case it has none declared but only some from inheritance
					else mod->setParameterNames(std::vector<std::string>());
				}
				else {
					mod = ModuleClassTable::get().alias(itmod->name,itmod->parameters);
					__context.declareAlias(itmod->name,mod);
				}
				if(scale != ModuleClass::DEFAULT_SCALE)mod->setScale(scale);
				if(!inheritance.empty())mod->setBases(inheritance);
			}
            for(LpyParsing::ModDeclarationList::const_iterator itmod = modules.first.begin(); 
                 itmod != modules.first.end(); ++itmod){
                if (itmod != modules.first.begin()) code += " ; ";
                if (LpyParsing::isValidVariableName(itmod->name))
                    code += itmod->name+" = ModuleClass.get('"+itmod->name+"')";
            }
            code+=" # "+std::string(_it2,_it);
			beg = _it;
			toendlineA(_it,endpycode);
		  }
		  else if(matchDirective(_it,begcode,endpycode,Directive::Makestring)){
			code += std::string(beg,_it2) + "AxialTree(";
		    while(_it != endpycode && (*_it == ' ' || *_it == '\t') )++_it;
		    char endproduction = '\n';
		    if(*_it == '(') { endproduction = ')'; ++_it; }
			if(_it!=endpycode){
			  code += LpyParsing::lstring2py(_it,endpycode,endproduction,lineno);
			  if(_it!=endpycode)_it++;
			}
			code += ')';
            if (endproduction == '\n') code += '\n';
			beg = _it;
			toendlineA(_it,endpycode);
		  }
          else {if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'u':
		  _it2 = _it;
          if(matchDirective(_it,begcode,endpycode,Directive::Undeclare)){
            code+=std::string(beg,_it2);
			LpyParsing::ModNameList modules = LpyParsing::parse_modlist(_it,endpycode,false);
			code+=" # "+std::string(_it2,_it);
			for(LpyParsing::ModNameList::const_iterator itmod = modules.begin(); 
				 itmod != modules.end(); ++itmod){
				ModuleClassPtr mod = ModuleClassTable::get().find(*itmod);
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
		  if(matchDirective(_it,begcode,endpycode,Directive::Production)){
			// printf("-'%c' '%c'\n",*(_it2-1),*_it);
			/*if ((_it2 != begcode && *(_it2-1) != '\n') || (_it == *_it != ' ' && *_it != ':' && *_it != '\t')){
				// We are not in the production statement
				if(_it!=endpycode)++_it; toendlineA(_it,endpycode);
			}
			else {*/
 				code+=std::string(beg,_it2);
				enterRuleSection(0, "# ");
			// }
		  }
		  else if(matchDirective(_it,begcode,endpycode,Directive::Produce)){
			  // LsysWarning("Cannot use 'produce' outside production body. Use 'nproduce' instead.",filename,lineno);
			code += std::string(beg,_it2);
			code += "return pproduce";
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
		  else { if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
		  break;
		case 'd':
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::DerivationLength)){
            const auto directive = readValueDirective(Directive::DerivationLength);
            code.append(beg, _it2);
            max_derivation_lineno = lineno;
            consumeDirective(directive.end);
            code += LsysContext::DerivationLengthVariable + " = " + directive.value();
            beg = _it;
		  }
		  else toendlineA(_it,endpycode);
		  break;
		case 'l':
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Lpyimport)){
            const auto directive = readImportDirective(_it, endpycode);
            code.append(beg, _it2);
            consumeDirective(directive.end);
            if (directive.moduleName.empty())
                LsysParserSyntaxError("invalid module to import");
            else
                addSubLsystem(directive.moduleName + ".lpy");
            code += "# " + std::string(_it2, _it);
            beg = _it;
		  }
		  else toendlineA(_it,endpycode);
		  break;
		case 'c':
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Consider)){
            const auto directive = readValueDirective(Directive::Consider);
            code.append(beg, _it2);
            consumeDirective(directive.end);
            if (directive.valueBegin == endpycode)
                LsysParserSyntaxError("Cannot find value for consider");
            currentConsider = ConsiderFilter::consider(directive.value());
            code += "# " + std::string(_it2, _it);
            beg = _it;
		  }
		  else toendlineA(_it,endpycode);
		  break;
		case 'e':
          _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Endlsystem)){
            LsysParserSyntaxError("endlsystem found before production statement.");
		  }
          else toendlineA(_it,endpycode);
		  break;
		case 'i':
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Ignore)){
            const auto directive = readValueDirective(Directive::Ignore);
            code.append(beg, _it2);
            consumeDirective(directive.end);
            if (directive.valueBegin == endpycode)
                LsysParserSyntaxError("Cannot find value for ignore");
            currentConsider = ConsiderFilter::ignore(directive.value());
            code += "# " + std::string(_it2, _it);
            beg = _it;
		  }
		  else toendlineA(_it,endpycode);
		  break;
		case 'I': // python comments before rules
		  _it2 = _it;
			if (has_keyword_pattern(_it,begcode,endpycode,"InLeftContext") || has_keyword_pattern(_it,begcode,endpycode,"InRightContext")){
				code+=std::string(beg,_it2);
				while(_it != endpycode && (*_it == ' ' || *_it == '\t') )++_it;
		
				if(*_it != '(') LsysParserSyntaxError("Cannot find opening bracket of InContext");
				std::string::const_iterator eit = next_token(_it,endpycode);
				std::string::const_iterator epit = eit-1;
				while (epit != _it && *epit != ',') {
					--epit;
					if (epit != _it && *epit == ')') 
						while (epit != _it && *epit != '(') --epit;
				}
				if (epit == _it){
					LsysParserWarning("No parameter dictionnay found in arguments. ");
					epit = eit-1;
				}

				PatternString q(std::string(_it+1,epit),lineno);
				size_t pid = PatternStringManager::get().register_pattern(q);
				code += std::string("p")+std::string(_it2,_it+1)+TOOLS(number)(pid)+std::string(epit,eit);
				beg = eit;
			}
			else { if(_it!=endpycode)++_it; toendlineA(_it,endpycode); }
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
		  if(matchDirective(_it,begcode,endpycode,Directive::Endlsystem)){
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
			}
			consumeDirective(directiveLineEnd(_it, endpycode));
            code+="# "+std::string(_it2,_it);
			mode = -1;
            beg = _it;
		  }
		  else if(matchDirective(_it,begcode,endpycode,Directive::Endgroup)){
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
			}
			consumeDirective(directiveLineEnd(_it, endpycode));
            code+="# "+std::string(_it2,_it);
			group = 0;
            beg = _it;
		  }
		  else {
			if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
			}
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'd':
		  if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
		  if(mode == 0 && matchDirective(_it,begcode,endpycode,Directive::DerivationLength)){
            const auto directive = readValueDirective(Directive::DerivationLength);
            max_derivation_lineno = lineno;
            consumeDirective(directive.end);
            code += LsysContext::DerivationLengthVariable + " = " + directive.value();
		  }
		  else if(matchDirective(_it,begcode,endpycode,Directive::Decomposition)){
			enterRuleSection(1, "#");
		  }
		  else if(has_keyword_pattern(_it,begcode,endpycode,"def")){
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
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
          if(matchDirective(_it,begcode,endpycode,Directive::Homomorphism)){
			enterRuleSection(2, "# ");
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'g':
		  if(!rule.empty()){
              PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Group)){
            const auto directive = readValueDirective(Directive::Group);
            group = __context.readInt(directive.value());
            if (group < 0) {
                std::stringstream message;
                message << "Invalid value for group : " << group << '.';
                LsysError(message.str(), filename, lineno);
            }
            consumeDirective(directive.end);
            code += "# " + std::string(_it2, _it);
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'p':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Production)){
			enterRuleSection(0, "# ");
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'm':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
		  if((mode == 1||mode == 2) && matchDirective(_it,begcode,endpycode,Directive::MaximumDepth)){
            const auto directive = readValueDirective(Directive::MaximumDepth);
            const bool decomposition = mode == 1;
            (decomposition ? decomposition_max_depth_lineno : homomorphism_max_depth_lineno) = lineno;
            consumeDirective(directive.end);
            const auto& variable = decomposition ? LsysContext::DecompositionMaxDepthVariable
                                                 : LsysContext::HomomorphismMaxDepthVariable;
            code += variable + " = " + directive.value();
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'c':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Consider)){
            const auto directive = readValueDirective(Directive::Consider);
            consumeDirective(directive.end);
            code += "# " + std::string(_it2, _it);
            if (directive.valueBegin == endpycode) currentConsider = ConsiderFilterPtr();
            else currentConsider = ConsiderFilter::consider(directive.value());
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case 'i':
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
		  }
		  _it2 = _it;
		  if(matchDirective(_it,begcode,endpycode,Directive::Ignore)){
            const auto directive = readValueDirective(Directive::Ignore);
            consumeDirective(directive.end);
            code += "# " + std::string(_it2, _it);
            if (directive.valueBegin == endpycode) currentConsider = ConsiderFilterPtr();
            else currentConsider = ConsiderFilter::ignore(std::string(directive.valueBegin, directive.valueEnd - 1));
		  }
		  if(matchDirective(_it,begcode,endpycode,Directive::Interpretation)){
			enterRuleSection(2, "# ");
		  }
		  else {
			beg = _it;
			toendline(_it,endpycode);
			rule += std::string(beg,_it);
		  }
		  break;
		case '#': // python comments before rules
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
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
#ifndef _WIN32
		case WindowSpecificEndline: // Window end line style
#endif
		case '\n':		  
		  if(!rule.empty()){
			  beg = _it;
			  size_t newlineno = lineno;
			  while(_it != endpycode && *_it == '\n') {++newlineno; ++_it; }
			  if (_it != endpycode && (*_it == ' ' || *_it == '\t')){
				  LsysParserWarning("IndentationWarning: missing indent.");
				  rule +=  std::string(beg,_it); 
			  }
			  else { 
				PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
				code += std::string(beg,_it);
			  }
			  lineno = newlineno;
		  }
		  else { code += *_it; ++lineno; ++_it; }
          // std::cerr << lineno << ':' << std::distance<std::string::const_iterator>(rules.begin(),_it)  << std::endl;
          break;
		default:
		  if(!rule.empty()){
            PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
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
    PROCESS_RULE(rule,code,addedcode,mode,group,currentConsider)
  }
  if (mode == -1)
   code.append(beg,_it);
  if (!addedcode.empty())
	code+='\n'+addedcode;
  if(pycode) *pycode = code;
  // printf("%s",code.c_str());
  try {
    __context.compile(code);
  }
  catch (const error_already_set& e) {
    if (PyErr_ExceptionMatches(PyExc_SyntaxError)){
        // PyErr_SyntaxLocation(getFilename().c_str(), 0);
    }
    boost::python::throw_error_already_set();
  //  boost::python::handle_exception();
  }
  __importPyFunctions();
  if (__context.hasObject(LsysContext::AxiomVariable)){
      if (!axiom_is_function){
          try
          { __axiom = AxialTree(extract<boost::python::list>(__context.getObject(LsysContext::AxiomVariable))); }
          catch(error_already_set const &)
          { 
		      PyErr_Clear();
              LsysError("Axiom has an invalid value.",filename,axiom_lineno); 
          }
      }
      else {
          // Execute function axiom
           __context.func(LsysContext::AxiomVariable);
           __axiom =  __context.get_nproduction();
           __context.reset_nproduction();
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
      { __interpretation_max_depth = extract<int>(__context.getObject(LsysContext::HomomorphismMaxDepthVariable)); }
      catch(error_already_set const &)
      { 
          PyErr_Clear();
          extract<float> t(__context.getObject(LsysContext::HomomorphismMaxDepthVariable)); 
          if (t.check()) {
              __interpretation_max_depth = (int)t();
              LsysWarning("Floating point value is given for homomorphism maximum length. Rounding.",filename,homomorphism_max_depth_lineno); 
          }
          else LsysError("Homomorphism maximum length has an invalid value.",filename,homomorphism_max_depth_lineno); 
      }
  }
  __context.check_init_functions();

  if (__context.axiomDecompositionEnabled())
    __axiom = __decompose(__axiom);

  RELEASE_RESSOURCE
}

/*---------------------------------------------------------------------------*/

namespace {

struct RuleHeader {
    std::string predecessor;
    std::string leftContext;
    std::string newLeftContext;
    std::string rightContext;
    std::string newRightContext;
    std::string::const_iterator body;
    bool arrow = false;
    bool isStatic = false;
};

using RuleHeaderField = std::string RuleHeader::*;

struct RuleHeaderProduction {
    std::initializer_list<const char*> separators;
    std::initializer_list<RuleHeaderField> fields;
};

const RuleHeaderProduction ruleHeaderProductions[] = {
#include "rule_header_productions.inc"
};

RuleHeader readRuleHeader(const std::string& rule, int lineno)
{
    RuleHeader header;
    const auto end = rule.end();
    header.body = end;
    auto section = rule.begin();
    auto it = section;
    std::vector<std::string> separators;
    std::vector<std::string> patterns;
    const std::string staticArrow = "-static->";
    const std::string staticMarker = "@static";

    while (it != end) {
        const auto offset = it - rule.begin();
        if (*it == ':') {
            header.body = it + 1;
            auto marker = header.body;
            while (marker != end && (*marker == ' ' || *marker == '\t')) ++marker;
            if (rule.compare(marker - rule.begin(), staticMarker.size(), staticMarker) == 0) {
                header.body = marker + staticMarker.size();
                header.isStatic = true;
            }
            break;
        }
        if (*it == '-' && (it == rule.begin() || *(it - 1) != '-')) {
            if (rule.compare(offset, 3, "-->") == 0) {
                header.body = it + 3;
                header.arrow = true;
                break;
            }
            if (rule.compare(offset, staticArrow.size(), staticArrow) == 0) {
                header.body = it + staticArrow.size();
                header.arrow = true;
                header.isStatic = true;
                break;
            }
        }
        if (*it == '(') {
            it = readPythonFragment(it + 1, end, ")", lineno);
            if (it != end) ++it;
            continue;
        }
        if (*it != '<' && *it != '>') {
            it = next_token(it, end, false);
            continue;
        }

        const char separator = *it;
        const bool doubled = it + 1 != end && *(it + 1) == separator;
        patterns.emplace_back(section, it);
        separators.emplace_back(doubled ? 2 : 1, separator);
        it += doubled ? 2 : 1;
        section = it;
    }

    if (it == end) {
        LsysError("Ill-formed Rule : unfound delimiter ':' in " + rule, "", lineno);
        return header;
    }

    patterns.emplace_back(section, it);
    for (const auto& production : ruleHeaderProductions) {
        if (!std::equal(separators.begin(), separators.end(),
                        production.separators.begin(), production.separators.end()))
            continue;
        auto pattern = patterns.begin();
        for (const auto field : production.fields)
            header.*field = std::move(*pattern++);
        if (header.predecessor.empty())
            LsysError("Ill-formed Rule Header : No Predecessor found : " + rule, "", lineno);
        return header;
    }
    LsysError("Ill-formed Rule Header : " + rule, "", lineno);
    return header;
}

} // namespace

void LsysRule::set(const std::string& rule)
{
  const RuleHeader header = readRuleHeader(rule, lineno);
  __codelength = std::count(rule.begin(), rule.end(), '\n');
  __predecessor = PatternString(header.predecessor, lineno);
  if (!header.leftContext.empty()) __leftcontext = PatternString(header.leftContext, lineno);
  if (!header.newLeftContext.empty()) __newleftcontext = PatternString(header.newLeftContext, lineno);
  if (!header.rightContext.empty()) __rightcontext = PatternString(header.rightContext, lineno);
  if (!header.newRightContext.empty()) __newrightcontext = PatternString(header.newRightContext, lineno);
  __definition.assign(header.body, rule.end());
  if (header.arrow) __definition.insert(0, " --> ");
  __hasquery = __predecessor.hasRequestModule() 
			|| __newleftcontext.hasRequestModule()
			|| __leftcontext.hasRequestModule()
			|| __newrightcontext.hasRequestModule()
			|| __rightcontext.hasRequestModule();
  // check variables
  if (header.isStatic) setStatic();
  else {
	if (LsysContext::current()->optimizationLevel >= 2)
		keepOnlyRelevantVariables();
	parseParameters();
  }
}

/*---------------------------------------------------------------------------*/

std::string 
LsysRule::getCoreCode() {
  std::stringstream res;
  int llineno = 0;
  std::string definition;
  std::string::const_iterator _beg = __definition.begin();
  std::string::const_iterator _end = __definition.end();
  std::string::const_iterator _lastit = __definition.begin();
  std::string::const_iterator _it = _lastit;
  while( _it != _end){
	std::string::const_iterator _cit = _it;
	if (*_it == '\n') { ++llineno; _it++;}
	else if (*_it == '#') { while(_it != _end && *_it != '\n') _it++; }
	else if (*_it == '\'') 
	{ 
	  _it++;  
	  while(_it != _end && *_it != '\'') { if (*_it == '\n') { ++llineno; } _it++; } 
	  if (_it != _end) _it++;  
	}
	else if (*_it == '"') 
	{ 
		_it++;  
		while(_it != _end && *_it != '"') { if (*_it == '\n') { ++llineno; } _it++; } 
	    if (_it != _end) _it++;  
	}
	else if (*_it == '-' && std::distance<std::string::const_iterator>(_it,_end)> 3){
	  if(std::string(_it,_it+3) == "-->"){
		definition.insert(definition.end(),_lastit,_it);
		_it += 3;
		size_t pprod_id;
		definition += "return ";
		definition += "pproduce";
		definition += LpyParsing::lstring2pyparam(_it,_end,'\n',lineno==-1?lineno:lineno+llineno,&pprod_id);
		_lastit = _it;
		if (LsysContext::current()->optimizationLevel >= 1 &&!ParametricProduction::get(pprod_id)->hasArgs()) 
			setStatic();
	  }
	  else _it++;
	}
	else if (has_keyword_pattern(_it,_beg,_end,"produce") ) {
		// if (*_it == 'p' && std::distance<std::string::const_iterator>(_it,_end)> 7 && (_it == _beg || !isalnum(*(_it-1)) && *(_it-1) != '_')){
	    // if(std::string(_it,_it+7) == "produce"){
		definition.insert(definition.end(),_lastit,_cit);
		while(_it != _end && (*_it == ' ' || *_it == '\t') )++_it;
		char endproduction = '\n';
		if(*_it == '(') { endproduction = ')'; ++_it; }
		definition += "return ";
		definition += "pproduce";
		definition += LpyParsing::lstring2pyparam(_it,_end,endproduction,lineno==-1?lineno:lineno+llineno);
		if (endproduction == ')') ++_it;
		_lastit = _it;
	  // }
	  // else _it++;
	}
	else if(*_it == 'm' && std::distance<std::string::const_iterator>(_it,_end)> 10){
	  if(std::string(_it,_it+10) == "makestring"){
		 definition.insert(definition.end(),_lastit,_it);
		 _it += 10;
		 while(_it != _end && (*_it == ' ' || *_it == '\t') )++_it;
		 char endproduction = '\n';
		 if(*_it == '(') { endproduction = ')'; ++_it; }
		 definition += "AxialTree(";
         definition += LpyParsing::lstring2py(_it,_end,endproduction,lineno==-1?lineno:lineno+llineno);
         definition += ')';
 		 if (endproduction == ')') ++_it;
		 _lastit = _it;
      }
      else _it++;
    }
	else if(*_it == 'n' && std::distance<std::string::const_iterator>(_it,_end)> 8){
	  if(std::string(_it,_it+8) == "nproduce"){
		definition.insert(definition.end(),_lastit,_it);
		_it += 8;
		while(_it != _end && (*_it == ' ' || *_it == '\t') )++_it;
		char endproduction = '\n';
		if(*_it == '(') { endproduction = ')'; ++_it; }
		definition += "pproduce";
		definition += LpyParsing::lstring2pyparam(_it,_end,endproduction,lineno==-1?lineno:lineno+llineno);
		if (endproduction == ')') ++_it;
		_lastit = _it;
	  }
	  else _it++;
	}
    else if (has_keyword_pattern(_it,_beg,_end,"InLeftContext") || has_keyword_pattern(_it,_beg,_end,"InRightContext")){
		definition.insert(definition.end(),_lastit,_cit);
		while(_it != _end && (*_it == ' ' || *_it == '\t') )++_it;
		
		if(*_it != '(') LsysError("Cannot find opening bracket of InContext","",lineno==-1?lineno:lineno+llineno);
		std::string::const_iterator eit = next_token(_it,_end);
		std::string::const_iterator epit = eit-1;
		while (epit != _it && *epit != ',') {
			--epit;
			if (epit != _it && *epit == ')') 
				while (epit != _it && *epit != '(') --epit;
		}
		if (epit == _it){
			LsysWarning("No parameter dictionnay found in arguments. ","",lineno==-1?lineno:lineno+llineno);
			epit = eit-1;
		}

		PatternString q(std::string(_it+1,epit),lineno==-1?lineno:lineno+llineno);
		size_t pid = PatternStringManager::get().register_pattern(q);
		definition += std::string("p")+std::string(_cit,_it+1)+TOOLS(number)(pid)+std::string(epit,eit);
		_lastit = eit;
    }
	else _it++;
  }
  if(_lastit!=_it)definition += std::string(_lastit,_it);
  if(definition.empty()) res << "\t\tpass\n";
  else {
	res << definition;
	if (*(_end-1) != '\t' && *(_end-1) != '\n') res << "\n";
  }
  std::stringstream head;
  head << "def " << functionName() << "(";

  if(__prefix == 'h' && LsysContext::current()->turtle_in_interpretation && !__isStatic){ head << "turtle," ; } 
  if(!__formalparameters.empty())
    for(std::vector<std::string>::const_iterator _it = __formalparameters.begin();
    _it != __formalparameters.end(); ++_it){
	     if(_it != __formalparameters.begin()) 
            head << ',';
	    head << *_it;
  }
  head << ") :"; //  #" << name() << "\n";
  return head.str() + res.str();
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
								    char delim, int lineno, int * nbModules){
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
  if(nbModules) *nbModules = parsedstring.size();
  return result;
}

std::string LpyParsing::lstring2pyparam( std::string::const_iterator& beg,
								    std::string::const_iterator endpos,
								    char delim, int lineno,
									size_t * pprod_id){
  std::string result;
  std::string::const_iterator initbeg = beg;
  std::vector<std::pair<size_t,std::string> > parsedstring = parselstring(beg, endpos, delim, lineno,true);
  ParametricProductionPtr pprod = ParametricProduction::create();
  if(parsedstring.empty()){ 
	  pprod->append_module_type("None");
  }
  else {
	  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
		  it != parsedstring.end(); ++it){
			  if (it->first == ModuleClass::GetModule->getId()){
				  // in the case of production $ is only followed by a var name.
				  pprod->append_variable_module();
				  result += "," + it->second;
			  }
			  else {
				  pprod->append_module_type(it->first);
				  if (!it->second.empty()) {
					  std::vector<std::string> args = parse_arguments(it->second);
					  for(std::vector<std::string>::const_iterator itArg = args.begin(); itArg != args.end(); ++itArg){
						  if(isAConstant(*itArg)){
							  pprod->append_module_value(LsysContext::current()->evaluate(*itArg));
						  }
						  else {
							  std::string m = "PackedArgs(";                                
							  if(itArg->size() > m.size() && std::string(itArg->begin(),itArg->begin()+m.size()) == m) {
								pprod->append_module_star_variable();
								result += "," + std::string(itArg->begin()+m.size(),itArg->end()-1); 
							  }
							  else {
								pprod->append_module_variable();
								result += "," + *itArg;
							  }
						  }
					  }
				  }
			  }
	  } // end for
  }
  LsysContext::current()->add_pproduction(pprod);
  size_t id = pprod->pid();
  if(pprod_id) *pprod_id = id;
  size_t nbInitialLine = std::count(initbeg,beg,'\n');
  size_t nbResultLine = std::count(result.begin(),result.end(),'\n');
  std::string complementcode;
  for(;nbResultLine < nbInitialLine; ++nbResultLine) complementcode += '\n';
  result = "(" + TOOLS(number)(id) + result + complementcode+ ")";
  return result;
}


/*---------------------------------------------------------------------------*/


namespace {

enum class ModuleStringMode { Production, Pattern };
enum class ModuleToken { Module, GetModule, Surface };
enum class ModuleLookahead { End, Arguments, Other };
enum class ModuleSyntax { Bare, WithArguments, Variable, Capture, Surface };

constexpr ModuleSyntax moduleStringSyntax[2][3][3] = {
#include "module_string_syntax.inc"
};

std::pair<size_t, std::string>
parseAModule(SourceIterator& it, SourceIterator end, ModuleStringMode mode, int lineno)
{
    size_t nameSize = 0;
    const ModuleClassPtr module = ModuleClassTable::get().parse(it, end, nameSize);
    if (!module) {
        LsysSyntaxError(std::string("Invalid symbol '") + *it + "' in AxialTree.", "", lineno);
        ++it;
        return {};
    }
    it += nameSize;
    while (it != end && (*it == ' ' || *it == '\t')) ++it;

    ModuleToken token = ModuleToken::Module;
    if (module == ModuleClass::GetModule) token = ModuleToken::GetModule;
    else if (module == ModuleClass::CpfgSurface) token = ModuleToken::Surface;

    ModuleLookahead lookahead = ModuleLookahead::End;
    if (it != end)
        lookahead = *it == '(' ? ModuleLookahead::Arguments : ModuleLookahead::Other;
    const auto syntax = moduleStringSyntax[static_cast<size_t>(mode)]
                                         [static_cast<size_t>(token)]
                                         [static_cast<size_t>(lookahead)];
    const size_t moduleId = module->getId();
    std::string args;
    switch (syntax) {
    case ModuleSyntax::Bare:
        return {moduleId, args};
    case ModuleSyntax::Variable: {
        const auto variable = it;
        while (it != end && (*it == '_' || isalnum(static_cast<unsigned char>(*it)))) ++it;
        return {moduleId, std::string(variable, it)};
    }
    case ModuleSyntax::Capture: {
        const auto captured = parseAModule(it, end, mode, lineno);
        const ModuleClassPtr capturedClass = ModuleClassTable::get().find(captured.first);
        if (!capturedClass) return {moduleId, args};
        if (capturedClass == ModuleClass::GetModule)
            LsysSyntaxError("Invalid construction for get module '$" + capturedClass->name + "'.", "", lineno);
        return {moduleId, captured.second + "," + capturedClass->name};
    }
    case ModuleSyntax::Surface:
        args = std::string("'") + *it + "',";
        ++it;
        if (it == end || *it != '(') return {moduleId, args};
        break;
    case ModuleSyntax::WithArguments:
        break;
    }

    const auto argsBegin = ++it;
    auto lastArg = argsBegin;
    auto argsEnd = readPythonFragment(argsBegin, end, ",)", lineno);
    while (argsEnd != end && *argsEnd == ',') {
        lastArg = argsEnd + 1;
        argsEnd = readPythonFragment(lastArg, end, ",)", lineno);
    }
    if (argsEnd == end) {
        it = end;
        LsysSyntaxError("Invalid syntax in AxialTree: expected ')'.", "", lineno);
        return {moduleId, args};
    }
    it = argsEnd + 1;

    while (lastArg != argsEnd && (*lastArg == ' ' || *lastArg == '\t')) ++lastArg;
    if (mode == ModuleStringMode::Production && lastArg != argsEnd && *lastArg == '*') {
        auto variable = lastArg + 1;
        if (variable != argsEnd && *variable == '*') ++variable;
        if (variable != argsEnd && LpyParsing::isValidVariableName(variable, argsEnd)) {
            args.append(argsBegin, lastArg);
            args += "PackedArgs(";
            args.append(variable, it);
            return {moduleId, args};
        }
    }
    args.append(argsBegin, argsEnd);
    return {moduleId, args};
}

} // namespace

std::vector<std::pair<size_t,std::string> > 
LpyParsing::parselstring( std::string::const_iterator& beg,
					  std::string::const_iterator endpos,
					  char delim,
					  int lineno,
					  bool production)
{
  const auto mode = production ? ModuleStringMode::Production : ModuleStringMode::Pattern;
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
	else if(*_it == ' ' || *_it == '\t' || *_it == '\n'){
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
		result.push_back(parseAModule(_it,endpos,mode,lineno));
	}
  } // end while
  beg = _it;
  return result;
}

/*---------------------------------------------------------------------------*/

namespace {

#include "module_declaration_syntax.inc"

bool isModuleDeclarationEnd(char c, char delimiter)
{
    return c == delimiter || c == '\n' || c == ModuleAttributesMarker || c == '#';
}

} // namespace

LpyParsing::ModDeclarationList
LpyParsing::parse_moddeclist(std::string::const_iterator& beg,
                            std::string::const_iterator endpos,
                            char delim)
{
    ModDeclarationList result;
    auto it = beg;
    const auto skipSpace = [&] {
        while (it != endpos && (*it == ' ' || *it == '\t')) ++it;
    };

    while (it != endpos) {
        skipSpace();
        if (it == endpos || isModuleDeclarationEnd(*it, delim)) break;
        if (!result.empty()) {
            if (*it != ModuleListSeparator) {
                LsysSyntaxError(std::string("Invalid syntax in module declaration: Wait for ','instead of '") + *it + "'");
                break;
            }
            ++it;
            skipSpace();
            if (it == endpos || isModuleDeclarationEnd(*it, delim)) break;
        }

        const auto nameBegin = it;
        while (it != endpos && !isModuleDeclarationEnd(*it, delim) &&
               *it != ModuleListSeparator && *it != ModuleAssignment &&
               *it != ModuleParametersBegin && *it != ' ' && *it != '\t')
            ++it;
        if (nameBegin == it) {
            LsysSyntaxError("Invalid empty name in declaration of " +
                           PGL_NAMESPACE_NAME::number(result.size() + 1) + " module.");
            break;
        }
        result.emplace_back(std::string(nameBegin, it));
        auto& declaration = result.back();
        skipSpace();
        if (it == endpos || isModuleDeclarationEnd(*it, delim)) break;

        switch (moduleSuffix(*it)) {
        case ModuleSuffix::Alias: {
            ++it;
            skipSpace();
            const auto target = it;
            while (it != endpos && (*it == '_' || isalnum(static_cast<unsigned char>(*it)))) ++it;
            declaration.alias = true;
            declaration.parameters.assign(target, it);
            break;
        }
        case ModuleSuffix::Parameters: {
            const auto parameters = ++it;
            it = readPythonFragment(parameters, endpos, std::string(1, ModuleParametersEnd));
            if (it == endpos) {
                LsysSyntaxError("Invalid syntax in module declaration: Wait for ')'.");
                break;
            }
            declaration.parameters.assign(parameters, it);
            ++it;
            break;
        }
        case ModuleSuffix::None:
            if (*it == ModuleListSeparator) break;
            LsysSyntaxError("Invalid syntax in module declaration: Wait for ',','=',':','(','#'.");
            beg = it;
            return result;
        }
    }
    beg = it;
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

LpyParsing::ModLineDeclaration
LpyParsing::parse_moddeclaration_line(std::string::const_iterator& beg,
                                      std::string::const_iterator endpos,
                                      char delim)
{
    ModLineDeclaration result;
    auto it = beg;
    result.first = parse_moddeclist(it, endpos, delim);
    if (it != endpos && *it == ModuleAttributesMarker) {
        const std::string valueDelimiters = std::string("\n#") + ModuleListSeparator + delim;
        const auto skipSpace = [&] {
            while (it != endpos && *it != delim && (*it == ' ' || *it == '\t' || *it == '\n'))
                ++it;
        };

        do {
            ++it;
            skipSpace();
            const auto keyBegin = it;
            while (it != endpos && *it != delim &&
                   (isalnum(static_cast<unsigned char>(*it)) || *it == '_'))
                ++it;
            const std::string key(keyBegin, it);
            skipSpace();
            if (it == endpos || *it == delim || *it != ModuleAssignment) {
                LsysSyntaxError("Invalid module attribute in module declaration ('=' missing).");
                break;
            }
            ++it;
            skipSpace();
            const auto valueBegin = it;
            it = readPythonFragment(valueBegin, endpos, valueDelimiters);
            result.second[key].assign(valueBegin, it);
        } while (it != endpos && *it != delim && *it == ModuleListSeparator);
    }
    beg = it;
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

std::string LpyParsing::removeSpaces(const std::string& str)
{
	/// triming name
	std::string res = str;
	std::string::iterator _itb = res.begin();
	std::string::iterator _itb2 = res.begin();
	std::string::iterator _ite = res.end();
	while(_itb!=_ite) {
		if (*_itb == ' ' || *_itb == '\t' || *_itb == '\n'){
			_itb2 = _itb;
			while (*_itb == ' ' || *_itb == '\t' || *_itb == '\n')_itb++;
			res.erase(_itb2,_itb);
		}
		else _itb++;
	}
	return res;
}

/*---------------------------------------------------------------------------*/

std::vector<std::string> LpyParsing::parse_arguments(std::string::const_iterator beg,
										 std::string::const_iterator end)
{
    std::vector<std::string> result;
    while (beg != end) {
        const auto separator = readPythonFragment(beg, end, ",");
        std::string argument = trim(std::string(beg, separator));
        if (!argument.empty()) result.push_back(argument);
        if (separator == end) break;
        beg = separator + 1;
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
		if(*beg == '*') { ++beg; if(beg == end) return false; }

		while(beg != end && (*beg == ' ' || *beg == '\t'))++beg; 
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
	if(it == end) LsysError("Error parsing variable name (2)","",lineno);
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
