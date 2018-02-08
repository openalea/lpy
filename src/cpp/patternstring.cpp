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


#include "patternstring.h"
#include "lpy_parser.h"
#include "tracker.h"

LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/


PatternString::PatternString() : BaseType(){  IncTracker(PatternString) }
PatternString::PatternString(const PatternModule& m ) : BaseType(){ IncTracker(PatternString) append(m); }
PatternString::PatternString(const_iterator beg, const_iterator end) : BaseType(beg,end) { IncTracker(PatternString) }

PatternString::~PatternString() { DecTracker(PatternString) }

PatternString::PatternString(const PatternString& s) : BaseType(s) {  IncTracker(PatternString) }



PatternString::PatternString(const std::string& s, int lineno) : BaseType() {
  IncTracker(PatternString)
  std::vector<std::pair<size_t,std::string> > parsedstring = LpyParsing::parselstring(s,lineno);
  reserve(parsedstring.size());
  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
	  it != parsedstring.end(); ++it){
		append(PatternModule(it->first,it->second,lineno));
  }
}

std::vector<std::string> PatternString::getVarNames() const
{
  std::vector<std::string> res;
  for(const_iterator _it = begin(); _it !=end(); ++_it){
	std::vector<std::string> modvar = _it->getVarNames();
	res.insert(res.end(),modvar.begin(),modvar.end());
  }
  return res;
}
	
size_t PatternString::getVarNb() const {  
  size_t res = 0;
  for(const_iterator _it = begin(); _it !=end(); ++_it){
	res += _it->getVarNb();
  }
  return res;
}

void PatternString::setUnnamedVariables()
{
  for(iterator _it = begin(); _it !=end(); ++_it){
	_it->setUnnamedVariables();
  }
}

void PatternString::setUnnamedVariable(size_t idvar)
{
  size_t count = 0;
  for(iterator _it = begin(); _it !=end(); ++_it){
	size_t l = _it->getVarNb();
	if(count + l > idvar) {
	   _it->setUnnamedVariable(idvar - count);
	   break;
	}
  }
}

std::vector<size_t> PatternString::getFirstClassId() const
{
	if(empty())return std::vector<size_t>();
	else return getAt(0).getFirstClassId();
}

std::vector<size_t> PatternString::getLastClassId() const
{
	if(empty())return std::vector<size_t>();
	else return getAt(size()-1).getLastClassId();
}


std::string PatternString::str() const{
  std::string str;
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 str += _it->str();
  return str;
}

std::string PatternString::repr() const{
  std::string str = "PatternString(";
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 str += _it->repr();
  str +=')';
  return str;
}


/*---------------------------------------------------------------------------*/


PatternStringManager * PatternStringManager::Instance(0);

PatternStringManager& PatternStringManager::get()
{
	if (PatternStringManager::Instance == NULL)
		PatternStringManager::Instance = new PatternStringManager();
	return *PatternStringManager::Instance;
}

PatternStringManager::~PatternStringManager()
{
}

PatternStringManager::PatternStringManager():
	__patterns(), __free_indices()
{
}

#include <plantgl/tool/util_string.h>

const PatternString& PatternStringManager::get_pattern(size_t pid)
{
	if (pid < __patterns.size())
	{
		return __patterns[pid];
	}
	else 
	{
		std::string msg("Cannot find parametric production ");
		msg += TOOLS(number(pid));
		LsysError(msg);
		return __nullpattern;
	}
}

size_t PatternStringManager::register_pattern(const PatternString& pattern)
{
	size_t pid;
	if (__free_indices.empty()){
		pid = __patterns.size();
		__patterns.push_back(pattern);
	}
	else {
		pid = __free_indices.front();
		__free_indices.pop();
		__patterns[pid] = pattern;
	}
	return pid;
}

void PatternStringManager::remove_pattern(size_t pid)
{
	if (pid == __patterns.size())
		__patterns.pop_back();
	else {
		__free_indices.push(pid);
		__patterns[pid] = PatternString();
	}
}

