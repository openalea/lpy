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

#include "axialtree.h"
#include "patternstring.h"
#include "lpy_parser.h"
#include "tracker.h"
#include "matching.h"

using namespace boost::python;

LPY_BEGIN_NAMESPACE


/*---------------------------------------------------------------------------*/

AxialTree::AxialTree():
  BaseType()
{ IncTracker(AxialTree) }

AxialTree::AxialTree(const AxialTree& m):
  BaseType(m)
  { IncTracker(AxialTree) }

 AxialTree::AxialTree(const std::string& s):
  BaseType()
{ 
  IncTracker(AxialTree)
  std::vector<std::pair<size_t,std::string> > parsedstring = LpyParsing::parselstring(s);

  size_t newcapacity = size()+parsedstring.size();
  if (__conststring().capacity() < newcapacity)
  __string().reserve(newcapacity);
  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
	  it != parsedstring.end(); ++it){
		__string().push_back(ParamModule(it->first,it->second));
  }
}

AxialTree::AxialTree(const ParamModule& m):
   BaseType(m)
{ IncTracker(AxialTree) }

AxialTree::AxialTree(const boost::python::list& l):
  BaseType(){
  IncTracker(AxialTree) 
  object iter_obj = object( handle<>( PyObject_GetIter( l.ptr() ) ) );
  while( true )
  {
        object obj;
        try {  obj = iter_obj.attr( "next" )(); }
        catch( error_already_set ){ PyErr_Clear(); break; }
        extract<size_t> idext(obj);
		if (idext.check())
			__string().push_back(idext());
		else {
			extract<std::string> st(obj);
			if(st.check())
				operator+=(AxialTree(st()));
			else {
				extract<tuple> tu(obj);
				if(tu.check())
					__string().push_back(ParamModule(tu()));
				else {
					extract<AxialTree> ax(obj);
					if(ax.check()) operator+=(ax());
					else __string().push_back(extract<ParamModule>(obj)());
				}
			}
		}
    }
}

AxialTree::AxialTree(const boost::python::tuple& t):
  BaseType(ParamModule(t))
{
	IncTracker(AxialTree) 
}

AxialTree::AxialTree(const_iterator beg, const_iterator end):
  BaseType(beg,end)
{
	IncTracker(AxialTree) 
}

AxialTree::~AxialTree()
{
	DecTracker(AxialTree) 
}

std::string AxialTree::str_slice(const_iterator beg, const_iterator end) const{
  std::string str;
  for(ModuleList::const_iterator _it = beg; _it != end; _it++)
		 str += _it->str();
  return str;
}

std::string AxialTree::repr() const{
  std::string str = "AxialTree(";
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 str += _it->repr();
  str +=')';
  return str;
}

size_t
AxialTree::count(const std::string& name) const{
  size_t c = 0;
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 if (_it->name() == name )
		   c++;
  return c;
}

size_t
AxialTree::count(const std::string& name, size_t nbparam) const{
  size_t c = 0;
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 if (_it->name() == name && _it->argSize() == nbparam)
		   c++;
  return c;
}

size_t
AxialTree::count(const ParamModule& module) const{
  return count(module.name(),module.argSize());
}

AxialTree::const_iterator 
AxialTree::find(const std::string& name, size_t nbparam, 
				const_iterator start, 
				const_iterator stop) const
{ 
  const_iterator _it = start;
  while(_it != stop && (_it->name() != name || _it->argSize() != nbparam))++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

AxialTree::const_iterator 
AxialTree::find(const PatternString& modules, 
				const_iterator start,
				const_iterator stop) const
{
  const_iterator _it = start;
  while(_it != stop && !match(modules,_it))++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

AxialTree::const_iterator 
AxialTree::find(const std::string& name, 
				const_iterator start, 
				const_iterator stop) const
{ 
  const_iterator _it = start;
  while(_it != stop && _it->name() != name)++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

AxialTree 
AxialTree::replace(const PatternModule& i, const ParamModule& j) const{
  AxialTree dest;
  const_iterator _it = const_begin();
  const_iterator _it2 = _it;
  while(!isEnd(_it2)){
	while(!isEnd(_it2) && !match(i,_it2))++_it2;
	dest.push_back(_it,_it2);
	if(!isEnd(_it2)){
	  dest += j;
	  ++_it2;
	  _it = _it2;
	}
  }
  return dest;
}

AxialTree 
AxialTree::replace(const PatternModule& i, const AxialTree& j) const{
  AxialTree dest;
  const_iterator _it = const_begin();
  const_iterator _it2 = _it;
  while(!isEnd(_it2)){
	while(!isEnd(_it2) && !match(i,_it2))++_it2;
	dest.push_back(_it,_it2);
	if(!isEnd(_it2)){
	  dest += j;
	  ++_it2;
	  _it = _it2;
	}
  }
  return dest;
}

AxialTree 
AxialTree::replace(const PatternString& i, const AxialTree& j) const{
  AxialTree dest;
  const_iterator _it = const_begin();
  const_iterator _it2 = _it;
  const_iterator _it3 = _it;
  while(!isEnd(_it2)){
	while(!isEnd(_it2) && !match(i,_it2,_it3))++_it2;
	dest.push_back(_it,_it2);
	if(!isEnd(_it2)){
	  dest += j;
	  _it2 = _it3;
	  _it = _it2;
	}
  }
  return dest;
}


bool AxialTree::match(const PatternString& pattern, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos,
					  AxialTree::const_iterator& last_matched,
					  ArgList& params) const
{ 
	return MatchingEngine::match(it,const_begin(),const_end(),pattern.const_begin(),pattern.const_end(),resultingpos,last_matched,params);
}

bool AxialTree::reverse_match(const PatternString& pattern, 
					  AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return reverse_match(pattern,it,res); }

bool AxialTree::reverse_match(const PatternString& pattern, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos) const
{ 
  ArgList params;
  if (isEnd(it)) return false;
  return MatchingEngine::reverse_match(it,begin(),end(),pattern.const_rbegin(),pattern.const_rend(),resultingpos,params);
}


bool AxialTree::reverse_match(const PatternString& pattern, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos,
					  ArgList& params) const
{ 
  if (isEnd(it)) return false;
  return MatchingEngine::reverse_match(it,begin(),end(),pattern.const_rbegin(),pattern.const_rend(),resultingpos,params);
}
 
bool AxialTree::rightmatch(const PatternString& pattern, 
						   AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return rightmatch(pattern,it,res); }

bool AxialTree::rightmatch(const PatternString& pattern, 
						   AxialTree::const_iterator it,
						   AxialTree::const_iterator& resultingpos) const{
  ArgList params;
  return rightmatch(pattern,it,resultingpos,params);
}

bool AxialTree::rightmatch(const PatternString& pattern, 
						   AxialTree::const_iterator it,
						   AxialTree::const_iterator& resultingpos,
						   ArgList& params) const{
  return rightmatch(pattern,it,it,resultingpos,params);
}

bool AxialTree::rightmatch(const PatternString& pattern, 
						   AxialTree::const_iterator it,
						   AxialTree::const_iterator last_matched,
						   AxialTree::const_iterator& resultingpos,
						   ArgList& params) const{
  if(pattern.empty())return true;
  return MatchingEngine::right_match(it,const_begin(),const_end(),pattern.const_begin(),pattern.const_end(),last_matched, resultingpos,params);
}

AxialTree::const_iterator
AxialTree::rightfind(const PatternString& a,
				 const_iterator start,
				 const_iterator stop) const {
  if(a.empty())return start;
  const_iterator _it = start;
  while(_it != stop && !rightmatch(a,_it))++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

bool AxialTree::leftmatch(const PatternString& pattern, 
						  AxialTree::const_iterator it,
						  AxialTree::const_iterator& resultingpos,
						  ArgList& params) const{
  if(pattern.empty())return true;
  return MatchingEngine::left_match(it,const_begin(),const_end(),pattern.const_rbegin(),pattern.const_rend(),resultingpos,params);
}

bool AxialTree::leftmatch(const PatternString& pattern, 
						  AxialTree::const_iterator it,
						  AxialTree::const_iterator& resultingpos) const{

  if(pattern.empty())return true;
  ArgList params;
  return MatchingEngine::left_match(it,const_begin(),const_end(),pattern.const_rbegin(),pattern.const_rend(),resultingpos,params);
}

bool AxialTree::leftmatch(const PatternString& a, AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return leftmatch(a,it,res); }

AxialTree::const_iterator
AxialTree::leftfind(const PatternString& a,
					const_iterator start,
					const_iterator stop) const {
  if(a.empty())return stop;
  const_iterator _it = stop;
  bool b = false;
  while(_it != start && !(b = leftmatch(a,_it)))++_it; 
  if(_it == start && !b)return end(); 
  else return _it;
}


LPY_END_NAMESPACE
