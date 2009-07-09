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



PatternString::PatternString(const std::string& s, int lineno) : BaseType() {
  IncTracker(PatternString)
  std::vector<std::pair<size_t,std::string> > parsedstring = LpyParsing::parselstring(s,lineno);
  __string().reserve(parsedstring.size());
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
